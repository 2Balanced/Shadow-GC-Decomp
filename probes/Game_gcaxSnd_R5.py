"""Wave 41, Game/gcaxSnd.c round 5: fn_8043F70C statement shapes (retail: loader r27, command r28,
count r29, base r30, result r31)."""
SCORE = ["fn_8043F70C"]
NL = chr(10)
A = """    result = 0;
    count = file[7] >> 4;
    while (count-- > 0) {
"""
B = """    command = file + 8;
    base = command;
"""
REPL = {
    "base": [],
    "count_first": [(A, "    count = file[7] >> 4;" + NL + "    result = 0;" + NL + "    while (count-- > 0) {" + NL)],
    "base_first": [(B, "    base = file + 8;" + NL + "    command = base;" + NL)],
    "for_loop": [(A, "    result = 0;" + NL + "    for (count = file[7] >> 4; count-- > 0;) {" + NL)],
    "u32count": [("    int count;" + NL, "    u32 count;" + NL)],
    "result_ret": [(A, "    count = file[7] >> 4;" + NL + "    result = 0;" + NL + "    while (count-- > 0) {" + NL), (B, "    base = file + 8;" + NL + "    command = base;" + NL)],
}
