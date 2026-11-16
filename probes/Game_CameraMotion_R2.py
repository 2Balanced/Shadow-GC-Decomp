"""Round 2 for fn_8000F9E8 of Game/CameraMotion.cpp (wave 39).  GC/2.7 hands r30, r29, r28 to the
locals in declaration order and the next register to the state parameter; retail has the state
(r28) before the count (r27)."""
SCORE = ["fn_8000F9E8"]
SIG = 'extern "C" void fn_8000F9E8(UnkCameraMotionState_8000F9E8 *state, UnkCameraMotion_8000F9E8 *motion, float frame) {\n'
OLD = SIG + """    UnkCameraTrack_8000F9E8 *track = motion->m_tracks;
    int count = motion->m_trackCount;
    state->m_set = 0;
    for (int i = 0; i < count; i++, track++) {
"""
T = "    UnkCameraTrack_8000F9E8 *track = motion->m_tracks;\n"
REPL = {
  "alias_between": [(OLD, SIG.replace("*state,", "*out,") + T + "    int i = 0;\n    UnkCameraMotionState_8000F9E8 *state = out;\n    int count = motion->m_trackCount;\n    state->m_set = 0;\n    for (; i < count; i++, track++) {\n")],
  "void_param": [(OLD, SIG.replace("UnkCameraMotionState_8000F9E8 *state,", "void *out,") + T + "    int i = 0;\n    UnkCameraMotionState_8000F9E8 *state = (UnkCameraMotionState_8000F9E8 *)out;\n    int count = motion->m_trackCount;\n    state->m_set = 0;\n    for (; i < count; i++, track++) {\n")],
  "values_ptr": [(OLD, SIG.replace("*state,", "*out,") + T + "    int i = 0;\n    UnkCameraMotionState_8000F9E8 *state = (UnkCameraMotionState_8000F9E8 *)out->m_values;\n    int count = motion->m_trackCount;\n    state->m_set = 0;\n    for (; i < count; i++, track++) {\n")],
  "ref_param": [(OLD, SIG.replace("*state,", "&out,") + T + "    int i = 0;\n    UnkCameraMotionState_8000F9E8 *state = &out;\n    int count = motion->m_trackCount;\n    state->m_set = 0;\n    for (; i < count; i++, track++) {\n")],
  "count_in_for": [(OLD, SIG + T + "    state->m_set = 0;\n    for (int i = 0, count = motion->m_trackCount; i < count; i++, track++) {\n")],
  "count_const": [(OLD, SIG + T + "    int i = 0;\n    const int count = motion->m_trackCount;\n    state->m_set = 0;\n    for (; i < count; i++, track++) {\n")],
  "count_s16": [(OLD, SIG + T + "    int i = 0;\n    u32 count = motion->m_trackCount;\n    state->m_set = 0;\n    for (; i < (int)count; i++, track++) {\n")],
  "end_ptr": [(OLD, SIG + T + "    int i = 0;\n    state->m_set = 0;\n    for (; i < motion->m_trackCount; i++, track++) {\n")],
}
