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

#ifndef AVCODEC_AVCODEC_H
#define AVCODEC_AVCODEC_H

#include "libavcodec/avcodec.h"
#include "libavutil/mathematics.h"
#include "libavformat/avformat.h"

class H264Decoder {
	public:
		H264Decoder();
		~H264Decoder();
		int decode();
		void stop();

	private:
		AVCodec *codec;
		AVCodecContext *context;
		AVFrame *frame;
		AVPacket avpkt;
		int bStop;
}

#endif
