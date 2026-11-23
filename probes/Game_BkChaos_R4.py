"""Wave 40: StretchCallback's node index (retail: subfic/subic/subfe into r27, tested unsigned with
cmplwi; the parameter in r31). bool and the != forms give the registers but add clrlwi."""
SCORE = ["StretchCallback__7BkChaosFP19_stMtnCallbackParam"]
I = "    int index = param->m_id == 0x1774 ? 0 : 1;\n"
REPL = {
  "bool_ne": [(I, "    bool index = param->m_id != 0x1774;\n")],
  "u32_ne": [(I, "    u32 index = param->m_id != 0x1774;\n")],
  "int_ne": [(I, "    int index = param->m_id != 0x1774;\n")],
  "u32_tern": [(I, "    u32 index = param->m_id == 0x1774 ? 0 : 1;\n")],
  "bool_tern": [(I, "    bool index = param->m_id == 0x1774 ? false : true;\n")],
  "u32_tern1": [(I, "    u32 index = param->m_id != 0x1774 ? 1 : 0;\n")],
  "u32_cast": [(I, "    u32 index = (u32)(param->m_id != 0x1774);\n")],
  "u32_if": [(I, "    u32 index = 1;\n    if (param->m_id == 0x1774) {\n        index = 0;\n    }\n")],
  "uint_tern": [(I, "    unsigned int index = param->m_id == 0x1774 ? 0u : 1u;\n")],
  "u32_tern_cast": [(I, "    u32 index = param->m_id == 0x1774 ? (u32)0 : (u32)1;\n")],
  "int_tern_utest": [(I, I), ("(index ? 0.0012f : -0.0012f)", "((u32)index ? 0.0012f : -0.0012f)")],
}
