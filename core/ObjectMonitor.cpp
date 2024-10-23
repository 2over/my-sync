//
// Created by xiehao on 2024/10/22.
//

#include "ObjectMonitor.h"
#include "../include/common.h"
#include "ObjectMonitor.h"
#include "Atomic.h"
#include "Thread.h"
#include "ObjectWaiter.h"
#include "ParkEvent.h"

extern ParkEvent *main_thread_event;

void ObjectMonitor::enter(Thread *t) {

    void *ret = NULL;

    if (NULL == (ret = Atomic::cmpxchg_ptr(t, &_owner, NULL))) {
        INFO_PRINT("[%s] 抢到了锁\n", t->name());

        return;
    }

    if (t == ret) {
        _recursions++;

        return;
    }

    INFO_PRINT("[%s] 抢锁失败\n", t->name());

    // 加入独列
    ObjectWaiter node(t);
    node._prev = reinterpret_cast<ObjectWaiter *>(0xBAD);

    ObjectWaiter *next = NULL;
    for (;;) {
        node._next = next = _cxq;

        if (next == Atomic::cmpxchg_ptr(&node, &_cxq, next)) {
            INFO_PRINT("[%s] 加入cxq\n", t->name());
            break;
        }
    }

    // 阻塞
    INFO_PRINT("[%s] 阻塞等待\n", t->name());

    t->_ParkEvent->park();

    INFO_PRINT("[%s] 唤醒运行\n", t->name());

    tryLock(t);
}

/**
 * 会出现这个情况
 * 1.持有锁的线程执行exit的时候，其他线程还未入队
 */
void ObjectMonitor::exit(Thread *t) {
    if (_owner != t) {
        // 进入这里的情况: 有线程将锁膨胀成重量级锁,轻量级锁就不需要释放了
        INFO_PRINT("[%s] 非持锁线程不得释放锁(正常)\n", t->name());
        return;
    }

    // 处理冲入
    if (0 != _recursions) {
        _recursions--;
        return;
    }
    /**
     * 释放锁
     */
    for (;;) {
        if (t == Atomic::cmpxchg_ptr(NULL, &_owner, t)) {
            INFO_PRINT("[%s] 成功将owner置为空\n", t->name());
            break;
        } else {
            INFO_PRINT("[%s] 尝试将owner置为空\n", t->name());
        }
    }

    /**
     * 唤醒线程
     * 1.获取首节点
     * 2.修改队列
     * 3.唤醒
     */

    int counter = 0;
    for (;;) {
        ObjectWaiter *node = _cxq;
        if (NULL != node) {
            ObjectWaiter *next = _cxq->_next;

            if (node == Atomic::cmpxchg_ptr(next, &_cxq, node)) {
                node->_thread->_ParkEvent->unpark();

                break;
            }
        } else {
            counter++;

            /**
             * 推出的线程等待3s,如果没有线程入队，结束
             */
            if (counter > 3) {
                main_thread_event->unpark();
            }

            sleep(1);
        }
    }
}

void ObjectMonitor::print_cxq() {
    ObjectWaiter *node = _cxq;
    INFO_PRINT("%X\n", node);

    while (NULL != node) {
        INFO_PRINT("%X\n", node);

        node = node->_next;
    }
}

int ObjectMonitor::tryLock(Thread *t) {
    for (;;) {
        if (NULL != _owner) {
            return 0;
        }

        if (NULL == Atomic::cmpxchg_ptr(t, &_owner, NULL)) {
            return 1;
        }

        return -1;
    }
}

/**
 * 该方法中操作共享数据都是通过CAS, 参考Hotspot源码
 *
 * 我就在想, 执行wait方法的，都是拿锁的线程，言外之意是在临界区中执行的, 需要考虑线程安全吗? 什么情况下会出现线程不安全?
 *
 */
void ObjectMonitor::wait(Thread *t) {
    // 放入队列
    ObjectWaiter node(t);

    Thread::SpinAcquire(&_WaitSetLock, t);
    AddWaiter(&node);
    _waiters++;
    Thread::SpinRelease(&_WaitSetLock, t);

    // 释放锁
    for (;;) {
        if (t == Atomic::cmpxchg_ptr(NULL, &_owner, t)) {
            INFO_PRINT("[%s] 成功将owner置为空\n", t->name());
            break;
        } else {
            INFO_PRINT("[%s] 尝试将owner置为空\n", t->name());
        }
    }

    // 自己进入阻塞
    INFO_PRINT("[%s] wait 进入阻塞\n", t->name());

    t->_ParkEvent->park();
    INFO_PRINT("[%s] 被notify\n", t->name());

    tryLock(t);
}

void ObjectMonitor::notify() {
    assert(_WaitSet != NULL, "_WaitSet = NULL");

    ObjectWaiter *node = DequeueWaiter();

    node->_thread->_ParkEvent->unpark();
}

void ObjectMonitor::AddWaiter(ObjectWaiter *node) {
    assert(node != NULL, "should not dequeue NULL node");
    assert(node->_prev == NULL, "node already in list");
    assert(node->_next == NULL, "node already in list");

    // put node at end of queue (circular doubly linked list)
    if (_WaitSet == NULL) {
        _WaitSet = node;
        node->_prev = node;
        node->_next = node;
    } else {
        ObjectWaiter *head = _WaitSet;
        ObjectWaiter *tail = head->_prev;

        assert(tail->_next == head, "invariant check");
        tail->_next = node;
        head->_prev = node;
        node->_next = head;
        node->_prev = tail;
    }
}

ObjectWaiter *ObjectMonitor::DequeueWaiter() {
    // dequeue the very first waiter
    ObjectWaiter* waiter = _WaitSet;

    if (waiter) {
        DequeueSpecificWaiter(waiter);
    }
    return waiter;
}

void ObjectMonitor::DequeueSpecificWaiter(ObjectWaiter *node) {
    assert(node != NULL, "should not dequeue NULL node");
    assert(node->_prev != NULL, "node already removed from list");
    assert(node->_next != NULL, "node already removed from list");

    // when the waiter has woken up because of interrupt,
    // timeout or other spurious wake-up, dequeue the waiter from waiting list

    ObjectWaiter *next = node->_next;
    if (next == node) {
        assert(node->_prev == node, "invariant check");
        _WaitSet = NULL;
    } else {
        ObjectWaiter *prev = node->_prev;
        assert(prev->_next == node, "invariant check");
        assert(next->_prev == node, "invariant check");

        next->_prev = prev;
        prev->_next = next;
        if (_WaitSet == node) {
            _WaitSet = next;
        }

    }
    node->_next = NULL;
    node->_prev = NULL;
}
