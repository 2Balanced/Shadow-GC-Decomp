# Probes for Game/RwCpp45: RwStreamOpen (fn_80481368), registers of type (r29) and result (r28).
# Round 1 (if/retnull/lateinit/outres/outres2/volat on the inline): all 26 rows, as base.
# Round 2 (the inline written out in RwStreamOpen): 15 rows; the nested inlines then load 0
# instead of copying result's register, so the inline is right and only its numbering is off.
SIG = """static inline RwStream *StreamInitialize(RwStream *stream, RwBool rwOwned, s32 type,
                                         s32 accessType, const void *data) {"""
def sig(new):
    return [(SIG, new)]
REPL = {
    "base": [],
    "tint": sig("static inline RwStream *StreamInitialize(RwStream *stream, RwBool rwOwned, int type,\n s32 accessType, const void *data) {"),
    "aint": sig("static inline RwStream *StreamInitialize(RwStream *stream, RwBool rwOwned, s32 type,\n int accessType, const void *data) {"),
    "tu": sig("static inline RwStream *StreamInitialize(RwStream *stream, RwBool rwOwned, u32 type,\n s32 accessType, const void *data) {"),
    "bothint": sig("static inline RwStream *StreamInitialize(RwStream *stream, RwBool rwOwned, int type,\n int accessType, const void *data) {"),
    "enum": [(SIG, "typedef enum { rwNASTREAM, rwSTREAMFILE, rwSTREAMFILENAME, rwSTREAMMEMORY, rwSTREAMCUSTOM } StreamTypeE;\nstatic inline RwStream *StreamInitialize(RwStream *stream, RwBool rwOwned, StreamTypeE type,\n s32 accessType, const void *data) {"), ("if (!StreamInitialize(stream, TRUE, type,", "if (!StreamInitialize(stream, TRUE, (StreamTypeE)type,")],
    "dvoid": sig("static inline RwStream *StreamInitialize(RwStream *stream, RwBool rwOwned, s32 type,\n s32 accessType, void *data) {"),
}
SCORE = ["fn_80481368"]
