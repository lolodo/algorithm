#ifndef __RTP_H264DEPAY_H__
#define __RTP_H264DEPAY_H__

#include <glib.h>

class RtpH264Depay {
	public:
		RtpH264Depay();
		~RtpH264Depay();
		void *DepayProcess(void *buffer, unsigned int payload_len, int marker);

	private:
		GQueue *queue;
};
#endif /* __RTP_H264DEPAY_H__*/
