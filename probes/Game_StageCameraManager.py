"""Wave 40: ChangeState's two comparisons of a mode's id with the handle mode's (the mode pointer
kept across the handle's call is r26 in retail, r27 from GC/2.7)."""
SCORE = ["ChangeState__23UnkStageCamera_801CBB30FiP13SetCameraAreaPQ29PJSCamera21ExternalCameraRequest"]
A = "                    if (m_request->GetMode()->GetModeId() == m_handle.m_mode->GetModeId()) {"
B = "                if (mode->GetModeId() == m_handle.m_mode->GetModeId()) {\n                    if (m_handle.IsRequested()) {"
BT = "\n                    if (m_handle.IsRequested()) {"
F = "void UnkStageCamera_801CBB30::ChangeState(int state"
HELP = """typedef SonicteamUSA::System::Camera::CameraMode CM_;
inline bool SameA_(const boost::shared_ptr<CM_> &a, const boost::shared_ptr<CM_> &b) { return a->GetModeId() == b->GetModeId(); }
inline bool SameB_(boost::shared_ptr<CM_> a, const boost::shared_ptr<CM_> &b) { return a->GetModeId() == b->GetModeId(); }
inline bool SameC_(CM_ *a, CM_ *b) { return a->GetModeId() == b->GetModeId(); }
inline bool SameD_(CM_ *a, const boost::shared_ptr<CM_> &b) { return a->GetModeId() == b->GetModeId(); }
inline bool SameE_(const boost::shared_ptr<CM_> &b, CM_ *a) { return a->GetModeId() == b->GetModeId(); }
inline bool SameF_(const boost::shared_ptr<CM_> &b, const boost::shared_ptr<CM_> &a) { return a->GetModeId() == b->GetModeId(); }
inline bool SameG_(const UnkCameraHandle_801CF664 &h, const boost::shared_ptr<CM_> &a) { return a->GetModeId() == h.m_mode->GetModeId(); }
inline bool SameH_(const UnkCameraHandle_801CF664 &h, CM_ *a) { return a->GetModeId() == h.m_mode->GetModeId(); }
"""
H = (F, HELP + F)
def v(a, b):
    return [H, (A, a), (B, b + BT)]
REPL = {
  "ptr_local": v("CM_ *p = m_request->GetMode().get();\n if (p->GetModeId() == m_handle.m_mode->GetModeId()) {", "CM_ *p = mode.get();\n if (p->GetModeId() == m_handle.m_mode->GetModeId()) {"),
  "sameA": v("if (SameA_(m_request->GetMode(), m_handle.m_mode)) {", "if (SameA_(mode, m_handle.m_mode)) {"),
  "sameB": v("if (SameB_(m_request->GetMode(), m_handle.m_mode)) {", "if (SameB_(mode, m_handle.m_mode)) {"),
  "sameC": v("if (SameC_(m_request->GetMode().get(), m_handle.m_mode.get())) {", "if (SameC_(mode.get(), m_handle.m_mode.get())) {"),
  "sameD": v("if (SameD_(m_request->GetMode().get(), m_handle.m_mode)) {", "if (SameD_(mode.get(), m_handle.m_mode)) {"),
  "sameE": v("if (SameE_(m_handle.m_mode, m_request->GetMode().get())) {", "if (SameE_(m_handle.m_mode, mode.get())) {"),
  "sameF": v("if (SameF_(m_handle.m_mode, m_request->GetMode())) {", "if (SameF_(m_handle.m_mode, mode)) {"),
  "sameG": v("if (SameG_(m_handle, m_request->GetMode())) {", "if (SameG_(m_handle, mode)) {"),
  "sameH": v("if (SameH_(m_handle, m_request->GetMode().get())) {", "if (SameH_(m_handle, mode.get())) {"),
}
