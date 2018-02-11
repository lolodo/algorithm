#include <string.h>
#include <stdio.h>
#include <stdbool.h>
#include <iostream>
#include "RtpH264Depay.h"
#include "PracticalSocket.h"
#include "H264Decoder.h"

#define BUFFER_LEN  (1024 * 1024)
using namespace std;
unsigned char sync_bytes[] = {0, 0, 0, 1 };
unsigned char pps[] = {9, 0x30, 0, 0 };
        
gboolean RtpH264Depay::getStatus()
{
    return mEnable;
}

void RtpH264Depay::sendQueue(GQueue *queue) 
{
    struct h264Buffer *info;

    while (!g_queue_is_empty(queue)) {
        info= (struct h264Buffer *)g_queue_pop_head(queue);
        printf("sendTo:0x%08x 0x%08x 0x%08x 0x%08x\n", *(unsigned int *)info->buffer, 
                *(unsigned int *)(info->buffer + 4), *(unsigned int *)(info->buffer + 8), 
                *(unsigned int *)(info->buffer + 12));
       // sock.sendTo(info->buffer, info->size, "127.0.0.1", 8890);
       
        decoder.decode(info->buffer, info->size);
    }
}

void RtpH264Depay::cleanQueue(GQueue *queue)
{
    char *buffer;
    struct h264Buffer *info;
    int out_idx = 0;

    while (!g_queue_is_empty(queue)) {
        buffer = (char *)g_queue_pop_head(queue);

        info = (struct h264Buffer *)buffer;
        cout << "The " << out_idx << "th buffer is cleaned" << endl;
        cout << "size is " << info->size << endl;

        out_idx++;
        delete [] buffer;
    }

}

#define NAL_TYPE_IS_KEY(nt) (((nt) == 5) || ((nt) == 7) || ((nt) == 8)) 
int RtpH264Depay::finishPackets(GQueue *queue)
{
    char *buffer;
    struct h264Buffer *info, *fuInfo;
    unsigned char *outputBuffer, *head;
    bool begin = false;
    bool end = false;
    gboolean keyFrame, outKeyframe;
    int count = 0;
    int nalType;
    bool flag = false;

    if (g_queue_get_length(outputQueue) >= 1) {
        sendQueue(outputQueue);
        cleanQueue(outputQueue);
    }

    /*pps/sps*/
    while(!g_queue_is_empty(singleQueue))
    {
        buffer = (char *)g_queue_pop_head(singleQueue);
        info = (struct h264Buffer *)buffer;
        if (info->size < 5) {
            cout << "too short!" << endl;
            return -1;
        }

        nalType = info->buffer[4] & 0x1f;
        cout << "handle nal type " << nalType << endl;
        printf("0x%08x 0x%08x 0x%08x 0x%08x\n", *(unsigned int *)info->buffer, *(unsigned int *)(info->buffer + 4),
                *(unsigned int *)(info->buffer + 8), *(unsigned int *)(info->buffer + 12)); 
        keyFrame = NAL_TYPE_IS_KEY(nalType);
        outKeyframe = keyFrame;
        delete [] buffer;
    }

    while (!g_queue_is_empty(queue)) {
        buffer = (char *)g_queue_pop_head(queue);
        info = (struct h264Buffer *)buffer;
        begin = !!(info->start);
        end = !!(info->end);

        if (!begin && !flag) {
            cout << "trying to get the first one!" << endl;
            delete [] buffer;
            continue;
        }

        if (flag == false) {
            head = new unsigned char[BUFFER_LEN];
            outputBuffer = head;
        }

        if (begin) {
            count += info->size + sizeof(struct h264Buffer);
            if (count >= BUFFER_LEN) {
                cout << "begin fu overflow, clean all of it!" << endl;
                
                delete [] buffer;
                cleanQueue(queue);
                delete [] head;
                return -1;
            }

            flag = true;
            outputBuffer += sizeof(struct h264Buffer);
            fuInfo = (struct h264Buffer *)head;
            fuInfo->buffer = outputBuffer;
            fuInfo->size = info->size; 
            fuInfo->start = 1; 

            memcpy(fuInfo->buffer, info->buffer, info->size);
            outputBuffer += info->size;
            if (end) {
                fuInfo->buffer[4] = 9;
                fuInfo->buffer[8] = ((fuInfo->size - 10) & 0xff00) >> 8;
                fuInfo->buffer[9] = (fuInfo->size - 10) & 0xff;
                fuInfo->buffer[10] = 0x41;
                fuInfo->end = 1; 
                g_queue_push_tail(outputQueue, fuInfo);
                flag = false;
            }
        } else {
            count += info->size;
            if ((count >= BUFFER_LEN)) {
                delete [] buffer;
                cleanQueue(queue);
                delete [] head;
                return -1;
             }

            fuInfo->size += info->size;
            memcpy(outputBuffer, info->buffer, info->size);
            outputBuffer += info->size;
           
            if (end) {
                fuInfo->buffer[4] = 9;
                fuInfo->buffer[8] = ((fuInfo->size - 10) & 0xff00) >> 8;
                fuInfo->buffer[9] = (fuInfo->size - 10) & 0xff;
                fuInfo->buffer[10] = 0x41;
                fuInfo->end = 1; 
                g_queue_push_tail(outputQueue, fuInfo);
                flag = false;
            } 
        }

        delete [] buffer;
    }
    
    current_fu_type = 0;
    return count;
}


