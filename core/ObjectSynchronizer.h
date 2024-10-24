//
// Created by xiehao on 2024/10/23.
//

#ifndef MY_SYNC_OBJECTSYNCHRONIZER_H
#define MY_SYNC_OBJECTSYNCHRONIZER_H

class Thread;

class InstanceOopDesc;

class BasicLock;

class ObjectMonitor;

class ObjectSynchronizer {

public:
    static void fast_enter(InstanceOopDesc *obj, BasicLock *lock, Thread *t);

    static void fast_exit(InstanceOopDesc *obj, BasicLock *lock, Thread *t);

    static void slow_enter(InstanceOopDesc *obj, BasicLock *lock, Thread *t);

    static void slow_exit(InstanceOopDesc *obj, BasicLock *lock, Thread *t);

    static ObjectMonitor *inflate(InstanceOopDesc *obj, Thread *t);

    static void wait(InstanceOopDesc *obj, Thread *t);

    static void notify(InstanceOopDesc *obj, Thread *t);


};


#endif //MY_SYNC_OBJECTSYNCHRONIZER_H
