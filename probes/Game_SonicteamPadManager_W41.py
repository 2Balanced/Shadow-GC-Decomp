"""Wave 41: fn_8040D9EC with copy propagation off (the wave-40 lever): retail keeps the state
pointer in r6 and the previous status in r3."""
SCORE = ["fn_8040D9EC"]
F = "void fn_8040D9EC(UnkPadManager_8040DC5C *manager) {"
END = "/* Installs the implementation and counts its pads (name inferred). */"
P = [(F, "#pragma opt_propagation off\n" + F), (END, "#pragma opt_propagation reset\n" + END)]
L = """            UnkPad_8040DB6C *pad = &manager->m_pads[i];
            PadState *state = &states[i];
            s32 status = pad->m_status;
"""
REPL = {
  "prag": P,
  "prag_status_first": P + [(L, """            UnkPad_8040DB6C *pad = &manager->m_pads[i];
            s32 status = pad->m_status;
            PadState *state = &states[i];
""")],
  "prag_state_first": P + [(L, """            PadState *state = &states[i];
            UnkPad_8040DB6C *pad = &manager->m_pads[i];
            s32 status = pad->m_status;
""")],
  "prag_mgr": P + [(F, F + "\n    UnkPadManager_8040DC5C *self = manager;"), ("UnkPad_8040DB6C *pad = &manager->m_pads[i];", "UnkPad_8040DB6C *pad = &self->m_pads[i];")],
  "prag_decl_top": P + [(L, """            UnkPad_8040DB6C *pad;
            PadState *state;
            s32 status;
            pad = &manager->m_pads[i];
            state = &states[i];
            status = pad->m_status;
""")],
  "prag_decl_top2": P + [(L, """            PadState *state;
            s32 status;
            UnkPad_8040DB6C *pad;
            pad = &manager->m_pads[i];
            state = &states[i];
            status = pad->m_status;
""")],
  "prag_decl_top3": P + [(L, """            s32 status;
            PadState *state;
            UnkPad_8040DB6C *pad;
            pad = &manager->m_pads[i];
            state = &states[i];
            status = pad->m_status;
""")],
}
