//
// Created by xiehao on 2024/10/24.
//

#include "InstanceOopDesc.h"
#include "MarkOopDesc.h"

InstanceOopDesc::InstanceOopDesc(Klass *klass) {
    _klass = klass;
    _mark = MarkOopDesc::biased_locking_prototype();
}
