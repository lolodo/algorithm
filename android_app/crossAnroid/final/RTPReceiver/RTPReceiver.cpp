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
#include "RTPH264Depay.h"

using namespace jrtplib;

#define PACK_SIZE (1024 * 512)
#define JPEG_HEAD_LEN 8
#define BUFFER_SIZE (1024 * 2)

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

    private:
            RTPH264Depay h264info;

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

    std::cout << "SSR:" << srcdat.GetSSRC() << std::endl;
    if (!h264info.getStatus()) {
        std::cout << "get status failed!" << std::endl;
        return;
    }

    // You can inspect the packet and the source's info here
    buffer = rtppack.GetPayloadData();
    payloadlen = rtppack.GetPayloadLength();
    h264info.DepayProcess(buffer, payloadlen, rtppack.HasMarker());
}

//
// The main routine
//

int main(void)
{
    MyRTPSession sess;
    std::string ipstr;
    int status;
    int num;
    uint16_t portbase;

    // First, we'll ask for the necessary information
#ifdef ENABLE_INPUT
    std::cout << "Enter local portbase:" << std::endl;
    std::cin >> portbase;
    std::cout << std::endl;

    std::cout << std::endl;
    std::cout << "Number of seconds you wish to wait:" << std::endl;
    std::cin >> num;
#else
    portbase = 8888;
    num = 100000;
#endif

    // Now, we'll create a RTP session, set the destination
    // and poll for incoming data.
    RTPUDPv4TransmissionParams transparams;
    RTPSessionParams sessparams;

    // IMPORTANT: The local timestamp unit MUST be set, otherwise
    // RTCP Sender Report info will be calculated wrong
    // In this case, we'll be just use 8000 samples per second.
    sessparams.SetOwnTimestampUnit(1.0/8000.0);

    transparams.SetPortbase(portbase);
    status = sess.Create(sessparams,&transparams);
    checkerror(status);

    // Wait a number of seconds
    RTPTime::Wait(RTPTime(num,0));

    sess.BYEDestroy(RTPTime(10,0),0,0);
    return 0;
}
