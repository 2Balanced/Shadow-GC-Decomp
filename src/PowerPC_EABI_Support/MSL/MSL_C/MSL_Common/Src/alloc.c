/* The heap behind malloc. Memory comes from the system (__sys_alloc) in blocks of at least 64 KB.
 * Requests above 68 bytes are carved out of a block as sub-blocks, kept in a circular list of the
 * free ones; smaller requests come from six pools of fixed-size cells (4 to 68 bytes), whose
 * cell arrays are themselves sub-blocks.
 *
 * A sub-block starts with its size, whose low bits are flags (2: allocated, 4: the sub-block
 * before it is allocated), and its block's address with bit 0 set; a free one also has its size
 * in its last word. A fixed-size cell starts with its pool block's address (bit 0 clear), which
 * is how free() tells the two kinds apart. */

#include "msl_internal.h"

void *__sys_alloc(size_t size);
void __sys_free(void *ptr);

typedef struct Block {
    struct Block *prev_;
    struct Block *next_;
    unsigned long max_size_; /* largest free sub-block, as far as known */
    unsigned long size_;     /* with flag bits */
} Block;

typedef struct SubBlock {
    unsigned long size_;
    Block *bp_;
    struct SubBlock *prev_; /* free list links, only while free */
    struct SubBlock *next_;
} SubBlock;

struct FixSubBlock;

typedef struct FixBlock {
    struct FixBlock *prev_;
    struct FixBlock *next_;
    unsigned long client_size_;
    struct FixSubBlock *start_; /* free cells */
    unsigned long n_allocated_;
} FixBlock;

typedef struct FixSubBlock {
    FixBlock *block_;
    struct FixSubBlock *next_;
} FixSubBlock;

typedef struct FixStart {
    FixBlock *tail_;
    FixBlock *head_;
} FixStart;

typedef struct __mem_pool_obj {
    Block *start_;
    FixStart fix_start[6];
} __mem_pool_obj;

typedef struct __mem_pool {
    void *reserved[14];
} __mem_pool;

static const unsigned long fix_pool_sizes[] = { 4, 12, 20, 36, 52, 68 };

#define max_fix_size 68
#define sub_block_min 80
#define block_min 0x10000

#define SubBlock_size(ths) ((ths)->size_ & 0xFFFFFFF8)
#define SubBlock_block(ths) ((Block *)((unsigned long)((ths)->bp_) & ~0x1))
#define SubBlock_is_free(ths) (!((ths)->size_ & 0x2))
#define SubBlock_from_pointer(ptr) ((SubBlock *)((char *)(ptr) - 8))

#define SubBlock_set_size(ths, sz)                                                 \
    (ths)->size_ &= ~0xFFFFFFF8;                                                   \
    (ths)->size_ |= (sz) & 0xFFFFFFF8;                                             \
    if (SubBlock_is_free((ths)))                                                   \
        *(unsigned long *)((char *)(ths) + (sz) - sizeof(unsigned long)) = (sz)

static inline void SubBlock_construct(SubBlock *ths, unsigned long size, Block *bp, int prev_alloc,
                                      int this_alloc) {
    ths->bp_ = (Block *)((unsigned long)bp | 0x1);
    ths->size_ = size;

    if (prev_alloc) {
        ths->size_ |= 0x4;
    }

    if (this_alloc) {
        ths->size_ |= 0x2;
        *(unsigned long *)((char *)ths + size) |= 0x4;
    } else {
        *(unsigned long *)((char *)ths + size - sizeof(unsigned long)) = size;
    }
}

#define Block_size(ths) ((ths)->size_ & 0xFFFFFFF8)
#define Block_start(ths) \
    (*(SubBlock **)((char *)(ths) + Block_size((ths)) - sizeof(unsigned long)))

#define FixSubBlock_from_pointer(ptr) ((FixSubBlock *)((char *)(ptr) - 4))

