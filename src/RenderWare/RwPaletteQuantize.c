/* RenderWare core: the palette quantiser (0x8048D85C-0x8049043C).
 * Colours go into an octree of 16-way nodes (one address bit per channel and level, from a table
 * that spreads a channel's bits one per nibble), weighted so that red counts 0.509 and blue 0.194;
 * resolving a palette sums the tree's moments and either takes every leaf or cuts the colour
 * space into boxes of least variance (Wu), then maps each image pixel to its leaf's index.
 * The file owns the tree depth (.sdata, padded to 8), the bit-spreading table (.bss), the
 * initialiser of the black the sums start from (.sbss2, a local's all-zero initialiser) and its
 * constant pool (.sdata2 0x805FB360-0x805FB380).
 * Boundary: the file starts with its helpers at 0x8048D85C (after _rwObjectHasFrameReleaseFrame)
 * and ends at 0x8049043C, where the GameCube driver's immediate-mode 2D file starts with its own
 * constant pool (0x805FB380): that tail (0x8048F5D0-0x8049043C) was cut into old Game/RwCpp54.
 * The recursive walks are inlined into themselves several levels deep, as in retail; the depth
 * (and, where retail inlined a larger body than GC/2.7's default allows, the size limit) is set
 * per function with #pragma inline_depth/inline_max_size, and the box cut is kept out of
 * automatic inlining (retail calls it). Several helpers take pointers to non-const (the const
 * lever: their loads stay behind the prologue and the stores).
 * NonMatching (8 of 21): fn_8048E300 and fn_8048E47C (the box tests: retail computes the four
 * differences of each test before branching, and the last test of a group jumps over a branch;
 * no spelling tried reproduces both), fn_8048E748 (float registers of the two halves, 45 rows),
 * fn_8048EB24 (164 rows), fn_8048F1E4 (registers of the inlined leaf colour, 29 rows),
 * fn_8048F5D0 (registers, 148 rows), fn_8048FAB0 (99 rows).
 * C, cflags_rw_core (configure.py). */
#include "Game/rw/r6.h"

RwFreeList *fn_8047FBD8(s32 entrySize, s32 entriesPerBlock, s32 alignment, u32 hint);
RwBool fn_8047FCDC(RwFreeList *list);

static s32 palQuantDepth = 6;
static u32 palQuantSpread[256];

#define PALQUANTADDRESS(c, shift)                                                                  \
    ((palQuantSpread[(c)->red >> (shift)] << 3) | (palQuantSpread[(c)->green >> (shift)] << 2) |   \
     (palQuantSpread[(c)->blue >> (shift)] << 1) | palQuantSpread[(c)->alpha >> (shift)])

/* Takes the channel weights back out of a colour. */
void fn_8048D85C(RwRGBAReal *color) {
    color->red /= 0.5093697f;
    color->blue /= 0.19420783f;
}

/* An empty leaf. */
void fn_8048D880(rwPalQuantLeaf *leaf) {
    leaf->index = 0;
    leaf->weight = 0.0f;
    leaf->r = 0.0f;
    leaf->g = 0.0f;
    leaf->b = 0.0f;
    leaf->a = 0.0f;
    leaf->var = 0.0f;
}

/* Adds a colour, weighted by the leaf's weight, to a leaf's sums. */
void fn_8048D8A8(rwPalQuantLeaf *leaf, const RwRGBA *color) {
    RwRGBAReal c;
    f32 weight;

    leaf->var -= (leaf->r * leaf->r + leaf->g * leaf->g + leaf->b * leaf->b + leaf->a * leaf->a) /
                 leaf->weight;
    if (leaf->var < 0.0f) {
        leaf->var = 0.0f;
    }
    c.red = (1.0f / 255.0f) * color->red;
    c.green = (1.0f / 255.0f) * color->green;
    c.blue = (1.0f / 255.0f) * color->blue;
    c.alpha = (1.0f / 255.0f) * color->alpha;
    c.red *= 0.5093697f;
    c.blue *= 0.19420783f;
    weight = leaf->weight;
    c.red *= weight;
    c.green *= weight;
    c.blue *= weight;
    c.alpha *= weight;
    leaf->r += c.red;
    leaf->g += c.green;
    leaf->b += c.blue;
    leaf->a += c.alpha;
}

