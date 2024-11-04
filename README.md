# my-sync
手写synchronized

# 实现效果，需要多跑几次才会出现
/home/ziya/Documents/my-sync/cmake-build-debug/my_sync

core/BiasedLocking.cpp:29->revoke_and_rebias): [t1] mark.epoch=0, prototype_header.epoch=0

core/BiasedLocking.cpp:87->revoke_and_rebias): [t1] 第一次拿到偏向锁 F4000880

main.cpp:22->thread_do_1): [t1] 0

core/BiasedLocking.cpp:29->revoke_and_rebias): [t2] mark.epoch=0, prototype_header.epoch=0

core/BiasedLocking.cpp:74->revoke_and_rebias): [t2] 将偏向锁恢复成无锁

core/ObjectSynchronizer.cpp:106->slow_enter): [t2] 轻量级锁抢锁成功

main.cpp:39->thread_do_2): [t2] 0

core/ObjectSynchronizer.cpp:125->slow_enter): [t7] 轻量级锁抢锁失败

core/ObjectSynchronizer.cpp:141->inflate): [t7] 开始膨胀成重量级锁

core/ObjectSynchronizer.cpp:178->inflate): [t7] 更正状态为正在膨胀成重量级锁

core/ObjectSynchronizer.cpp:194->inflate): [t7] 膨胀成重量级锁成功 0

core/ObjectMonitor.cpp:20->enter): [t7] 抢到了锁

main.cpp:39->thread_do_2): [t7] 0

core/ObjectSynchronizer.cpp:125->slow_enter): [t3] 轻量级锁抢锁失败

core/ObjectSynchronizer.cpp:141->inflate): [t3] 开始膨胀成重量级锁

core/ObjectSynchronizer.cpp:145->inflate): [t3] 检测到已经是重量级锁

core/ObjectMonitor.cpp:31->enter): [t3] 抢锁失败

core/ObjectMonitor.cpp:42->enter): [t3] 加入cxq

core/ObjectMonitor.cpp:48->enter): [t3] 阻塞等待

core/ObjectSynchronizer.cpp:125->slow_enter): [t4] 轻量级锁抢锁失败

core/ObjectSynchronizer.cpp:141->inflate): [t4] 开始膨胀成重量级锁

core/ObjectSynchronizer.cpp:145->inflate): [t4] 检测到已经是重量级锁

core/ObjectMonitor.cpp:31->enter): [t4] 抢锁失败

core/ObjectMonitor.cpp:42->enter): [t4] 加入cxq

core/ObjectMonitor.cpp:48->enter): [t4] 阻塞等待

core/ObjectSynchronizer.cpp:125->slow_enter): [t5] 轻量级锁抢锁失败

core/ObjectSynchronizer.cpp:141->inflate): [t5] 开始膨胀成重量级锁

core/ObjectSynchronizer.cpp:145->inflate): [t5] 检测到已经是重量级锁

core/ObjectMonitor.cpp:31->enter): [t5] 抢锁失败

core/ObjectMonitor.cpp:42->enter): [t5] 加入cxq

core/ObjectMonitor.cpp:48->enter): [t5] 阻塞等待

main.cpp:39->thread_do_2): [t2] 1

main.cpp:22->thread_do_1): [t1] 1

core/ObjectSynchronizer.cpp:125->slow_enter): [t6] 轻量级锁抢锁失败

core/ObjectSynchronizer.cpp:141->inflate): [t6] 开始膨胀成重量级锁

core/ObjectSynchronizer.cpp:145->inflate): [t6] 检测到已经是重量级锁

core/ObjectMonitor.cpp:31->enter): [t6] 抢锁失败

core/ObjectMonitor.cpp:42->enter): [t6] 加入cxq

core/ObjectMonitor.cpp:48->enter): [t6] 阻塞等待

main.cpp:39->thread_do_2): [t7] 1

main.cpp:22->thread_do_1): [t1] 2

main.cpp:39->thread_do_2): [t7] 2

core/ObjectSynchronizer.cpp:59->fast_exit): [t1] 开始解锁

core/ObjectSynchronizer.cpp:141->inflate): [t1] 开始膨胀成重量级锁

core/ObjectSynchronizer.cpp:145->inflate): [t1] 检测到已经是重量级锁

core/ObjectMonitor.cpp:64->exit): [t1] 非持锁线程不得释放锁(正常)

core/ObjectSynchronizer.cpp:59->fast_exit): [t7] 开始解锁

core/ObjectSynchronizer.cpp:141->inflate): [t7] 开始膨胀成重量级锁

core/ObjectSynchronizer.cpp:145->inflate): [t7] 检测到已经是重量级锁

core/ObjectMonitor.cpp:78->exit): [t7] 成功将owner置为空

core/ObjectMonitor.cpp:52->enter): [t6] 唤醒运行

main.cpp:39->thread_do_2): [t6] 0

main.cpp:39->thread_do_2): [t2] 2

core/ObjectSynchronizer.cpp:59->fast_exit): [t2] 开始解锁

core/ObjectSynchronizer.cpp:141->inflate): [t2] 开始膨胀成重量级锁

core/ObjectSynchronizer.cpp:145->inflate): [t2] 检测到已经是重量级锁

core/ObjectMonitor.cpp:64->exit): [t2] 非持锁线程不得释放锁(正常)

main.cpp:39->thread_do_2): [t6] 1

main.cpp:39->thread_do_2): [t6] 2

core/ObjectSynchronizer.cpp:59->fast_exit): [t6] 开始解锁

core/ObjectSynchronizer.cpp:141->inflate): [t6] 开始膨胀成重量级锁

core/ObjectSynchronizer.cpp:145->inflate): [t6] 检测到已经是重量级锁

core/ObjectMonitor.cpp:78->exit): [t6] 成功将owner置为空

core/ObjectMonitor.cpp:52->enter): [t5] 唤醒运行

main.cpp:39->thread_do_2): [t5] 0

main.cpp:39->thread_do_2): [t5] 1

main.cpp:39->thread_do_2): [t5] 2

core/ObjectSynchronizer.cpp:59->fast_exit): [t5] 开始解锁

core/ObjectSynchronizer.cpp:141->inflate): [t5] 开始膨胀成重量级锁

core/ObjectSynchronizer.cpp:145->inflate): [t5] 检测到已经是重量级锁

core/ObjectMonitor.cpp:78->exit): [t5] 成功将owner置为空

core/ObjectMonitor.cpp:52->enter): [t4] 唤醒运行

main.cpp:39->thread_do_2): [t4] 0

main.cpp:39->thread_do_2): [t4] 1

main.cpp:39->thread_do_2): [t4] 2

core/ObjectSynchronizer.cpp:59->fast_exit): [t4] 开始解锁

core/ObjectSynchronizer.cpp:141->inflate): [t4] 开始膨胀成重量级锁

core/ObjectSynchronizer.cpp:145->inflate): [t4] 检测到已经是重量级锁

core/ObjectMonitor.cpp:78->exit): [t4] 成功将owner置为空

core/ObjectMonitor.cpp:52->enter): [t3] 唤醒运行

main.cpp:39->thread_do_2): [t3] 0

main.cpp:39->thread_do_2): [t3] 1

main.cpp:39->thread_do_2): [t3] 2

core/ObjectSynchronizer.cpp:59->fast_exit): [t3] 开始解锁

core/ObjectSynchronizer.cpp:141->inflate): [t3] 开始膨胀成重量级锁

core/ObjectSynchronizer.cpp:145->inflate): [t3] 检测到已经是重量级锁

core/ObjectMonitor.cpp:78->exit): [t3] 成功将owner置为空

Process finished with exit code 0
