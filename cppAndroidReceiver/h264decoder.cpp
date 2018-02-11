/*
 * (C) Copyright 2010
 * Steve Chang
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License as
 * published by the Free Software Foundation; either version 2 of
 * the License, or (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place, Suite 330, Boston,
 * MA 02111-1307 USA
 *
*/

#include <libavutil/opt.h>
#include <libavcodec/avcodec.h>
#include <libavutil/channel_layout.h>
#include <libavutil/common.h>
#include <libavutil/imgutils.h>
#include <libavutil/mathematics.h>
#include <libavutil/samplefmt.h>

H264Decoder::H264Decoder():bStop(0)
{
  /* register all the codecs */
  avcodec_register_all();
  codec = avcodec_find_decoder(AV_CODEC_ID_H264);
  if (!codec) {
	  printf("[%]find decoder failed!\n", __func__);
	  return -1;
  }
  
  context = avcodec_alloc_context3(codec);
  if (!context) {
	  printf("[%]alloc context failed!\n", __func__);
	  return -1;
  }

#if 0
  /* put sample parameters */
 context->bit_rate = ; 
 context->sample_rate = ; 
 context->channels = ; 
 context->sample_fmt = ; 
#endif
 
  /* open it */
  if(avcodec_open2(context, codec, NULL) < 0) {
    fprintf(stderr, "could not open codec\n");
    exit(EXIT_FAILURE);
  }

  /* the codec gives us the frame size, in samples */
  
  Mp4mux_Init();
  Mp4mux_Open("/tmp/scv.mp4");
}

H264Decoder::~H264Decoder()
{
  Mp4mux_Close();

  avcodec_close(context);
  av_free(context);
}

void H264Decoder::stop()
{
  bStop = 1;
}

int H264Decoder::decode(unsigned char *buffer, int size)
{
  AVFrame *picture = avcodec_alloc_frame();
  av_init_packet(&avpkt);
  int frame_count = 0;

  avpkt->size = size;
  avpkt->data = buffer;
  if (avpkt.size > 0) {
	  int len = avcodec_decode_video2(context, picture, &got_picture, &avpkt);
      
	  Mp4Mux_WriteVideo(&avpkt, timestamp);
	  if(len < 0) {
		  fprintf(stderr, "Error while decoding frame\n");
		  av_init_packet(&avpkt);
		  break;
	  }
      
	  if(got_picture) {
		  /* the picture is allocated by the decoder. no need to free it */
		  printf("size:%d, width:%d, height:%d\n", picture->linesize[0], context->width, context->height);
	  }

		  av_init_packet(&avpkt);
		  break;
	  }
  }

cleanup:
  av_free(picture);
  return;
}
