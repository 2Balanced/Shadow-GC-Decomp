#ifndef SIZE_ASSERT_H_
#define SIZE_ASSERT_H_

/* Layout checks that cost nothing at run time:
 *
 *   SIZE_ASSERT(T, n)          sizeof(T) has to be n
 *   OFFSET_ASSERT(T, field, n) T::field has to start n bytes into T
 *
 * Each check declares an array typedef whose bound is 1 when the check holds and -1 (a compile
 * error) when it does not. The typedef is named after the source line, so checks can share a
 * scope. */
#define SIZE_ASSERT_PASTE_(a, b) a##b
#define SIZE_ASSERT_PASTE(a, b) SIZE_ASSERT_PASTE_(a, b)

#define SIZE_ASSERT(T, n)                                                                          \
    typedef int SIZE_ASSERT_PASTE(size_assert_at_line_, __LINE__)[(sizeof(T) == (n)) * 2 - 1]

#define OFFSET_ASSERT(T, field, n)                                                                 \
    typedef int SIZE_ASSERT_PASTE(offset_assert_at_line_, __LINE__)                                \
        [(((unsigned long)&((T *)0)->field) == (n)) * 2 - 1]

#endif
