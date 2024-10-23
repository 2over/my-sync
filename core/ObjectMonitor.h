//
// Created by xiehao on 2024/10/22.
//

#ifndef MY_SYNC_OBJECTMONITOR_H
#define MY_SYNC_OBJECTMONITOR_H

#include "../include/common.h"

class Thread;
class ObjectWaiter;

class ObjectMonitor {
private:
    volatile Thread*        _owner;
    volatile int            _recursions;

    ObjectWaiter*           _cxq;

protected:
    volatile intptr_t       _waiters;   // number of waiting threads
protected:
    ObjectWaiter* volatile  _WaitSet;   // LL of threads wait() ing on the monitor

private:
    volatile int            _WaitSetLock;   // protects Wait Queue - simple spinlock
private:
    volatile markOop        _header;
    void* volatile          _object;

public:
    ObjectMonitor() {
        _owner = NULL;
    }

    void set_owner(Thread *t) {
        _owner = t;
    }

public:
    void enter(Thread *t);

    void exit(Thread *t);

    int tryLock(Thread *t);

    void print_cxq();

    void wait(Thread *t);

    void notify();

    void AddWaiter(ObjectWaiter *node);

    ObjectWaiter *DequeueWaiter();

    void DequeueSpecificWaiter(ObjectWaiter *node);

    markOop header() const {
        return _header;
    }

    inline void set_header(markOop hdr) {
        _header = hdr;
    }

    inline void *object() const {
        return _object;
    }

    inline void *object_addr() {
        return (void *) (&_object);
    }

    inline void set_object(void *obj) {
        _object = obj;
    }

};


#endif //MY_SYNC_OBJECTMONITOR_H
