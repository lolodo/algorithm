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

AVCodec *codec = NULL;
AVCodecContext *context;
AVFrame *frame;
AVPacket pkt;

int RtpH264_Init()
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

  /* put sample parameters */
 context->bit_rate = ; 
 context->sample_rate = ; 
 context->channels = ; 
 context->sample_fmt = ; 
 
  /* open it */
  if(avcodec_open2(context, codec, NULL) < 0) {
    fprintf(stderr, "could not open codec\n");
    exit(EXIT_FAILURE);
  }

  /* the codec gives us the frame size, in samples */
  
  Mp4mux_Init();
  Mp4mux_Open("/tmp/scv.mp4");
}

void RtpH264_Deinit()
{
  Mp4mux_Close();

  avcodec_close(context);
  av_free(context);
}

static int bStop = 0;

void RtpH264_Stop()
{
  bStop = 1;
}

void RtpH264_Run(int sfd, RtpH264_OnPicture onPicture)
{
#define INBUF_SIZE (1024 * 128)
  uint8_t inbuf[INBUF_SIZE + FF_INPUT_BUFFER_PADDING_SIZE];
  /* set end of buffer to 0 (this ensures that no overreading happens for damaged mpeg streams) */
  memset(inbuf + INBUF_SIZE, 0, FF_INPUT_BUFFER_PADDING_SIZE);

  AVFrame *picture = avcodec_alloc_frame();
  AVPacket avpkt;
  av_init_packet(&avpkt);
  int frame_count = 0;

  unsigned short sequence = 0;
  unsigned int timestamp = 0;
  
  while(1)  {
    //int ready = 0;
    fd_set rfds;
    FD_ZERO(&rfds);
    FD_SET(sfd, &rfds);

    struct timeval timeout;  /*Timer for operation select*/
    timeout.tv_sec = 0;
    timeout.tv_usec = 10000; /*10 ms*/

    if(select(sfd+1, &rfds, 0, 0, &timeout) <= 0) {
      if(bStop)
        break;
      else
        continue;
    }

    if(FD_ISSET(sfd, &rfds) <= 0)
      continue;
    
    rtp_hdr_t rtp;
    unsigned char buf[64];
    memset(buf, 0, 64);
    int r = recv(sfd, buf, sizeof(rtp_hdr_t) + 8, MSG_PEEK);  /*  Peek data from socket, so that we could determin how to read data from socket*/
    if(r <= sizeof(rtp_hdr_t) || r == -1)  {
      recv(sfd, buf, sizeof(rtp_hdr_t) + 8, 0); /*Read invalid packet*/
      printf("Warning !!! Invalid packet\n");
      continue; /*Invalid packet ???*/
    }

    int ready = 0;
    int got_picture;

    while(ready && avpkt.size > 0) {
      avpkt.pts = ++frame_count;
      int len = avcodec_decode_video2(context, picture, &got_picture, &avpkt);
      
      Mp4Mux_WriteVideo(&avpkt, timestamp);

      if(len < 0) {
        fprintf(stderr, "Error while decoding frame\n");
        av_init_packet(&avpkt);
        break;
      }
      
      if(got_picture) {
        /* the picture is allocated by the decoder. no need to
               free it */
        if(onPicture)
          onPicture(picture->data[0], picture->linesize[0], context->width, context->height);

        av_init_packet(&avpkt);
        break;
      }
      avpkt.size -= len;
      avpkt.data += len;
    }
  }

cleanup:
  av_free(picture);
  
  return;
}
