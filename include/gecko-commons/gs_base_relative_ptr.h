#pragma once

#include <gecko-commons/stdinc.h>

/* life would be a bit more easier if Google doesn't have a patent on auto-relative pointers */

typedef struct gs_base_relative_ptr_t
{
    void *              base;
    uint64_t            offset;
} gs_base_relative_ptr_t;

__BEGIN_DECLS

GS_DECLARE(gs_status_t) gs_base_relative_ptr_create(gs_base_relative_ptr_t *ptr, void *base, void *dst);
GS_DECLARE(gs_status_t) gs_base_relative_ptr_update(gs_base_relative_ptr_t *ptr, void *base);
GS_DECLARE(void *) gs_base_relative_ptr_get(const gs_base_relative_ptr_t *ptr);

__END_DECLS