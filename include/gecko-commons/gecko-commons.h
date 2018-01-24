#ifndef GECKO_COMMONS_GECKO_COMMONS_H
#define GECKO_COMMONS_GECKO_COMMONS_H

#include <gecko-commons/c11threads.h>
#include <gecko-commons/containers/gs_freelist.h>
#include <gecko-commons/containers/gs_hash.h>
#include <gecko-commons/containers/gs_hashset.h>
#include <gecko-commons/containers/gs_list.h>
#include <gecko-commons/containers/gs_vec.h>
#include <gecko-commons/gs_async.h>
#include <gecko-commons/gs_debug.h>
#include <gecko-commons/gs_error.h>
#include <gecko-commons/gs_hash.h>
#include <gecko-commons/gs_interval.h>
#include <gecko-commons/gs_msg.h>
#include <gecko-commons/gs_require.h>
#include <gecko-commons/gs_spinlock.h>
#include <gecko-commons/gs_timer.h>
#include <gecko-commons/gs_utils.h>

#define GS_OPTIONAL(expr, statement) \
{                                    \
   if(expr) { statement; }           \
}

#define GS_SYSTEM_TIME_MS()                                             \
({                                                                      \
    struct timeval timeval;                                             \
    gettimeofday(&timeval, NULL);                                       \
    long int elpased = timeval.tv_sec * 1000 + timeval.tv_usec / 1000;  \
    elpased;                                                            \
})

#endif
