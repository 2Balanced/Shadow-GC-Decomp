"""Wave 40 (r7): fn_80493ABC: statement orders x volatility of the three flags."""
import itertools

OLD = """        /* Retail keeps the 1 in r0 and the 0 in r3; every order of these three statements
         * tried here gives the two registers the other way round (7 rows). */
        lbl_805F1624 = 1;
        lbl_805F1614 = (lbl_805F1614 + 1) % lbl_805EDF34;
        lbl_805F161C = 0;"""
S = {
    "one": "        lbl_805F1624 = 1;",
    "mod": "        lbl_805F1614 = (lbl_805F1614 + 1) % lbl_805EDF34;",
    "zero": "        lbl_805F161C = 0;",
}
DECL = {
    "14": ("extern volatile RwInt32 lbl_805F1614;", "extern RwInt32 lbl_805F1614;"),
    "1C": ("extern RwInt32 lbl_805F161C;", "extern volatile RwInt32 lbl_805F161C;"),
    "24": ("extern RwInt32 lbl_805F1624;", "extern volatile RwInt32 lbl_805F1624;"),
}
REPL = {}
for order in itertools.permutations(["one", "mod", "zero"]):
    for bits in range(8):
        flips = [k for i, k in enumerate(["14", "1C", "24"]) if bits & (1 << i)]
        REPL["_".join(order) + "_" + "".join(flips)] = [(OLD, "\n".join(S[k] for k in order))] + [DECL[k] for k in flips]
SCORE = ["fn_80493ABC", "fn_804939DC", "fn_80493B98"]