/* Bit 0 of the word before the client's memory: set for a sub-block, clear for a fixed cell. */
#define classify(ptr) (*(unsigned long *)((char *)(ptr) - sizeof(unsigned long)) & 1)

#define __msize_inline(ptr)                                                     \
    (!classify(ptr) ? FixSubBlock_from_pointer(ptr)->block_->client_size_       \
                    : SubBlock_size(SubBlock_from_pointer(ptr)) - 8)

/* Joins a free sub-block with the one before it, if that is free too. */
static inline SubBlock *SubBlock_merge_prev(SubBlock *ths, SubBlock **start) {
    unsigned long prevsz;
    SubBlock *p;

    if (!(ths->size_ & 0x04)) {
        prevsz = *(unsigned long *)((char *)ths - sizeof(unsigned long));
        if (prevsz & 0x2) {
            return ths;
        }
        p = (SubBlock *)((char *)ths - prevsz);
        SubBlock_set_size(p, prevsz + SubBlock_size(ths));

        if (*start == ths) {
            *start = (*start)->next_;
        }
        ths->next_->prev_ = ths->prev_;
        ths->next_->prev_->next_ = ths->next_;
        return p;
    }
    return ths;
}

/* Joins a sub-block with the one after it, if that one is free. */
static inline void SubBlock_merge_next(SubBlock *pBlock, SubBlock **pStart) {
    SubBlock *next_sub_block;
    unsigned long this_cur_size;

    next_sub_block = (SubBlock *)((char *)pBlock + (pBlock->size_ & 0xFFFFFFF8));

    if (!(next_sub_block->size_ & 0x2)) {
        this_cur_size = (pBlock->size_ & 0xFFFFFFF8) + (next_sub_block->size_ & 0xFFFFFFF8);

        pBlock->size_ &= ~0xFFFFFFF8;
        pBlock->size_ |= this_cur_size & 0xFFFFFFF8;

        if (!(pBlock->size_ & 0x2)) {
            *(unsigned long *)((char *)(pBlock) + (this_cur_size) - 4) = (this_cur_size);
        }

        if (!(pBlock->size_ & 0x2)) {
            *(unsigned long *)((char *)pBlock + this_cur_size) &= ~0x4;
        } else {
            *(unsigned long *)((char *)pBlock + this_cur_size) |= 0x4;
        }

        if (*pStart == next_sub_block) {
            *pStart = (*pStart)->next_;
        }

        if (*pStart == next_sub_block) {
            *pStart = 0;
        }

        next_sub_block->next_->prev_ = next_sub_block->prev_;
        next_sub_block->prev_->next_ = next_sub_block->next_;
    }
}

/* Marks a sub-block free and puts it on its block's free list, joined with its neighbours. */
static inline void Block_link(Block *ths, SubBlock *sb) {
    SubBlock **st;

    {
        unsigned long this_size = SubBlock_size(sb);

        sb->size_ &= ~0x2;
        *(unsigned long *)((char *)sb + this_size) &= ~0x4;
        *(unsigned long *)((char *)sb + this_size - sizeof(unsigned long)) = this_size;
    }

    st = &Block_start(ths);

    if (*st != 0) {
        sb->prev_ = (*st)->prev_;
        sb->prev_->next_ = sb;
        sb->next_ = *st;
        (*st)->prev_ = sb;
        *st = sb;
        *st = SubBlock_merge_prev(*st, st);
        SubBlock_merge_next(*st, st);
    } else {
        *st = sb;
        sb->prev_ = sb;
        sb->next_ = sb;
    }

    if (ths->max_size_ < SubBlock_size(*st)) {
        ths->max_size_ = SubBlock_size(*st);
    }
}

/* Marks a sub-block allocated and takes it off the free list. */
static inline void Block_unlink(Block *ths, SubBlock *sb) {
    SubBlock **st;

    {
        unsigned long this_size = SubBlock_size(sb);

        sb->size_ |= 0x2;
        *(unsigned long *)((char *)sb + this_size) |= 0x4;
    }

    st = &Block_start(ths);

    if (*st == sb) {
        *st = sb->next_;
    }

    if (*st == sb) {
        *st = 0;
        ths->max_size_ = 0;
    } else {
        sb->next_->prev_ = sb->prev_;
        sb->prev_->next_ = sb->next_;
    }
}

