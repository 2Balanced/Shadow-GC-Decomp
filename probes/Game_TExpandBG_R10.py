"""fn_80043FB8 of Game/TExpandBG.cpp (wave 39, second pass): pragmas for the last 3 rows (retail
copies dst to start after the other three set-ups and has no blr after the loop)."""
A = 'extern "C" u32 fn_80043FB8(const u8 *src, u8 *dst) {\n'
P = ["scheduling off", "peephole off", "optimize_for_size on", "optimization_level 2", "optimization_level 3", "optimization_level 1", "opt_dead_code off", "opt_propagation off", "opt_lifetimes off", "opt_loop_invariants off", "opt_strength_reduction off", "opt_dead_assignments off", "opt_unroll_loops off", "global_optimizer off"]
REPL = {p.replace(" ", "_"): [(A, "#pragma " + p + "\n" + A)] for p in P}
SCORE = ["fn_80043FB8"]
