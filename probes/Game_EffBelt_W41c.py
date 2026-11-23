"""Wave 41: EffBelt::Impl::Update (retail: count r29, i r28; fade then count's register)."""
SCORE = ["Update__Q34Test7EffBelt4ImplFv"]
H = "bool EffBelt::Impl::Update() {\n"
P0 = "#pragma opt_propagation off\n"
B = """    s32 count = m_ring.m_count;
    if (count == 0) {
        return true;
    }
    BeltPoint *point = m_ring.Get(0);
    point->m_alpha = 0.0f;
    int i;
    for (i = 1; i < count; i++) {
        point = m_ring.Get(i);
        point->m_alpha = (int)alpha;
    }
    int fade = 0;
    for (i = count - 1; i >= count - 9; i--) {
        point = m_ring.Get(i);
        point->m_alpha = fade;
        fade += 0x20;
        if (fade > m_color.m_a) {
            break;
        }
    }
    return true;
"""
INL1 = """inline void SetAlphas_(BeltRing &ring, int count, int alpha) {
    for (int i = 1; i < count; i++) {
        BeltPoint *point = ring.Get(i);
        point->m_alpha = (int)alpha;
    }
}
"""
V1 = """    s32 count = m_ring.m_count;
    if (count == 0) {
        return true;
    }
    BeltPoint *point = m_ring.Get(0);
    point->m_alpha = 0.0f;
    SetAlphas_(m_ring, count, alpha);
    int i;
    int fade = 0;
    for (i = count - 1; i >= count - 9; i--) {
        point = m_ring.Get(i);
        point->m_alpha = fade;
        fade += 0x20;
        if (fade > m_color.m_a) {
            break;
        }
    }
    return true;
"""
REPL = {
  "inl_first_loop": [(B, V1), (H, INL1 + H)],
  "prag_count_copy": [(H, P0 + H), (B, B.replace("    int i;\n    for (i = 1; i < count; i++) {", "    int i;\n    int n = count;\n    for (i = 1; i < n; i++) {"))],
  "i_decl_first": [(B, "    int i;\n" + B.replace("    int i;\n", ""))],
  "prag_i_decl_first": [(H, P0 + H), (B, "    int i;\n" + B.replace("    int i;\n", ""))],
}
