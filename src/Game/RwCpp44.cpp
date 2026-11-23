/* RenderWare, RwCpp44 (0x8047ECD4-0x80480878): the end of the matrix module (rotations,
 * RwMatrixInvert, RwMatrixScale, RwMatrixTranslate, RwMatrixTransform: not written yet;
 * RwMatrixDestroy and RwMatrixCreate), then the free list module (RwFreeList*), then the start of
 * the memory function vectors (fn_80480214 on). Written from the retail code; the fn_ symbols are
 * kept because game code calls them by address. Built as C: -O4,p, no stmw/lmw, automatic
 * inlining in definition order. */
#include "Game/rw/r5.h"

#ifdef __cplusplus
extern "C" {
#endif

extern R5ModuleInfo lbl_805F1560; /* the matrix module's */
extern const f32 lbl_805FB2A0;    /* 0.0f */
extern const f32 lbl_805FB2A4;    /* 1.0f */
#define R5MATRIXFREELIST (*(void **)((u8 *)lbl_805F15B4 + lbl_805F1560.globalsOffset))

/* RwMatrixDestroy. */
RwBool fn_8047F958(RwMatrix *matrix) {
    RWSRCGLOBAL(memoryFree)(R5MATRIXFREELIST, matrix);
    return 1;
}

/* RwMatrixCreate: a new identity matrix. */
RwMatrix *fn_8047F994(void) {
    RwMatrix *matrix = (RwMatrix *)RWSRCGLOBAL(memoryAlloc)(R5MATRIXFREELIST, 0x3000D);

    if (matrix != 0) {
        matrix->flags = 3;
        matrix->at.z = lbl_805FB2A4;
        matrix->up.y = lbl_805FB2A4;
        matrix->right.x = lbl_805FB2A4;
        matrix->up.x = lbl_805FB2A0;
        matrix->right.z = lbl_805FB2A0;
        matrix->right.y = lbl_805FB2A0;
        matrix->at.y = lbl_805FB2A0;
        matrix->at.x = lbl_805FB2A0;
        matrix->up.z = lbl_805FB2A0;
        matrix->pos.z = lbl_805FB2A0;
        matrix->pos.y = lbl_805FB2A0;
        matrix->pos.x = lbl_805FB2A0;
        matrix->flags |= 0x20003;
    }
    return matrix;
}

/* The free list module ---------------------------------------------------------------------- */

extern u32 lbl_805EDEA0;           /* the flags of new free lists */
extern R5FreeList *lbl_805F156C;   /* the free list the free lists come from */

/* RwFreeListSetFlags. */
void fn_8047FA28(u32 flags) {
    lbl_805EDEA0 = flags;
}

void *fn_8047FA30(s32 entrySize, s32 entriesPerBlock, s32 alignment, s32 preallocBlocks, void *inPlace,
                  u32 hint);

/* RwFreeListCreate. */
void *fn_8047FBD8(s32 entrySize, s32 entriesPerBlock, s32 alignment, u32 hint) {
    return fn_8047FA30(entrySize, entriesPerBlock, alignment, 1, 0, hint);
}

/* RwFreeListCreateAndPreallocateSpace. */
void *fn_8047FC04(s32 entrySize, s32 entriesPerBlock, s32 alignment, s32 preallocBlocks, void *inPlace,
                  u32 hint) {
    return fn_8047FA30(entrySize, entriesPerBlock, alignment, preallocBlocks, inPlace, hint);
}

/* Frees a free list's blocks and the list itself. */
void fn_8047FC24(R5FreeList *freeList) {
    R5Link *block;

    for (block = freeList->blockList.next; block != &freeList->blockList; block = freeList->blockList.next) {
        block->prev->next = block->next;
        block->next->prev = block->prev;
        RWSRCGLOBAL(rwfree)(block);
    }
    if (!(freeList->flags & 1)) {
        if (lbl_805F156C == freeList || lbl_805F156C == 0) {
            RWSRCGLOBAL(rwfree)(freeList);
        } else {
            RWSRCGLOBAL(memoryFree)(lbl_805F156C, freeList);
        }
    }
}

/* RwFreeListDestroy. */
RwBool fn_8047FCDC(R5FreeList *freeList) {
    freeList->link.prev->next = freeList->link.next;
    freeList->link.next->prev = freeList->link.prev;
    fn_8047FC24(freeList);
    return 1;
}

void fn_803A2DC4(void);
void fn_803A2CC8(void);
void fn_803A2C40(void);

void fn_80480214(void) {
    fn_803A2DC4();
}

void fn_80480234(void) {
    fn_803A2CC8();
}

void fn_80480254(void) {
    fn_803A2C40();
}

/* The memory function vectors (RwEngineInstance + 0x108). */
void *fn_80480530(void) {
    return &RWSRCGLOBAL(rwmalloc);
}

#ifdef __cplusplus
}
#endif
