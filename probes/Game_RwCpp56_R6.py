"""Wave 40 (r7): fn_80493ABC: volatile frame-queue flags keep retail's store order."""
import itertools

OLD = """        /* Retail keeps the 1 in r0 and the 0 in r3; every order of these three statements
         * tried here gives the two registers the other way round (7 rows). */
        lbl_805F1624 = 1;
        lbl_805F1614 = (lbl_805F1614 + 1) % lbl_805EDF34;
        lbl_805F161C = 0;"""
NEW = """        lbl_805F1624 = 1;
        lbl_805F1614 = (lbl_805F1614 + 1) % lbl_805EDF34;
        lbl_805F161C = 0;"""
D1C = ("extern RwInt32 lbl_805F161C;", "extern volatile RwInt32 lbl_805F161C;")
D24 = ("extern RwInt32 lbl_805F1624;", "extern volatile RwInt32 lbl_805F1624;")
REPL = {
    "v1c": [(OLD, NEW), D1C],
    "v24": [(OLD, NEW), D24],
    "vboth": [(OLD, NEW), D1C, D24],
}
SCORE = ["fn_80493ABC", "fn_804939DC", "fn_80493B98"]
