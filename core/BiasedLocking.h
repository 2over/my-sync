//
// Created by xiehao on 2024/10/28.
//

#ifndef MY_SYNC_BIASEDLOCKING_H
#define MY_SYNC_BIASEDLOCKING_H
class InstanceOopDesc;
class Thread;

class BiasedLocking {

public:

    /**
     * NOT_BIASED 表示该对象没有持有偏向锁
     * BIAS_REVOKED表示该对象爱嗯的偏向爱嗯锁已经被撤销了，即其对象头已经恢复成默认的不开启偏向锁时的状态
     * BIAS_REVOKED_AND_REBIASED表示当前线程重新获取了该偏向锁
     */
    enum Condition {
        NOT_BIASED = 1,
        BIAS_REVOKED = 2,
        BIAS_REVOKED_AND_REBIASED = 3
    };

    static BiasedLocking::Condition revoke_and_rebias(InstanceOopDesc* obj, bool attempt_rebias, Thread* t);
};


#endif //MY_SYNC_BIASEDLOCKING_H
