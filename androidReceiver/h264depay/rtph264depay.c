#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <stdbool.h>
#include <glib.h>
#include "rtph264depay.h"

GQueue *stapQueue;
GQueue *fuQueue;
GQueue *singleQueue;
gboolean byte_stream;
gboolean wait_start;
gboolean mEnable = false;
gboolean fu_marker;
unsigned char current_fu_type;
unsigned char sync_bytes[] = {0, 0, 0, 1 };
        
int sendSingleBuffer()
{
    char *buffer;
    int count = 0;

    while (!g_queue_is_empty(singleQueue)) {
        buffer = (char *)g_queue_pop_head(singleQueue);
        free(buffer);
        count++;
    }

    return count;
}
              
void finishFragmentation_unit()
{
    char *buffer;

    while (!g_queue_is_empty(fuQueue)) {
        buffer = (char *)g_queue_pop_head(fuQueue);
        free(buffer);
    }
    
    current_fu_type = 0;
}

int sendStapBuffer()
{
    char *buffer;
    int count = 0;

    while (!g_queue_is_empty(stapQueue)) {
        buffer = (char *)g_queue_pop_head(stapQueue);
        free(buffer);
        count++;
    }
    
    return count;
}

void setStreamMode(gboolean mode)
{
    byte_stream = mode;

}

gboolean getStreamMode()
{
    return byte_stream;
}

void rtph264depay_init(void)
{
    byte_stream = true;
    wait_start = false;
    fuQueue = NULL;
    stapQueue = NULL;
    current_fu_type = 0;
	stapQueue = g_queue_new();
    fuQueue = g_queue_new();
    singleQueue =  g_queue_new();
    if (!stapQueue || !fuQueue || !singleQueue) {
        printf("alloc queue failed!\n");
        mEnable = false;
    } else {
        mEnable = true;
    }
}

void rtph264depay_exit(void)
{
    char *buffer;

    while (!g_queue_is_empty(stapQueue)) {
        buffer = (char *)g_queue_pop_head(stapQueue);
        free(buffer);
    }

	g_queue_free(stapQueue);

    while (!g_queue_is_empty(fuQueue)) {
        buffer = (char *)g_queue_pop_head(fuQueue);
        free(buffer);
    }

	g_queue_free(fuQueue);

    while (!g_queue_is_empty(singleQueue)) {
        buffer = (char *)g_queue_pop_head(singleQueue);
        free(buffer);
    }

	g_queue_free(singleQueue);
}

