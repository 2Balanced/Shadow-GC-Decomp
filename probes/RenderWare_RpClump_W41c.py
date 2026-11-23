"""Wave 41, RenderWare/RpClump: RpAtomicCreate (fn_80456DD8) scratch registers."""
SCORE = ["fn_80456DD8"]
TAIL = """    ((s16 *)object)[40] = 0;
    ((s16 *)object)[41] = 0;
    { f32 other = 1.0f;
    ((f32 *)object)[21] = other;
    ((f32 *)object)[22] = other;
    ((f32 *)object)[23] = 0.0f;
    fields[19] = 3;
    fields[17] = 0;
    fields[16] = 0;
    fields[15] = 0;
    fields[27] = 0;
    fields[25] = (u32)(fields + 25);
    fields[26] = (u32)(fields + 25);"""
def t(body):
    return [(TAIL, body)]
REPL = {
    "base": [],
    "chain": t(TAIL.replace("    fields[25] = (u32)(fields + 25);\n    fields[26] = (u32)(fields + 25);", "    fields[25] = fields[26] = (u32)(fields + 25);")),
    "chain2": t(TAIL.replace("    fields[25] = (u32)(fields + 25);\n    fields[26] = (u32)(fields + 25);", "    fields[26] = fields[25] = (u32)(fields + 25);")),
    "s16_chain": t(TAIL.replace("    ((s16 *)object)[40] = 0;\n    ((s16 *)object)[41] = 0;", "    ((s16 *)object)[41] = ((s16 *)object)[40] = 0;")),
    "s16_chain_b": t(TAIL.replace("    ((s16 *)object)[40] = 0;\n    ((s16 *)object)[41] = 0;", "    ((s16 *)object)[40] = ((s16 *)object)[41] = 0;")),
    "z_chain": t(TAIL.replace("    fields[17] = 0;\n    fields[16] = 0;\n    fields[15] = 0;\n    fields[27] = 0;", "    fields[27] = fields[15] = fields[16] = fields[17] = 0;")),
    "z_chain2": t(TAIL.replace("    fields[17] = 0;\n    fields[16] = 0;\n    fields[15] = 0;\n    fields[27] = 0;", "    fields[17] = fields[16] = fields[15] = 0;\n    fields[27] = 0;")),
    "all_chain": t(TAIL.replace("    ((s16 *)object)[40] = 0;\n    ((s16 *)object)[41] = 0;", "    ((s16 *)object)[41] = ((s16 *)object)[40] = 0;").replace("    fields[17] = 0;\n    fields[16] = 0;\n    fields[15] = 0;\n    fields[27] = 0;", "    fields[17] = fields[16] = fields[15] = 0;\n    fields[27] = 0;").replace("    fields[25] = (u32)(fields + 25);\n    fields[26] = (u32)(fields + 25);", "    fields[25] = fields[26] = (u32)(fields + 25);")),
}
