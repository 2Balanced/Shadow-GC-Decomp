"""Wave 41: '#pragma opt_propagation off' (and a few other single pragmas) around one function.
Driven by the environment: W41_UNIT (Game/X), W41_ANCHOR (text that starts the function),
W41_SYM (symbol to score)."""
import os
SRC = open(os.path.join("src", os.environ["W41_UNIT"] + (".c" if os.environ["W41_UNIT"].startswith("dolphin") else ".cpp")), encoding="utf-8").read()
A = os.environ["W41_ANCHOR"]
s = SRC.index(A)
# find the function's closing brace
i = SRC.index("{", s + len(A) - 1)
depth = 0
j = i
while True:
    c = SRC[j]
    if c == "{":
        depth += 1
    elif c == "}":
        depth -= 1
        if depth == 0:
            break
    j += 1
FN = SRC[s:j + 1]
SCORE = [os.environ["W41_SYM"]]
P = os.environ.get("W41_PRAGMAS", "opt_propagation off|opt_common_subs off|opt_lifetimes off|opt_dead_assignments off|scheduling off|opt_loop_invariants off|opt_strength_reduction off").split("|")
REPL = {}
for p in P:
    name = p.split()[0]
    REPL["prag_" + name] = [(FN, "#pragma push\n#pragma " + p + "\n" + FN + "\n#pragma pop\n")]
