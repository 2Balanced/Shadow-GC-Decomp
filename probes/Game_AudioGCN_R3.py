"""Wave 40 (resumed agent): vf_0x50: voice and slot locals times optimisation pragmas (the winner: an s8 voice local with opt_propagation off).
Written as a JOBS list (unit, name, [(old, new)], [symbols]) against the source of the time;
the REPL/SCORE lines at the end give tools/pprobe.py the same variants."""
F = "void AudioGCN::vf_0x50(s8 group, u8 voice, s8 bank, s8 program) {\n"
OLDB = """    UnkAudioMessage_8043F41C message;
    fn_8043F41C(m_groups[group], &message, (s8)voice | 0xB0, 0x20, bank);
    fn_8043F35C(m_groups[group], &message, 0);
    fn_8043F41C(m_groups[group], &message, (s8)voice | 0xC0, program);
    fn_8043F35C(m_groups[group], &message, 0);
"""
def body(g, v, pre=""):
    return pre + "    UnkAudioMessage_8043F41C message;\n    fn_8043F41C(%s, &message, %s | 0xB0, 0x20, bank);\n    fn_8043F35C(%s, &message, 0);\n    fn_8043F41C(%s, &message, %s | 0xC0, program);\n    fn_8043F35C(%s, &message, 0);\n" % (g, v, g, g, v, g)
SRC = {
 "orig": OLDB,
 "slot_voice": body("*slot", "v", "    void **slot = &m_groups[group];\n    s8 v = voice;\n"),
 "voice_slot": body("*slot", "v", "    s8 v = voice;\n    void **slot = &m_groups[group];\n"),
 "slot_first": body("*slot", "(s8)voice", "    void **slot = &m_groups[group];\n"),
 "v_first": body("m_groups[group]", "v", "    s8 v = voice;\n"),
}
P = [None, "opt_propagation off", "opt_dead_assignments off", "opt_lifetimes off", "opt_common_subs off",
     "opt_loop_invariants off", "scheduling off", "optimization_level 2", "opt_strength_reduction off"]
JOBS = []
for k, b in SRC.items():
    for p in P:
        rep = [] if b == OLDB else [(OLDB, b)]
        if p:
            rep.append((F, "#pragma " + p + "\n" + F))
        JOBS.append(("AudioGCN", k + "_" + (p.split()[0] if p else "none"), rep, ["vf_0x50"]))

REPL = {name: pairs for (_unit, name, pairs, _syms) in JOBS}
SCORE = ['vf_0x50__8AudioGCNFScUcScSc']
