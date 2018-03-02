#ifndef __RTP_H264DEPAY_H__
#define __RTP_H264DEPAY_H__

#include <stdbool.h>
#include "BufferQueue.h"

struct h264Buffer {
    unsigned char *buffer;
    unsigned long size;
    unsigned long start;
    unsigned long end;
};

class RTPH264Depay {
    public:
        RTPH264Depay();
        ~RTPH264Depay();
        void *DepayProcess(void *buffer, unsigned int payload_len, int marker);
        void setStreamMode(bool mode);
        bool getStreamMode();
        int getH264BufferNum();
        struct h264Buffer *getH264Buffer();
        bool getStatus();

    private:
        BufferQueue *fuQueue;
        BufferQueue *singleQueue;
        BufferQueue *outputQueue;
        bool byte_stream;
        bool wait_start;
        bool mEnable;
        bool fu_marker;
        unsigned char current_fu_type;
        int finishPackets(BufferQueue *queue);
        void sendQueue(BufferQueue *queue);
        void cleanQueue(BufferQueue *queue);
};
#endif /* __RTP_H264DEPAY_H__*/
