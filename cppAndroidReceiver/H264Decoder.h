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

#ifndef H264DECODER_H
#define H264DECODER_H

extern "C" {
#include "libavcodec/avcodec.h"
#include "libavutil/mathematics.h"
#include "libavformat/avformat.h"
#include <stdbool.h>
}

class H264Decoder {
	public:
		H264Decoder();
		~H264Decoder();
        int decode(unsigned char *buffer, int size);
		void stop();
        bool getStatus();

	private:
		AVCodec *codec;
		AVFrame *frame;
        AVCodecContext *context;
        AVFrame *picture;
        AVPacket avpkt;
		int bStop;
        bool mEnable;
};

typedef int (*h264DecodeFunc)(unsigned char *buffer, int size);
#endif
