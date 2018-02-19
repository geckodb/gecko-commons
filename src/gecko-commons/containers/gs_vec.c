// An implementation of the vector data structure
// Copyright (C) 2017 Marcus Pinnecke
//
// This program is free software: you can redistribute it and/or modify it under the terms of the
// GNU General Public License as published by the Free Software Foundation, either user_port 3 of the License, or
// (at your option) any later user_port.
//
// This program is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; without even the implied
// warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
// See the GNU General Public License for more details.
//
// You should have received a copy of the GNU General Public License along with this program.
// If not, see <http://www.gnu.org/licenses/>.

// ---------------------------------------------------------------------------------------------------------------------
// I N C L U D E S
// ---------------------------------------------------------------------------------------------------------------------

#include <gecko-commons/stdinc.h>
#include <gecko-commons/containers/gs_vec.h>

// ---------------------------------------------------------------------------------------------------------------------
// H E L P E R   P R O T O T Y P E S
// ---------------------------------------------------------------------------------------------------------------------

 bool check_create_args(size_t, gs_vector_flags_e, float);
 gs_vec_t *alloc_vector();
 gs_vec_t *alloc_data(gs_vec_t *, gs_vector_flags_e, size_t, size_t);
 void init_vector(gs_vec_t *, gs_vector_flags_e, size_t, size_t, float);
 bool check_add_args(gs_vec_t *, size_t, const void *);
 bool check_auto_resize(gs_vec_t *, size_t);
 bool check_set_args(gs_vec_t *, const void *);
 bool outside_bounds_enabled(gs_vec_t *, size_t, size_t);
 bool realloc_vector(gs_vec_t *, size_t);
 bool advance(gs_vec_t *, size_t, size_t);

// ---------------------------------------------------------------------------------------------------------------------
// I N T E R F A C E  I M P L E M E N T A T I O N
// ---------------------------------------------------------------------------------------------------------------------

gs_vec_t *gs_vec_new(size_t element_size, size_t capacity)
{
    return gs_vec_new_ex(element_size, capacity, auto_resize, GROW_FACTOR);
}

gs_vec_t *gs_vec_new_ex(size_t element_size, size_t capacity, gs_vector_flags_e flags, float grow_factor)
{
    gs_vec_t *result = NULL;
    if (check_create_args(element_size, flags, grow_factor)) {
        result = alloc_vector();
        init_vector(result, flags, capacity, element_size, grow_factor);
        result = alloc_data(result, flags, capacity, element_size);
    }
    return result;
}

bool gs_vec_resize(gs_vec_t *vec, size_t num_elements)
{
    GS_REQUIRE_NONNULL(vec)
    if (num_elements < vec->num_elements) {
        vec->num_elements = num_elements;
        return true;
    } else {
        if (advance(vec, 0, num_elements)) {
            vec->num_elements = num_elements;
            return true;
        } else return false;
    }
}

bool gs_vec_reserve(gs_vec_t *vec, size_t num_elements)
{
    return advance(vec, 0, num_elements - 1);
}

size_t gs_vec_length(const gs_vec_t *vec)
{
    GS_REQUIRE_NONNULL(vec);
    return (vec->num_elements);
}

void gs_vec_memset(gs_vec_t *vec, size_t pos_start, size_t num_elements, const void *data)
{
    GS_REQUIRE_NONNULL(vec);
    REQUIRE((num_elements > 0), "illegal argument");
    REQUIRE((pos_start + num_elements <= vec->element_capacity), "out of bounds");
    for (size_t i = pos_start; i < pos_start + num_elements; i++) {
        gs_vec_set(vec, i, 1, data);
    }
    vec->is_sorted = false;
}

gs_vec_t *gs_vec_cpy_deep(gs_vec_t *proto)
{
    gs_vec_t *result = NULL;
    GS_REQUIRE_NONNULL(proto)
    GS_REQUIRE_NONNULL(proto->data)
    if ((result = gs_vec_new_ex(proto->sizeof_element, proto->element_capacity, proto->flags, proto->grow_factor))) {
        gs_vec_set(result, 0, proto->num_elements, proto->data);
    }
    return result;
}

