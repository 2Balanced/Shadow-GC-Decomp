"""Source shapes for Player::PathImpl::ChooseBranch(joint) (wave 39): retail keeps this and then
the branch in r31 and the result in r30.  GC/2.7 hands the saved registers to locals in declaration
order, so the branch pointer is declared before the result."""
SCORE = ["ChooseBranch__Q26Player8PathImplFPQ312SonicteamUSA6System13JointPathData"]
A = "    float best = -100.0f;\n    Vec3f direction;\n    JointPathData *result = 0;\n"
B = "        JointPathData *branch = reinterpret_cast<JointPathData *>(it.m_node + 8);\n"
B2 = "        branch = reinterpret_cast<JointPathData *>(it.m_node + 8);\n"
REPL = {
  "branch_before_result": [(A, "    float best = -100.0f;\n    Vec3f direction;\n    JointPathData *branch;\n    JointPathData *result = 0;\n"), (B, B2)],
  "branch_first": [(A, "    JointPathData *branch;\n    float best = -100.0f;\n    Vec3f direction;\n    JointPathData *result = 0;\n"), (B, B2)],
  "branch_after_result": [(A, "    float best = -100.0f;\n    Vec3f direction;\n    JointPathData *result = 0;\n    JointPathData *branch;\n"), (B, B2)],
}
