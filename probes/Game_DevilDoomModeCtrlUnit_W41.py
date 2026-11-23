"""Wave 41: BELOW_HALF's constructor (retail: this r30, moveCamera r31) under propagation off."""
SCORE = ["__ct__32DevilDoomModeCtrlUnit_BELOW_HALFFP17DevilDoomVarParam"]
C = "class DevilDoomModeCtrlUnit_BELOW_HALF : public DevilDoomCtrlUnit {\n"
E = """    virtual ~DevilDoomModeCtrlUnit_BELOW_HALF();
"""
P0 = "#pragma opt_propagation off\n"
B0 = "        bool moveCamera = false;\n        fn_804165D8(m_owner->m_motion, 0, -1, -1);\n"
REPL = {
  "prag": [(C, P0 + C)],
  "prag_self": [(C, P0 + C), (B0, "        DevilDoomModeCtrlUnit_BELOW_HALF *self = this;\n" + B0)],
  "prag_self_after": [(C, P0 + C), (B0, B0.replace("        bool moveCamera = false;\n", "        bool moveCamera = false;\n        DevilDoomModeCtrlUnit_BELOW_HALF *self = this;\n"))],
  "prag_owner": [(C, P0 + C), (B0, "        bool moveCamera = false;\n        DevilDoomVarParam *o = m_owner;\n        fn_804165D8(m_owner->m_motion, 0, -1, -1);\n")],
  "prag_mc_decl_assign": [(C, P0 + C), (B0, "        bool moveCamera;\n        moveCamera = false;\n        fn_804165D8(m_owner->m_motion, 0, -1, -1);\n")],
}
