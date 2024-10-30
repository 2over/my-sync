#include "include/common.h"
#include "core/Thread.h"
#include "core/ParkEvent.h"
#include "core/ObjectMonitor.h"
#include "core/ObjectSynchronizer.h"
#include "oops/InstanceOopDesc.h"
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
        INFO_PRINT("[%s] %d\n", Self->name(), i);
        usleep(100);
    }

    // 出临界区
    ObjectSynchronizer::fast_exit(obj, lock, Self);

    return 0;
}
void* main_thread_do(void* args) {

    Thread* t1 = new Thread(thread_do_1, NULL, "t1");
    Thread* t2 = new Thread(thread_do_2, NULL, "t2");
    Thread* t3 = new Thread(thread_do_2, NULL, "t3");
    Thread* t4 = new Thread(thread_do_2, NULL, "t4");
    Thread* t5 = new Thread(thread_do_2, NULL, "t5");
    Thread* t6 = new Thread(thread_do_2, NULL, "t6");
    Thread* t7 = new Thread(thread_do_2, NULL, "t7");

    t1->run();
    t2->run();
    t3->run();
    t4->run();
    t5->run();
    t6->run();
    t7->run();


    t1->join();

    return 0;
}

int main() {
    pthread_t tid;
    pthread_create(&tid, NULL, main_thread_do, NULL);
    pthread_join(tid, NULL);
    return 0;
}
