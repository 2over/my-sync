#include "include/common.h"
#include "core/Thread.h"
#include "core/ParkEvent.h"
#include "core/ObjectMonitor.h"
#include "core/ObjectSynchronizer.h"
#include "oops/InstanceOopDesc.h"
#include "core/BiasedLocking.h"
#include "core/BasicLock.h"
#include "oops/Klass.h"


Klass* klass = new Klass;
InstanceOopDesc* obj = new InstanceOopDesc(klass);
BasicLock* lock = new BasicLock;

void* thread_do_1(void* arg) {
    Thread* Self = static_cast<Thread *>(arg);

    // 进入临界区
    ObjectSynchronizer::fast_enter(obj, lock, Self);

    for (int i = 0; i < 3; i++) {
        INFO_PRINT("[%s] %d\n", Self->name(), i);
        usleep(100);
    }

    // 出临界区
    ObjectSynchronizer::fast_exit(obj, lock, Self);

    return 0;
}

void* thread_do_2(void *arg) {
    Thread* Self = static_cast<Thread*>(arg);

    // 进入临界区
    ObjectSynchronizer::fast_enter(obj, lock, Self);

    for (int i = 0; i < 3; i++) {

    }
}
int main() {
    return 0;
}