/* Merges two leaves; the variance grows by the distance between their means. */
void fn_8048D9C8(rwPalQuantLeaf *dst, rwPalQuantLeaf *a, rwPalQuantLeaf *b) {
    dst->var = a->var + b->var;
    if (a->weight > 0.0f && b->weight > 0.0f) {
        f32 ia = 1.0f / a->weight;
        f32 ib = 1.0f / b->weight;
        RwRGBAReal ca, cb;

        ca.red = a->r * ia;
        ca.green = a->g * ia;
        ca.blue = a->b * ia;
        ca.alpha = a->a * ia;
        cb.red = b->r * ib;
        cb.green = b->g * ib;
        cb.blue = b->b * ib;
        cb.alpha = b->a * ib;
        ca.red -= cb.red;
        ca.green -= cb.green;
        ca.blue -= cb.blue;
        ca.alpha -= cb.alpha;
        dst->var += (ca.red * ca.red + ca.green * ca.green + ca.blue * ca.blue +
                     ca.alpha * ca.alpha) / (ia + ib);
    }
    dst->r = a->r + b->r;
    dst->g = a->g + b->g;
    dst->b = a->b + b->b;
    dst->a = a->a + b->a;
    dst->weight = a->weight + b->weight;
}

/* The colour a leaf stands for. */
void fn_8048DAC8(RwRGBA *color, rwPalQuantLeaf *leaf) {
    RwRGBAReal c;
    f32 recip = 1.0f / leaf->weight;
    s32 r, g, b, a;

    c.red = leaf->r * recip;
    c.green = leaf->g * recip;
    c.blue = leaf->b * recip;
    c.alpha = leaf->a * recip;
    fn_8048D85C(&c);
    r = (s32)(255.99f * c.red);
    g = (s32)(255.99f * c.green);
    b = (s32)(255.99f * c.blue);
    a = (s32)(255.99f * c.alpha);

    color->red = 255;
    if (r < 255) {
        color->red = r;
    }
    color->green = 255;
    if (g < 255) {
        color->green = g;
    }
    color->blue = 255;
    if (b < 255) {
        color->blue = b;
    }
    color->alpha = 255;
    if (a < 255) {
        color->alpha = a;
    }
}

/* No children yet. */
void fn_8048DBA4(rwPalQuantNode **child) {
    child[0] = NULL;
    child[1] = NULL;
    child[2] = NULL;
    child[3] = NULL;
    child[4] = NULL;
    child[5] = NULL;
    child[6] = NULL;
    child[7] = NULL;
    child[8] = NULL;
    child[9] = NULL;
    child[10] = NULL;
    child[11] = NULL;
    child[12] = NULL;
    child[13] = NULL;
    child[14] = NULL;
    child[15] = NULL;
}

/* The leaf of a colour address, created on the way down. */
#pragma push
#pragma inline_max_size(800)
#pragma inline_depth(3)
rwPalQuantNode *fn_8048DBEC(rwPalQuant *pq, rwPalQuantNode *node, u32 address, s32 depth) {
    if (!depth) {
        return node;
    }
    if (!node->child[address & 0xF]) {
        rwPalQuantNode *child =
            (rwPalQuantNode *)RwEngineInstance->memoryAlloc(pq->cubeFreeList, 0x30411);

        node->child[address & 0xF] = child;
        fn_8048DBA4(child->child);
        if (depth == 1) {
            fn_8048D880(&child->leaf);
        }
    }
    return fn_8048DBEC(pq, node->child[address & 0xF], address >> 4, depth - 1);
}
#pragma pop

