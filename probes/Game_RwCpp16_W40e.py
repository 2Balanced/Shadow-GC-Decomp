"""Wave 40 (r2), RwCpp16 round 5: fn_804536B4's float registers (retail: from[1] f2, numerator f3,
to[2] f1, denominator f2, from[2] f4, delta f1, ratio f2)."""
SCORE = ["fn_804536B4"]
H0 = """    float numerator = time - from[1];
    float denominator = to[1] - from[1];
    float delta = to[2] - from[2];
    float ratio = numerator / denominator;
"""
REPL = {
 "base": [(H0, H0)],
 "r1": [(H0, "    float ratio = (time - from[1]) / (to[1] - from[1]);\n    float delta = to[2] - from[2];\n")],
 "r2": [(H0, "    float delta = to[2] - from[2];\n    float ratio = (time - from[1]) / (to[1] - from[1]);\n")],
 "r3": [(H0, "    float ratio;\n    float delta;\n    ratio = (time - from[1]) / (to[1] - from[1]);\n    delta = to[2] - from[2];\n")],
 "r4": [(H0, "    float delta;\n    float ratio;\n    ratio = (time - from[1]) / (to[1] - from[1]);\n    delta = to[2] - from[2];\n")],
 "r5": [(H0, "    float ratio = (time - from[1]) / (to[1] - from[1]);\n    float delta;\n    delta = to[2] - from[2];\n")],
 "r6": [(H0, "    float delta;\n    float ratio = (time - from[1]) / (to[1] - from[1]);\n    delta = to[2] - from[2];\n")],
 "r7": [(H0, "    float denominator = to[1] - from[1];\n    float numerator = time - from[1];\n    float delta = to[2] - from[2];\n    float ratio = numerator / denominator;\n")],
 "r8": [(H0, "    float ratio = (time - from[1]) / (to[1] - from[1]);\n    float delta = to[2] - from[2];\n"), ("    out[2] = ratio * delta + from[2];\n", "    out[2] = from[2] + ratio * delta;\n")],
}
