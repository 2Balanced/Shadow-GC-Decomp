"""Wave 40: BkChibiChaos::Update's upper height test (retail loads the position first and compares
m_maxY with it)."""
SCORE = ["Update__Q27BkChaos12BkChibiChaosFf"]
F = "void BkChibiChaos::Update(float dt) {"
A = "        if (m_info->m_position.y > m_owner->m_maxY) {"
A1 = "        if (m_info->m_position.y < m_owner->m_minY) {"
HELP = """inline float PosY_(const boost::shared_ptr<CharInfo> &info) { return info->m_position.y; }
inline const Vec3f &Pos_(CharInfo *info) { return info->m_position; }
inline float Y_(const Vec3f &v) { return v.y; }
inline bool Less_(float a, float b) { return a < b; }
inline bool Greater_(float a, float b) { return a > b; }
"""
H = (F, HELP + F)
REPL = {
  "lt": [(A, "        if (m_owner->m_maxY < m_info->m_position.y) {")],
  "lt_pos": [H, (A, "        if (m_owner->m_maxY < Pos_(m_info.get()).y) {")],
  "lt_y": [H, (A, "        if (m_owner->m_maxY < Y_(m_info->m_position)) {")],
  "lt_posy": [H, (A, "        if (m_owner->m_maxY < PosY_(m_info)) {")],
  "less": [H, (A, "        if (Less_(m_owner->m_maxY, m_info->m_position.y)) {")],
  "greater": [H, (A, "        if (Greater_(m_info->m_position.y, m_owner->m_maxY)) {")],
  "lt_both": [H, (A, "        if (m_owner->m_maxY < Pos_(m_info.get()).y) {"), (A1, "        if (Pos_(m_info.get()).y < m_owner->m_minY) {")],
  "less_both": [H, (A, "        if (Less_(m_owner->m_maxY, m_info->m_position.y)) {"), (A1, "        if (Less_(m_info->m_position.y, m_owner->m_minY)) {")],
}