/* Cuts a sub-block down to `sz` bytes; the rest becomes a sub-block of its own, in the same
 * state. */
static inline SubBlock *SubBlock_split(SubBlock *ths, unsigned long sz) {
    unsigned long origsize = SubBlock_size(ths);
    int isfree = SubBlock_is_free(ths);
    int isprevalloc = ths->size_ & 0x4;
    SubBlock *np = (SubBlock *)((char *)ths + sz);
    Block *bp = SubBlock_block(ths);

    SubBlock_construct(ths, sz, bp, isprevalloc, !isfree);
    SubBlock_construct(np, origsize - sz, bp, !isfree, !isfree);

    if (isfree) {
        np->next_ = ths->next_;
        np->next_->prev_ = np;
        np->prev_ = ths;
        ths->next_ = np;
    }

    return np;
}

/* A new block is one free sub-block, between the block header and a trailer of two words: the
 * block's size and the head of its free list. */
static void Block_construct(Block *ths, unsigned long size) {
    SubBlock *sb;

    ths->size_ = size | 0x3;
    *(unsigned long *)((char *)ths + size - 8) = ths->size_;
    sb = (SubBlock *)((char *)ths + 16);
    SubBlock_construct(sb, size - 24, ths, 0, 0);
    ths->max_size_ = size - 24;
    Block_start(ths) = 0;
    Block_link(ths, sb);
}

/* Takes a sub-block of at least `size` bytes from the block's free list (first fit). */
static SubBlock *Block_subBlock(Block *ths, unsigned long size) {
    SubBlock *st;
    SubBlock *sb;
    unsigned long sb_size;
    unsigned long max_found;

    st = Block_start(ths);

    if (st == 0) {
        ths->max_size_ = 0;
        return 0;
    }

    sb = st;
    sb_size = SubBlock_size(sb);
    max_found = sb_size;

    while (sb_size < size) {
        sb = sb->next_;
        sb_size = SubBlock_size(sb);

        if (max_found < sb_size) {
            max_found = sb_size;
        }

        if (sb == st) {
            ths->max_size_ = max_found;
            return 0;
        }
    }

    if (sb_size - size >= sub_block_min) {
        SubBlock_split(sb, size);
    }

    Block_start(ths) = sb->next_;
    Block_unlink(ths, sb);

    return sb;
}

/* Gets a block from the system and puts it at the head of the pool's block list. */
static Block *link_new_block(__mem_pool_obj *pool_obj, unsigned long size) {
    Block *bp;

    size = (size + 24 + 7) & 0xFFFFFFF8;

    if (size < block_min) {
        size = block_min;
    }

    bp = (Block *)__sys_alloc(size);

    if (bp == 0) {
        return 0;
    }

    Block_construct(bp, size);

    if (pool_obj->start_ != 0) {
        bp->prev_ = pool_obj->start_->prev_;
        bp->prev_->next_ = bp;
        bp->next_ = pool_obj->start_;
        pool_obj->start_->prev_ = bp;
        pool_obj->start_ = bp;
    } else {
        pool_obj->start_ = bp;
        bp->prev_ = bp;
        bp->next_ = bp;
    }

    return bp;
}

static void *allocate_from_var_pools(__mem_pool_obj *pool_obj, unsigned long size) {
    Block *bp;
    SubBlock *ptr;

    size = (size + 8 + 7) & 0xFFFFFFF8;

    if (size < sub_block_min) {
        size = sub_block_min;
    }

    bp = pool_obj->start_ != 0 ? pool_obj->start_ : link_new_block(pool_obj, size);

    if (bp == 0) {
        return 0;
    }

    do {
        if (size <= bp->max_size_) {
            ptr = Block_subBlock(bp, size);

            if (ptr != 0) {
                pool_obj->start_ = bp;
                goto done;
            }
        }

        bp = bp->next_;
    } while (bp != pool_obj->start_);

    bp = link_new_block(pool_obj, size);

    if (bp == 0) {
        return 0;
    }

    ptr = Block_subBlock(bp, size);

done:
    return (char *)ptr + 8;
}