/* _rwPalQuantAddImage: every pixel's colour goes to its leaf with the given weight. */
/* A pixel's weighted colour (the bits below the leaf's address) added to its leaf. */
static void PalQuantLeafAddPixel(rwPalQuantLeaf *leaf, RwRGBA *color, f32 weight) {
    u8 mask = (1 << (8 - palQuantDepth)) - 1;
    RwRGBAReal c;

    c.red = (1.0f / 255.0f) * (u8)(color->red & mask);
    c.green = (1.0f / 255.0f) * (u8)(color->green & mask);
    c.blue = (1.0f / 255.0f) * (u8)(color->blue & mask);
    c.alpha = (1.0f / 255.0f) * (u8)(color->alpha & mask);
    c.red *= 0.5093697f;
    c.blue *= 0.19420783f;
    leaf->weight += weight;
    leaf->var += weight * (c.red * c.red + c.green * c.green + c.blue * c.blue + c.alpha * c.alpha);
    c.red *= weight;
    c.green *= weight;
    c.blue *= weight;
    c.alpha *= weight;
    leaf->r += c.red;
    leaf->g += c.green;
    leaf->b += c.blue;
    leaf->a += c.alpha;
}

void fn_8048DEB0(rwPalQuant *pq, RwImage *image, f32 weight) {
    s32 width;
    s32 height;
    s32 stride;
    u8 *pixels;
    RwRGBA *palette;

    stride = image->stride;
    pixels = image->cpPixels;
    palette = image->palette;
    height = image->height;
    switch (image->depth) {
    case 4:
    case 8:
        while (height--) {
            u8 *pixel = pixels;

            width = image->width;
            while (width--) {
                RwRGBA *color = &palette[*pixel];
                rwPalQuantNode *node = fn_8048DBEC(pq, pq->root,
                    PALQUANTADDRESS(color, 8 - palQuantDepth), palQuantDepth);

                PalQuantLeafAddPixel(&node->leaf, color, weight);
                pixel++;
            }
            pixels += stride;
        }
        break;
    case 32:
        while (height--) {
            RwRGBA *color = (RwRGBA *)pixels;

            width = image->width;
            while (width--) {
                rwPalQuantNode *node = fn_8048DBEC(pq, pq->root,
                    PALQUANTADDRESS(color, 8 - palQuantDepth), palQuantDepth);

                PalQuantLeafAddPixel(&node->leaf, color, weight);
                color++;
            }
            pixels += stride;
        }
        break;
    }
}

/* Gives every leaf of a box the palette index of the box. */
void fn_8048E300(rwPalQuantNode *node, const s32 *pos, s32 level, const rwPalQuantBox *box,
                 u8 index) {
    if (node) {
        s32 size = 1 << level;

        if (pos[0] + size - box->col0[0] > 0 && pos[1] + size - box->col0[1] > 0 &&
            pos[2] + size - box->col0[2] > 0 && pos[3] + size - box->col0[3] > 0 &&
            pos[0] - box->col1[0] < 0 && pos[1] - box->col1[1] < 0 &&
            pos[2] - box->col1[2] < 0 && pos[3] - box->col1[3] < 0) {
            if (!level) {
                node->leaf.index = index;
            } else {
                s32 i;

                for (i = 0; i < 16; i++) {
                    s32 sub[4];

                    sub[0] = pos[0] + (((i >> 3) & 1) << (level - 1));
                    sub[1] = pos[1] + (((i >> 2) & 1) << (level - 1));
                    sub[2] = pos[2] + (((i >> 1) & 1) << (level - 1));
                    sub[3] = pos[3] + ((i & 1) << (level - 1));
                    fn_8048E300(node->child[i], sub, level - 1, box, index);
                }
            }
        }
    }
}

