"""Source shapes for StageCameraPostProcess::Tick (wave 39): retail keeps the player index of the
blocking loop in r28 and the iterator in r29; GC/2.7 had them the other way round."""
SCORE = ["Tick__Q29PJSCamera22StageCameraPostProcessFf"]
OLD = """                s32 index;
                std::vector<boost::shared_ptr<CharInfo> >::iterator it;
                for (it = m_blocking.begin(), index = player; it != m_blocking.end(); ++it) {
"""
REPL = {
  "decl_swap": [(OLD, """                std::vector<boost::shared_ptr<CharInfo> >::iterator it;
                s32 index;
                for (it = m_blocking.begin(), index = player; it != m_blocking.end(); ++it) {
""")],
  "init_swap": [(OLD, """                s32 index;
                std::vector<boost::shared_ptr<CharInfo> >::iterator it;
                for (index = player, it = m_blocking.begin(); it != m_blocking.end(); ++it) {
""")],
  "both_swap": [(OLD, """                std::vector<boost::shared_ptr<CharInfo> >::iterator it;
                s32 index;
                for (index = player, it = m_blocking.begin(); it != m_blocking.end(); ++it) {
""")],
  "index_init": [(OLD, """                s32 index = player;
                std::vector<boost::shared_ptr<CharInfo> >::iterator it;
                for (it = m_blocking.begin(); it != m_blocking.end(); ++it) {
""")],
  "it_init": [(OLD, """                std::vector<boost::shared_ptr<CharInfo> >::iterator it = m_blocking.begin();
                s32 index = player;
                for (; it != m_blocking.end(); ++it) {
""")],
  "for_decl": [(OLD, """                s32 index = player;
                for (std::vector<boost::shared_ptr<CharInfo> >::iterator it = m_blocking.begin(); it != m_blocking.end(); ++it) {
""")],
  "no_index": [(OLD, """                for (std::vector<boost::shared_ptr<CharInfo> >::iterator it = m_blocking.begin(); it != m_blocking.end(); ++it) {
"""), ("new SetTransparentCommand(true, 0.4f, index)", "new SetTransparentCommand(true, 0.4f, player)")],
}