void gs_vec_free(struct gs_vec_t *vec)
{
    GS_REQUIRE_NONNULL(vec)
    GS_REQUIRE_NONNULL(vec->data)
    gs_vec_dispose(vec);
    free (vec);
}

void gs_vec_dispose(struct gs_vec_t *vec)
{
    free (vec->data);
    vec->data = NULL;
    vec->element_capacity = 0;
    vec->num_elements = 0;
    apr_pool_destroy(vec->pool);
}

void gs_vec_free_ex(gs_vec_t *vec, void *capture, bool (*func)(void *capture, void *begin, void *end))
{
    if (gs_vec_foreach(vec, capture, func)) {
        gs_vec_free(vec);
    }
}

bool gs_vec_pushback(gs_vec_t *vec, size_t num_elements, const void *data)
{
    if (check_add_args(vec, num_elements, data) && check_auto_resize(vec, num_elements)) {
        size_t new_num_elements = vec->num_elements + num_elements;
        if (!realloc_vector(vec, new_num_elements))
            return false;
        void *base = vec->data + vec->num_elements * vec->sizeof_element;
        memcpy(base, data, num_elements * vec->sizeof_element);
        vec->num_elements = new_num_elements;
        vec->is_sorted = false;
        return true;
    } else return false;
}

bool gs_vec_add_all(gs_vec_t *dest, const gs_vec_t *src)
{
    if (dest == NULL || src == NULL || dest->sizeof_element != src->sizeof_element) {
        return false;
    } else {
        gs_vec_reserve(dest, dest->num_elements + src->num_elements);
        return gs_vec_add_all_unsafe(dest, src);
    }
}

bool gs_vec_add_all_unsafe(gs_vec_t *dest, const gs_vec_t *src)
{
    if (dest == NULL || src == NULL || dest->sizeof_element != src->sizeof_element) {
        return false;
    } else {
        gs_vec_add_unsafe(dest, src->num_elements, src->data);
        return true;
    }
}

void gs_vec_add_unsafe(gs_vec_t *vec, size_t num_elements, const void *data)
{
    size_t new_num_elements = vec->num_elements + num_elements;
    void *base = vec->data + vec->num_elements * vec->sizeof_element;
    memcpy(base, data, num_elements * vec->sizeof_element);
    vec->num_elements = new_num_elements;
    vec->is_sorted = false;
}

void *gs_vec_data(gs_vec_t *vec)
{
    GS_REQUIRE_NONNULL(vec)
    return vec->data;
}

void *gs_vec_at(const gs_vec_t *vec, size_t pos)
{
    GS_REQUIRE_NONNULL(vec)
    REQUIRE_LESSTHAN(pos, vec->num_elements)
    void *result = (vec->data + pos * vec->sizeof_element);
    return result;
}

void *gs_vec_peek(const gs_vec_t *vec)
{
    GS_REQUIRE_NONNULL(vec)
    REQUIRE_NONZERO(vec->num_elements)
    void *result = gs_vec_at(vec, vec->num_elements - 1);
    return result;
}

void *gs_vec_begin(const gs_vec_t *vec)
{
    if (vec) {
        return (vec->num_elements > 0 ? gs_vec_at(vec, 0) : gs_vec_end(vec));
    } else return NULL;
}

void *gs_vec_end(const gs_vec_t *vec)
{
    GS_REQUIRE_NONNULL(vec)
    void *result = (vec->data + vec->num_elements * vec->sizeof_element);
    return result;
}

bool gs_vec_issorted(gs_vec_t *vec, gs_cache_consideration_policy_e policy, gs_comp_t comp)
{
    GS_REQUIRE_NONNULL(vec);
    REQUIRE((policy != CCP_IGNORECACHE || (comp != NULL)), "Null pointer to required function pointer");
    return (policy == CCP_USECACHE) ? vec->is_sorted : gs_vec_updatesort(vec, comp);
}

