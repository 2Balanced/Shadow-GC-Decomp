"""Wave 41: fn_800201D4 and fn_8001EE0C with copy propagation off (the wave-40 lever)."""
SCORE = ["fn_800201D4", "fn_8001EE0C"]
F1 = '/* A sphere against a world triangle: the nearest point of the triangle to the sphere\'s centre. */\nextern "C" void fn_800201D4('
F2 = '/* A sphere moving through a world: its walk of the world\'s sectors and of their collision trees,\n * testing each triangle (fn_800221A8 or, once the sphere has stopped, fn_80024AC8). */\nextern "C" CollisionResultList *fn_8001EE0C('
REPL = {
  "prag1": [(F1, "#pragma opt_propagation off\n" + F1)],
  "prag2": [(F2, "#pragma opt_propagation off\n" + F2)],
}
E1 = "/* The triangle pool's destructor. */"
E2 = "/* The list's first and end results, an iterator at a result, and a contact's edge or corner. */"
REPL = {
  "prag1": [(F1, "#pragma opt_propagation off\n" + F1), (E1, "#pragma opt_propagation reset\n" + E1)],
  "prag2": [(F2, "#pragma opt_propagation off\n" + F2), (E2, "#pragma opt_propagation reset\n" + E2)],
  "base": [],
}
