"""Wave 40: MakePatternFrame's true parameter order. Its name and signature are inferred (only the
registers are fixed: two integers in r3/r4 and the float in f1); with the float last, ApplyLink's
arguments are loaded in retail's order (integers first)."""
SCORE = ["ApplyLink__Q34Chao3CSD5CNodeFRQ23std71__tree_iterator<Q23std46pair<Cl,Q34Chao3CSD24RCPtr<Q34Chao3CSD5CNode>>>",
         "fn_8036AD14"]
HDR = {"float_last": {"Game/CsdScene.h": [
    ("float MakePatternFrame(float frac, s32 index, s32 pattern);", "float MakePatternFrame(s32 index, s32 pattern, float frac);"),
    ("m_pattern->OverridePattern(MakePatternFrame(frac, index, pattern));", "m_pattern->OverridePattern(MakePatternFrame(index, pattern, frac));"),
]}}
REPL = {"float_last": [("node->SetPatternFrame(MakePatternFrame(0.0f, pattern, pattern));", "node->SetPatternFrame(MakePatternFrame(pattern, pattern, 0.0f));")]}
