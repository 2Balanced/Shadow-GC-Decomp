"""Wave 40: BkChibiChaos::Update's upper height test: retail loads the position's y first and
compares m_maxY < y (fcmpo maxY,y; bge)."""
SCORE = ["Update__Q27BkChaos12BkChibiChaosFf"]
F = "void BkChibiChaos::Update(float dt) {"
A = """        if (m_info->m_position.y > m_owner->m_maxY) {
            m_info->m_position.y = m_owner->m_maxY;"""
HELP = """inline float PosY2_(CharInfo *info) { return info->m_position.y; }
inline float MaxY2_(BkChaos *chaos) { return chaos->m_maxY; }
"""
H = (F, HELP + F)
REPL = {
  "inl_posy": [H, (A, """        if (m_owner->m_maxY < PosY2_(m_info.get())) {
            m_info->m_position.y = m_owner->m_maxY;""")],
  "local_y": [(A, """        float y = m_info->m_position.y;
        if (m_owner->m_maxY < y) {
            m_info->m_position.y = m_owner->m_maxY;""")],
  "local_y_block": [(A, """        {
        float y = m_info->m_position.y;
        if (m_owner->m_maxY < y) {
            m_info->m_position.y = m_owner->m_maxY;"""), ("""            m_info->field_0x18.y = 0.0f;
        }
        if (m_flags & 2) {""", """            m_info->field_0x18.y = 0.0f;
        }
        }
        if (m_flags & 2) {""")],
  "inl_both": [H, (A, """        if (MaxY2_(m_owner) < PosY2_(m_info.get())) {
            m_info->m_position.y = m_owner->m_maxY;""")],
}
