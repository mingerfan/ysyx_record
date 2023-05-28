#ifndef _DEBUG_H
#define _DEBUG_H

#include <stdio.h>
#include "generated/autoconf.h"
#include <assert.h>

#define ANSI_FG_BLACK   "\33[1;30m"
#define ANSI_FG_RED     "\33[1;31m"
#define ANSI_FG_GREEN   "\33[1;32m"
#define ANSI_FG_YELLOW  "\33[1;33m"
#define ANSI_FG_BLUE    "\33[1;34m"
#define ANSI_FG_MAGENTA "\33[1;35m"
#define ANSI_FG_CYAN    "\33[1;36m"
#define ANSI_FG_WHITE   "\33[1;37m"
#define ANSI_BG_BLACK   "\33[1;40m"
#define ANSI_BG_RED     "\33[1;41m"
#define ANSI_BG_GREEN   "\33[1;42m"
#define ANSI_BG_YELLOW  "\33[1;43m"
#define ANSI_BG_BLUE    "\33[1;44m"
#define ANSI_BG_MAGENTA "\33[1;35m"
#define ANSI_BG_CYAN    "\33[1;46m"
#define ANSI_BG_WHITE   "\33[1;47m"
#define ANSI_NONE       "\33[0m"

#define MyPrintf printf

#define G_DEBUG_WR(fmt, args...) MyPrintf(fmt, ##args)

#if CONFIG_DBGI_LEVEL >= 1
#define G_DEBUG_E(fmt, args...) MyPrintf(ANSI_FG_RED"ERROR! file:%s, line:%d\t" fmt "\n" ANSI_NONE, __FILE__, __LINE__, ##args)
#else
#define G_DEBUG_E(fmt, args...) (void)fmt
#define G_DEBUG_WR(fmt, args...) (void)fmt
#endif

#if CONFIG_DBGI_LEVEL >= 2
#define G_DEBUG_W(fmt, args...) MyPrintf(ANSI_FG_YELLOW"WARNING file:%s, line:%d\t" fmt "\n" ANSI_NONE, __FILE__, __LINE__, ##args)
#else
#define G_DEBUG_W(fmt, args...) (void)fmt
#endif

#if CONFIG_DBGI_LEVEL >= 3
#define G_DEBUG_I(fmt, args...) MyPrintf(ANSI_FG_GREEN fmt "\n" ANSI_NONE, ##args)
#else
#define G_DEBUG_I(fmt, args...) (void)fmt
#endif

#define Assert(cond, fmt, args...) do { \
    if (!cond) { \
        G_DEBUG_E(fmt, ##args); \
        fflush(stdout); \
        assert(0); \
    } \
} while(0)

#define panic(format, ...) Assert(0, format, ## __VA_ARGS__)

#endif // #ifndef _DEBUG_H