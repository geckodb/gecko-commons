#include <gecko-commons/gs_base_relative_ptr.h>

GS_DECLARE(gs_status_t) gs_base_relative_ptr_create(gs_base_relative_ptr_t *ptr, void *base, void *dst)
{
    RETURN_FAILED_IF(!ptr)
    RETURN_FAILED_IF(!base || !dst)
    RETURN_FAILED_IF(base > dst)
    ptr->base = base;
    ptr->offset = (dst - base);
    return GS_SUCCESS;
}

GS_DECLARE(gs_status_t) gs_base_relative_ptr_update(gs_base_relative_ptr_t *ptr, void *base)
{
    RETURN_FAILED_IF(!ptr)
    RETURN_FAILED_IF(!base)
    ptr->base = base;
    return GS_SUCCESS;
}

GS_DECLARE(void *) gs_base_relative_ptr_get(const gs_base_relative_ptr_t *ptr)
{
    return ptr ? (ptr->base + ptr->offset) : NULL;
}