//
// Created by xiehao on 2024/10/24.
//

#ifndef MY_SYNC_GLOBALDEFINITIONS_H
#define MY_SYNC_GLOBALDEFINITIONS_H

#include <stdint.h>
#include <stddef.h>

// --------------------------------------------------------------------
// Java type definitions

// All kinds of 'plain' byte addrsses

typedef     signed      char s_char;
typedef     unsigned    char u_char;
typedef     u_char*     address;
typedef     uintptr_t   address_word; // unsigned integer which will hold a pointer

// except for some implementation of a C++
// linkage pointer to funciton. Should never
// need one of those to be placed in this
// type anyway

// Utility functions to "portably" (?) bit twiddle pointers
// Where portable means keep ANSI C++ compilers quiet

inline address          set_address_bits(address x, int m)      {   return address(intptr_t(x) | m);  }
inline address          clear_address_bits(address x, int m)    {   return address(intptr_t(x) & ~m);   }

// Utility functions to "portably" make cast to/from function pointers

inline address_word     mask_address_bits(address x, int m)     {   return address_word(x) & m; }
inline address_word     castable_address(address x)             {   return address_word(x); }
inline address_word     castable_address(void* x)               {   return address_word(x); }


// This file holds all globally used constants & type ,class (forward)
// declarations and a few frequently used utility functions

// --------------------------------------------------------------------
// Constants

const int LogBytesPerShort      = 1;
const int LogBytesPerInt        = 2;
const int LogBytesPerWord       = 3;
const int LogBytesPerLong       = 3;

const int BytesPerShort         = 1 << LogBytesPerShort;
const int BytesPerInt           = 1 << LogBytesPerInt;
const int BytesPerWord          = 1 << LogBytesPerWord;
const int BytesPerLong          = 1 << LogBytesPerLong;

const int LogBitsPerByte        = 3;
const int LogBitsPerShort       = LogBitsPerByte + LogBytesPerShort;
const int LogBitsPerInt         = LogBitsPerByte + LogBytesPerInt;
const int LogBitsPerWord        = LogBitsPerByte + LogBytesPerWord;
const int LogBitsPerLong          = LogBitsPerByte + LogBytesPerLong;

const int BitsPerByte           = 1 << LogBitsPerByte;
const int BitsPerShort          = 1 << LogBitsPerShort;
const int BitsPerInt            = 1 << LogBitsPerInt;
const int BitsPerWord           = 1 << LogBitsPerWord;
const int BitsPerLong           = 1 << LogBitsPerLong;

const int WordAlignmentMask     = (1 << LogBytesPerWord) - 1;
const int LongAlignmentMask     = (1 << LogBytesPerLong) - 1;

const int WordsPerLong          = 2; // Number of stack entries for longs

const int oopSize               = sizeof(char*); // Full-width oop
extern int heapOopSize;                            // Oop within a java object
const int wordSize              = sizeof(char*);
const int longSize              = sizeof(long);
const int jintSize              = sizeof(int);
const int size_tSize            = sizeof(size_t);

const int BytesPerOop           = BytesPerWord; // Full-width oop

// --------------------------------------------------------------------
// Utility functions for bitfield manipulations

const intptr_t  AllBits     = ~0; // all bits set in a word
const intptr_t  NoBits      =  0; // no bits set in a word;
const long      NoLongBits  =  0; // no bits set in a long
const intptr_t  OneBit      =  1; // only right_most bit set in a word

// get a word with the n.th or the right-most or left-most n bits set
// (note: #define used only so that they can be used in enum constant definitions)
#define nth_bit(n)          (n >= BitsPerWord ? 0 : OneBit << (n))
#define right_n_bits(n)     (nth_bit(n) - 1)
#define left_n_bits(n)      (right_n_bits(n) << (n >= BitsPerWord ? 0 : (BitsPerWord - n)))

// bit-operations using a mask m
inline void         set_bits            (intptr_t &x, intptr_t m)       { x |= m; }
inline void         clear_bits          (intptr_t &x, intptr_t m)       { x&= ~m; }
inline intptr_t     mask_bits           (intptr_t x, intptr_t m)        { return x & m; }
inline long         mast_long_bits      (long x, long m)                { return x & m;}
inline bool         mast_bits_are_true  (intptr_t flags, intptr_t mask) { (flags & mask) == mask; }

// bit-operations using the n.th bit
inline void         set_nth_bit     (intptr_t &x, int n)    {   set_bits(x, nth_bit(n)); }
inline void         clear_nth_bit   (intptr_t &x, int n)    {   clear_bits(x, nth_bit(n)); }
inline bool         is_set_nth_bit  (intptr_t  x, int n)    {   return mask_bits(x, nth_bit(n)) != NoBits; }

// returns the bitfield of x starting at start_bit_no with length field_length (no sign-exension!)
inline intptr_t bitfield(intptr_t x, int start_bit_no, int field_length) {
    return mask_bits(x >> start_bit_no, right_n_bits(field_length));
}


#endif //MY_SYNC_GLOBALDEFINITIONS_H
