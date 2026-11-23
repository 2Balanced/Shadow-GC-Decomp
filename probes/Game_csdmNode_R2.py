"""Wave 40: ApplyLink's case 6 (CNode::CopyPatternFrame, inline in Game/CsdScene.h): retail loads
MakePatternFrame's two integers before its float."""
SCORE = ["ApplyLink__Q34Chao3CSD5CNodeFRQ23std71__tree_iterator<Q23std46pair<Cl,Q34Chao3CSD24RCPtr<Q34Chao3CSD5CNode>>>"]
B = """        float frac;
        s32 pattern;
        s32 index;
        SplitPatternFrame(frame, &index, &pattern, &frac);
        m_pattern->OverridePattern(MakePatternFrame(frac, index, pattern));
"""
def h(body):
    return {"Game/CsdScene.h": [(B, body)]}
HDR = {
  "ipf": h("""        s32 index;
        s32 pattern;
        float frac;
        SplitPatternFrame(frame, &index, &pattern, &frac);
        m_pattern->OverridePattern(MakePatternFrame(frac, index, pattern));
"""),
  "pif": h("""        s32 pattern;
        s32 index;
        float frac;
        SplitPatternFrame(frame, &index, &pattern, &frac);
        m_pattern->OverridePattern(MakePatternFrame(frac, index, pattern));
"""),
  "fip": h("""        float frac;
        s32 index;
        s32 pattern;
        SplitPatternFrame(frame, &index, &pattern, &frac);
        m_pattern->OverridePattern(MakePatternFrame(frac, index, pattern));
"""),
  "result_local": h("""        float frac;
        s32 pattern;
        s32 index;
        SplitPatternFrame(frame, &index, &pattern, &frac);
        float result = MakePatternFrame(frac, index, pattern);
        m_pattern->OverridePattern(result);
"""),
  "arr": h("""        float frac;
        s32 parts[2];
        SplitPatternFrame(frame, &parts[1], &parts[0], &frac);
        m_pattern->OverridePattern(MakePatternFrame(frac, parts[1], parts[0]));
"""),
  "ptr_frac": h("""        float frac;
        s32 pattern;
        s32 index;
        float *f = &frac;
        SplitPatternFrame(frame, &index, &pattern, f);
        m_pattern->OverridePattern(MakePatternFrame(*f, index, pattern));
"""),
}
REPL = {k: [] for k in HDR}

AL = "void CNode::ApplyLink(LinkMap::iterator &link) {"
END = """    case 14:
        SetParam(2, link->second->GetParam(2));
        break;
    }
}
"""
HDR = {}
REPL = {
  "sched_off": [(AL, "#pragma scheduling off\n" + AL), (END, END + "#pragma scheduling reset\n")],
}
