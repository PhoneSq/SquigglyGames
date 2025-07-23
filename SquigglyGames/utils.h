#ifndef UTILS_H
#define UTILS_H

// Type definitions for cleaner code
typedef char s8;
typedef unsigned char u8;
typedef short s16;
typedef unsigned short u16;
typedef int s32;
typedef unsigned int u32;
typedef long long s64;
typedef unsigned long long u64;

// Macros for better code readability
#define global_variable static
#define internal static

// Utility functions
 int clamp(int min, int val, int max);

#endif 