"""Wave 41: does a pragma at the class reach the deferred inline constructor? (sanity check)"""
SCORE = ["__ct__32DevilDoomModeCtrlUnit_BELOW_HALFFP17DevilDoomVarParam"]
C = "class DevilDoomModeCtrlUnit_BELOW_HALF : public DevilDoomCtrlUnit {\n"
CM = "BossCtrlUnit *DevilDoomModeCtrlUnit::CreateMode(int mode) {\n"
T = """int DevilDoomModeCtrlUnit::Tick(float dt) {
    return BossModeCtrlUnit::Tick(dt);
}
"""
REPL = {
  "sched_class": [(C, "#pragma scheduling off\n" + C)],
  "sched_createmode": [(CM, "#pragma scheduling off\n" + CM)],
  "sched_eof": [(T, T + "#pragma scheduling off\n")],
}
