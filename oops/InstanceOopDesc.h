//
// Created by xiehao on 2024/10/24.
//

#ifndef MY_SYNC_INSTANCEOOPDESC_H
#define MY_SYNC_INSTANCEOOPDESC_H
#include "MarkOopDesc.h"

class MarkOopDesc;
class Klass;

class InstanceOopDesc {

private:
    markOop     _mark;
    Klass*      _klass;

public:
    InstanceOopDesc(Klass* klass);

    markOop mark() {
        return _mark;
    }

    markOop* mark_addr() const {
        return (markOop*) &_mark;
    }

    void set_mark(volatile markOop m) {
        _mark = m;
    }

    Klass* klass() {
        return _klass;
    }
};


#endif //MY_SYNC_INSTANCEOOPDESC_H