/* The same without asking the system for more: on failure `*max_size` is the largest request
 * that could have been met. */
static void *soft_allocate_from_var_pools(__mem_pool_obj *pool_obj, unsigned long size,
                                          unsigned long *max_size) {
    Block *bp;
    SubBlock *ptr;

    size = (size + 8 + 7) & 0xFFFFFFF8;

    if (size < sub_block_min) {
        size = sub_block_min;
    }

    *max_size = 0;
    bp = pool_obj->start_;

    if (bp == 0) {
        return 0;
    }

    do {
        if (size <= bp->max_size_) {
            ptr = Block_subBlock(bp, size);

            if (ptr != 0) {
                pool_obj->start_ = bp;
                goto done;
            }
        }

        if (bp->max_size_ > 8 && *max_size < bp->max_size_ - 8) {
            *max_size = bp->max_size_ - 8;
        }

        bp = bp->next_;
    } while (bp != pool_obj->start_);

    return 0;

done:
    return (char *)ptr + 8;
}

static void deallocate_from_var_pools(__mem_pool_obj *pool_obj, void *ptr) {
    SubBlock *sb = SubBlock_from_pointer(ptr);
    SubBlock *first;
    Block *bp = SubBlock_block(sb);
    Block *bn;
    int empty;

    Block_link(bp, sb);

    /* A block whose one sub-block is free again goes back to the system. */
    first = (SubBlock *)((char *)bp + 16);
    empty = 0;

    if (SubBlock_is_free(first) && SubBlock_size(first) == Block_size(bp) - 24) {
        empty = 1;
    }

    if (empty) {
        bn = bp->next_;

        if (bn == bp) {
            bn = 0;
        }

        if (pool_obj->start_ == bp) {
            pool_obj->start_ = bn;
        }

        if (bn != 0) {
            bn->prev_ = bp->prev_;
            bn->prev_->next_ = bn;
        }

        bp->next_ = 0;
        bp->prev_ = 0;
        __sys_free(bp);
    }
}

static inline void FixSubBlock_construct(FixSubBlock *ths, FixBlock *block, FixSubBlock *next) {
    ths->block_ = block;
    ths->next_ = next;
}

/* Lays a pool block out as a chain of free cells and links it between `prev` and `next`. */
static inline void FixBlock_construct(FixBlock *ths, FixBlock *prev, FixBlock *next, unsigned long index,
                               FixSubBlock *chunk, unsigned long chunk_size) {
    unsigned long fixSubBlock_size;
    unsigned long n;
    unsigned long k;
    char *p;
    char *np;

    ths->prev_ = prev;
    ths->next_ = next;
    prev->next_ = ths;
    next->prev_ = ths;
    ths->client_size_ = fix_pool_sizes[index];
    fixSubBlock_size = fix_pool_sizes[index] + 4;
    n = chunk_size / fixSubBlock_size;
    p = (char *)chunk;

    for (k = 0; k < n - 1; ++k) {
        np = p + fixSubBlock_size;
        FixSubBlock_construct((FixSubBlock *)p, ths, (FixSubBlock *)np);
        p = np;
    }

    FixSubBlock_construct((FixSubBlock *)p, ths, 0);
    ths->start_ = chunk;
    ths->n_allocated_ = 0;
}

