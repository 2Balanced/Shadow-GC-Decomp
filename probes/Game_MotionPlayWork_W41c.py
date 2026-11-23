"""Wave 41: fn_8041CFB0: retail loads the weight-scale array into r4 and the scaled index into r5."""
SRC = open("src/Game/MotionPlayWork.cpp", encoding="utf-8").read()
SCORE = ["fn_8041CFB0"]
s = SRC.index('extern "C" int fn_8041CFB0(const MotionModel *model, int index, u32 key, float scale, float offset) {')
e = SRC.index("\n}\n", s) + 3
FN = SRC[s:e]
A = "        weightScale = &model->m_weightScales[index];\n"
V = {
  "plus": FN.replace(A, "        weightScale = model->m_weightScales + index;\n"),
  "named": FN.replace(A, "        MotionWeightScale *scales = model->m_weightScales;\n        weightScale = &scales[index];\n"),
  "named_top": FN.replace("    MotionWeightScale *weightScale;\n", "    MotionWeightScale *scales;\n    MotionWeightScale *weightScale;\n").replace(A, "        scales = model->m_weightScales;\n        weightScale = &scales[index];\n"),
  "bytes": FN.replace(A, "        weightScale = (MotionWeightScale *)((u8 *)model->m_weightScales + index * sizeof(MotionWeightScale));\n"),
  "plain": FN,
  "idx_plus": FN.replace(A, "        weightScale = index + model->m_weightScales;\n"),
}
REPL = {}
for k, v in V.items():
    REPL[k] = [(FN, v)]
    REPL["prag_" + k] = [(FN, "#pragma opt_propagation off\n" + v + "#pragma opt_propagation reset\n")]
H1 = "inline MotionWeightScale *WeightScaleAt_(const MotionModel *model, int index) {\n    return &model->m_weightScales[index];\n}\n\n"
H2 = "inline MotionWeightScale *WeightScaleAt_(int index, const MotionModel *model) {\n    return &model->m_weightScales[index];\n}\n\n"
H3 = "inline void SetPending_(MotionWeightScale *ws, float scale, float offset) {\n    ws->m_newScale = scale;\n    ws->m_newOffset = offset;\n    ws->m_pending = 1;\n}\n\n"
H4 = "inline MotionWeightScale *WeightScales_(const MotionModel *model) {\n    return model->m_weightScales;\n}\n\n"
REPL["h1"] = [(FN, H1 + FN.replace(A, "        weightScale = WeightScaleAt_(model, index);\n"))]
REPL["h2"] = [(FN, H2 + FN.replace(A, "        weightScale = WeightScaleAt_(index, model);\n"))]
REPL["h3"] = [(FN, H3 + FN.replace(A + "        weightScale->m_newScale = scale;\n        weightScale->m_newOffset = offset;\n        weightScale->m_pending = 1;\n", "        SetPending_(&model->m_weightScales[index], scale, offset);\n"))]
REPL["h4"] = [(FN, H4 + FN.replace(A, "        weightScale = &WeightScales_(model)[index];\n"))]
REPL["h4b"] = [(FN, H4 + FN.replace(A, "        weightScale = WeightScales_(model) + index;\n"))]
REPL["two"] = [(FN, FN.replace(A, "        weightScale = model->m_weightScales;\n        weightScale += index;\n"))]
REPL["prag_two"] = [(FN, "#pragma opt_propagation off\n" + FN.replace(A, "        weightScale = model->m_weightScales;\n        weightScale += index;\n") + "#pragma opt_propagation reset\n")]
REPL["two_b"] = [(FN, FN.replace(A, "        weightScale = model->m_weightScales;\n        weightScale = &weightScale[index];\n"))]
REPL["off_local"] = [(FN, FN.replace(A, "        int offset = index;\n        weightScale = model->m_weightScales + offset;\n"))]
