"""Wave 40 (r7): fn_80493ABC's last three statements (retail: 1 in r0, 0 in r3)."""
import itertools

OLD = """        /* Retail keeps the 1 in r0 and the 0 in r3; every order of these three statements
         * tried here gives the two registers the other way round (7 rows). */
        lbl_805F1624 = 1;
        lbl_805F1614 = (lbl_805F1614 + 1) % lbl_805EDF34;
        lbl_805F161C = 0;"""
S = {
    "one": "        lbl_805F1624 = 1;",
    "mod": "        lbl_805F1614 = (lbl_805F1614 + 1) % lbl_805EDF34;",
    "inl": "        lbl_805F1614 = RwDlQueueNext(lbl_805F1614);",
    "zero": "        lbl_805F161C = 0;",
}
REPL = {}
for m in ("mod", "inl"):
    for order in itertools.permutations(["one", m, "zero"]):
        REPL["_".join(order)] = [(OLD, "\n".join(S[k] for k in order))]
SCORE = ["fn_80493ABC"]
