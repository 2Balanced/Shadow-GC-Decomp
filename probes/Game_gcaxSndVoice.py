"""Wave 41, Game/gcaxSndVoice.c: fn_8043C9D0 (retail forms the program table's pointer first and
indexes it; the walk's registers)."""
SCORE = ["fn_8043C9D0"]
NL = chr(10)
A = """        GcaxTableRef *programs = (GcaxTableRef *)((u8 *)table + GCAX_REF_OFFSET(table->m_programs));
        GcaxTableRef layerRef = programs[program];
        GcaxLayer *layers = (GcaxLayer *)((u8 *)table + GCAX_REF_OFFSET(layerRef));
        int i;
        for (i = 0; i < (int)GCAX_REF_COUNT(layerRef); i++) {
            GcaxTableRef regionRef = layers[i].m_regions;
            GcaxRegion *regions = (GcaxRegion *)((u8 *)table + GCAX_REF_OFFSET(regionRef));
            int j;
            for (j = 0; j < (int)GCAX_REF_COUNT(regionRef); j++) {
"""
def v(pre, loopi, inner):
    return pre + loopi + inner
B1 = """        u8 *base = (u8 *)table;
        GcaxTableRef *programs = (GcaxTableRef *)(base + GCAX_REF_OFFSET(table->m_programs));
        GcaxTableRef layerRef = programs[program];
        GcaxLayer *layers = (GcaxLayer *)(base + GCAX_REF_OFFSET(layerRef));
        int i;
        for (i = 0; i < (int)GCAX_REF_COUNT(layerRef); i++) {
            GcaxTableRef regionRef = layers[i].m_regions;
            GcaxRegion *regions = (GcaxRegion *)(base + GCAX_REF_OFFSET(regionRef));
            int j;
            for (j = 0; j < (int)GCAX_REF_COUNT(regionRef); j++) {
"""
B2 = """        GcaxTableRef layerRef = ((GcaxTableRef *)((u8 *)table + GCAX_REF_OFFSET(table->m_programs)))[program];
        GcaxLayer *layers = (GcaxLayer *)((u8 *)table + GCAX_REF_OFFSET(layerRef));
        int i;
        for (i = 0; i < (int)GCAX_REF_COUNT(layerRef); i++) {
            GcaxTableRef regionRef = layers[i].m_regions;
            GcaxRegion *regions = (GcaxRegion *)((u8 *)table + GCAX_REF_OFFSET(regionRef));
            int j;
            for (j = 0; j < (int)GCAX_REF_COUNT(regionRef); j++) {
"""
B3 = """        GcaxTableRef *programs = (GcaxTableRef *)((u8 *)table + GCAX_REF_OFFSET(table->m_programs));
        GcaxTableRef layerRef = programs[program];
        int layerCount = GCAX_REF_COUNT(layerRef);
        GcaxLayer *layers = (GcaxLayer *)((u8 *)table + GCAX_REF_OFFSET(layerRef));
        int i;
        for (i = 0; i < layerCount; i++) {
            GcaxTableRef regionRef = layers[i].m_regions;
            int regionCount = GCAX_REF_COUNT(regionRef);
            GcaxRegion *regions = (GcaxRegion *)((u8 *)table + GCAX_REF_OFFSET(regionRef));
            int j;
            for (j = 0; j < regionCount; j++) {
"""
B4 = """        GcaxTableRef *programs;
        GcaxTableRef layerRef;
        GcaxLayer *layers;
        int i;
        programs = (GcaxTableRef *)((u8 *)table + GCAX_REF_OFFSET(table->m_programs));
        layerRef = programs[program];
        layers = (GcaxLayer *)((u8 *)table + GCAX_REF_OFFSET(layerRef));
        for (i = 0; i < (int)GCAX_REF_COUNT(layerRef); i++) {
            GcaxTableRef regionRef = layers[i].m_regions;
            GcaxRegion *regions = (GcaxRegion *)((u8 *)table + GCAX_REF_OFFSET(regionRef));
            int j;
            for (j = 0; j < (int)GCAX_REF_COUNT(regionRef); j++) {
"""
REPL = {
    "base": [],
    "b1": [(A, B1)],
    "b2": [(A, B2)],
    "b3": [(A, B3)],
    "b4": [(A, B4)],
}

FHEAD = "/* Counts the regions of a program that cover the key. */" + NL
T = "    GcaxProgramBank *table = (GcaxProgramBank *)lbl_805F1404->m_programBanks[bank].m_data;" + NL + "    if (table != NULL && GCAX_REF_COUNT(table->m_programs) > program) {" + NL
INL_T = """static inline GcaxProgramBank *ProgramBank(int bank) {
    return (GcaxProgramBank *)lbl_805F1404->m_programBanks[bank].m_data;
}

"""
INL_CNT = """static inline int CountRegions(GcaxRegion *regions, int count, int key) {
    int n = 0;
    int j;
    for (j = 0; j < count; j++) {
        if (regions[j].m_keyLow <= key && key <= regions[j].m_keyHigh) {
            n++;
        }
    }
    return n;
}

"""
INNER = """            int j;
            for (j = 0; j < (int)GCAX_REF_COUNT(regionRef); j++) {
                if (regions[j].m_keyLow <= key && key <= regions[j].m_keyHigh) {
                    count++;
                }
            }
"""
REPL.update({
    "inl_table": [(T, "    GcaxProgramBank *table = ProgramBank(bank);" + NL + "    if (table != NULL && GCAX_REF_COUNT(table->m_programs) > program) {" + NL), (FHEAD, INL_T + FHEAD)],
    "inl_count": [(INNER, "            count += CountRegions(regions, GCAX_REF_COUNT(regionRef), key);" + NL), (FHEAD, INL_CNT + FHEAD)],
})
