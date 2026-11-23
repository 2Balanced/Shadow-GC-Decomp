"""Wave 40 (r2), RwCpp16 round 9: the float constants as const objects (their loads may then move
above the prologue's stores, as in retail's fn_804532E4 and fn_804535C8)."""
SCORE = ['fn_80452434', 'fn_80452730', 'fn_80452744', 'fn_80452860', 'fn_80452898', 'fn_8045294C', 'fn_80452A40', 'fn_80452B90', 'fn_80452BD8', 'fn_80452C04', 'fn_80452C4C', 'fn_80452C78', 'fn_80452C80', 'fn_80452C88', 'fn_80452C98', 'fn_80452CDC', 'fn_80453018', 'fn_804530CC', 'fn_804531B0', 'fn_804532A4', 'fn_804532E4', 'fn_80453350', 'fn_804533E0', 'fn_8045345C', 'fn_80453500', 'fn_80453594', 'fn_804535C8', 'fn_804536B4', 'fn_80453770', 'fn_80453818', 'fn_8045381C', 'fn_80453820', 'fn_80453980', 'fn_80453A14', 'fn_80453A78', 'fn_80453B24', 'fn_80453B64', 'fn_80453C04', 'fn_80453D08', 'fn_80453E24']
C = ["lbl_805FB0D8", "lbl_805FB0DC", "lbl_805FB0E0", "lbl_805FB0E4", "lbl_805FB0E8", "lbl_805FB0EC", "lbl_805FB0F0"]
REPL = {
 "base": [("extern float lbl_805FB0D8;", "extern float lbl_805FB0D8;")],
 "allconst": [("extern float %s;" % c, "extern const float %s;" % c) for c in C],
 "d8e0": [("extern float %s;" % c, "extern const float %s;" % c) for c in ("lbl_805FB0D8", "lbl_805FB0E0")],
}
