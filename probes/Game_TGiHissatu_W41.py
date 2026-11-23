"""Wave 41: TGiHissatu::Tick. Retail: counter r31, player r30, this r29, then the compiler's
temporaries (row offsets r28/r27, table base r26, the side r25, the state's address r24, the
csd r24/r23). Shapes with no named side (every access indexed), the counter declared first."""
T = "Tick__10TGiHissatuFf"
SYMBOLS = {T: "void TGiHissatu::Tick(float dt) {"}


def tick(top, loop, c1, b1, c2, b2, pre=""):
    return (top + "    UnkPlayerInfo_80178B5C *player = fn_80178B5C(fn_80079808(), 0);\n    if (player != 0) {\n        "
            + loop + " {\n" + pre + "            if (" + c1 + ") {\n" + b1 + "            } else if (" + c2 + ") {\n" + b2
            + "            }\n            fn_8003FE2C(m_sides[i].m_csd);\n        }\n    }\n")


C1 = "s_sides[i].m_state == fn_8016EEC0(player) && !m_sides[i].m_open"
C2 = "s_sides[i].m_state != fn_8016EEC0(player) && m_sides[i].m_open"
C1R = "fn_8016EEC0(player) == s_sides[i].m_state && !m_sides[i].m_open"
C2R = "fn_8016EEC0(player) != s_sides[i].m_state && m_sides[i].m_open"
B1 = "                m_sides[i].m_open = 1;\n                fn_8003E388(m_sides[i].m_csd, s_sides[i].m_open, 1);\n                fn_8003E388(m_sides[i].m_csd, s_sides[i].m_loop, 0);\n"
B2 = "                fn_8003FB10(m_sides[i].m_csd);\n                m_sides[i].m_open = 0;\n                fn_8003E388(m_sides[i].m_csd, s_sides[i].m_loop, 1);\n                fn_8003E388(m_sides[i].m_csd, s_sides[i].m_close, 1);\n"
LI = "for (i = 0; i < 2; i++)"
LD = "for (int i = 0; i < 2; i++)"
BODIES = {T: {
    "idx_itop": tick("    int i;\n", LI, C1, B1, C2, B2),
    "idx_idecl": tick("", LD, C1, B1, C2, B2),
    "idx_itop_rev": tick("    int i;\n", LI, C1R, B1, C2R, B2),
    "idx_idecl_rev": tick("", LD, C1R, B1, C2R, B2),
}}
OFF_T = [("void TGiHissatu::Tick(float dt) {", "#pragma opt_propagation off\nvoid TGiHissatu::Tick(float dt) {"),
         ("void TGiHissatu::Load() {", "#pragma opt_propagation reset\nvoid TGiHissatu::Load() {")]
