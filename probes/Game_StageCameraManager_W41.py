"""Wave 41: ChangeState's mode pointer (r26 in retail, r27 from GC/2.7) with copy propagation off and
the two mode ids through inline functions."""
SCORE = ["ChangeState__23UnkStageCamera_801CBB30FiP13SetCameraAreaPQ29PJSCamera21ExternalCameraRequest"]
F = "void UnkStageCamera_801CBB30::ChangeState(int state"
N = "SetCameraArea *UnkStageCamera_801CBB30::FindPriorityArea("
PRAG = [(F, "#pragma opt_propagation off\n" + F), (N, "#pragma opt_propagation reset\n" + N)]
AP = "                if (top->m_flags & 2) {\n"
A = AP + "                    if (m_request->GetMode()->GetModeId() == m_handle.m_mode->GetModeId()) {"
BM = "                boost::shared_ptr<CameraMode> mode = m_slot.GetMode();\n"
BP = "            } else {\n"
B = BP + BM + "                if (mode->GetModeId() == m_handle.m_mode->GetModeId()) {"
OPEN = "                                          ExternalCameraRequest *request) {\n"
HM = ("inline int HandleModeId(UnkCameraHandle_801CF664 &h) {\n"
      "    return h.m_mode->GetModeId();\n"
      "}\n\n")
MID = ("inline int ModeIdOf(const boost::shared_ptr<CameraMode> &m) {\n"
       "    return m->GetModeId();\n"
       "}\n\n")


def v(*extra, prag=True):
    return list(extra) + (PRAG if prag else [])


HMA = (A, AP + "                    if (m_request->GetMode()->GetModeId() == HandleModeId(m_handle)) {")
HMB = (B, BP + BM + "                if (mode->GetModeId() == HandleModeId(m_handle)) {")
REPL = {
    "prag": v(),
    "prag_self": v((OPEN, OPEN + "    UnkStageCamera_801CBB30 *self = this;\n")),
    "prag_hmode": v((F, HM + F), HMA, HMB),
    "noprag_hmode": v((F, HM + F), HMA, HMB, prag=False),
    "both_inl": v((F, HM + MID + F), (A, AP + "                    if (ModeIdOf(m_request->GetMode()) == HandleModeId(m_handle)) {"),
                  (B, BP + BM + "                if (ModeIdOf(mode) == HandleModeId(m_handle)) {"), prag=False),
    "left_inl": v((F, MID + F), (A, AP + "                    if (ModeIdOf(m_request->GetMode()) == m_handle.m_mode->GetModeId()) {"),
                  (B, BP + BM + "                if (ModeIdOf(mode) == m_handle.m_mode->GetModeId()) {"), prag=False),
    "left_inl_prag": v((F, MID + F), (A, AP + "                    if (ModeIdOf(m_request->GetMode()) == m_handle.m_mode->GetModeId()) {"),
                       (B, BP + BM + "                if (ModeIdOf(mode) == m_handle.m_mode->GetModeId()) {")),
}
