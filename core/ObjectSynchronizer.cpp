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

    if (obj->mark()->is_neutral()) {
        INFO_PRINT("[%s] 当前是无锁，不需要解锁\n", t->name());

        return;
    }

    if (obj->mark()->has_bias_pattern()) {
        INFO_PRINT("[%s] 当前是偏向锁,不需要解锁\n", t->name());

        return;
    }

    markOop dhw = lock->displaced_header();
    markOop mark;

    if (NULL == dhw) {
        mark = obj->mark();

        // 处理重入的，逻辑未写

        // 偏向锁膨胀成重量级锁也会走到这里来
        if (mark->has_monitor()) {
            ObjectMonitor *m = mark->monitor();
            INFO_PRINT("[%s] 重量级锁开始解锁 dhw = NULL\n", t->name());
        }

        return;
    }

    INFO_PRINT("[%s] 开始解锁\n", t->name());

    mark = obj->mark();

    /**
     * 轻量级锁解锁有两种请卡u嗯:
     * 1. t1之前是偏向锁，后面由t2膨胀成了轻量级锁,t1 先进入解锁逻辑
     * 2.当前持有轻量级锁的线程来释放锁
     */

    if (mark == (markOop) lock) {
        if (lock->owner() == t) {
            if ((markOop) Atomic::cmpxchg_ptr(dhw, obj->mark_addr(), mark) == mark) {
                INFO_PRINT("[%s] 轻量级锁解锁成功\n", t->name());

                return;
            }
        } else {
            INFO_PRINT("[%s] 非持有轻量级锁的线程解锁, 直接返回。持有锁的线程=%s\n", t->name(), lock->owner()->name());
            return;
        }
    }


    if (obj->mark()->is_neutral()) {
        INFO_PRINT("[%s] 当前是无锁, 不需要解锁\n", t->name());
        return;
    }

    // 重量级解锁
    ObjectSynchronizer::inflate(obj, t)->exit(t);

}

/**
 * 进入这个函数的情况:
 * 1.偏向锁未开启
 * 2.延迟偏向期间执行加锁
 * 3.重入
 */
void ObjectSynchronizer::slow_enter(InstanceOopDesc *obj, BasicLock* lock, Thread* t) {
    markOop mark = obj->mark();
    assert(!mark->has_bias_pattern(), "should not see bias pattern here");
    if (mark->is_neutral()) {
        lock->set_displaced_header(mark);

        // 这里其实应该对之前持有偏向锁的线程，应该做处理，让其进行阻塞
        // 但是当前项目中没有做处理，而是偏向锁线程继续执行
        if (mark == Atomic::cmpxchg_ptr(lock, obj->mark_addr(), mark)) {
            INFO_PRINT("[%s] 轻量级锁抢锁成功\n", t->name());

            // 这个HotSpot源码中没有, 为了判断重入加的
            lock->set_owner(t);

            return ;
        }
    } else {
        // 轻量级锁 且 持有轻量级锁的线程是当前线程
        if (mark && mark->has_locker() && t == mark->locker()->owner()) {
            assert(lock != mark->locker(), "must not re-lock the same lock");
            assert(lock != (BasicLock*) obj->mark(), "don't relock with same BasicLock");

            // 为什么要将BasicLock._displaced_header置为NULL
            lock->set_displaced_header(NULL);
            return ;
        }
     }

    INFO_PRINT("[%s] 轻量级锁抢锁失败\n", t->name());

    // HotSpot中的这句代码，没太明白意图
    lock->set_displaced_header(MarkOopDesc::unused_mark());

    ObjectSynchronizer::inflate(obj, t)->enter(t);
}
void ObjectSynchronizer::slow_exit(InstanceOopDesc *obj, BasicLock *lock, Thread *t) {
    fast_exit(obj, lock, t);
}

