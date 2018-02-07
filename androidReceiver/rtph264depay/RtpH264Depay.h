#ifndef __RTP_H264DEPAY_H__
#define __RTP_H264DEPAY_H__

#include "queue.h"

struct h264Buffer {
    unsigned char *buffer;
    unsigned long size;
};

class RtpH264Depay {
	public:
		RtpH264Depay();
		~RtpH264Depay();
		void *DepayProcess(void *buffer, unsigned int payload_len, int marker);
        void setStreamMode(gboolean mode);
        gboolean getStreamMode();

	private:
        GQueue *stapQueue;
		GQueue *fuQueue;
		GQueue *singleQueue;
        gboolean byte_stream;
        gboolean wait_start;
        gboolean mEnable;
        gboolean fu_marker;
        unsigned char current_fu_type;
        void finishFragmentation_unit();
        int sendStapBuffer();
        int sendSingleBuffer();
};
#endif /* __RTP_H264DEPAY_H__*/
