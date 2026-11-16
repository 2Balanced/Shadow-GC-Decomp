"""Record of the source shapes tried for UnkGlare_80430524::Draw (wave 39; the unit is Matching).

Before this wave the shrink step was written out in Draw: `color = param * lbl_8056AC30;
src = ...; dst = ...; SetColor(color, color, color, 0xFF); SHRINK(src, dst);` with SHRINK a macro.
That left 23 rows: the brightness byte and the vertex pointer traded r4/r5 in the three colour
loops fed by a float product, and the first Add of the "separate" branch held its rasters in
r31/r29 instead of r29/r31.

What did not move the 23 rows (FAMILIES, about 110 compiles): none of them changes the instruction
stream, only which virtual register the allocator reaches first.

What did: the shrink step as an inline member `Shrink(dst, src, color)` whose arguments are the
expressions themselves. Of the six parameter orders, (dst, src, color) and (dst, color, src) give
0 rows; (src, dst, color) 15; the three with the colour before dst 33 to 43 (they also fold the
`addi` of lbl_8056AC30's address into its `lfs`). Naming the Add arguments instead (`dst = dims;
src = texture; Add(dst, src)`) fixes the Add rows alone and is not needed with the inline Shrink.
"""

SYM = "Draw__17UnkGlare_80430524Fv"
SYMBOLS = {SYM: "void UnkGlare_80430524::Draw() {"}

FAMILIES = [
    "declaration order of src, dst and color; color declared in each block",
    "color as int, u32, u16, s8, float, const u8; an int temporary before it",
    "statement order of the color, src and dst assignments (6 orders)",
    "SetColor with int, u32 or const-reference parameters, with a pointer walk, with local copies",
    "the colour loop as a macro (own or shared counter)",
    "a one-parameter SetGrey, with the conversion in the caller, in the inline, before or in the loop",
    "the product spelled lbl * param, with explicit (u8) or (s32) casts, three times as arguments",
    "named locals for the camera, the separate/large flags and the level around the first Add",
    "Add with its parameters swapped, as a macro, and with src/dst or new locals as arguments",
]

BODIES = {SYM: {}}
