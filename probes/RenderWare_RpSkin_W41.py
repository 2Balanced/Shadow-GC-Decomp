"""Wave 41, RenderWare/RpSkin: fn_8044C8C0 with the two weight scans as inline helpers."""
SCORE = ["fn_8044C8C0"]
SRC = open("src/RenderWare/RpSkin.c", newline="").read().replace("\r\n", "\n")


def _span(text, start, end, after=0):
    """The text from start up to and including end (searched from start); a marker when the
    anchor is gone because a variant of this table was kept in the source."""
    try:
        a = text.index(start)
        b = text.index(end, a) + len(end) + after
    except ValueError:
        return "<<anchor gone: a variant of this table is in the source>>"
    return text[a:b]

OLD_START = """        {
        u32 j;
        u32 i;
        const u32 *weight;
        const R2MatrixWeights *weights;

        weights = skin->weights;
        skin->maxWeights = 1;"""
OLD_END = """        fn_8044CCC0(geometry, skin);
    } else {"""
OLD = _span(SRC, OLD_START, OLD_END)[:-len(OLD_END)]
SIG = "/* Atomic stream read callback: the skin chunk older files kept on the atomic. */\n"
HELPERS = """/* Works out how many weights a vertex of the skin uses at most. */
static inline void SkinFindMaxWeights(R2Skin *skin, u32 numVertices) {
    const R2MatrixWeights *weights = skin->weights;
    u32 i;

    skin->maxWeights = 1;
    for (i = 0; i < numVertices; i++) {
        u32 j;
        const u32 *weight = &weights->w[skin->maxWeights];

        for (j = skin->maxWeights; j < 4; j++) {
            if (*weight == 0) {
                break;
            }
            skin->maxWeights++;
            if (skin->maxWeights == 4) {
                return;
            }
            weight++;
        }
        weights++;
    }
}

/* Lists the bones the skin's weighted indices use. */
static inline void SkinFindUsedBones(R2Skin *skin, u32 numVertices) {
    const R2MatrixWeights *weights = skin->weights;
    const u32 *indices = skin->indices;
    u8 *usedBones = skin->usedBones;
    u32 i;

    skin->numUsedBones = 0;
    for (i = 0; i < numVertices; i++) {
        const u32 *weight = &weights->w[0];
        u32 j;
        u32 shift;

        for (j = 0, shift = 0; j < skin->maxWeights; j++) {
            if (*weight != 0) {
                u8 bone = (u8)(*indices >> shift);
                u32 numUsedBones = skin->numUsedBones;
                s32 isNew = 1;
                u32 k;

                for (k = 0; k < numUsedBones; k++) {
                    if (bone == usedBones[k]) {
                        isNew = 0;
                        break;
                    }
                }
                if (isNew) {
                    usedBones[numUsedBones] = bone;
                    skin->numUsedBones++;
                }
            }
            weight++;
            shift += 8;
        }
        weights++;
        indices++;
    }
}

"""
NEW = """        SkinFindMaxWeights(skin, numVertices);
        SkinFindUsedBones(skin, numVertices);
"""
REPL = {"base": [], "inl": [(SIG, HELPERS + SIG), (OLD + "counted:\n", NEW) if (OLD + "counted:\n") in SRC else (OLD, NEW)]}