/* Sums the leaves of a box. */
void fn_8048E47C(rwPalQuantNode *node, const s32 *pos, s32 level, const rwPalQuantBox *box,
                 rwPalQuantLeaf *sum) {
    if (node) {
        s32 size = 1 << level;

        if (pos[0] + size - box->col0[0] > 0 && pos[1] + size - box->col0[1] > 0 &&
            pos[2] + size - box->col0[2] > 0 && pos[3] + size - box->col0[3] > 0 &&
            pos[0] - box->col1[0] < 0 && pos[1] - box->col1[1] < 0 &&
            pos[2] - box->col1[2] < 0 && pos[3] - box->col1[3] < 0) {
            if (pos[0] - box->col0[0] >= 0 && pos[1] - box->col0[1] >= 0 &&
                pos[2] - box->col0[2] >= 0 && pos[3] - box->col0[3] >= 0 &&
                box->col1[0] - (pos[0] + size) >= 0 && box->col1[1] - (pos[1] + size) >= 0 &&
                box->col1[2] - (pos[2] + size) >= 0 && box->col1[3] - (pos[3] + size) >= 0) {
                fn_8048D9C8(sum, sum, &node->leaf);
            } else if (level > 0) {
                s32 i;

                for (i = 0; i < 16; i++) {
                    s32 sub[4];

                    sub[0] = pos[0] + (((i >> 3) & 1) << (level - 1));
                    sub[1] = pos[1] + (((i >> 2) & 1) << (level - 1));
                    sub[2] = pos[2] + (((i >> 1) & 1) << (level - 1));
                    sub[3] = pos[3] + ((i & 1) << (level - 1));
                    fn_8048E47C(node->child[i], sub, level - 1, box, sum);
                }
            }
        }
    }
}

/* The best cut of a box along one axis: a binary search for the cut that leaves the least
 * variance in the two halves. */
f32 fn_8048E748(rwPalQuantNode *root, rwPalQuantBox *box, s32 axis, s32 *cut,
                rwPalQuantLeaf *whole) {
    rwPalQuantBox half;
    s32 middle;
    s32 first, last;
    s32 range;
    f32 best;

    best = whole->var;
    half = *box;
    first = box->col0[axis];
    last = box->col1[axis];
    while ((range = last - first) > 1) {
        rwPalQuantLeaf lower;
        s32 pos[4];
        rwPalQuantLeaf upper;
        middle = first + (range >> 1);

        half.col1[axis] = middle;
        pos[0] = 0;
        pos[1] = 0;
        pos[2] = 0;
        pos[3] = 0;
        fn_8048D880(&lower);
        fn_8048E47C(root, pos, palQuantDepth, &half, &lower);
        upper.weight = whole->weight - lower.weight;
        upper.r = whole->r - lower.r;
        upper.g = whole->g - lower.g;
        upper.b = whole->b - lower.b;
        upper.a = whole->a - lower.a;
        upper.var = whole->var - lower.var;
        if (upper.weight > 0.0f && lower.weight > 0.0f) {
            f32 il = 1.0f / lower.weight;
            f32 iu = 1.0f / upper.weight;
            RwRGBAReal cl, cu;

            cl.red = lower.r * il;
            cl.green = lower.g * il;
            cl.blue = lower.b * il;
            cl.alpha = lower.a * il;
            cu.red = upper.r * iu;
            cu.green = upper.g * iu;
            cu.blue = upper.b * iu;
            cu.alpha = upper.a * iu;
            cl.red -= cu.red;
            cl.green -= cu.green;
            cl.blue -= cu.blue;
            cl.alpha -= cu.alpha;
            upper.var -= (cl.red * cl.red + cl.green * cl.green + cl.blue * cl.blue +
                          cl.alpha * cl.alpha) / (il + iu);
        }
        if (0.0f == lower.weight) {
            first = middle;
        } else if (0.0f == upper.weight) {
            last = middle;
        } else {
            if (lower.var + upper.var < best) {
                best = lower.var + upper.var;
                cut[axis] = half.col1[axis];
            }
            if (lower.var < upper.var) {
                first = middle;
            } else {
                last = middle;
            }
        }
    }
    return best;
}

