//
// Created by xiehao on 2024/10/28.
//

#include "BiasedLocking.h"
#include "../include/common.h"
#include "../oops/InstanceOopDesc.h"
#include "Atomic.h"
#include "Thread.h"

#include "../oops/Klass.h"

BiasedLocking::Condition BiasedLocking::revoke_and_rebias(InstanceOopDesc *obj, bool attempt_rebias, Thread *t) {
    markOop mark = obj->mark();

    if (mark->is_biased_anonymously() & !attempt_rebias) {
        markOop biased_value = mark;
        markOop unbiased_prototype = MarkOopDesc::prototype()->set_age(mark->age());

        markOop res_mark = (markOop) Atomic::cmpxchg_ptr(unbiased_prototype, obj->mark_addr(), mark);
        if (res_mark == biased_value) {
            return BIAS_REVOKED;
        }
    } else if (mark->has_bias_pattern()) {
        Klass *klass = obj->klass();
        markOop prototype_header = klass->prototype_header();

        INFO_PRINT("[%s] mark.epoch=%d, prototype_header.epoch=%d\n", t->name(), mark->bias_epoch(),
                   prototype_header->bias_epoch());

        if (!prototype_header->has_bias_pattern()) {
            markOop biased_value = mark;
            markOop res_mark = (markOop) Atomic::cmpxchg_ptr(prototype_header, obj->mark_addr(), mark);
            assert(!(*(obj->mark_addr()))->has_bias_pattern(), "even if we raced, should still be revoked");
            return BIAS_REVOKED;
        } else if (prototype_header->bias_epoch() != mark->bias_epoch()) {
            if (attempt_rebias) {
                markOop biased_value = mark;
                markOop rebiased_prototype = MarkOopDesc::encode(t, mark->age(), biased_value->get_epoch());
                markOop res_mark = (markOop) Atomic::cmpxchg_ptr(rebiased_prototype, obj->mark_addr(), mark);
                if (res_mark == biased_value) {
                    INFO_PRINT("[%s] 重新拿到偏向锁\n", t->name());

                    return BIAS_REVOKED_AND_REBIASED;
                }
            } else {
                markOop biased_value = mark;
                markOop unbiased_prototype = MarkOopDesc::prototype()->set_age(mark->age());
                markOop res_mark = (markOop) Atomic::cmpxchg_ptr(unbiased_prototype, obj->mark_addr(), mark);
                if (res_mark == biased_value) {
                    return BIAS_REVOKED;
                }
            }
        }


        /**
         * 走到这里有两种情况:
         * 1.已经有线程持有了偏向锁,且klass.epoch == oop.epoch 需要膨胀成轻量级锁
         * 2.第一次获取到偏向锁
         */

        /**
         * 1.已经有线程持有了偏向锁, 恢复到无锁.进入slow_enter逻辑
         * 这边HotSpot是用VMThread实现的,如果不通过其他线程实现，后面抢轻量级锁都能抢到，因为这里也是一次CAS
         * 相当于把多线程变成了有序执行
         */

        if (NULL != obj->mark()->biased_locker()) {
            markOop biased_value = obj->mark();
            markOop unbiased_prototype = MarkOopDesc::prototype()->set_age(mark->age());
            markOop res_mark = (markOop) Atomic::cmpxchg_ptr(unbiased_prototype, obj->mark_addr(), biased_value);
            if (res_mark == biased_value) {
                INFO_PRINT("[%s] 将偏向锁恢复成无锁\n", t->name());
                return BIAS_REVOKED;
            }
        }

        /**
         * 2.第一次获取到偏向锁是走这里完成的
         */
        if (mark->is_biased_anonymously()) {
            markOop biased_value = mark;
            markOop rebiased_prototype = MarkOopDesc::encode(t, mark->age(), 0);
            markOop res_mark = (markOop) Atomic::cmpxchg_ptr(rebiased_prototype, obj->mark_addr(), mark);
            if (res_mark == biased_value) {
                INFO_PRINT("[%s] 第一次拿到偏向锁 %X\n", t->name(), obj->mark()->biased_locker());

                return BIAS_REVOKED_AND_REBIASED;
            }
        }

        INFO_PRINT("[%s] 获取或撤销偏向锁失败\n", t->name());
        return NOT_BIASED;
    }
}