"""Round 3 for fn_801E7094 of Game/Target.cpp (wave 39): pragmas around the function, to see which
optimization turns the two returns into addic/subfe."""
SCORE = ["fn_801E7094"]
A = '/* Whether a target still exists and is active (name inferred). */\nextern "C" bool fn_801E7094('
REPL = {}
for name, on, off in [
    ("peephole", "#pragma peephole off", "#pragma peephole reset"),
    ("sched", "#pragma scheduling off", "#pragma scheduling reset"),
    ("opt3", "#pragma optimization_level 3", "#pragma optimization_level reset"),
    ("opt2", "#pragma optimization_level 2", "#pragma optimization_level reset"),
    ("opt1", "#pragma optimization_level 1", "#pragma optimization_level reset"),
    ("opt0", "#pragma optimization_level 0", "#pragma optimization_level reset"),
    ("prop", "#pragma opt_propagation off", "#pragma opt_propagation reset"),
    ("cse", "#pragma opt_common_subs off", "#pragma opt_common_subs reset"),
    ("dead", "#pragma opt_dead_code off", "#pragma opt_dead_code reset"),
    ("life", "#pragma opt_lifetimes off", "#pragma opt_lifetimes reset"),
    ("space", "#pragma optimize_for_size off", "#pragma optimize_for_size reset"),
    ("unroll", "#pragma opt_strength_reduction off", "#pragma opt_strength_reduction reset"),
]:
    REPL[name] = [(A, on + "\n" + A), ("    return true;\n}\n", "    return true;\n}\n" + off + "\n")]
