#pragma once

#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <memory.h>

#include <iostream>
#include <string>
#include <vector>
#include <stack>

using namespace std;

typedef void *pvoid;
typedef unsigned char byte;
typedef unsigned short ushort;
typedef unsigned int uint;
typedef unsigned long ulong;

typedef void *(*thread_fun)(void *);

typedef union {
    long l_dummy;
    double d_dummy;
    void *p_dummy;
} Align;

#define ALIGN_SIZE      (sizeof(Align))


typedef enum {
    GC_MARK_CLEAN,
    GC_MARK_COLLECT,
    GC_MARK_COPY,
    GC_G1,
} GC_Type;

#define UseBiasedLocking true

typedef class MarkOopDesc *markOop;

/* ==================================================
 *  自定义打印输出
 ==================================================*/

#define INFO_OUTPUT     3
#define WARNING_OUTPUT  2
#define DEBUG_OUTPUT    1
#define ERROR_OUTPUT    0

#define DEBUG
#define DEBUG_LEVEL     INFO_OUTPUT

#define INFO_PRINT(info, ...) do { \
    if (DEBUG_LEVEL >= INFO_OUTPUT) {\
        printf("[info] (%s:%d->%s): " info "",__FILE__, __LINE__, __FUNCTION__, ##__VA_ARGS__);\
    } \
} while(0)

#define assert(cond, msg) { if (!(cond)) { fprintf(stderr, "assert fails %s %d: %s\n", __FILE__,__LINE__,msg); abort(); }}

#define WRITE(str) do {\
    write(STDOUT_FILENO, str, strlen(str)); \
} while(0)

#define ASSERT_ZERO(status, str) do {\
    if (0 != status) {\
        WRITE(str); \
        exit(-1); \
    } \
}while(0)