bool gs_vec_updatesort(gs_vec_t *vec, gs_comp_t comp)
{
    GS_REQUIRE_NONNULL(vec);
    GS_REQUIRE_NONNULL(comp);

    // update sort state
    vec->is_sorted = true;
    size_t num = vec->num_elements - 1;
    void *lhs = vec->data, *rhs = vec->data + vec->sizeof_element;
    while (num--) {
        vec->is_sorted &= (comp(lhs, rhs) < 1);
        lhs += vec->sizeof_element;
        rhs += vec->sizeof_element;
    }
    return vec->is_sorted;
}

void *gs_vec_pop_unsafe(gs_vec_t *vec)
{
    return (vec->data + (vec->num_elements-- - 1) * vec->sizeof_element);
}

void *gs_vec_peek_unsafe(gs_vec_t *vec)
{
    return (vec->data + (vec->num_elements - 1) * vec->sizeof_element);
}

bool gs_vec_set(gs_vec_t *vec, size_t idx, size_t num_elements, const void *data)
{
    GS_REQUIRE_NONNULL(vec)
    if (check_set_args(vec, data)) {
        vec->is_sorted = false;
        size_t last_idx = idx + num_elements;
        if (last_idx < vec->element_capacity) {
            memcpy(vec->data + idx * vec->sizeof_element, data, num_elements * vec->sizeof_element);
            vec->num_elements = max(vec->num_elements, last_idx);
            return true;
        } else if (advance(vec, idx, num_elements))
            return gs_vec_set(vec, idx, num_elements, data);
    }
    return false;
}

size_t gs_vector_get_num_elements(gs_vec_t *vec)
{
    GS_REQUIRE_NONNULL(vec)
    return vec->num_elements;
}

size_t gs_vector_get_elements_size(gs_vec_t *vec)
{
    GS_REQUIRE_NONNULL(vec)
    return vec->sizeof_element;
}

bool gs_vec_foreach(gs_vec_t *vec, void *capture, bool (*func)(void *capture, void *begin, void *end))
{
    GS_REQUIRE_NONNULL(vec)
    GS_REQUIRE_NONNULL(func)
    vec->is_sorted = false;
    return func(capture, vec->data, vec->data + vec->num_elements * vec->sizeof_element);
}

void gs_vec_cpy_shallow(gs_vec_t *dst, gs_vec_t *src)
{
    dst->sizeof_element = src->sizeof_element;
    dst->num_elements = src->num_elements;
    dst->element_capacity = src->element_capacity;
    dst->flags = src->flags;
    dst->grow_factor = src->grow_factor;
    dst->data = src->data;
    dst->is_sorted = src->is_sorted;
}

void gs_vec_swap(gs_vec_t *lhs, gs_vec_t *rhs)
{
    gs_vec_t tmp;
    gs_vec_cpy_shallow(&tmp, lhs);
    gs_vec_cpy_shallow(lhs, rhs);
    gs_vec_cpy_shallow(rhs, &tmp);
}

size_t gs_vec_count(gs_vec_t *vec, void *capture, bool (*pred)(void *capture, void *it))
{
    GS_REQUIRE_NONNULL(vec)
    GS_REQUIRE_NONNULL(pred)
    size_t count = 0;
    void *end = (vec->data + vec->num_elements * vec->sizeof_element);
    for (void *it = vec->data; it != end; it += vec->sizeof_element) {
        count += pred(capture, it);
    }
    return count;
}

bool gs_vec_contains(gs_vec_t *vec, void *needle)
{
    GS_REQUIRE_NONNULL(vec)
    GS_REQUIRE_NONNULL(needle)

    void *end = (vec->data + vec->num_elements * vec->sizeof_element);
    for (void *chunk_start = vec->data; chunk_start < end; chunk_start += vec->sizeof_element) {
        void *chunk_end = (chunk_start + vec->sizeof_element);
        bool is_equal = true;
        for (void *byte = chunk_start; byte < chunk_end; byte++) {
            size_t byte_num = (byte - chunk_start);
            if (*(char *) byte != *((char *) needle + byte_num)) {
                is_equal = false;
                break;
            }
        }
        if (is_equal)
            return true;
    }
    return false;
}

