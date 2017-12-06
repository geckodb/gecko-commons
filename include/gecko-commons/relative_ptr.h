#pragma once

#include <stdint.h>

__BEGIN_DECLS

typedef int32_t relative_ptr_t;

#define RELATIVE_PTR_UPDATE(auto_relative_ptr, dst_ptr)                                \
    auto_relative_ptr_t = (dst_ptr - &auto_relative_ptr_t);

#define RELATIVE_PTR(x, dst_ptr)                                                       \
{                                                                                      \
    auto_relative_ptr_t x;                                                             \
    RELATIVE_PTR_UPDATE(x, dst_ptr)                                                    \
}

#define RELATIVE_PTR_GET_DST(auto_relative_ptr, prefetch_read, prefetch_write)         \
({                                                                                     \
    void *retval = ((void *) (&auto_relative_ptr_t + auto_relative_ptr_t));            \
    __builtin_prefetch(retval, prefetch_read, prefetch_write);                         \
    retval;                                                                            \
})

__END_DECLS