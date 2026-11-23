"""TargetManageTask (wave 40): loops of one pass.
fn_801E848C: the default distance assigned in one (retail loads it straight into f31)."""
A = "        nearestDistance = 999999.9f;\n"
W = "for (int n = 0; n < 1; n++) {\n"
REPL = {
 "far_for": [(A, "        " + W + "    " + A + "        }\n")],
}
SCORE = ["fn_801E848C"]
