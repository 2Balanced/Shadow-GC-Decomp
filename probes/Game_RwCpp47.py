# Probes for Game/RwCpp47: the perspective frustum (fn_804842C8), its first block.
# Round 1: offset computed after the corners reloads the axes (base 210 rows); computed first,
# 156; planes 4/5 fixed (cross products of c3-c7, not negations): 73 rows, 500 of 505 insns.
# Round 2: the first block with the axes and window read into float locals: 110-127.
# Round 3: loop forms; the far corner through a pointer set from corners[i + 4] gives retail's
# addi/mulli/add and every opcode (75 rows: registers, and one saved FPR fewer).
src = open("src/Game/RwCpp47.cpp", newline="").read().replace("\r\n", "\n")
a = src.index("    offset.x = ltm->right.x * -camera->viewOffset.x;")
b = src.index("    corners[3] = temp;\n") + len("    corners[3] = temp;\n")
FIRST = src[a:b]
OFFSET = """    offset.x = ltm->right.x * -camera->viewOffset.x;
    offset.y = ltm->right.y * -camera->viewOffset.x;
    offset.z = ltm->right.z * -camera->viewOffset.x;
    offset.x += ltm->up.x * camera->viewOffset.y;
    offset.y += ltm->up.y * camera->viewOffset.y;
    offset.z += ltm->up.z * camera->viewOffset.y;
"""
REST = FIRST.replace(OFFSET, "")
C3 = "    corners[3] = temp;\n"
REPL = {
    "base": [],
    "offend": [(FIRST, REST + OFFSET)],
    "offmid2": [(FIRST, REST.replace("    corners[2] = temp;\n", "    corners[2] = temp;\n" + OFFSET))],
    "offbefore3": [(FIRST, REST.replace(C3, OFFSET + C3))],
    "offbefore0": [(FIRST, REST.replace("    corners[0] = temp;\n", OFFSET + "    corners[0] = temp;\n"))],
}
SCORE = ["fn_804842C8"]
# Round 4: the offset block at each statement boundary of the first block.
lines = REST.splitlines(True)
REPL = {"base": [], "offend": [(FIRST, REST + OFFSET)]}
for k in range(0, 13):
    REPL["at%02d" % k] = [(FIRST, "".join(lines[:k]) + OFFSET + "".join(lines[k:]))]
# the two halves of the offset apart
H1 = "".join(OFFSET.splitlines(True)[:3]); H2 = "".join(OFFSET.splitlines(True)[3:])
for k in range(0, 13, 3):
    REPL["split%02d" % k] = [(FIRST, H1 + "".join(lines[:k]) + H2 + "".join(lines[k:]))]