size_t gs_vec_memused(gs_vec_t *vec)
{
    return (vec->element_capacity * vec->sizeof_element);
}

size_t gs_vec_memused__str(gs_vec_t *vec)
{
    size_t memused = gs_vec_memused(vec);
    size_t total_str_size = 0;
    gs_vec_foreach(vec, &total_str_size, gs_get_sizeof_strings);
    return (memused + total_str_size);
}

size_t gs_vec_sizeof(const gs_vec_t *vec)
{
    return (vec == NULL ? 0 : vec->element_capacity * vec->sizeof_element);
}

void gs_vec_sort(gs_vec_t *vec, gs_comp_t comp)
{
    GS_REQUIRE_NONNULL(vec);
    GS_REQUIRE_NONNULL(comp);

    if (!vec->is_sorted) {
        mergesort(vec->data, vec->num_elements, vec->sizeof_element, comp);
        vec->is_sorted = true;
    }
}

void *gs_vec_bsearch(gs_vec_t *vec, const void *needle, gs_comp_t sort_comp, gs_comp_t find_comp)
{
    GS_REQUIRE_NONNULL(vec);
    GS_REQUIRE_NONNULL(needle);
    GS_REQUIRE_NONNULL(sort_comp);
    GS_REQUIRE_NONNULL(find_comp);

    if (vec->num_elements == 0) {
        return gs_vec_end(vec);
    } else if (vec->num_elements == 1) {
        return (find_comp(needle, gs_vec_at(vec, 0)) == 0 ? gs_vec_begin(vec) : gs_vec_end(vec));
    } else {
        gs_vec_sort(vec, sort_comp);

        int64_t lower = 0;
        int64_t upper = vec->num_elements - 1;
        int64_t mid;

        while(lower <= upper) {
            mid = (lower + upper) / 2;
            void *element = vec->data + mid * vec->sizeof_element;
            int comp = find_comp(needle, element);
            if(comp == 0) {
                return element;
            } else {
                if(comp < 0) {
                    upper = mid - 1;
                } else {
                    lower = mid + 1;
                }
            }
        }

        return gs_vec_end(vec);
    }
}

bool gs_vec_comp(const gs_vec_t *lhs, const gs_vec_t *rhs, gs_comp_t comp)
{
    GS_REQUIRE_NONNULL(lhs)
    GS_REQUIRE_NONNULL(rhs)
    if ((lhs->num_elements == rhs->num_elements) && (lhs->sizeof_element == rhs->sizeof_element)) {
        size_t step = lhs->sizeof_element;
        size_t end = lhs->num_elements * step;
        for (void *lp = lhs->data; lp != lhs->data + end; lp += step) {
            bool has_found = false;
            for (void *rp = rhs->data; rp != rhs->data + end; rp += step) {
                if (comp(lp, rp) == 0) {
                    has_found = true;
                    goto break_inner_loop;
                }
            }
break_inner_loop:
            if (!has_found) {
                return false;
            }
        }
    }

    return true;
}


void gs_vec_free__str(gs_vec_t *vec)
{
    if (gs_vec_foreach(vec, NULL, gs_free_strings)) {
        gs_vec_free(vec);
    }
}

// ---------------------------------------------------------------------------------------------------------------------
// H E L P E R  I M P L E M E N T A T I O N
// ---------------------------------------------------------------------------------------------------------------------

bool gs_free_strings(void *capture, void *begin, void *end)
{
    GS_REQUIRE_NONNULL(begin)
    GS_REQUIRE_NONNULL(end)
    REQUIRE_LESSTHAN(begin, end)
    for (char **it = (char **) begin; it < (char **) end; it++) {
        free (*it);
    }
    return true;
};

