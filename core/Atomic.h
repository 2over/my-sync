//
// Created by xiehao on 2024/10/22.
//

#ifndef MY_SYNC_ATOMIC_H
#define MY_SYNC_ATOMIC_H


class Atomic {
public:
    /**
     * int exchange_value = 10;
     *
     * int *dest = new int;
     * *dest = 1;
     *
     *
     * @param exchange_value
     * @param dest
     * @return
     */
    static int xchg(int exchange_value, volatile int *dest) {
        __asm__ volatile (
        "xchgl (%2), %0"
        :"=r" (exchange_value)
        : "0" (exchange_value), "r" (dest)
        : "memory");

        return exchange_value;
    }

    /**
     * cmpxchangel exhcange_value, (dest)
     *
     * 比较eax(compare_value)与dest中的值
     * 相等: *dest\exhcange_value的值发生交换，返回交换前*dest
     * 不相等: exchange_value = *dest, 返回原exchange_value
     */
    static int cmpxchg(int exchange_value, volatile int *dest, int compare_value) {
        __asm__ volatile ("cmpxchgl %1, (%3)"
        : "=a" (exchange_value)
        : "r" (exchange_value), "a" (compare_value), "r" (dest)
        : "cc", "memory");

        return exchange_value;
    }

    // ===================================================================
    long _cmpxchg(long exchange_value, long *dest, long compare_value) {
        int ret = *dest;

        // 如果compare_value =*dest, *dest的值与exchange_value的值发生交换
        // 返回交换后的exchange_value,即原*dest
        if (*dest == compare_value) {
            *dest = exchange_value;
            exchange_value = ret;
        }

        // 如果compare_value != *dest,返回*dest
        return ret;
    }

    // 本段代码的执行效果类似上面的方法的效果，不过这段代码是原子性的
    static long cmpxchg(long exchange_value, volatile long *dest, long compare_value) {
        __asm__ __volatile__ ("lock; cmpxchgq %1, (%3)"
        : "=a" (exchange_value)
        : "r" (exchange_value), "a" (compare_value), "r" (dest)
        : "cc", "memory");

        return exchange_value;
    }

    static void *cmpxchg_ptr(void *exchange_value, void *dest, void *compare_value) {
        return (void *) cmpxchg((long) exchange_value, (long *) dest, (long) compare_value);
    }

};

#endif //MY_SYNC_ATOMIC_H
