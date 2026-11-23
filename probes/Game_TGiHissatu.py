"""Wave 40: TGiHissatu::Load. Retail: the side's row in r28, the counter in r26."""
F = "Load__10TGiHissatuFv"
SYMBOLS = {F: "void TGiHissatu::Load() {"}
CALLS = """        fn_8003FA08(csd, side->m_file);
        strcpy(csd.m_sceneName, side->m_scene);
        fn_8003E388(csd, side->m_close, 1);
        fn_8003FB10(csd);
    }
"""
BODIES = {F: {
    "side_top": "    const UnkHissatuSide_804B52E4 *side;\n    for (int i = 0; i < 2; i++) {\n        TPjsCsd &csd = m_sides[i].m_csd;\n        side = &s_sides[i];\n" + CALLS,
    "side_first": "    for (int i = 0; i < 2; i++) {\n        const UnkHissatuSide_804B52E4 *side = &s_sides[i];\n        TPjsCsd &csd = m_sides[i].m_csd;\n" + CALLS,
    "all_top": "    const UnkHissatuSide_804B52E4 *side;\n    TPjsCsd *pcsd;\n    int i;\n    for (i = 0; i < 2; i++) {\n        pcsd = &m_sides[i].m_csd;\n        TPjsCsd &csd = *pcsd;\n        side = &s_sides[i];\n" + CALLS,
    "side_i_top": "    const UnkHissatuSide_804B52E4 *side;\n    int i;\n    for (i = 0; i < 2; i++) {\n        TPjsCsd &csd = m_sides[i].m_csd;\n        side = &s_sides[i];\n" + CALLS,
    "side_ptr_loop": "    const UnkHissatuSide_804B52E4 *side = s_sides;\n    for (int i = 0; i < 2; i++, side++) {\n        TPjsCsd &csd = m_sides[i].m_csd;\n" + CALLS,
    "side_csd_top": "    const UnkHissatuSide_804B52E4 *side;\n    TPjsCsd *pcsd;\n    for (int i = 0; i < 2; i++) {\n        pcsd = &m_sides[i].m_csd;\n        TPjsCsd &csd = *pcsd;\n        side = &s_sides[i];\n" + CALLS,
}}

# Tick: retail keeps the counter in r31, the player in r30, the row in r25, the address of the
# row's state in r24 (for both tests), and in the closing branch the side in r24 and its scene
# in r23.
T = "Tick__10TGiHissatuFf"
SYMBOLS[T] = "void TGiHissatu::Tick(float dt) {"
def tick(top, pre, c1, b1, c2, b2):
    return top + "    UnkPlayerInfo_80178B5C *player = fn_80178B5C(fn_80079808(), 0);\n    if (player != 0) {\n        for (i = 0; i < 2; i++) {\n            const UnkHissatuSide_804B52E4 *side = &s_sides[i];\n" + pre + "            if (" + c1 + ") {\n" + b1 + "            } else if (" + c2 + ") {\n" + b2 + "            }\n            fn_8003FE2C(m_sides[i].m_csd);\n        }\n    }\n"
B1 = "                m_sides[i].m_open = 1;\n                fn_8003E388(m_sides[i].m_csd, side->m_open, 1);\n                fn_8003E388(m_sides[i].m_csd, side->m_loop, 0);\n"
B1L = "                m_sides[i].m_open = 1;\n                TPjsCsd *csd = &m_sides[i].m_csd;\n                fn_8003E388(*csd, side->m_open, 1);\n                fn_8003E388(*csd, side->m_loop, 0);\n"
B2 = "                fn_8003FB10(m_sides[i].m_csd);\n                m_sides[i].m_open = 0;\n                fn_8003E388(m_sides[i].m_csd, side->m_loop, 1);\n                fn_8003E388(m_sides[i].m_csd, side->m_close, 1);\n"
B2L = "                Side *s = &m_sides[i];\n                TPjsCsd *csd = &s->m_csd;\n                fn_8003FB10(*csd);\n                s->m_open = 0;\n                fn_8003E388(*csd, side->m_loop, 1);\n                fn_8003E388(*csd, side->m_close, 1);\n"
ST = "            const s32 *state = &side->m_state;\n"
BODIES[T] = {
    "t_base": tick("    int i;\n", "", "side->m_state == fn_8016EEC0(player) && !m_sides[i].m_open", B1, "side->m_state != fn_8016EEC0(player) && m_sides[i].m_open", B2),
    "t_state": tick("    int i;\n", ST, "*state == fn_8016EEC0(player) && !m_sides[i].m_open", B1, "*state != fn_8016EEC0(player) && m_sides[i].m_open", B2),
    "t_state_locals": tick("    int i;\n", ST, "*state == fn_8016EEC0(player) && !m_sides[i].m_open", B1L, "*state != fn_8016EEC0(player) && m_sides[i].m_open", B2L),
    "t_locals": tick("    int i;\n", "", "side->m_state == fn_8016EEC0(player) && !m_sides[i].m_open", B1L, "side->m_state != fn_8016EEC0(player) && m_sides[i].m_open", B2L),
    "t_state_ref": tick("    int i;\n", "            const s32 &state = side->m_state;\n", "state == fn_8016EEC0(player) && !m_sides[i].m_open", B1L, "state != fn_8016EEC0(player) && m_sides[i].m_open", B2L),
}