/* Cuts a box in two along the axis whose best cut leaves the least variance. */
#pragma push
#pragma auto_inline off
RwBool fn_8048E988(rwPalQuantNode *root, rwPalQuantLeaf *whole, rwPalQuantBox *box1,
                   rwPalQuantBox *box2) {
    f32 var[4];
    s32 cut[4];
    s32 axis;
    s32 i;

    var[0] = fn_8048E748(root, box1, 0, cut, whole);
    var[1] = fn_8048E748(root, box1, 1, cut, whole);
    var[2] = fn_8048E748(root, box1, 2, cut, whole);
    var[3] = fn_8048E748(root, box1, 3, cut, whole);
    axis = 0;
    for (i = 0; i < 4; i++) {
        if (var[i] < var[axis]) {
            axis = i;
        }
    }
    if (var[axis] < whole->var) {
        *box2 = *box1;
        box1->col1[axis] = box2->col0[axis] = (u8)cut[axis];
        return TRUE;
    }
    return FALSE;
}

#pragma pop
/* The sums of every node from its children, the leaves from their colour address. */
rwPalQuantNode *fn_8048EB24(rwPalQuantNode *node, const RwRGBA *color, s32 level) {
    rwPalQuantNode *result = NULL;

    if (node) {
        result = node;
        if (level > 0) {
            s32 i;

            fn_8048D880(&node->leaf);
            for (i = 0; i < 16; i++) {
                RwRGBA sub;
                rwPalQuantNode *child;
                s32 shift = level - 1 + (8 - palQuantDepth);

                sub.red = color->red + (((i >> 3) & 1) << shift);
                sub.green = color->green + (((i >> 2) & 1) << shift);
                sub.blue = color->blue + (((i >> 1) & 1) << shift);
                sub.alpha = color->alpha + ((i & 1) << shift);
                child = fn_8048EB24(node->child[i], &sub, level - 1);
                if (child) {
                    fn_8048D9C8(&node->leaf, &node->leaf, &child->leaf);
                }
            }
        } else {
            fn_8048D8A8(&node->leaf, color);
        }
    }
    return result;
}

/* The number of leaves under a node. */
#pragma push
#pragma inline_depth(8)
int fn_8048EEFC(rwPalQuantNode *node, int level) {
    int i;
    int count = 0;

    if (node) {
        if (level > 0) {
            for (i = 0; i < 16; i++) {
                count += fn_8048EEFC(node->child[i], level - 1);
            }
        } else {
            count = 1;
        }
    }
    return count;
}

#pragma pop
/* Every leaf gets the next palette index and its colour. */
#pragma push
#pragma inline_max_size(800)
#pragma inline_depth(2)
int fn_8048F1E4(rwPalQuantNode *node, RwRGBA *palette, int index, int level) {
    if (node) {
        if (level > 0) {
            s32 i;

            for (i = 0; i < 16; i++) {
                index = fn_8048F1E4(node->child[i], palette, index, level - 1);
            }
        } else {
            fn_8048DAC8(&palette[index], &node->leaf);
            node->leaf.index = index;
            index++;
        }
    }
    return index;
}

#pragma pop
/* The palette index of a colour address: four bits per level (retail inlines the walk into
 * itself eight levels deep). */
#pragma inline_depth(8)
u8 fn_8048F4B4(rwPalQuantNode *node, u32 address, s32 level) {
    if (level == 0) {
        return node->leaf.index;
    }
    return fn_8048F4B4(node->child[address & 0xF], address >> 4, level - 1);
}
#pragma inline_depth(5)

