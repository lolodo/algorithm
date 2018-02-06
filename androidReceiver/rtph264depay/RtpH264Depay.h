#ifndef __RTP_H264DEPAY_H__
#define __RTP_H264DEPAY_H__

#include <glib.h>

struct h264Buffer {
    unsigned char *buffer;
    unsigned long size;
}

class RtpH264Depay {
	public:
		RtpH264Depay();
		~RtpH264Depay();
		void *DepayProcess(void *buffer, unsigned int payload_len, int marker);

	private:
    fu_marker
        GQueue *stapQueue
		GQueue *fuQueue;
		GQueue *singleQueue;
        gboolean byte_stream;
        gboolean wait_start;
        gboolean mEnable;
        gboolean fu_marker;
        unsigned char current_fu_type;
        void finishFragmentation_unit();
};
#endif /* __RTP_H264DEPAY_H__*/
