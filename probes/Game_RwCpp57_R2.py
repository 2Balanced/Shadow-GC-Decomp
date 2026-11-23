"""Wave 40 (r7): fn_80495558 calls fn_80495428 in retail; GC/1.3.2..2.7 inline it under -inline auto."""

A = "/* Bytes of one mip level of a texture raster"
B = "/* Bytes of all the mip levels of a texture raster. */"
C = "/* Bytes of one row of tiles of a mip level"
REPL = {
    "auto_off_428": [(A, "#pragma auto_inline off\n" + A), (B, "#pragma auto_inline reset\n" + B)],
    "dont_558": [(B, "#pragma dont_inline on\n" + B), (C, "#pragma dont_inline reset\n" + C)],
    "auto_off_558": [(B, "#pragma auto_inline off\n" + B), (C, "#pragma auto_inline reset\n" + C)],
}
SCORE = ["fn_80495558", "fn_80495428"]