/* _rwPalQuantResolvePalette: up to maxColors colours, every leaf when there are few enough. */
s32 fn_8048F5D0(RwRGBA *palette, s32 maxColors, rwPalQuant *pq) {
    RwRGBA black = {0, 0, 0, 0};
    s32 numColors;
    s32 i;

    fn_8048EB24(pq->root, &black, palQuantDepth);
    for (i = 0; i < maxColors; i++) {
        palette[i].red = 0;
        palette[i].green = 0;
        palette[i].blue = 0;
        palette[i].alpha = 0;
    }
    numColors = fn_8048EEFC(pq->root, palQuantDepth);
    if (numColors <= maxColors) {
        maxColors = numColors;
        fn_8048F1E4(pq->root, palette, 0, palQuantDepth);
    } else {
        {
            s32 pos[4];

            pq->Mbox[0].col0[0] = 0;
            pq->Mbox[0].col0[1] = 0;
            pq->Mbox[0].col0[2] = 0;
            pq->Mbox[0].col0[3] = 0;
            pq->Mbox[0].col1[0] = 1 << palQuantDepth;
            pq->Mbox[0].col1[1] = 1 << palQuantDepth;
            pq->Mbox[0].col1[2] = 1 << palQuantDepth;
            pq->Mbox[0].col1[3] = 1 << palQuantDepth;
            pos[0] = 0;
            pos[1] = 0;
            pos[2] = 0;
            pos[3] = 0;
            fn_8048D880(&pq->Mvol[0]);
            fn_8048E47C(pq->root, pos, palQuantDepth, &pq->Mbox[0], &pq->Mvol[0]);
            pq->Mvv[0] = pq->Mvol[0].var;
        }
        for (i = 1; i < maxColors; i++) {
            f32 maxVar = 0.0f;
            s32 next = -1;
            s32 k;

            for (k = 0; k < i; k++) {
                if (pq->Mvv[k] > maxVar) {
                    maxVar = pq->Mvv[k];
                    next = k;
                }
            }
            if (next == -1) {
                break;
            }
            if (fn_8048E988(pq->root, &pq->Mvol[next], &pq->Mbox[next], &pq->Mbox[i])) {
                s32 pos1[4];
                s32 pos2[4];

                pos1[0] = 0;
                pos1[1] = 0;
                pos1[2] = 0;
                pos1[3] = 0;
                fn_8048D880(&pq->Mvol[next]);
                fn_8048E47C(pq->root, pos1, palQuantDepth, &pq->Mbox[next], &pq->Mvol[next]);
                pos2[0] = 0;
                pos2[1] = 0;
                pos2[2] = 0;
                pos2[3] = 0;
                fn_8048D880(&pq->Mvol[i]);
                fn_8048E47C(pq->root, pos2, palQuantDepth, &pq->Mbox[i], &pq->Mvol[i]);
                pq->Mvv[next] = pq->Mvol[next].var;
                pq->Mvv[i] = pq->Mvol[i].var;
            } else {
                pq->Mvv[next] = 0.0f;
                i--;
            }
        }
        for (i = 0; i < maxColors; i++) {
            s32 pos[4];

            pos[0] = 0;
            pos[1] = 0;
            pos[2] = 0;
            pos[3] = 0;
            fn_8048E300(pq->root, pos, palQuantDepth, &pq->Mbox[i], i);
            fn_8048DAC8(&palette[i], &pq->Mvol[i]);
        }
    }
    return maxColors;
}

