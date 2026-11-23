"""Wave 41: fn_80413318's event offset (retail branches: beq / clrlwi / b / li 0) through a label,
the lever that kept Target's test a branch."""
F = "fn_80413318"
SCORE = [F]
OLD = """            if (desc->m_eventAddr != 0) {
                eventOffset = desc->m_eventAddr & 0x1F;
            } else {
                eventOffset = 0;
            }
"""
REPL = {
    "goto_zero": [(OLD, """            if (desc->m_eventAddr == 0) {
                goto noEvents;
            }
            eventOffset = desc->m_eventAddr & 0x1F;
            goto haveOffset;
        noEvents:
            eventOffset = 0;
        haveOffset:
""")],
    "goto_has": [(OLD, """            if (desc->m_eventAddr != 0) {
                goto hasEvents;
            }
            eventOffset = 0;
            goto haveOffset;
        hasEvents:
            eventOffset = desc->m_eventAddr & 0x1F;
        haveOffset:
""")],
    "goto_else": [(OLD, """            if (desc->m_eventAddr != 0) {
                eventOffset = desc->m_eventAddr & 0x1F;
                goto haveOffset;
            }
            eventOffset = 0;
        haveOffset:
""")],
}

# fn_80412BFC: the bare name (retail r31, above the parameters) is a copy of path; with copy
# propagation off it is not merged back into the parameter.
B = "fn_80412BFC"
SCORE = [F, B]
BH = "u16 fn_80412BFC(void *data, const char *path, void (*release)(void *data), int autoId) {\n"
BHC = "/* Registers one motion held in memory as a bank of its own, named after the file name of path\n"
import re as _re
_s = open("src/Game/SonicteamMotionBank.cpp", encoding="utf-8").read()
_i = _s.index(BH)
_j = _s.index("\n}\n", _i) + 3
BBODY = _s[_i:_j]
REPL.update({
    "b_off": [(BBODY, "#pragma opt_propagation off\n" + BBODY + "#pragma opt_propagation reset\n")],
    "b_off_namefirst": [(BBODY, "#pragma opt_propagation off\n" + BBODY.replace("    const char *separator = \"/\";\n", "    const char *name;\n    const char *separator = \"/\";\n").replace("    const char *name = path;\n", "    name = path;\n") + "#pragma opt_propagation reset\n")],
})
REPL["base"] = []