ObjectMonitor *ObjectSynchronizer::inflate(InstanceOopDesc *obj, Thread *t) {

    for (;;) {
        markOop mark = obj->mark();

        INFO_PRINT("[%s] 开始膨胀成重量级锁\n", t->name());

        // 已经是重量级锁
        if (mark->has_monitor()) {
            INFO_PRINT("[%s] 检测到已经是重量级锁\n", t->name());

            return mark->monitor();
        }


        /**
         * 正在膨胀
         * 问题点: 如果其他线程执行到这里发现正在膨胀，就会一直走这里
         */
        if (mark == MarkOopDesc::INFLATING()) {
            INFO_PRINT("[%s] 检测到正在膨胀成重量级锁 %d\n", t->name(), mark->has_monitor());

            // HotSpot 源码的做法是阻塞等待唤醒

            sleep(1);

            continue;
        }

        // 轻量级锁
        if (mark->has_locker()) {
            ObjectMonitor *m = new ObjectMonitor;

            // 存在问题点
            markOop cmp = (markOop) Atomic::cmpxchg_ptr(MarkOopDesc::INFLATING(), obj->mark_addr(), mark);
            if (cmp != mark) {
                // 不相等表示有其他线程过在膨胀,retry,进入正在在膨胀逻辑, yield
                INFO_PRINT("[%s] 更正状态为正在膨胀成重量级锁失败\n", t->name());

                continue; // Interference -- just retry
            }

            INFO_PRINT("[%s] 更正状态为正在膨胀成重量级锁\n", t->name());

            // 设置成正在膨胀之后
            markOop dmw = mark->displaced_mark_helper();

            m->set_header(dmw);

            // HotSpot源码在此处设置了owner,重量级锁CAS是怎么成功的？
            // m->set_owner(reinterpret_cast<Thread*>(mark->locker()));;

            m->set_object(obj);

            // 将monitor写入Mark
            mark = MarkOopDesc::INFLATING();

            if (mark == Atomic::cmpxchg_ptr(MarkOopDesc::encode(m), obj->mark_addr(), mark)) {
                INFO_PRINT("[%s] 膨胀成重量级锁成功 %X\n", t->name(), mark->has_monitor());
            } else {
                INFO_PRINT("[%s] 膨胀成重量级锁失败\n", t->name());
            }

            return m;
        }

        /**
         * 偏向锁直接膨胀成重量级锁
         *
         * 区别轻量级锁膨胀成重量级锁
         */

        assert(mark->is_neutral(), "invariant");

        ObjectMonitor *m = new ObjectMonitor;
        m->set_header(mark);
        m->set_object(obj);
        m->set_owner(t);

        if (mark == Atomic::cmpxchg_ptr(MarkOopDesc::encode(m), obj->mark_addr(), mark)) {
            INFO_PRINT("[%s] 偏向锁膨胀成重量级锁成功 %X\n", t->name(), obj->mark()->has_monitor());
        } else {
            INFO_PRINT("[%s] 偏向锁膨胀成重量级锁失败\n", t->name());
        }

        return m;
    }
}

void ObjectSynchronizer::wait(InstanceOopDesc *obj, Thread *t) {
    if (UseBiasedLocking) {
        INFO_PRINT("[%s] wait  撤销偏向锁\n", t->name());
        BiasedLocking::revoke_and_rebias(obj, false, t);
        assert(!obj->mark()->has_bias_pattern(), "biases should be revoked by now");
    }

    ObjectMonitor *monitor = ObjectSynchronizer::inflate(obj, t);

    monitor->wait(t);
}

void ObjectSynchronizer::notify(InstanceOopDesc *obj, Thread *t) {
    if (UseBiasedLocking) {
        INFO_PRINT("[%s] wait 撤销偏向锁\n", t->name());

        BiasedLocking::revoke_and_rebias(obj, false, t);
        assert(!obj->mark()->has_bias_pattern(), "biases should be revoked by now");
    }

    ObjectMonitor *monitor = ObjectSynchronizer::inflate(obj, t);

    monitor->notify();

}



