"""Wave 41 round 2: StageCameraPostProcess::Tick, the blocking loop's index as other kinds of copy
(propagation on), and the pragma with the predicate and erase spelled apart."""
SCORE = ["Tick__Q29PJSCamera22StageCameraPostProcessFf"]
F = "void StageCameraPostProcess::Tick(float dt) {"
END = "bool StageCameraPostProcess::Restore("
P = [(F, "#pragma opt_propagation off\n" + F), (END, "#pragma opt_propagation reset\n" + END)]
IDF = [(F, "inline s32 PassIndex(s32 v) { return v; }\n" + F)]
OLD = """                s32 index;
                std::vector<boost::shared_ptr<CharInfo> >::iterator it;
                for (it = m_blocking.begin(), index = player; it != m_blocking.end(); ++it) {
"""
ERASE = """                last = m_hidden.end();
                m_hidden.erase(std::remove_if(m_hidden.begin(), m_hidden.end(),
                                              boost::bind(&StageCameraPostProcess::Restore, this, _1)),
                               last);
"""
REPL = {
  "const_index": [(OLD, """                std::vector<boost::shared_ptr<CharInfo> >::iterator it;
                const s32 index = player;
                for (it = m_blocking.begin(); it != m_blocking.end(); ++it) {
""")],
  "const_index2": [(OLD, """                const s32 index = player;
                std::vector<boost::shared_ptr<CharInfo> >::iterator it;
                for (it = m_blocking.begin(); it != m_blocking.end(); ++it) {
""")],
  "id_index": IDF + [(OLD, """                s32 index;
                std::vector<boost::shared_ptr<CharInfo> >::iterator it;
                for (it = m_blocking.begin(), index = PassIndex(player); it != m_blocking.end(); ++it) {
""")],
  "id_index2": IDF + [(OLD, """                std::vector<boost::shared_ptr<CharInfo> >::iterator it;
                s32 index;
                for (it = m_blocking.begin(), index = PassIndex(player); it != m_blocking.end(); ++it) {
""")],
  "id_arg": IDF + [("new SetTransparentCommand(true, 0.4f, index)", "new SetTransparentCommand(true, 0.4f, PassIndex(player))"),
                    (OLD, """                std::vector<boost::shared_ptr<CharInfo> >::iterator it;
                for (it = m_blocking.begin(); it != m_blocking.end(); ++it) {
""")],
  "prag_pred_local": P + [(ERASE, """                last = m_hidden.end();
                m_hidden.erase(std::remove_if(m_hidden.begin(), m_hidden.end(),
                                              boost::bind(&StageCameraPostProcess::Restore, this, _1)),
                               last);
""".replace("last = m_hidden.end();", "last = m_hidden.end();"))],
  "prag_first": P + [(ERASE, """                last = m_hidden.end();
                std::vector<boost::weak_ptr<CharInfo> >::iterator first = std::remove_if(m_hidden.begin(), m_hidden.end(),
                                              boost::bind(&StageCameraPostProcess::Restore, this, _1));
                m_hidden.erase(first, last);
""")],
  "prag_ends": P + [(ERASE, """                last = m_hidden.end();
                std::vector<boost::weak_ptr<CharInfo> >::iterator e = m_hidden.end();
                std::vector<boost::weak_ptr<CharInfo> >::iterator b = m_hidden.begin();
                m_hidden.erase(std::remove_if(b, e, boost::bind(&StageCameraPostProcess::Restore, this, _1)), last);
""")],
}

BT = "boost::_bi::bind_t<bool, boost::_mfi::mf1<bool, StageCameraPostProcess, boost::weak_ptr<CharInfo> >, boost::_bi::list2<boost::_bi::value<StageCameraPostProcess *>, boost::arg<1> > >"
REPL.update({
  "prag_pred_named": P + [(ERASE, """                last = m_hidden.end();
                """ + BT + """ pred = boost::bind(&StageCameraPostProcess::Restore, this, _1);
                m_hidden.erase(std::remove_if(m_hidden.begin(), m_hidden.end(), pred), last);
""")],
  "prag_pred_named2": P + [(ERASE, """                """ + BT + """ pred = boost::bind(&StageCameraPostProcess::Restore, this, _1);
                last = m_hidden.end();
                m_hidden.erase(std::remove_if(m_hidden.begin(), m_hidden.end(), pred), last);
""")],
})
