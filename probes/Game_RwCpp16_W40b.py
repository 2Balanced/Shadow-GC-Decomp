"""Wave 40 (r2), RwCpp16 round 2: increment order of fn_804531B0's loop (retail i++ first)."""
SCORE = ["fn_804531B0"]
L0 = "    for (i = 0; i < count; i++, node += 0x20) {\n"
REPL = {
 "base": [(L0, L0)],
 "swap": [(L0, "    for (i = 0; i < count; node += 0x20, i++) {\n")],
}