void RtpH264Depay::setStreamMode(gboolean mode)
{
    byte_stream = mode;

}

gboolean RtpH264Depay::getStreamMode()
{
    return byte_stream;
}

RtpH264Depay::RtpH264Depay(void):stapQueue(NULL), byte_stream(true), wait_start(true), fuQueue(NULL), mEnable(false) 
{
    current_fu_type = 0;
	stapQueue = g_queue_new();
    fuQueue = g_queue_new();
    singleQueue =  g_queue_new();
    outputQueue =  g_queue_new();
    if (!stapQueue || !fuQueue || !singleQueue || !outputQueue) {
        cout << "alloc queue failed!" << endl;
        mEnable = false;
    } else {
        mEnable = true;
    }
}

RtpH264Depay::~RtpH264Depay(void)
{
    char *buffer;

    while (!g_queue_is_empty(stapQueue)) {
        buffer = (char *)g_queue_pop_head(stapQueue);
        delete [] buffer;
    }

	g_queue_free(stapQueue);

    while (!g_queue_is_empty(fuQueue)) {
        buffer = (char *)g_queue_pop_head(fuQueue);
        delete [] buffer;
    }

	g_queue_free(fuQueue);

    while (!g_queue_is_empty(singleQueue)) {
        buffer = (char *)g_queue_pop_head(singleQueue);
        delete [] buffer;
    }

	g_queue_free(singleQueue);
}