void *DepayProcess (void *buffer, unsigned int payload_len, int marker)
{
	char *payload;
	unsigned char nal_ref_idc;
	unsigned char nal_unit_type;
	unsigned int header_len;
	unsigned int nalu_size;
	unsigned int outsize;
    unsigned char *outbuf;
    struct h264Buffer *info;

	payload = (char *)buffer;
    if (mEnable == false) {
        rtph264depay_init();
    }

    /* +---------------+
     * |0|1|2|3|4|5|6|7|
     * +-+-+-+-+-+-+-+-+
     * |F|NRI|  Type   |
     * +---------------+
     *
     * F must be 0.
     */
    nal_ref_idc = (payload[0] & 0x60) >> 5;
    nal_unit_type = payload[0] & 0x1f;
	printf("nal_ref_idc is %d, nal_unit_type is %d\n", nal_ref_idc, nal_unit_type);

    /* at least one byte header with type */
    header_len = 1;

    /* If FU unit was being processed, but the current nal is of a different
     * type.  Assume that the remote payloader is buggy (didn't set the end bit
     * when the FU ended) and send out what we gathered thusfar */
    switch (nal_unit_type) {
      case 0:
      case 30:
      case 31:
        /* undefined */
        goto undefined_type;
      case 25:
        /* STAP-B    Single-time aggregation packet     5.7.1 */
        /* 2 byte extra header for DON */
        header_len += 2;
        /* fallthrough */
      case 24:
      {
        int len;
        int count;

        /* strip headers */
        payload += header_len;
        payload_len -= header_len;

        /* STAP-A    Single-time aggregation packet     5.7.1 */
        while (payload_len > 2) {
          /*                      1
           *  0 1 2 3 4 5 6 7 8 9 0 1 2 3 4 5
           * +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
           * |         NALU Size             |
           * +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
           */
          nalu_size = (payload[0] << 8) | payload[1];
		  printf("stap mode>>nalu_size is %d.\n", nalu_size);

          /* dont include nalu_size */
          if (nalu_size > (payload_len - 2))
            nalu_size = payload_len - 2;

          outsize = nalu_size + sizeof (sync_bytes);
          outbuf = malloc(outsize + sizeof(struct h264Buffer));
          info = (struct h264Buffer *)outbuf;
          outbuf += sizeof(struct h264Buffer);

          if (byte_stream) {
            memcpy (outbuf, sync_bytes, sizeof (sync_bytes));
          } else {
            outbuf[0] = outbuf[1] = 0;
            outbuf[2] = payload[0];
            outbuf[3] = payload[1];
          }

          /* strip NALU size */
          payload += 2;
          payload_len -= 2;

          memcpy(outbuf + sizeof (sync_bytes), payload, nalu_size);

          info->buffer = outbuf;
          info->size = outsize;
          g_queue_push_tail(stapQueue, info);
          len = g_queue_get_length(stapQueue);
          printf("stapQueue's length is %d.\n", len);
          count = sendStapBuffer();
          printf("send %d buffers\n", count);
          if (len != count) {
            printf("failed!\n");
          }

          payload += nalu_size;
          payload_len -= nalu_size;
        }
        break;
      }
      case 26:
        /* MTAP16    Multi-time aggregation packet      5.7.2 */
        // header_len = 5;
        /* fallthrough, not implemented */
      case 27:
        /* MTAP24    Multi-time aggregation packet      5.7.2 */
        // header_len = 6;
        goto not_implemented;
        break;
      case 28:
      case 29:
      {
        /* FU-A      Fragmentation unit                 5.8 */
        /* FU-B      Fragmentation unit                 5.8 */
        int S, E;

        /* +---------------+
         * |0|1|2|3|4|5|6|7|
         * +-+-+-+-+-+-+-+-+
         * |S|E|R|  Type   |
         * +---------------+
         *
         * R is reserved and always 0
         */
        S = (payload[1] & 0x80) == 0x80;
        E = (payload[1] & 0x40) == 0x40;
        printf("FU mode>>S is %d, E is %d.\n", S, E);

        if (!S) {
            goto waiting_start;
        }

        if (S) {
          /* NAL unit starts here */
          unsigned char nal_header;

          /* If a new FU unit started, while still processing an older one.
           * Assume that the remote payloader is buggy (doesn't set the end
           * bit) and send out what we've gathered thusfar */
          if (current_fu_type != 0) {
              finishFragmentation_unit();
          }

          current_fu_type = nal_unit_type;
         // fu_timestamp = timestamp;

          wait_start = false;

          /* reconstruct NAL header */
          nal_header = (payload[0] & 0xe0) | (payload[1] & 0x1f);

          /* strip type header, keep FU header, we'll reuse it to reconstruct
           * the NAL header. */
          payload += 1;
          payload_len -= 1;

          nalu_size = payload_len;
          outsize = nalu_size + sizeof (sync_bytes);
          outbuf = malloc(outsize + sizeof(struct h264Buffer));
          info = (struct h264Buffer *)outbuf;
          outbuf += sizeof(struct h264Buffer);

          memcpy(outbuf, sync_bytes, sizeof(sync_bytes));
          memcpy(outbuf + sizeof (sync_bytes), payload, nalu_size);
          outbuf[sizeof (sync_bytes)] = nal_header;
          info->buffer = outbuf; 
          info->size = outsize;
          g_queue_push_tail(fuQueue, info);
          printf("start>>S is %d, queue is %d bytes", S, outsize);
        } else {
          /* strip off FU indicator and FU header bytes */
          payload += 2;
          payload_len -= 2;

          outsize = payload_len;
          outbuf = malloc(outsize + sizeof(struct h264Buffer));
          info = (struct h264Buffer *)outbuf;
          outbuf += sizeof(struct h264Buffer);
          memcpy(outbuf, payload, outsize);
          info->buffer = outbuf; 
          info->size = outsize;
          g_queue_push_tail(fuQueue, info);
          printf("end>>S is %d , outsize is %d bytes.\n", S, outsize);
        }

        fu_marker = marker;

        /* if NAL unit ends, flush the adapter */
        if (E) {
            finishFragmentation_unit();
        }
        break;
      }

      default:
      {
          
        printf("default mode\n");
        wait_start = false;
        int count;

        /* 1-23   NAL unit  Single NAL unit packet per H.264   5.6 */
        /* the entire payload is the output buffer */
        nalu_size = payload_len;
        outsize = nalu_size + sizeof (sync_bytes);
        outbuf = malloc(outsize + sizeof(struct h264Buffer));
        info = (struct h264Buffer *)outbuf;
        outbuf += sizeof(struct h264Buffer);

        if (byte_stream) {
          memcpy (outbuf, sync_bytes, sizeof (sync_bytes));
        } else {
          outbuf[0] = outbuf[1] = 0;
          outbuf[2] = nalu_size >> 8;
          outbuf[3] = nalu_size & 0xff;
        }

        memcpy (outbuf + sizeof (sync_bytes), payload, nalu_size);
        info->buffer = outbuf;
        info->size = outsize;
        g_queue_push_tail(singleQueue, info);
        count = sendSingleBuffer();
        printf("default count is %d.\n", count);

        break;
      }
    }


  rtph264depay_exit();
  return NULL;

  /* ERRORS */
empty_packet:
  {
	  printf("empty_packet!\n"); 
      rtph264depay_exit();
	  return NULL;
  }

undefined_type:
  {
	  printf("Undefined packet type.\n"); 
      rtph264depay_exit();
	  return NULL;
  }
waiting_start:
  {
	  printf("waiting for start.\n"); 
      rtph264depay_exit();
	  return NULL;
  }
not_implemented:
  {
	  printf("NAL unit type is %d, not supported yet.\n",  nal_unit_type); 
      rtph264depay_exit();
	  return NULL;
  }
}
