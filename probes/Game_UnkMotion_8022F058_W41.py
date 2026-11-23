"""Wave 41: ProcessEvents' prologue (retail copies this to r30, then forms 0xD004 in r3; GC/2.7
forms it first in r5) and LoadParts' saved registers, with copy propagation off."""
P = "ProcessEvents__Q218UnkMotion_8022F0584ImplFP14_stMtnPlayWork"
SCORE = [P, "LoadParts__Q218UnkMotion_8022F0584ImplFlPC15BirdMotionTable"]
HEAD = "void UnkMotion_8022F058::Impl::ProcessEvents(_stMtnPlayWork *work) {\n"
NEXT = "void UnkMotion_8022F058::Impl::LoadParts(s32 slot, const BirdMotionTable *table) {\n"
OFF = [(HEAD, "#pragma opt_propagation off\n" + HEAD), (NEXT, "#pragma opt_propagation reset\n" + NEXT)]
SOFF = [(HEAD, "#pragma scheduling off\n" + HEAD), (NEXT, "#pragma scheduling reset\n" + NEXT)]
REPL = {
    "off": OFF,
    "off_plain": OFF + [("    Impl *const self = this;\n", "    Impl *self = this;\n"), ("            s32 *const value = data;\n", "            s32 *value = data;\n")],
    "off_work": OFF + [("    Impl *const self = this;\n", "    Impl *const self = this;\n    _stMtnPlayWork *w = work;\n"), ("    while (fn_80415854(work, 0, &event, &data, 0) != 0) {\n        work = 0;\n", "    while (fn_80415854(w, 0, &event, &data, 0) != 0) {\n        w = 0;\n")],
    "soff": SOFF,
    "off_soff": OFF + SOFF,
}

# LoadParts: retail part r30 (just below the sets base r31), then manager r29, stage r28, i r27,
# set r26; GC/2.7 gives the part the lowest register.
L = "LoadParts__Q218UnkMotion_8022F0584ImplFlPC15BirdMotionTable"
LHEAD = "void UnkMotion_8022F058::Impl::LoadParts(s32 slot, const BirdMotionTable *table) {\n"
LNEXT = "void UnkMotion_8022F058::Impl::DestroyParts() {\n"
LOFF = [(LHEAD, "#pragma opt_propagation off\n" + LHEAD), (LNEXT, "#pragma opt_propagation reset\n" + LNEXT)]
LB = """    m_partCount = table->m_setCount;
    m_parts.reset(new Part[m_partCount]);
    void *manager = fn_800122D4();
    UnkStage_8003A840 *stage = fn_8003A840();
    for (int i = 0; i < m_partCount; i++) {
        const BirdMotionSet *set = &table->m_sets[i];
        Part *part = &m_parts[i];
"""
def lb(new): return [(LB, new)]
REPL.update({
    "l_off": LOFF,
    "l_off_ptop": LOFF + lb(LB.replace("    void *manager", "    Part *part;\n    void *manager").replace("        Part *part = &m_parts[i];", "        part = &m_parts[i];")),
    "l_off_pfirst": LOFF + lb(LB.replace("        const BirdMotionSet *set = &table->m_sets[i];\n        Part *part = &m_parts[i];", "        Part *part = &m_parts[i];\n        const BirdMotionSet *set = &table->m_sets[i];")),
    "l_off_ptop_first": LOFF + lb(LB.replace("    m_partCount = table->m_setCount;\n", "    Part *part;\n    m_partCount = table->m_setCount;\n").replace("        Part *part = &m_parts[i];", "        part = &m_parts[i];")),
})
REPL["base"] = []
SCORE = [P]
D0 = "    Impl *const self = this;\n    u16 event;\n    s32 *data;\n"
REPL.update({
    "p_decl_first": [(D0, "    u16 event;\n    s32 *data;\n    Impl *const self = this;\n")],
    "p_data_self": [(D0, "    u16 event;\n    Impl *const self = this;\n    s32 *data;\n")],
    "p_case_order": [("        case 0x5003: {", "        case 0xD004X: {")] if False else [],
})
