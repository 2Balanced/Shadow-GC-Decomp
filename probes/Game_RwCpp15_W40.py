"""Wave 40 (r2), RwCpp15 round 1: RpSkinAtomicSetType (fn_80451E6C) as a real switch."""
SCORE = ["fn_80451E6C"]
B0 = """    s32 value;
    if (mode == 1) goto modeOne;
    if (mode >= 1) {
        if (mode >= 3) goto modeDefault;
        goto modeTwo;
    }
    goto modeDefault;

modeOne:
    record[11] = 1;
    value = 1;
    goto modeDone;
modeTwo:
    record[11] = 2;
    value = 2;
    goto modeDone;
modeDefault:
    if (fn_80458C90(0x120) > 0 && fn_804A2064(object) != 0) {
        record[11] = 2;
        value = 2;
    } else {
        record[11] = 1;
        value = 1;
    }
modeDone:
"""
SW = """    s32 value;
    switch (mode) {
    case 1:
        record[11] = 1;
        value = 1;
        break;
    case 2:
        record[11] = 2;
        value = 2;
        break;
    default:
        if (fn_80458C90(0x120) > 0 && fn_804A2064(object) != 0) {
            record[11] = 2;
            value = 2;
        } else {
            record[11] = 1;
            value = 1;
        }
        break;
    }
"""
REPL = {
 "base": [(B0, B0)],
 "switch": [(B0, SW)],
 "switch3": [(B0, SW.replace("    default:\n", "    case 3:\n    default:\n"))],
}