static void *allocate_from_fixed_pools(__mem_pool_obj *pool_obj, unsigned long size) {
    unsigned long i = 0;
    FixSubBlock *p;
    FixStart *fs;

    while (size > fix_pool_sizes[i]) {
        ++i;
    }

    fs = &pool_obj->fix_start[i];

    if (fs->head_ == 0 || fs->head_->start_ == 0) {
        char *newblock;
        unsigned long size_received;
        unsigned long max_avail;
        unsigned long n;
        unsigned long nsave;

        /* A pool block holds up to 256 cells in about 4 KB; when memory is fragmented, as few
         * as ten are accepted before another block is requested from the system. */
        n = 0xFEC / (fix_pool_sizes[i] + 4);

        if (n > 256) {
            n = 256;
        }

        nsave = n;

        while (n >= 10) {
            newblock = (char *)soft_allocate_from_var_pools(
                pool_obj, n * (fix_pool_sizes[i] + 4) + sizeof(FixBlock), &max_avail);

            if (newblock != 0) {
                break;
            }

            if (max_avail > sizeof(FixBlock)) {
                n = (max_avail - sizeof(FixBlock)) / (fix_pool_sizes[i] + 4);
            } else {
                n = 0;
            }
        }

        if (newblock == 0 && n < nsave) {
            n = nsave;
            newblock = (char *)allocate_from_var_pools(
                pool_obj, n * (fix_pool_sizes[i] + 4) + sizeof(FixBlock));

            if (newblock == 0) {
                return 0;
            }
        }

        size_received = __msize_inline(newblock);

        if (fs->head_ == 0) {
            fs->head_ = (FixBlock *)newblock;
            fs->tail_ = (FixBlock *)newblock;
        }

        FixBlock_construct((FixBlock *)newblock, fs->tail_, fs->head_, i,
                           (FixSubBlock *)(newblock + sizeof(FixBlock)),
                           size_received - sizeof(FixBlock));
        fs->head_ = (FixBlock *)newblock;
    }

    p = fs->head_->start_;
    fs->head_->start_ = p->next_;
    ++fs->head_->n_allocated_;

    /* A block with no free cell left moves to the tail. */
    if (fs->head_->start_ == 0) {
        fs->head_ = fs->head_->next_;
        fs->tail_ = fs->tail_->next_;
    }

    return (char *)p + 4;
}

static void deallocate_from_fixed_pools(__mem_pool_obj *pool_obj, void *ptr, unsigned long size) {
    unsigned long i = 0;
    FixSubBlock *p;
    FixBlock *b;
    FixStart *fs;

    while (size > fix_pool_sizes[i]) {
        ++i;
    }

    fs = &pool_obj->fix_start[i];
    p = FixSubBlock_from_pointer(ptr);
    b = p->block_;

    /* A full block gets a free cell: it moves to the head. */
    if (b->start_ == 0 && fs->head_ != b) {
        if (fs->tail_ == b) {
            fs->head_ = fs->head_->prev_;
            fs->tail_ = fs->tail_->prev_;
        } else {
            b->prev_->next_ = b->next_;
            b->next_->prev_ = b->prev_;
            b->next_ = fs->head_;
            b->prev_ = b->next_->prev_;
            b->prev_->next_ = b;
            b->next_->prev_ = b;
            fs->head_ = b;
        }
    }

    p->next_ = b->start_;
    b->start_ = p;

    /* A block with no cell in use goes back to the general heap. */
    if (--b->n_allocated_ == 0) {
        if (fs->head_ == b) {
            fs->head_ = b->next_;
        }

        if (fs->tail_ == b) {
            fs->tail_ = b->prev_;
        }

        b->prev_->next_ = b->next_;
        b->next_->prev_ = b->prev_;

        if (fs->head_ == b) {
            fs->head_ = 0;
        }

        if (fs->tail_ == b) {
            fs->tail_ = 0;
        }

        deallocate_from_var_pools(pool_obj, b);
    }
}

void *__pool_alloc(__mem_pool *pool, size_t size) {
    __mem_pool_obj *pool_obj;

    if (size == 0) {
        return 0;
    }

    if (size > 0xFFFFFFCF) {
        return 0;
    }

    pool_obj = (__mem_pool_obj *)pool;

    if (size <= max_fix_size) {
        return allocate_from_fixed_pools(pool_obj, size);
    }

    return allocate_from_var_pools(pool_obj, size);
}

