#include "easysort.h"

/**
 * @brief bubble sort and quick sort
 *
 * @param array  start of target array.
 * @param count array size in elements.
 * @param size  element size in bytes.
 * @param cmp   pointer to a user-supplied routine that compares two array elements
 *              and returns a value that specifies their relationship.
 *
 * @example s32 compare(RO void* p1, RO void* p2) { return (*(s32*)p1 - *(s32*)p2); }
 *     @retval -1, 0, +1
 *
 * @return N/A
 */

void _bsort(void* array, u32 count, u32 size, sort_cmp_t cmp)
{
    u32 i, j;

    if (size < 2 || count < 2) { return; }

    for (i = 0; i < count - 1; ++i)
    {
        for (j = 0; j < count - i - 1; ++j)
        {
            u8* p1 = (u8*)array + j * size;
            u8* p2 = p1 + size;
            if (cmp(p1, p2) > 0)
            {
                _memswap(p1, p2, size);
            }
        }
    }
}

void _qsort(void* array, u32 count, u32 size, sort_cmp_t cmp)
{
    if (count <= 1)
    {
        return;
    }

    u8* base  = (u8*)array;
    u8* left  = base;
    u8* right = base + (count - 1) * size;
    u8* pivot = base + (count / 2) * size;

    while (left <= right)
    {
        while (cmp(left, pivot) < 0) { left += size; }
        while (cmp(right, pivot) > 0) { right -= size; }
        if (left <= right)
        {
            _memswap(left, right, size);
            left += size;
            right -= size;
        }
    }

    _qsort(base, (right - base) / size + 1, size, cmp);
    _qsort(left, (base + count * size - left) / size, size, cmp);
}
