//
// Created by xiehao on 2024/10/24.
//

#ifndef MY_SYNC_MARKOOPDESC_H
#define MY_SYNC_MARKOOPDESC_H

#include "../include/common.h"
#include "../core/GlobalDefinitions.h"

class BasicLock;
class Thread;
class ObjectMonitor;

class MarkOopDesc {

private:
    // Coversion
    uintptr_t value() const {
        return (uintptr_t) this;
    }

public:
    // Constants
    enum {
        age_bits            = 4,
        lock_bits           = 2,
        biased_lock_bits    = 1,
        max_hash_bits       = 64 - age_bits - lock_bits - biased_lock_bits,
        hash_bits           = max_hash_bits > 31 ? 31 : max_hash_bits,
        cms_bits            = 1, // 它的作用还不得而知
        epoch_bits          = 2
    };

    // The biased locking code currently requires that the age bits be
    // contiguous to the lock bits
    enum {
        lock_shift          = 0,
        biased_lock_shift   = lock_bits, // 2
        age_shift           = lock_bits + biased_lock_bits, // 3
        cms_shift           = age_shift + age_bits, // 7
        hash_shift          = cms_shift + cms_bits, // 8
        epoch_shift         = hash_shift // 8
    };

    enum {
        lock_mask                   = right_n_bits(lock_bits),
        lock_mask_in_place          = lock_mask << lock_shift,
        biased_lock_mask            = right_n_bits(lock_bits + biased_lock_bits),
        biased_lock_mask_in_place   = biased_lock_mask << lock_shift,
        biased_lock_bit_in_place    = 1 << biased_lock_shift,
        age_mask                    = right_n_bits(age_bits),
        age_mask_in_place           = age_mask << age_shift,
        epoch_mask                  = right_n_bits(epoch_bits),
        epoch_mask_in_place         = epoch_mask << epoch_shift,
        cms_mask                    = right_n_bits(cms_bits),
        cms_mask_in_place           = cms_mask << cms_shift
#ifndef _WIN64
        , hash_mask                 = right_n_bits(hash_bits),
        hash_mask_in_place          = (address_word) hash_mask << hash_shift
#endif
    };

    // Alignment of JavaThread pointers encoded in object header requireed by biased lockiing
    enum {
        biased_lock_alignment       = 2 << (epoch_shift + epoch_bits)
    };
#ifdef _WIN64
    // These values are too big for Win64
    const static uintptr_t hash_mask = right_n_bits(hash_bits);
    const static uintptr_t hash_mask_in_place = (address_word)hash_mask << hash_shift;
#endif

    enum {
        locked_value                = 0,
        unlocked_value              = 1,
        monitor_value               = 2,
        marked_value                = 3,
        biased_lock_pattern         = 5
    };

    enum {  no_hash                     = 0 };  // no hash value assigned

    enum {
        no_hash_in_place            = (address_word)no_hash << hash_shift,
        no_lock_in_place            = unlocked_value
    };

    enum {  max_age                 = age_mask };
    enum {  max_bias_epoch          = epoch_mask };

    bool has_bias_pattern() const {
        INFO_PRINT("%X\n", biased_lock_mask_in_place);

        /**
         * value() 返回对象头的内容
         * biased_lock_mask_in_place 7  0111
         * biased_lock_pattern      5 0101 支持偏向的偏向锁
         */
        return (mask_bits(value(), biased_lock_mask_in_place) == biased_lock_pattern);
    }

    Thread* biased_locker() const {
        INFO_PRINT("%X\n", biased_lock_mask_in_place);
        INFO_PRINT("%X]n", age_mask_in_place);
        INFO_PRINT("%X\n", epoch_mask_in_place);
        INFO_PRINT("%X\n", biased_lock_mask_in_place | age_mask_in_place);
        INFO_PRINT("%X\n", biased_lock_mask_in_place | age_mask_in_place | epoch_mask_in_place);
        INFO_PRINT("%X\n", ~(biased_lock_mask_in_place | age_mask_in_place | epoch_mask_in_place));

        assert(has_bias_pattern(), "should not call this otherwise");

        /**
         * value() 返回对象头的内容
         * biased_lock_mask_in_place 7 0111
         * age_mask_in_place 7 8 0111 1111
         * epoch_mask_in_place   300     0011 0000 0000
         *
         * ~(biased_lock_mask_in_place | age_mask_in_place | epoch_mask_inplace)
         * 表示只取C++对象JavaThread的地址
         */
        return (Thread*) ((intptr_t) (mask_bits(value(),
                ~(biased_lock_mask_in_place | age_mask_in_place | epoch_mask_in_place))));
    }


    /**
     * 该函数的作用:
     * 1. 判断某个对象头是否已经支持偏向锁
     * 2. 判断某个对象头是否还未偏向任何线程
     * @return
     */
    bool is_biased_anonymously() const {
        return (has_bias_pattern() && (biased_locker() == NULL));
    }

    int bias_epoch() const {
        INFO_PRINT("%X\n", mask_bits(value()))
    }


};


#endif //MY_SYNC_MARKOOPDESC_H
