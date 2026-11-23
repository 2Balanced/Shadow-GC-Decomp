"""Wave 41: where the deferred inline constructors take their pragma state from."""
SCORE = ["__ct__32DevilDoomModeCtrlUnit_BELOW_HALFFP17DevilDoomVarParam", "__ct__27DevilDoomModeCtrlUnit_DEATHFP17DevilDoomVarParam"]
CM = "BossCtrlUnit *DevilDoomModeCtrlUnit::CreateMode(int mode) {\n"
T = """int DevilDoomModeCtrlUnit::Tick(float dt) {
    return BossModeCtrlUnit::Tick(dt);
}
"""
P0 = "#pragma opt_propagation off\n"
REPL = {
  "base": [(CM, CM)],
  "prag_createmode": [(CM, P0 + CM)],
  "prag_eof": [(T, T + P0)],
}