void *RtpH264Depay::DepayProcess (void *buffer, unsigned int payload_len, int marker)
{
	char *payload;
	unsigned char nal_ref_idc;
	unsigned char nal_unit_type;
	unsigned int header_len;
	unsigned int nalu_size;
	unsigned int outsize;
    unsigned char *outbuf;
    unsigned char *tmp;
    struct h264Buffer *info;
    int count;

	payload = (char *)buffer;

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
    printf("nal_ref_idc:%d, nal_unit_type:%d, len:%d.\n", nal_ref_idc, nal_unit_type, payload_len);

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
		/* STAP-B */
        /* fallthrough */
      case 24:
		/* STAP-A */
		break;
      case 26:
        /* MTAP16    Multi-time aggregation packet      5.7.2 */
        // header_len = 5;
        /* fallthrough, not implemented */
      case 27:
        /* MTAP24    Multi-time aggregation packet      5.7.2 */
        // header_len = 6;
        break;
      case 29:
		/* FU-A */
		break;
      case 28:
      {
        /* FU-B      Fragmentation unit                 5.8 */
        int S, E;
		unsigned char fu_header;

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

        if (S) {
          /* NAL unit starts here */
          unsigned char nal_header;

          /* If a new FU unit started, while still processing an older one.
           * Assume that the remote payloader is buggy (doesn't set the end
           * bit) and send out what we've gathered thusfar */
          if (current_fu_type != 0) {
             count = finishPackets(fuQueue);
             cout << "send " << count << " bytes" << endl;
             if (count < 0) {
                 cout << " sss failed!" << endl;
             }
          }

          current_fu_type = nal_unit_type;
	      fu_header =  payload[1];

          /* reconstruct NAL header */
          nal_header = (payload[0] & 0xe0) | (payload[1] & 0x1f);

          /* strip type header, keep FU header, we'll reuse it to reconstruct
           * the NAL header. */
          payload += 1;
          payload_len -= 1;

          nalu_size = payload_len;
          outsize = nalu_size + sizeof (sync_bytes);
          outbuf = new unsigned char[outsize + sizeof(struct h264Buffer)];
          info = (struct h264Buffer *)outbuf;
          outbuf += sizeof(struct h264Buffer);

          memcpy(outbuf, sync_bytes, sizeof(sync_bytes));
          tmp = outbuf + sizeof(sync_bytes);
		  if ((fu_header & 0x1f) == 7) {
			  //keyFrame
		  }
         
          memcpy(tmp, payload, nalu_size);
          outbuf[sizeof (sync_bytes)] = nal_header;

          info->buffer = outbuf; 
          info->size = outsize;
          info->start = S;
          info->end = E;

          g_queue_push_tail(fuQueue, info);
        } else {

		  fu_header = payload[1];

          /* strip off FU indicator and FU header bytes */
          payload += 2;
          payload_len -= 2;

          outsize = payload_len;
          outbuf = new unsigned char[outsize + sizeof(struct h264Buffer)];
          info = (struct h264Buffer *)outbuf;
          outbuf += sizeof(struct h264Buffer);
          memcpy(outbuf, payload, outsize);

          info->buffer = outbuf; 
          info->size = outsize;
          info->start = S;
          info->end = E;
          
          g_queue_push_tail(fuQueue, info);
        }

        cout << "S is " << S << ", E is " << E << ", queue " << outsize << "bytes" << endl;
        fu_marker = marker;

        /* if NAL unit ends, flush the adapter */
        if (E) {
            count = finishPackets(fuQueue);
            cout << "send " << count << " bytes" << endl;
        }
        break;
      }

      default:
      {
          cout << "default mode" << endl;
          /* 1-23   NAL unit  Single NAL unit packet per H.264   5.6 */
          /* the entire payload is the output buffer */
        nalu_size = payload_len;
        outsize = nalu_size + sizeof (sync_bytes);
        outbuf = new unsigned char[outsize + sizeof(struct h264Buffer)];
        info = (struct h264Buffer *)outbuf;
        outbuf += sizeof(struct h264Buffer);

        memcpy (outbuf, sync_bytes, sizeof (sync_bytes));
        memcpy (outbuf + sizeof (sync_bytes), payload, nalu_size);
        info->buffer = outbuf;
        info->size = outsize;
        info->start = true;
        info->end = true;
        g_queue_push_tail(singleQueue, info);
        break;
      }
    }

  return NULL;

  /* ERRORS */
empty_packet:
  {
	  cout << "empty_packet!" << endl; 
	  return NULL;
  }

undefined_type:
  {
	  cout << "Undefined packet type." << endl; 
	  return NULL;
  }
waiting_start:
  {
	  cout << "waiting for start." << endl; 
	  return NULL;
  }
not_implemented:
  {
	  cout << "NAL unit type " << nal_unit_type << "not supported yet" << endl; 
	  return NULL;
  }
}
