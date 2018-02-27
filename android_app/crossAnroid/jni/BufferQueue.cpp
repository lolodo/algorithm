/*************************************************************************
    > File Name: BufferQueue.cpp
    > Author: Alex
    > Mail: fangyuan.liu@nio.com 
    > Created Time: Tue 27 Feb 2018 05:49:26 PM CST
 ************************************************************************/

#include <iostream>
#include "BufferQueue.h"

BufferQueue::BufferQueue():head(NULL), tail(NULL)
{

}

BufferQueue::~BufferQueue()
{
    Node *node;

    while(!isEmpty()) {
        node = popHead();
        if (node->data) {
            delete [] node->data;
            node->data = NULL;
        }

        delete node;
    }
}

void BufferQueue::pushTail(unsigned char *buffer, int size)
{
    Node *node;

    node = new Node;
    node->data = buffer;
    node->len = size;
    node->next = NULL;

    if (tail) {
        tail->next = node;
    } else {
        head = node;
    }
        
    tail = node;
}

int BufferQueue::isEmpty()
{
    return ((head == tail) && (head == NULL));
}

Node *BufferQueue::popHead()
{
    Node *node;

    if (head) {
        node = head;
        head = head->next;
        if (!head) {
            tail = NULL;
        }

        return node;
    } else {
        tail = NULL;
        return NULL;
    }
}
