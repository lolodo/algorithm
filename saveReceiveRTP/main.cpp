/*************************************************************************
    > File Name: main.cpp
    > Author: Alex
    > Mail: fangyuan.liu@nio.com
    > Created Time: Mon 19 Mar 2018 08:44:20 PM CST
 ************************************************************************/

#include <iostream>
#include <string>
#include <fcntl.h>
#include <unistd.h>
#include "H264Decoder.h"
#include "PracticalSocket.h"
using namespace std;

#define BUF_LEN (1024 * 512)// Larger than maximum UDP packet size

int main()
{
    uint16_t port;
    int recvMsgSize;
    int flag = 0;
    int counter = 0;
    unsigned char *buffer; // Buffer for echo string
    string sourceAddress;
    H264Decoder decoder;
    int savefd = -1;
    int ret = -1;

    std::cout << "Enter local portbase:" << std::endl;
    std::cin >> port; 
    std::cout << std::endl;

    buffer = new unsigned char[BUF_LEN];
    if (!decoder.getStatus()) {
        cerr << "init decoder failed!" << endl;
        return -1;
    }

    savefd = open("video.data", O_RDWR | O_CREAT, 0644);
    if (savefd < 0) {
        cerr << "create video failed!" << endl;
        return -1;
    }

    UDPSocket sock(port);
    while (1) {
        try {
            recvMsgSize = sock.recvFrom(buffer, BUF_LEN, sourceAddress, port);
            if (recvMsgSize < 0) {
                cerr << "recv failed!" << endl;
                continue;
            }
        
            cout << "length: " << recvMsgSize << ", received packet from " << sourceAddress << ":" << port << endl;
            if (flag == 1) {
                cout << "decode the " << counter << " buffer!" << endl;
                decoder.decode(buffer, recvMsgSize);
                counter++;
            } else if ((recvMsgSize >= 5) && ((buffer[4] == 0x67) || (buffer[4] == 0x68))) { //sps or pps
                flag = 1;
                cout << "decode the first!" << endl;
                decoder.decode(buffer, recvMsgSize);
                counter++;
            } else {
                continue;
            }

            ret = write(savefd, buffer, recvMsgSize);
            if (ret < 0) {
                cerr << "write failed!" << endl;
            }
        } catch (SocketException & e){
            cerr<< "error happend!" << endl;
            cerr << e.what() << endl;
            delete [] buffer;
            exit(1);
        }
    }

    delete [] buffer;
    return 0;
}
