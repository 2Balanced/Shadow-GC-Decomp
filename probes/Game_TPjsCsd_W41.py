"""Wave 41: fn_8003CEF8 with copy propagation off (the wave-40 lever)."""
SCORE = ["fn_8003CEF8"]
F = '/* Adds the scenes of list named names (fn_8003CEF8); false when one is missing. */\nextern "C" bool fn_8003CEF8('
END = "/* The scene named name, or 0 (fn_8003CEAC). */"
P = [(F, "#pragma opt_propagation off\n" + F), (END, "#pragma opt_propagation reset\n" + END)]
V = "        bool valid = tour->GetScene() != lbl_805EE118;\n        if (valid) {\n"
REPL = {
  "prag": P,
  "prag_novalid": P + [(V, "        if (tour->GetScene() != lbl_805EE118) {\n")],
  "novalid": [(V, "        if (tour->GetScene() != lbl_805EE118) {\n")],
}

OK = "    bool ok = true;\n    for (int i = 0; i < count; i++) {\n"
AND = "            ok = ok & fn_8003D080(set, tour, names[i]);\n"
REPL.update({
  "prag_andeq": P + [(AND, "            ok &= fn_8003D080(set, tour, names[i]);\n")],
  "prag_i_first": P + [(OK, "    int i;\n    bool ok = true;\n    for (i = 0; i < count; i++) {\n")],
  "prag_i_first_andeq": P + [(OK, "    int i;\n    bool ok = true;\n    for (i = 0; i < count; i++) {\n"), (AND, "            ok &= fn_8003D080(set, tour, names[i]);\n")],
  "andeq": [(AND, "            ok &= fn_8003D080(set, tour, names[i]);\n")],
  "i_first_andeq": [(OK, "    int i;\n    bool ok = true;\n    for (i = 0; i < count; i++) {\n"), (AND, "            ok &= fn_8003D080(set, tour, names[i]);\n")],
})

NEWT = "        TPjsSceneTour *tour = new TPjsSceneTour(fn_8036F470(list.Get(), names[i]));\n"
BASE = P + [(OK, "    int i;\n    bool ok = true;\n    for (i = 0; i < count; i++) {\n"), (AND, "            ok &= fn_8003D080(set, tour, names[i]);\n")]
REPL.update({
  "t_assign": BASE + [(NEWT, "        TPjsSceneTour *tour;\n        tour = new TPjsSceneTour(fn_8036F470(list.Get(), names[i]));\n")],
  "t_top": [(OK, "    int i;\n    TPjsSceneTour *tour;\n    bool ok = true;\n    for (i = 0; i < count; i++) {\n")] + P + [(AND, "            ok &= fn_8003D080(set, tour, names[i]);\n"), (NEWT, "        tour = new TPjsSceneTour(fn_8036F470(list.Get(), names[i]));\n")],
  "t_top2": [(OK, "    int i;\n    bool ok = true;\n    TPjsSceneTour *tour;\n    for (i = 0; i < count; i++) {\n")] + P + [(AND, "            ok &= fn_8003D080(set, tour, names[i]);\n"), (NEWT, "        tour = new TPjsSceneTour(fn_8036F470(list.Get(), names[i]));\n")],
  "t_top0": [(OK, "    TPjsSceneTour *tour;\n    int i;\n    bool ok = true;\n    for (i = 0; i < count; i++) {\n")] + P + [(AND, "            ok &= fn_8003D080(set, tour, names[i]);\n"), (NEWT, "        tour = new TPjsSceneTour(fn_8036F470(list.Get(), names[i]));\n")],
})
