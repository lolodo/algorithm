/*************************************************************************
    > File Name: BufferQueue.h
    > Author: Alex
    > Mail: fangyuan.liu@nio.com 
    > Created Time: Tue 27 Feb 2018 05:49:32 PM CST
 ************************************************************************/
#ifndef __BUFFER_QUEUE_H__
#define __BUFFER_QUEUE_H__
class Node {
    public:
        unsigned char *data;
        unsigned int len;
        Node *next;
};

class BufferQueue {
    public:
        BufferQueue();
        ~BufferQueue();
        void pushTail(unsigned char *buffer, int size);
        int isEmpty();
        Node *popHead();

    private:
        Node *head;
        Node *tail;
};
#endif //__BUFFER_QUEUE_H__
