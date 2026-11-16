/* Binary search of a sorted array. */

#include "msl_internal.h"

#define table_ptr(i) (((char *)table_base) + (member_size * (i)))

void *bsearch(const void *key, const void *table_base, size_t num_members, size_t member_size,
              int (*compare_members)(const void *, const void *)) {
    size_t low;
    size_t high;
    size_t mid;
    int comparison;
    char *member;

    if (key == NULL || table_base == NULL || num_members == 0 || member_size == 0 ||
        compare_members == NULL) {
        return NULL;
    }

    /* The first member is tested on its own, so the loop's unsigned bounds can start at 1. */
    member = table_ptr(0);
    comparison = compare_members(key, member);

    if (comparison == 0) {
        return member;
    }
    if (comparison < 0) {
        return NULL;
    }

    low = 1;
    high = num_members - 1;

    while (low <= high) {
        mid = (low + high) / 2;
        member = table_ptr(mid);
        comparison = compare_members(key, member);

        if (comparison == 0) {
            return member;
        }
        if (comparison < 0) {
            high = mid - 1;
        } else {
            low = mid + 1;
        }
    }

    return NULL;
}