void __pool_free(__mem_pool *pool, void *ptr) {
    __mem_pool_obj *pool_obj;
    unsigned long size;

    if (ptr == 0) {
        return;
    }

    pool_obj = (__mem_pool_obj *)pool;
    size = __msize_inline(ptr);

    if (size <= max_fix_size) {
        deallocate_from_fixed_pools(pool_obj, ptr, size);
    } else {
        deallocate_from_var_pools(pool_obj, ptr);
    }
}

void *__pool_realloc(__mem_pool *pool, void *ptr, size_t size) {
    __mem_pool_obj *pool_obj;
    void *newp;
    size_t current_size;
    size_t sz;
    SubBlock *sb;

    if (ptr == 0) {
        return __pool_alloc(pool, size);
    }

    if (size == 0) {
        __pool_free(pool, ptr);
        return 0;
    }

    pool_obj = (__mem_pool_obj *)pool;
    current_size = __msize_inline(ptr);

    if (size > current_size) {
        /* A sub-block can grow in place into a free one behind it. */
        if (classify(ptr)) {
            if (size > 0xFFFFFFCF) {
                return 0;
            }

            sz = (size + 8 + 7) & 0xFFFFFFF8;

            if (sz < sub_block_min) {
                sz = sub_block_min;
            }

            sb = SubBlock_from_pointer(ptr);
            SubBlock_merge_next(sb, &Block_start(SubBlock_block(sb)));

            if (SubBlock_size(sb) >= sz) {
                if (SubBlock_size(sb) - sz >= sub_block_min) {
                    Block_link(SubBlock_block(sb), SubBlock_split(sb, sz));
                }

                return ptr;
            }
        }

        newp = __pool_alloc(pool, size);

        if (newp == 0) {
            return 0;
        }

        memcpy(newp, ptr, current_size);
        __pool_free(pool, ptr);

        return newp;
    }

    /* Shrinking gives the tail of a sub-block back when it is worth a sub-block. */
    if (classify(ptr)) {
        size = (size + 8 + 7) & 0xFFFFFFF8;

        if (size < sub_block_min) {
            size = sub_block_min;
        }

        sb = SubBlock_from_pointer(ptr);

        if (SubBlock_size(sb) - size >= sub_block_min) {
            Block_link(SubBlock_block(sb), SubBlock_split(sb, size));
        }
    }

    return ptr;
}

void *__pool_alloc_clear(__mem_pool *pool, size_t size) {
    void *result = __pool_alloc(pool, size);

    if (result != 0) {
        memset(result, 0, size);
    }

    return result;
}

static __mem_pool protopool;
static unsigned char init = 0;

static inline void __init_pool_obj(__mem_pool *pool_obj) {
    memset(pool_obj, 0, sizeof(__mem_pool_obj));
}

static inline __mem_pool *get_malloc_pool(void) {
    if (!init) {
        __init_pool_obj(&protopool);
        init = 1;
    }

    return &protopool;
}

void *malloc(size_t size) {
    void *result;

    __begin_critical_region(malloc_pool_access);
    result = __pool_alloc(get_malloc_pool(), size);
    __end_critical_region(malloc_pool_access);

    return result;
}

void free(void *ptr) {
    __begin_critical_region(malloc_pool_access);
    __pool_free(get_malloc_pool(), ptr);
    __end_critical_region(malloc_pool_access);
}

void *realloc(void *ptr, size_t size) {
    void *result;

    __begin_critical_region(malloc_pool_access);
    result = __pool_realloc(get_malloc_pool(), ptr, size);
    __end_critical_region(malloc_pool_access);

    return result;
}

void *calloc(size_t nmemb, size_t size) {
    void *result;

    __begin_critical_region(malloc_pool_access);
    result = __pool_alloc_clear(get_malloc_pool(), size * nmemb);
    __end_critical_region(malloc_pool_access);

    return result;
}
