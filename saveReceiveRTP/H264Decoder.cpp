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

extern "C" {
#include <stdio.h>
#include <stdlib.h>
#include <libavutil/opt.h>
#include <libavcodec/avcodec.h>
#include <libavutil/channel_layout.h>
#include <libavutil/common.h>
#include <libavutil/imgutils.h>
#include <libavutil/mathematics.h>
#include <libavutil/samplefmt.h>
#include <libswscale/swscale.h>
}

#include "opencv2/opencv.hpp"
#include "H264Decoder.h"

using namespace cv;
bool H264Decoder::getStatus()
{
    return mEnable;
}

H264Decoder::H264Decoder():bStop(0), context(NULL), codec(NULL), frame(NULL), mEnable(false), swsContext(NULL), picture(NULL)
{
  /* register all the codecs */
  avcodec_register_all();
  codec = avcodec_find_decoder(AV_CODEC_ID_H264);
  if (!codec) {
	  printf("[%s]find decoder failed!\n", __func__);
	  return;
  }
  
  context = avcodec_alloc_context3(codec);
  if (!context) {
	  printf("[%s]alloc context failed!\n", __func__);
	  return;
  }

  /* open it */
  if(avcodec_open2(context, codec, NULL) < 0) {
    fprintf(stderr, "could not open codec\n");
    return;
  }

  mEnable = true;
#if 0
  /* the codec gives us the frame size, in samples */
  
  Mp4mux_Init();
  Mp4mux_Open("/tmp/scv.mp4");
#endif
}

H264Decoder::~H264Decoder()
{
#if 0
  Mp4mux_Close();
#endif
  avcodec_close(context);
  av_free(context);
}

void H264Decoder::stop()
{
  bStop = 1;
}

int H264Decoder::decode(unsigned char *buffer, int size)
{
  int ret = -1;
  int len;
  int got_picture;
  AVFrame *pFrameRGB = NULL;
  uint8_t *out_bufferRGB = NULL;
  IplImage* pCVFrame = NULL; 

  picture = av_frame_alloc();
  if (!picture) {
      printf("alloc frame failed!\n");
      return -1;
  }

  av_init_packet(&avpkt);
  avpkt.size = size;
  avpkt.data = buffer;
  if (avpkt.size > 0) {
      printf("prepare to decode video, size:%d\n", avpkt.size);
	  len = avcodec_decode_video2(context, picture, &got_picture, &avpkt);
	  if(len < 0) {
		  fprintf(stderr, "Error while decoding frame\n");
		  av_init_packet(&avpkt);
          ret = -1;
          goto cleanup;
	  }
      
	  if(got_picture) {
		  /* the picture is allocated by the decoder. no need to free it */
		  printf("size:%d, width:%d, height:%d\n", picture->linesize[0], context->width, context->height);

          pFrameRGB = av_frame_alloc();
          if (!pFrameRGB) {
            printf("alloc rgb frame failed!\n");
            ret = -1;
            goto cleanup;
          }

          out_bufferRGB = new uint8_t[avpicture_get_size(PIX_FMT_BGR24, context->width, context->height)];
          if (!out_bufferRGB) {
              printf("alloc out_bufferRGB failed!\n");
              ret = -1;
              goto cleanup;
          }

          avpicture_fill((AVPicture *)pFrameRGB, out_bufferRGB, PIX_FMT_BGR24, context->width, context->height);
          swsContext = sws_getContext(context->width, context->height, context->pix_fmt, context->width, context->height, PIX_FMT_BGR24, SWS_BICUBIC, NULL, NULL, NULL); 
          if(swsContext == NULL)  
          {
              ret = -1;
              printf("swsContext is null!\n");
              goto cleanup;
          }
              
          sws_scale(swsContext, (const uint8_t* const*)picture->data, picture->linesize, 0, context->height, pFrameRGB->data, pFrameRGB->linesize);
          pCVFrame = cvCreateImage(cvSize(context->width, context->height),8,3); 
          memcpy(pCVFrame->imageData,out_bufferRGB,context->width * context->height * 24 / 8);
          pCVFrame->widthStep=context->width*3; //4096
          pCVFrame->origin=0;
          cvShowImage("decode",pCVFrame);//显示  
          cvWaitKey(20);
	  }

    av_init_packet(&avpkt);
    ret = 0;
  }

cleanup:
  if (pCVFrame) {
      cvReleaseImage(&pCVFrame);
  }

  if (picture){
      av_free(picture);
      picture = NULL;
  }

  if (out_bufferRGB) {
      delete[] out_bufferRGB;
  }

  if (pFrameRGB) {
      av_free(pFrameRGB);
  }

  return ret;
}
