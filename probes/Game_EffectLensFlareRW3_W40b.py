"""Wave 40: where UnkLensFlareParam's constructor is defined.  Retail has its code last in the
unit's .text (0x8043232C) but its exception table fourth, behind the element's constructor.
This table only builds the variants (score = the constructor itself); the two orders are read with
build/t/etb.py-style listings of build/pprobe/<name>/EffectLensFlareRW3.o (text order by nm,
extab order by the extabindex relocations)."""
SCORE = ["__ct__Q36Effect3RW326UnkLensFlareParam_8043232CFUcUcUc"]
_src = open("src/Game/EffectLensFlareRW3.cpp", encoding="utf-8").read()
_a = _src.index("UnkLensFlareParam_8043232C::UnkLensFlareParam_8043232C(u8 enabled_, u8 alpha_,")
_b = _src.index('extern "C" {', _a)
DEF = _src[_a:_b]
END = "} // namespace RW3\n} // namespace Effect\n"
HD = "    UnkLensFlareParam_8043232C(u8 enabled_ = 0, u8 alpha_ = 0, u8 whiteout_ = 0);"
INCLASS = """    UnkLensFlareParam_8043232C(u8 enabled_ = 0, u8 alpha_ = 0, u8 whiteout_ = 0)
        : enabled(enabled_), alpha(alpha_), whiteout(whiteout_) {
        UnkLensFlarePosition_8043232C zero = {0.0f, 0.0f, 0.0f};
        position.x = zero.x;
        position.y = zero.y;
        position.z = zero.z;
    }"""
REPL = {
    "first_plain": [(DEF, DEF)],
    "first_inline": [(DEF, "inline " + DEF)],
    "end_inline": [(DEF, ""), (END, END + "\ninline " + DEF)],
    "end_plain": [(DEF, ""), (END, END + "\n" + DEF)],
    "decl_inline": [(DEF, DEF)],
    "inclass": [(DEF, "")],
    "pragma_defer": [(DEF, "#pragma defer_codegen on\n" + DEF + "#pragma defer_codegen reset\n")],
    "pragma_inl_depth": [(DEF, "#pragma always_inline on\n" + DEF + "#pragma always_inline reset\n")],
}
HDR = {"decl_inline": {"Game/EffectManagerPJS.h": [(HD, "    inline" + HD[3:])]},
       "inclass": {"Game/EffectManagerPJS.h": [(HD, INCLASS)]}}
