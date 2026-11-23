"""Wave 40 (r2), RwCpp16 round 10: fn_80453018 (UV keyframe init) with typed animation and
interpolator info (retail compares signed type ids and reads the animation through a pointer to
const)."""
SCORE = ["fn_80453018", "fn_80453594"]
F0 = """u32 fn_80453018(u32 *object, u32 *record, u32 id, float time, u32 *source)
{
    float *key = (float *)record;
    record[0] = id;
    key[1] = time;
    if (**(u32 **)object == lbl_8056D77C) {
        fn_80453594(&key[2], (const float *)source);
        /* Retail branches on this call's leftover r3 here; it is never zero, so
           the branch is not reachable in practice. */
    }
    if (**(u32 **)object == lbl_8056D7AC) {
        if (fn_80453820(&key[2], (const float *)source) != 0) {
            return 0;
        }
    }
    /* Every path leaves zero; upstream keeps that result for its caller. */
    return 0;
}
"""
NEW = """u32 fn_80453018(const R2UVAnimation *anim, R2UVKeyFrame *frame, void *prev, float time,
                const float *matrix)
{
    frame->prev = prev;
    frame->time = time;
    if (anim->interpInfo->typeID == lbl_8056D77C.typeID) {
        if (fn_80453594(frame->uv, matrix) != NULL) {
            return 0;
        }
    }
    if (anim->interpInfo->typeID == lbl_8056D7AC.typeID) {
        if (fn_80453820(frame->uv, matrix) != NULL) {
            return 0;
        }
    }
    return 0;
}
"""
TY = """typedef struct R2AnimInterpInfo {
    s32 typeID;
} R2AnimInterpInfo;
typedef struct R2UVAnimation {
    R2AnimInterpInfo *interpInfo;
} R2UVAnimation;
typedef struct R2UVKeyFrame {
    void *prev;
    float time;
    float uv[6];
} R2UVKeyFrame;
extern R2AnimInterpInfo lbl_8056D77C;
extern R2AnimInterpInfo lbl_8056D7AC;
"""
REPL = {
 "base": [(F0, F0)],
 "typed": [("extern u32 lbl_8056D77C;\nextern u32 lbl_8056D7AC;\n", TY), (F0, NEW),
           ("u32 fn_80453018(u32 *, u32 *, u32, float, u32 *);", "struct R2UVAnimation;\nstruct R2UVKeyFrame;\nu32 fn_80453018(const struct R2UVAnimation *, struct R2UVKeyFrame *, void *, float, const float *);"),
           ("void fn_80453594(float *dest, const float *source) {", "float *fn_80453594(float *dest, const float *source) {"),
           ("    dest[5] = source[0x0D];\n}", "    dest[5] = source[0x0D];\n    return dest;\n}")],
}
