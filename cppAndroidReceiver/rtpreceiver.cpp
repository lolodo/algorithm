/*
   This IPv4 example uses the background thread itself to process all packets.
   You can use example one to send data to the session that's created in this
   example.
*/

#include <jrtplib3/rtpsession.h>
#include <jrtplib3/rtppacket.h>
#include <jrtplib3/rtpudpv4transmitter.h>
#include <jrtplib3/rtpipv4address.h>
#include <jrtplib3/rtpsessionparams.h>
#include <jrtplib3/rtperrors.h>
#include <jrtplib3/rtpsourcedata.h>
#include <stdlib.h>
#include <stdio.h>
#include <iostream>
#include <string>
#include "opencv2/opencv.hpp"
#include "RtpH264Depay.h"

using namespace jrtplib;

#define PACK_SIZE (1024 * 512)
#define JPEG_HEAD_LEN 8
#define BUFFER_SIZE (1024 * 2)

#ifdef RTP_SUPPORT_THREAD

//
// This function checks if there was a RTP error. If so, it displays an error
// message and exists.
//

void checkerror(int rtperr)
{
	if (rtperr < 0)
	{
		std::cout << "ERROR: " << RTPGetErrorString(rtperr) << std::endl;
		exit(-1);
	}
}

//
// The new class routine
//

class MyRTPSession : public RTPSession
{
    unsigned char *buffer;
    char *longbuf;
    unsigned bufpos;
    unsigned width;
    unsigned height;

    public: MyRTPSession();
    public: ~MyRTPSession();

protected:
	void OnPollThreadStep();
	void ProcessRTPPacket(const RTPSourceData &srcdat,const RTPPacket &rtppack);
};

void MyRTPSession::OnPollThreadStep()
{
	BeginDataAccess();
		
	// check incoming packets
	if (GotoFirstSourceWithData())
	{
		do
		{
			RTPPacket *pack;
			RTPSourceData *srcdat;
			
			srcdat = GetCurrentSourceInfo();
			
			while ((pack = GetNextPacket()) != NULL)
			{
				ProcessRTPPacket(*srcdat,*pack);
				DeletePacket(pack);
			}
		} while (GotoNextSourceWithData());
	}
		
	EndDataAccess();
}

MyRTPSession::MyRTPSession()
{
    bufpos = 0;
    width = 0;
    height = 0;
    bufpos = 0;
    longbuf = new char[PACK_SIZE];
    if (longbuf == NULL) {
        std::cout << "Alloc packet buffer failed!" << std::endl;
    }
}


MyRTPSession::~MyRTPSession()
{
    bufpos = 0;
    if (longbuf) {
        free(longbuf);
    }
}

void MyRTPSession::ProcessRTPPacket(const RTPSourceData &srcdat,const RTPPacket &rtppack)
{
    unsigned int offset = 0;
    unsigned int head = 0;
    unsigned char jpegtype = 0;
    unsigned int payloadlen;
    RtpH264Depay h264info;

	// You can inspect the packet and the source's info here
	std::cout << "Got packet " << rtppack.GetExtendedSequenceNumber() << " from SSRC " << srcdat.GetSSRC() << " payload len:"<< rtppack.GetPayloadLength() << " entire packet len:" << rtppack.GetPacketLength() << std::endl;
    buffer = rtppack.GetPayloadData();
    payloadlen = rtppack.GetPayloadLength();

    h264info.DepayProcess(buffer, payloadlen, rtppack.HasMarker());
#if 0
    offset = *(unsigned *)buffer;
    offset = ntohl(offset);
    offset = offset & 0x00ffffff;
    //std::cout << std::hex << "offset is:" << offset << std::endl;
//    offset = ntohl(*(unsigned int *)buffer) & 0x00ffffff;
 
    /* Jpeg */
    if (rtppack.GetPayloadType() != 26) {
        std::cout << "payload type is :" << rtppack.GetPayloadType() << std::endl;
        return;
    }

    head += JPEG_HEAD_LEN;
    payloadlen = rtppack.GetPayloadLength() - JPEG_HEAD_LEN;
    jpegtype = *(buffer + 4);
    if ((jpegtype >= 0x40) && (jpegtype < 0x80 )) {
        std::cout << "jpeg type is :" << jpegtype << std::endl;
        payloadlen -= 4;
        head += 4;
    }
    
    if (offset == 0) {
        //Initialize JPEG header
        
        head += 132;
        width = *(buffer + 6) * 8;
        height = *(buffer + 7) * 8;
        payloadlen = rtppack.GetPayloadLength() - JPEG_HEAD_LEN;
        std::cout << "offset is 0 ====bufpos:" << bufpos << " width:" << width << " height:" << height << std::endl;
        memcpy(longbuf + offset, rtppack.GetPayloadData() + JPEG_HEAD_LEN + 132, rtppack.GetPayloadLength() - JPEG_HEAD_LEN - 132);
    } else {
        memcpy(longbuf + offset, rtppack.GetPayloadData() + JPEG_HEAD_LEN, rtppack.GetPayloadLength() - JPEG_HEAD_LEN);
    }
   
    bufpos += rtppack.GetPayloadLength() - JPEG_HEAD_LEN;
    if (rtppack.HasMarker()) {
        std::cout << "show img bufpos:" << bufpos << std::endl;
        cv::Mat rawdata = cv::Mat(1, bufpos, CV_8UC1, longbuf);
        cv::Mat frame = cv::imdecode(rawdata, CV_LOAD_IMAGE_COLOR);
        if (frame.size().width == 0) {
            std::cerr << "decode failed!" << std::endl;
            bufpos = 0;
            return ;
        }
        
        cv::imshow("recv", frame);
        bufpos = 0;
    }
#endif
}

//
// The main routine
// 

int main(void)
{
#ifdef RTP_SOCKETTYPE_WINSOCK
	WSADATA dat;
	WSAStartup(MAKEWORD(2,2),&dat);
#endif // RTP_SOCKETTYPE_WINSOCK
	
	MyRTPSession sess;
	uint16_t portbase;
	std::string ipstr;
	int status,num;

        // First, we'll ask for the necessary information
		
	std::cout << "Enter local portbase:" << std::endl;
	std::cin >> portbase;
	std::cout << std::endl;
	
	std::cout << std::endl;
	std::cout << "Number of seconds you wish to wait:" << std::endl;
	std::cin >> num;
	
	// Now, we'll create a RTP session, set the destination
	// and poll for incoming data.
	
	RTPUDPv4TransmissionParams transparams;
	RTPSessionParams sessparams;
	
	// IMPORTANT: The local timestamp unit MUST be set, otherwise
	//            RTCP Sender Report info will be calculated wrong
	// In this case, we'll be just use 8000 samples per second.
	sessparams.SetOwnTimestampUnit(1.0/8000.0);		
	
	transparams.SetPortbase(portbase);
	status = sess.Create(sessparams,&transparams);	
	checkerror(status);
	
	// Wait a number of seconds
	RTPTime::Wait(RTPTime(num,0));
	
	sess.BYEDestroy(RTPTime(10,0),0,0);

#ifdef RTP_SOCKETTYPE_WINSOCK
	WSACleanup();
#endif // RTP_SOCKETTYPE_WINSOCK
	return 0;
}

#else

int main(void)
{
	std::cerr << "Thread support is required for this example" << std::endl;
	return 0;
}

#endif // RTP_SUPPORT_THREAD

