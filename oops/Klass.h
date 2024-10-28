//
// Created by xiehao on 2024/10/24.
//

#ifndef MY_SYNC_KLASS_H
#define MY_SYNC_KLASS_H
#include "../include/common.h"
#include "MarkOopDesc.h"

class Klass {
private:
    markOop _prototype_header; // Used when biased locking is both enabled and disabled for this type
public:
    Klass() {

        _prototype_header = MarkOopDesc::biased_locking_prototype(); // 101
    }

public:
    markOop prototype_header() const {
        return _prototype_header;
    }

    inline void set_prototype_header(markOop header) {
        _prototype_header = header;
    }
};


#endif //MY_SYNC_KLASS_H
