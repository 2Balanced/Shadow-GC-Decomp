"""Wave 40 (r7): fn_8049375C's early return (retail ORs 0x300 with the palette bit at run time)."""

OLD = """                        format = 0x300;
                        return (depth == 4) ? (format | 0x4000) : (format | 0x2000);"""
FN = "RwInt32 fn_8049375C(RwDlImage *image) {"

def helper(body):
    return "static inline RwInt32 RwDlPalettised(RwInt32 format, RwInt32 depth) {\n" + body + "\n}\n\n" + FN

REPL = {
    "h_ternary": [(FN, helper("    return (depth == 4) ? (format | 0x4000) : (format | 0x2000);")),
                  (OLD, "                        return RwDlPalettised(0x300, depth);")],
    "h_ifelse": [(FN, helper("    if (depth == 4) return format | 0x4000;\n    return format | 0x2000;")),
                 (OLD, "                        return RwDlPalettised(0x300, depth);")],
    "h_ifelse2": [(FN, helper("    if (depth == 4) {\n        format |= 0x4000;\n    } else {\n        format |= 0x2000;\n    }\n    return format;")),
                  (OLD, "                        return RwDlPalettised(0x300, depth);")],
    "h_noinl": [(FN, helper("    return (depth == 4) ? (format | 0x4000) : (format | 0x2000);").replace("static inline", "static")),
                (OLD, "                        return RwDlPalettised(0x300, depth);")],
    "or_tern": [(OLD, "                        format = 0x300;\n                        return format | ((depth == 4) ? 0x4000 : 0x2000);")],
    "ifelse": [(OLD, "                        format = 0x300;\n                        if (depth == 4) return format | 0x4000;\n                        return format | 0x2000;")],
}
SCORE = ["fn_8049375C"]
