"""Round 2 for StageCameraPostProcess::Tick (wave 39): where index and the iterators are declared."""
SCORE = ["Tick__Q29PJSCamera22StageCameraPostProcessFf"]
TOP = "                std::vector<boost::weak_ptr<CharInfo> >::iterator last;\n"
OLD = """                s32 index;
                std::vector<boost::shared_ptr<CharInfo> >::iterator it;
                for (it = m_blocking.begin(), index = player; it != m_blocking.end(); ++it) {
"""
LOOP = "                for (it = m_blocking.begin(), index = player; it != m_blocking.end(); ++it) {\n"
IT = "                std::vector<boost::shared_ptr<CharInfo> >::iterator it;\n"
IX = "                s32 index;\n"
REPL = {
  "top_index_after_last": [(TOP, TOP + IX), (OLD, IT + LOOP)],
  "top_index_before_last": [(TOP, IX + TOP), (OLD, IT + LOOP)],
  "top_it_after_last": [(TOP, TOP + IT), (OLD, IX + LOOP)],
  "top_it_before_last": [(TOP, IT + TOP), (OLD, IX + LOOP)],
  "top_both_ix_it": [(TOP, TOP + IX + IT), (OLD, LOOP)],
  "top_both_it_ix": [(TOP, TOP + IT + IX), (OLD, LOOP)],
  "top_it_last_ix": [(TOP, IT + TOP + IX), (OLD, LOOP)],
  "top_ix_last_it": [(TOP, IX + TOP + IT), (OLD, LOOP)],
  "top_ix_it_last": [(TOP, IX + IT + TOP), (OLD, LOOP)],
  "top_it_ix_last": [(TOP, IT + IX + TOP), (OLD, LOOP)],
  "last_reuse": [(OLD, """                s32 index;
                std::vector<boost::shared_ptr<CharInfo> >::iterator it;
                for (it = m_blocking.begin(), index = player; it != m_blocking.end(); ++it) {
""".replace("s32 index;", "int index;"))],
}