/* _rwPalQuantMatchImage: each pixel's palette index, two to a byte when packing 4-bit pixels. */
void fn_8048FAB0(u8 *dstPixels, s32 dstStride, s32 dstDepth, RwBool dstPacked, rwPalQuant *pq,
                 const RwImage *img) {
    s32 srcStride = img->stride;
    u8 *srcPixels = img->cpPixels;
    s32 height;
    s32 width;

    if (dstDepth == 4 && dstPacked == TRUE) {
        switch (img->depth) {
        case 4:
        case 8: {
            RwRGBA *palette = img->palette;

            height = img->height;
            while (height--) {
                u8 *src = srcPixels;
                u8 *dst = dstPixels;
                u32 x;

                width = img->width;
                for (x = 0; x < (u32)width; x++) {
                    RwRGBA *color = &palette[*src++];
                    s32 shift = 8 - palQuantDepth;
                    u8 index = fn_8048F4B4(pq->root, PALQUANTADDRESS(color, shift), palQuantDepth);

                    if (x & 1) {
                        *dst = *dst & 0xF;
                        *dst |= index << 4;
                        dst++;
                    } else {
                        *dst = *dst & 0xF0;
                        *dst |= index & 0xF;
                    }
                }
                srcPixels += srcStride;
                dstPixels += dstStride;
            }
            break;
        }
        case 32:
            height = img->height;
            while (height--) {
                RwRGBA *color = (RwRGBA *)srcPixels;
                u8 *dst = dstPixels;
                u32 x;

                width = img->width;
                for (x = 0; x < (u32)width; x++) {
                    s32 shift = 8 - palQuantDepth;
                    u8 index = fn_8048F4B4(pq->root, PALQUANTADDRESS(color, shift), palQuantDepth);

                    color++;
                    if (x & 1) {
                        *dst = *dst & 0xF;
                        *dst |= index << 4;
                        dst++;
                    } else {
                        *dst = *dst & 0xF0;
                        *dst |= index & 0xF;
                    }
                }
                srcPixels += srcStride;
                dstPixels += dstStride;
            }
            break;
        }
    } else {
        switch (img->depth) {
        case 4:
        case 8: {
            RwRGBA *palette = img->palette;

            height = img->height;
            while (height--) {
                u8 *src = srcPixels;
                u8 *dst = dstPixels;

                width = img->width;
                while (width--) {
                    RwRGBA *color = &palette[*src++];
                    s32 shift = 8 - palQuantDepth;

                    *dst++ = fn_8048F4B4(pq->root, PALQUANTADDRESS(color, shift), palQuantDepth);
                }
                srcPixels += srcStride;
                dstPixels += dstStride;
            }
            break;
        }
        case 32:
            height = img->height;
            while (height--) {
                RwRGBA *color = (RwRGBA *)srcPixels;
                u8 *dst = dstPixels;

                width = img->width;
                while (width--) {
                    s32 shift = 8 - palQuantDepth;

                    *dst++ = fn_8048F4B4(pq->root, PALQUANTADDRESS(color, shift), palQuantDepth);
                    color++;
                }
                srcPixels += srcStride;
                dstPixels += dstStride;
            }
            break;
        }
    }
}

/* _rwPalQuantInit: the bit-spreading table, the node free list and an empty root. */
RwBool fn_8048FED8(rwPalQuant *pq) {
    u32 i;
    u32 bit;
    u32 count;
    u32 spread;
    rwPalQuantNode *root;

    count = 1 << palQuantDepth;
    for (i = 0; i < count; i++) {
        spread = 0;
        for (bit = 0; bit < palQuantDepth; bit++) {
            spread |= (i & (1 << bit)) ? (1 << ((palQuantDepth - 1 - bit) * 4)) : 0;
        }
        palQuantSpread[i] = spread;
    }
    pq->cubeFreeList = fn_8047FBD8(sizeof(rwPalQuantNode), 0x400, 4, 0x30411);
    pq->root = (rwPalQuantNode *)RwEngineInstance->memoryAlloc(pq->cubeFreeList, 0x30411);
    root = pq->root;
    for (i = 0; i < 16; i++) {
        root->child[i] = NULL;
    }
    return TRUE;
}

/* Frees a subtree (retail inlines it into itself eight levels deep). */
#pragma inline_depth(8)
static inline void fn_804900D4(rwPalQuant *pq, rwPalQuantNode *node, int level) {
    if (node != NULL) {
        if (level > 0) {
            int i;

            for (i = 0; i < 16; i++) {
                fn_804900D4(pq, node->child[i], level - 1);
            }
        }
        RwEngineInstance->memoryFree(pq->cubeFreeList, node);
    }
}

/* _rwPalQuantTerm. */
void fn_804903F8(rwPalQuant *pq) {
    fn_804900D4(pq, pq->root, palQuantDepth);
    pq->root = NULL;
    fn_8047FCDC(pq->cubeFreeList);
}
#pragma inline_depth(5)
