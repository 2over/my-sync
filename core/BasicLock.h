//
// Created by xiehao on 2024/10/28.
//

#ifndef MY_SYNC_BASICLOCK_H
#define MY_SYNC_BASICLOCK_H
#include "../include/common.h"

class MarkOopDesc;

class Thread;

class BasicLock {
private:
    markOop     _displaced_header;
    Thread*     _owner;

public:
    markOop displaced_header() const {
        return _displaced_header;
    }

    void set_displaced_header(markOop header) {
        _displaced_header = header;
    }

    Thread* owner() {
        return _owner;
    }

    void set_owner(Thread* owner) {
        _owner = owner;
    }

};


#endif //MY_SYNC_BASICLOCK_H
