//
// Created by xiehao on 2024/10/22.
//

#include "Thread.h"
#include "Monitor.h"
#include "ObjectMonitor.h"
#include "ParkEvent.h"

extern ObjectMonitor* monitor;

ParkEvent* Thread::_MainThreadEvent = new ParkEvent;

void* thread_run(void* arg) {
    Thread* t = static_cast<Thread *>(arg);

    INFO_PRINT("[%s] 等待创建线程工作完成\n", t->name());

    Monitor* sync = t->startThread_lock();

    sync->wait(t);

    INFO_PRINT("[%s] 创建工作已完成，线程被唤醒\n", t->name());

    // 执行业务逻辑
    t->entry_point()(t);
}

Thread::Thread(thread_fun entry_point, void *args, char *name) {
    _entry_point = entry_point;
    _args = args;
    _name = name;


    set_state(ALLOCATED);

    _startThread_lock = new Monitor;

    _MutexEvent = new ParkEvent(this);
    _ParkEvent = new ParkEvent(this);

    int status = pthread_create(_tid, NULL, thread_run, this);
    ASSERT_ZERO(status, "create thread fail\n");

    // 调用就变成了同步
    // pthread_join(tid(), NULL);
}

/**
 * 会出现的情况:
 * 1.run比线程函数先执行(因为OS创建线程需要一定的时间)
 * 解决办法: 借助线程状态
 */
void Thread::run() {
    assert(NULL != _entry_point, "entry point is null");
    while (true) {
        if (INITIALIZED == get_state()) {
            INFO_PRINT("[notify] 创建线程工作已完成,尝试唤醒线程\n");

            set_state(RUNNABLE);

            startThread_lock()->notify(this);

            break;
        } else {
            INFO_PRINT("[%s] 等待线程完成初始化 %d\n", name(), get_state());
        }
    }
}

void Thread::SpinAcquire(volatile int *addr, Thread* self) {
    for (;;) {
        if (0 == Atomic::cmpxchg(1, addr, 0)) {
            INFO_PRINT("[SpinAcquire %s] 拿到了锁\n", self->name());
            return ;
        }
    }
}

void Thread::SpinRelease(volatile int *addr, Thread* self) {
    INFO_PRINT("[SPinRelease %s] 释放了锁\n", self->name());
    *addr = 0;
}

extern ParkEvent* main_thread_event;

void Thread::join() {
    if (0 == main_thread_event->state()) {
        main_thread_event->lock();
    }
}