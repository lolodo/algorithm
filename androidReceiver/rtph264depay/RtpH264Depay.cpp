#include <stdio.h>
#include <string.h>
#include <glib.h>
#include "RtpH264Depay.h"

unsigned char sync_bytes[] = {0, 0, 0, 1 };
int byte_stream = 0;

RtpH264Depay::RtpH264Depay(void):queue(NULL)
{
	queue = g_queue_new();
}

RtpH264Depay::~RtpH264Depay(void)
{
	g_queue_free(queue);
}

void *RtpH264Depay::DepayProcess (void *buffer, unsigned int payload_len, int marker)
{
	char *payload;
	unsigned char nal_ref_idc;
	unsigned char nal_unit_type;
	unsigned int header_len;
	unsigned int nalu_size;
	unsigned int outsize;

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
	printf("[%s]line:%d, nal_ref_idc is %d, nal_unit_type is %d.\n", __func__, __LINE__, nal_ref_idc, nal_unit_type);

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
		  printf("[%s]line:%d, nalu_size is %d.\n", __func__, __LINE__, nalu_size);

          /* dont include nalu_size */
          if (nalu_size > (payload_len - 2))
            nalu_size = payload_len - 2;

          outsize = nalu_size + sizeof (sync_bytes);
         // outbuf = (unsigned char *)malloc(outsize);
#if 0
          if (byte_stream) {
          //  memcpy (outbuf, sync_bytes, sizeof (sync_bytes));
          } else {
            outbuf[0] = outbuf[1] = 0;
            outbuf[2] = payload[0];
            outbuf[3] = payload[1];
          }
#endif

          /* strip NALU size */
          payload += 2;
          payload_len -= 2;

          //memcpy (outbuf + sizeof (sync_bytes), payload, nalu_size);

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

        if (S) {
          /* NAL unit starts here */
          unsigned char nal_header;

          /* If a new FU unit started, while still processing an older one.
           * Assume that the remote payloader is buggy (doesn't set the end
           * bit) and send out what we've gathered thusfar */
//          if (G_UNLIKELY (rtph264depay->current_fu_type != 0))
  //          gst_rtp_h264_finish_fragmentation_unit (rtph264depay);

   //       rtph264depay->current_fu_type = nal_unit_type;
     //     rtph264depay->fu_timestamp = timestamp;

       //   rtph264depay->wait_start = FALSE;

          /* reconstruct NAL header */
          nal_header = (payload[0] & 0xe0) | (payload[1] & 0x1f);

          /* strip type header, keep FU header, we'll reuse it to reconstruct
           * the NAL header. */
          payload += 1;
          payload_len -= 1;

          nalu_size = payload_len;
          outsize = nalu_size + sizeof (sync_bytes);
        //  outbuf = gst_buffer_new_and_alloc (outsize);

        //  gst_buffer_map (outbuf, &map, GST_MAP_WRITE);
        //  memcpy (map.data + sizeof (sync_bytes), payload, nalu_size);
//          map.data[sizeof (sync_bytes)] = nal_header;
  //        gst_buffer_unmap (outbuf, &map);

    //      gst_rtp_copy_video_meta (rtph264depay, outbuf, rtp->buffer);

     //     GST_DEBUG_OBJECT (rtph264depay, "queueing %d bytes", outsize);

          /* and assemble in the adapter */
     //     gst_adapter_push (rtph264depay->adapter, outbuf);
        } else {
          /* strip off FU indicator and FU header bytes */
          payload += 2;
          payload_len -= 2;

          outsize = payload_len;
     //     outbuf = gst_buffer_new_and_alloc (outsize);
//          gst_buffer_fill (outbuf, 0, payload, outsize);

  //        gst_rtp_copy_video_meta (rtph264depay, outbuf, rtp->buffer);

    //      GST_DEBUG_OBJECT (rtph264depay, "queueing %d bytes", outsize);

          /* and assemble in the adapter */
 //         gst_adapter_push (rtph264depay->adapter, outbuf);
        }

     //   outbuf = NULL;
   //     rtph264depay->fu_marker = marker;

        /* if NAL unit ends, flush the adapter */
  //      if (E)
//          gst_rtp_h264_finish_fragmentation_unit (rtph264depay);
 //       break;
      }

      default:
      {
     //   rtph264depay->wait_start = FALSE;

        /* 1-23   NAL unit  Single NAL unit packet per H.264   5.6 */
        /* the entire payload is the output buffer */
        nalu_size = payload_len;
        outsize = nalu_size + sizeof (sync_bytes);
      //  outbuf = gst_buffer_new_and_alloc (outsize);

 //       gst_buffer_map (outbuf, &map, GST_MAP_WRITE);
  //      if (rtph264depay->byte_stream) {
  //        memcpy (map.data, sync_bytes, sizeof (sync_bytes));
   //     } else {
  //        map.data[0] = map.data[1] = 0;
  //        map.data[2] = nalu_size >> 8;
  //        map.data[3] = nalu_size & 0xff;
   //     }
    //    memcpy (map.data + sizeof (sync_bytes), payload, nalu_size);
 //       gst_buffer_unmap (outbuf, &map);

  //      gst_rtp_copy_video_meta (rtph264depay, outbuf, rtp->buffer);

   //     gst_rtp_h264_depay_handle_nal(rtph264depay, outbuf, timestamp, marker);
 //       break;
      }
    }

  return NULL;

  /* ERRORS */
empty_packet:
  {

	  printf("[%s]line:%d, empty_packet!\n", __func__, __LINE__);
	  return NULL;
  }

undefined_type:
  {
	  printf("Undefined packet type.\n");
	  return NULL;
  }
waiting_start:
  {
	  printf("waiting for start.\n");
	  return NULL;
  }
not_implemented:
  {
	  printf("NAL unit type %d not supported yet.\n", nal_unit_type);
	  return NULL;
  }
}
