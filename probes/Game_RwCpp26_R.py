"""Reinstance callback fn_80463840 of Game/RwCpp26.cpp (wave 40, agent r3)."""
REPL = {
    "volatile": [("extern u16 lbl_805EDF40;", "extern volatile u16 lbl_805EDF40;")],
    "rev": [("    if (header->serialNum == lbl_805EDF40) {", "    if (lbl_805EDF40 == header->serialNum) {")],
    "rev_vol": [("extern u16 lbl_805EDF40;", "extern volatile u16 lbl_805EDF40;"), ("    if (header->serialNum == lbl_805EDF40) {", "    if (lbl_805EDF40 == header->serialNum) {")],
}
SCORE = ["fn_80463840"]
