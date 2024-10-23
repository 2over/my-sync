//
// Created by xiehao on 2024/10/22.
//

#ifndef MY_SYNC_OBJECTWAITER_H
#define MY_SYNC_OBJECTWAITER_H

#include "../include/common.h"

class Thread;

class ObjectWaiter {
public:
    Thread *_thread;

    ObjectWaiter *_prev;
    ObjectWaiter *_next;

public:
    ObjectWaiter(Thread *t) {
        _thread = t;

        _prev = NULL;
        _next = NULL;
    }

    void print();
};


#endif //MY_SYNC_OBJECTWAITER_H
