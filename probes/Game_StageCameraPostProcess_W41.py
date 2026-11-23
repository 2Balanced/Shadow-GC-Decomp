"""Wave 41: StageCameraPostProcess::Tick with copy propagation off (the wave-40 lever): retail keeps
the player index of the blocking loop in r28 and the iterator in r29."""
SCORE = ["Tick__Q29PJSCamera22StageCameraPostProcessFf"]
F = "void StageCameraPostProcess::Tick(float dt) {"
END = "bool StageCameraPostProcess::Restore("
P = [(F, "#pragma opt_propagation off\n" + F), (END, "#pragma opt_propagation reset\n" + END)]
OLD = """                s32 index;
                std::vector<boost::shared_ptr<CharInfo> >::iterator it;
                for (it = m_blocking.begin(), index = player; it != m_blocking.end(); ++it) {
"""
TOP = "                std::vector<boost::weak_ptr<CharInfo> >::iterator last;\n"
def v(*pairs):
    return P + list(pairs)
REPL = {
  "prag": v(),
  "prag_decl_swap": v((OLD, """                std::vector<boost::shared_ptr<CharInfo> >::iterator it;
                s32 index;
                for (it = m_blocking.begin(), index = player; it != m_blocking.end(); ++it) {
""")),
  "prag_init_swap": v((OLD, """                s32 index;
                std::vector<boost::shared_ptr<CharInfo> >::iterator it;
                for (index = player, it = m_blocking.begin(); it != m_blocking.end(); ++it) {
""")),
  "prag_index_init": v((OLD, """                s32 index = player;
                std::vector<boost::shared_ptr<CharInfo> >::iterator it;
                for (it = m_blocking.begin(); it != m_blocking.end(); ++it) {
""")),
  "prag_it_init": v((OLD, """                std::vector<boost::shared_ptr<CharInfo> >::iterator it = m_blocking.begin();
                s32 index = player;
                for (; it != m_blocking.end(); ++it) {
""")),
  "prag_for_decl": v((OLD, """                s32 index = player;
                for (std::vector<boost::shared_ptr<CharInfo> >::iterator it = m_blocking.begin(); it != m_blocking.end(); ++it) {
""")),
  "prag_no_index": v((OLD, """                for (std::vector<boost::shared_ptr<CharInfo> >::iterator it = m_blocking.begin(); it != m_blocking.end(); ++it) {
"""), ("new SetTransparentCommand(true, 0.4f, index)", "new SetTransparentCommand(true, 0.4f, player)")),
  "prag_top_ix": v((TOP, TOP + "                s32 index = player;\n"), (OLD, """                std::vector<boost::shared_ptr<CharInfo> >::iterator it;
                for (it = m_blocking.begin(); it != m_blocking.end(); ++it) {
""")),
  "prag_top_ix_before": v((TOP, "                s32 index = player;\n" + TOP), (OLD, """                std::vector<boost::shared_ptr<CharInfo> >::iterator it;
                for (it = m_blocking.begin(); it != m_blocking.end(); ++it) {
""")),
}
