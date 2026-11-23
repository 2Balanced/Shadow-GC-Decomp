# Round 4 for src/RenderWare/RwStreamChunk.c: RwStreamReadChunkHeaderInfo's inlined byte swap.
A = "/* RwStreamReadChunkHeaderInfo. */"
P = ["opt_unroll_loops off", "opt_propagation off", "opt_strength_reduction off", "opt_loop_invariants off",
     "opt_lifetimes off", "opt_dead_code off", "opt_common_subs off", "inline_depth(1)", "inline_depth(2)",
     "optimization_level 3", "optimization_level 2", "scheduling off", "peephole off"]
REPL = {}
for p in P:
    REPL[p.replace(' ', '_').replace('(', '').replace(')', '')] = [(A, "#pragma " + p + "\n" + A)]
SCORE = ["fn_8047D94C"]
