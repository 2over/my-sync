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
    // 在HotSpot中,在判断轻量级锁重入的时候，还额外地判断了下
    // 锁对象是否在线程栈里，有点复杂化了，这里额外增加了一个持有锁的线程属性
    // 比较方便地判断重入
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
