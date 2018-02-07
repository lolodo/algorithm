#ifndef __RTP_H264DEPAY_H__
#define __RTP_H264DEPAY_H__

#include <glib.h>

struct h264Buffer {
    unsigned char *buffer;
    unsigned long size;
    unsigned long start;
    unsigned long end;
};

class RtpH264Depay {
	public:
		RtpH264Depay();
		~RtpH264Depay();
		void *DepayProcess(void *buffer, unsigned int payload_len, int marker);
        void setStreamMode(gboolean mode);
        gboolean getStreamMode();
        int getH264BufferNum();
        struct h264Buffer *getH264Buffer();

	private:
        GQueue *stapQueue;
		GQueue *fuQueue;
		GQueue *singleQueue;
        GQueue *outputQueue;
        gboolean byte_stream;
        gboolean wait_start;
        gboolean mEnable;
        gboolean fu_marker;
        unsigned char current_fu_type;
        int finishPackets(GQueue *queue);
};
#endif /* __RTP_H264DEPAY_H__*/
