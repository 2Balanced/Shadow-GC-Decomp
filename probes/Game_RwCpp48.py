# Probes for Game/RwCpp48: _rwDeviceSystemRequest (fn_80485C3C), case 13's store of 1.
# Earlier rounds: fn_80486990 matched with the parameter as loop variable and inline_depth(8);
# RwEngineOpen and RwEngineInit with the device open and the module registration as inline
# helpers (their results come back through a temporary, their error blocks are numbered last).
# Round: case 13 spelled six ways (order, types, result as the value): all 2 rows (CSE of 1).
SW = open("build/t/sw.txt").read()
HELPER = """static inline RwBool DeviceSystemDefault(s32 request, void *out, s32 number) {
    switch (request) {
    case 17:
    case 18:
        return TRUE;
    case 13:
        *(s32 *)out = 1;
        return TRUE;
    case 14:
        if (number == 0) {
            RwEngineInstance->stringFuncs.vecStrcpy((char *)out, lbl_8056DFC8);
            return TRUE;
        }
        return FALSE;
    case 15:
        *(s32 *)out = 0;
        return TRUE;
    case 16:
        return (number == 0);
    }
    return FALSE;
}

"""
HELPER2 = HELPER.replace("""        if (number == 0) {
            RwEngineInstance->stringFuncs.vecStrcpy((char *)out, lbl_8056DFC8);
            return TRUE;
        }
        return FALSE;""", """        {
            RwBool ok = (number == 0);

            if (ok) {
                RwEngineInstance->stringFuncs.vecStrcpy((char *)out, lbl_8056DFC8);
            }
            return ok;
        }""")
ANCH = "/* _rwDeviceSystemRequest: the device's system function"
REPL = {
    "base": [],
    "helper": [(SW, "    if (!result) {\n        result = DeviceSystemDefault(request, out, number);\n    }\n"), (ANCH, HELPER + ANCH)],
    "helper2": [(SW, "    if (!result) {\n        result = DeviceSystemDefault(request, out, number);\n    }\n"), (ANCH, HELPER2 + ANCH)],
}
SCORE = ["fn_80485C3C", "fn_80485EF4", "fn_80485FE0", "fn_80486204", "fn_80485DE4"]
