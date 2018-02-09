/*
 *   C++ UDP socket server for live image upstreaming
 *   Modified from http://cs.ecs.baylor.edu/~donahoo/practical/CSockets/practical/UDPEchoServer.cpp
 *   Copyright (C) 2015
 *
 *   This program is free software; you can redistribute it and/or modify
 *   it under the terms of the GNU General Public License as published by
 *   the Free Software Foundation; either version 2 of the License, or
 *   (at your option) any later version.
 *
 *   This program is distributed in the hope that it will be useful,
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *   GNU General Public License for more details.
 *
 *   You should have received a copy of the GNU General Public License
 *   along with this program; if not, write to the Free Software
 *   Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
 */

#include <stdio.h>          // For cout and cerr
#include "PracticalSocket.h" // For UDPSocket and SocketException
#include <iostream>          // For cout and cerr
#include <cstdlib>           // For atoi()
#include <fcntl.h>           // For atoi()
#include <stdlib.h>           // For atoi()
#include <unistd.h>           // For atoi()

#define BUF_LEN 65540 // Larger than maximum UDP packet size
#define PACKET_NUM 2194 

#include "opencv2/opencv.hpp"
using namespace cv;
#include "config.h"

int main(int argc, char * argv[]) {
    int image_fd = -1;
    int count = 0;

    if (argc != 2) { // Test for correct number of parameters
        cerr << "Usage: " << argv[0] << " <Server Port>" << endl;
        exit(1);
    }

    unsigned short servPort = atoi(argv[1]); // First arg:  local port

    image_fd = open("video1", O_RDWR | O_CREAT, 0777);
    if (image_fd < 0) {
        cout << "image fd is error:" << image_fd << endl;
        return image_fd;
    }

    try {
        UDPSocket sock(servPort);

        char buffer[BUF_LEN]; // Buffer for echo string
        int recvMsgSize; // Size of received message
        string sourceAddress; // Address of datagram source
        unsigned short sourcePort; // Port of datagram source
        while(1) {
            recvMsgSize = sock.recvFrom(buffer, BUF_LEN, sourceAddress, sourcePort);
            if (recvMsgSize < 0) {
                cerr << "recv failed!" << endl;
                continue;
            }

            if (!(count % 10000)) {
                cerr << "count is " << count << endl;
            }

            printf("msgsize is %-6d ", recvMsgSize);
            printf("0x%08x ", *(unsigned *)(buffer));
            printf("0x%08x ", *(unsigned *)(buffer + 4));
            printf("0x%08x ", *(unsigned *)(buffer + 8));
            printf("0x%08x ", *(unsigned *)(buffer + 12));

            printf("end:");

            printf("0x%08x ", *(unsigned *)(buffer + recvMsgSize -12));
            printf("0x%08x ", *(unsigned *)(buffer + recvMsgSize - 8));
            printf("0x%08x ", *(unsigned *)(buffer + recvMsgSize - 4));
            printf("0x%08x\n", *(unsigned *)(buffer + recvMsgSize - 0));
          //  lseek(image_fd, 0, SEEK_SET);
           // write(image_fd, buffer, recvMsgSize);
            count++;
        }
    } catch (SocketException & e) {
        close(image_fd);
        cerr << e.what() << endl;
        exit(1);
    }
    
    close(image_fd);
    return 0;
}
