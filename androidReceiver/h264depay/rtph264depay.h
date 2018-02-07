#ifndef __RTP_H264DEPAY_H__
#define __RTP_H264DEPAY_H__

#ifdef __cplusplus
extern "C" {
#endif
struct h264Buffer {
    unsigned char *buffer;
    unsigned long size;
};

void *DepayProcess(void *buffer, unsigned int payload_len, int marker);

#ifdef __cplusplus
}
#endif

#endif /* __RTP_H264DEPAY_H__*/
