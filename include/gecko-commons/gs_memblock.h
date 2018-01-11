#pragma once

#include <gecko-commons/relative_ptr.h>
#include <gecko-commons/stdinc.h>

typedef struct gs_freelist_entry_t
{
    int32_t               offset_relative_to_memblock;
    uint32_t              size;
} gs_freelist_entry_t;

typedef struct gs_freelist_header_t
{
    relative_ptr_t        memblock;
    uint32_t              size;
    uint32_t              capacity;
} gs_freelist_t;

typedef struct gs_memblock_header_t
{
    relative_ptr_t        freelist;
    uint32_t              size;
    BYTE *                data;
} gs_memblock_header_t;

typedef struct gs_memchunk_header_t
{
    relative_ptr_t        memblock;
    uint32_t              size;
    uint32_t              capacity;
} gs_memchunk_header_t;