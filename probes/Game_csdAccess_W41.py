"""Wave 41: InitCast with '#pragma pool_data off' scoped to it (and variants)."""
SCORE = ["InitCast__Q24Chao3CSDFPv"]
H = "void InitCast(void *cast) {\n"
E = """    *(f32 *)(p + 0x70) = -1000000.0f;
}
"""
REPL = {
  "pool_off": [(H, "#pragma pool_data off\n" + H), (E, E + "#pragma pool_data reset\n")],
  "pool_off_all_from_defaults": [("inline void SetDefaults() {\n", "#pragma pool_data off\ninline void SetDefaults() {\n"), (E, E + "#pragma pool_data reset\n")],
}
