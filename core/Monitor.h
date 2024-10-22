//
// Created by xiehao on 2024/10/22.
//

#ifndef MY_SYNC_MONITOR_H
#define MY_SYNC_MONITOR_H

class Thread;

class Monitor {

private:
    Thread*     _owner;
public:
    void wait(Thread* t);
    void notify(Thread* t);
    void notify_all(Thread* t);
};


#endif //MY_SYNC_MONITOR_H