bool gs_get_sizeof_strings(void *capture, void *begin, void *end)
{
    GS_REQUIRE_NONNULL(begin)
    GS_REQUIRE_NONNULL(end)
    REQUIRE_LESSTHAN(begin, end)

    size_t total_size = 0;
    for (char **it = (char **) begin; it < (char **) end; it++) {
        total_size += (strlen (*it) + 1);
    }
    *((size_t *) capture) = total_size;
    return true;
};

 bool check_create_args(size_t size, gs_vector_flags_e flags, float grow_factor)
{
    bool valid_args = (size > 0) && (((flags & auto_resize) != auto_resize) || (grow_factor > 1));
    gs_error_if(!valid_args, err_illegal_args);
    return valid_args;
}

 gs_vec_t *alloc_vector()
{
    gs_vec_t *result = GS_REQUIRE_MALLOC (sizeof(gs_vec_t));
    gs_error_if((result == NULL), err_bad_malloc);
    return result;
}

 gs_vec_t *alloc_data(gs_vec_t *vec, gs_vector_flags_e flags, size_t capacity, size_t size)
{
    if (__builtin_expect((vec != NULL) && (vec->data = ((flags & zero_memory) == zero_memory) ?
                                                       calloc(capacity, size) :
                                                       GS_REQUIRE_MALLOC(capacity * size)) == NULL, false)) {
        error(err_bad_malloc);
        free(vec);
        return NULL;
    } else return vec;
}

 void init_vector(gs_vec_t *vec, gs_vector_flags_e flags, size_t capacity, size_t size, float factor)
{
    if (__builtin_expect(vec != NULL, true)) {
        vec->flags = flags;
        vec->element_capacity = capacity;
        vec->sizeof_element = size;
        vec->grow_factor = factor;
        vec->num_elements = 0;
        vec->is_sorted = true;
        apr_pool_create(&vec->pool, NULL);
    }
}

 bool check_add_args(gs_vec_t *vec, size_t num_elements, const void *data)
{
    bool result = ((vec != NULL) && (num_elements > 0) && (data != NULL));
    gs_error_if(!result, err_illegal_args);
    return result;
}

 bool check_auto_resize(gs_vec_t *vec, size_t num_elements)
{
    bool result = ((vec->sizeof_element + num_elements < vec->element_capacity) ||
                   (vec->flags & auto_resize) == auto_resize);
    gs_error_if(!result, err_illegal_opp);
    return result;
}

 bool check_set_args(gs_vec_t *vec, const void *data)
{
    bool result = (vec != NULL && data != NULL);
    gs_error_if(!result, err_illegal_args);
    return result;
}

 bool outside_bounds_enabled(gs_vec_t *vec, size_t idx, size_t num_elements)
{
    bool result = (vec != NULL &&
                   ((idx + num_elements < vec->num_elements) || ((vec->flags & auto_resize) == auto_resize)));
    gs_error_if(!result, err_illegal_args);
    return result;
}

 bool realloc_vector(gs_vec_t *vec, size_t new_num_elements)
{
    if (new_num_elements >= vec->element_capacity) {
        while (new_num_elements >= vec->element_capacity)
            vec->element_capacity = ceil(vec->element_capacity * vec->grow_factor);

        if ((vec->data = realloc(vec->data, vec->element_capacity * vec->sizeof_element)) == NULL) {
            error(err_bad_realloc);
            return false;
        } else {
            if ((vec->flags & zero_memory) == zero_memory) {
                void *base = vec->data + vec->num_elements * vec->sizeof_element;
                memset(base, 0, (vec->element_capacity - vec->num_elements) * vec->sizeof_element);
            }
            return true;
        }
    } else return true;
}

 bool advance(gs_vec_t *vec, size_t idx, size_t num_elements)
{
    if ((idx + num_elements) < vec->num_elements) {
        return true;
    } else if (outside_bounds_enabled(vec, idx, num_elements)) {
        if (idx < vec->num_elements)
            return advance(vec, vec->num_elements, num_elements - vec->num_elements);
        else return realloc_vector(vec, idx + num_elements);
    }
    return false;
}