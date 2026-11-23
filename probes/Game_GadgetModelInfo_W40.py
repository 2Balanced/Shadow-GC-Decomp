"""GadgetModelInfo::fn_80162FB0 (wave 40): where the collider iterator is declared."""
A = """    if (!m_flags.Test(0) && !m_flags.Test(1)) {
        ColliDetectArgs args;
        for (ColliderIter it = m_colliders.begin(); it != m_colliders.end();
             ++it) {
"""
REPL = {
 "it_top": [(A, """    ColliderIter it;
    if (!m_flags.Test(0) && !m_flags.Test(1)) {
        ColliDetectArgs args;
        for (it = m_colliders.begin(); it != m_colliders.end();
             ++it) {
""")],
 "it_args_top": [(A, """    ColliDetectArgs args;
    ColliderIter it;
    if (!m_flags.Test(0) && !m_flags.Test(1)) {
        for (it = m_colliders.begin(); it != m_colliders.end();
             ++it) {
""")],
 "it_block": [(A, """    if (!m_flags.Test(0) && !m_flags.Test(1)) {
        ColliDetectArgs args;
        ColliderIter it;
        for (it = m_colliders.begin(); it != m_colliders.end();
             ++it) {
""")],
 "it_block_init": [(A, """    if (!m_flags.Test(0) && !m_flags.Test(1)) {
        ColliDetectArgs args;
        ColliderIter it = m_colliders.begin();
        for (; it != m_colliders.end();
             ++it) {
""")],
 "it_before_args": [(A, """    if (!m_flags.Test(0) && !m_flags.Test(1)) {
        ColliderIter it = m_colliders.begin();
        ColliDetectArgs args;
        for (; it != m_colliders.end();
             ++it) {
""")],
 "end_local": [(A, """    if (!m_flags.Test(0) && !m_flags.Test(1)) {
        ColliDetectArgs args;
        ColliderIter end = m_colliders.end();
        for (ColliderIter it = m_colliders.begin(); it != end;
             ++it) {
""")],
 "while": [(A, """    if (!m_flags.Test(0) && !m_flags.Test(1)) {
        ColliDetectArgs args;
        ColliderIter it = m_colliders.begin();
        while (it != m_colliders.end()) {
"""), ("                it->m_flags &= ~1;\n            }\n        }\n", "                it->m_flags &= ~1;\n            }\n            ++it;\n        }\n"), ]
}
SCORE = ["fn_80162FB0__15GadgetModelInfoFf"]
