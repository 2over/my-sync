//
// Created by xiehao on 2024/10/23.
//

#include "ObjectSynchronizer.h"
#include "../include/common.h"
#include "../oops/InstanceOopDesc.h"
#include "BasicLock.h"
#include "Atomic.h"
#include "Thread.h"
#include "ObjectMonitor.h"
#include "BiasedLocking.h"

void ObjectSynchronizer::fast_enter(InstanceOopDesc *obj, BasicLock *lock, Thread *t) {
    if (UseBiasedLocking) {
        BiasedLocking::Condition cond = BiasedLocking::revoke_and_rebias(obj, true, t);

        if (cond == BiasedLocking::BIAS_REVOKED_AND_REBIASED) {
            return;
        }
    }

    slow_enter(obj, lock, t);
}

void ObjectSynchronizer::fast_exit(InstanceOopDesc *obj, BasicLock *lock, Thread *t) {
    // 仿照HotSpot这样做会abort,为什么jvm中最终没看到报错
    // assert(!obj->mark()->has_bias_pattern(), "should not see bias pattern here");

    if (obj->mark() -> is_neutral()) {
        INFO_PRINT("[%s] 当前是无锁，不需要解锁\n", t->name());

        return ;
    }

    if (obj->mark() ->has_bias_pattern()) {
        INFO_PRINT("[%s] 当前是偏向锁,不需要解锁\n", t->name());

        return ;
    }

    markOop dhw = lock->displaced_header();
    markOop mark;

    if (NULL == dhw) {
        mark = obj->mark();

        // 处理冲入的，逻辑未写

        // 偏向锁膨胀成重量级锁也会走到这里来
        if (mark ->has_monitor()) {
            ObjectMonitor * m = mark->monitor();
            INFO_PRINT("[%s] 重量级锁开始解锁 dhw = NULL\n", t->name());
        }

        return ;
    }
}




