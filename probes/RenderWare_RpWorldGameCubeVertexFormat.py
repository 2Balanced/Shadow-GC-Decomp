"""Wave 41: fn_8046FF34's parameter types."""
SCORE = ["fn_8046FF34"]
D = "void fn_8046FF34(RwGameCubeVtxDesc *desc, s32 attr, GXAttrType type)"
def v(a, t):
    new = "void fn_8046FF34(RwGameCubeVtxDesc *desc, %s attr, %s type)" % (a, t)
    return [(D + " {", new + " {")], {"Game/rw/r4.h": [(D + ";", new + ";")]}
REPL = {}
HDR = {}
for name, (a, t) in {"u32_u32": ("u32", "u32"), "s32_s32": ("s32", "s32"), "s32_u32": ("s32", "u32"),
                     "u32_s32": ("u32", "s32"), "int_int": ("int", "int"), "gx_gx": ("GXAttr", "GXAttrType"),
                     "s32_u8": ("s32", "u8")}.items():
    REPL[name], HDR[name] = v(a, t)
