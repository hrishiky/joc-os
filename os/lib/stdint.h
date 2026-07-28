#ifndef STDINT_H
#define STDINT_H

typedef char int8_t;
typedef unsigned char uint8_t;

typedef short int16_t;
typedef unsigned short uint16_t;

typedef int int32_t;
typedef unsigned int uint32_t;

typedef long long int64_t;
typedef unsigned long long uint64_t;

typedef long long ssize_t;
typedef unsigned long long size_t;

typedef unsigned long long uintptr_t;

#define UINT8_MAX ((uint8_t) - 1)
#define INT8_MAX ((int8_t) (UINT8_MAX >> 1))
#define INT8_MIN (-INT8_MAX - 1)

#define UINT16_MAX ((uint16_t) - 1)
#define INT16_MAX ((int16_t) (UINT16_MAX >> 1))
#define INT16_MIN (-INT16_MAX - 1)

#define UINT32_MAX ((uint32_t) - 1)
#define INT32_MAX ((int32_t) (UINT32_MAX >> 1))
#define INT32_MIN (-INT32_MAX - 1)

#define UINT64_MAX ((uint64_t) - 1)
#define INT64_MAX ((int64_t) (UINT64_MAX >> 1))
#define INT64_MIN (-INT64_MAX - 1)

#define SIZE_MAX ((size_t) - 1)
#define SSIZE_MAX ((ssize_t) (SIZE_MAX >> 1))
#define SSIZE_MIN (-SSIZE_MAX - 1)

#endif
