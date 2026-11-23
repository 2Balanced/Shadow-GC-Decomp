"""Wave 41: InitCast with '#pragma pool_data off' and the file's statics in one struct."""
SCORE = ["InitCast__Q24Chao3CSDFPv"]
H = "void InitCast(void *cast) {\n"
E = """    *(f32 *)(p + 0x70) = -1000000.0f;
}
"""
S0 = """static s32 s_defaultsSet;
static CValue s_defaults[15];
static s32 s_cachedIndex;
static s32 s_cachedAttribute;
static u32 s_cachedMask;
static u32 s_allAttributes;
"""
S1 = """static struct {
    s32 m_defaultsSet;
    CValue m_defaults[15];
    s32 m_cachedIndex;
    s32 m_cachedAttribute;
    u32 m_cachedMask;
    u32 m_allAttributes;
} s_attributes;
#define s_defaultsSet s_attributes.m_defaultsSet
#define s_defaults s_attributes.m_defaults
#define s_cachedIndex s_attributes.m_cachedIndex
#define s_cachedAttribute s_attributes.m_cachedAttribute
#define s_cachedMask s_attributes.m_cachedMask
#define s_allAttributes s_attributes.m_allAttributes
"""
REPL = {
  "struct_pool_off": [(S0, S1), (H, "#pragma pool_data off\n" + H), (E, E + "#pragma pool_data reset\n")],
  "struct_pool_on": [(S0, S1)],
}
