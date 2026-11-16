"""Round 3 for fn_8000F9E8 of Game/CameraMotion.cpp (wave 39): with the state as a typed local of a
void * parameter, declared between i and count, the registers are retail's; what is left is the
order of the first loads (retail: tracks, i = 0, count)."""
import itertools
SCORE = ["fn_8000F9E8"]
SIG = 'extern "C" void fn_8000F9E8(UnkCameraMotionState_8000F9E8 *state, UnkCameraMotion_8000F9E8 *motion, float frame) {\n'
OLD = SIG + """    UnkCameraTrack_8000F9E8 *track = motion->m_tracks;
    int count = motion->m_trackCount;
    state->m_set = 0;
    for (int i = 0; i < count; i++, track++) {
"""
NS = SIG.replace("UnkCameraMotionState_8000F9E8 *state,", "void *out,")
ST = "(UnkCameraMotionState_8000F9E8 *)out"
REPL = {
  "init_decls_for_i": [(OLD, NS + f"    UnkCameraTrack_8000F9E8 *track = motion->m_tracks;\n    int i;\n    UnkCameraMotionState_8000F9E8 *state = {ST};\n    int count = motion->m_trackCount;\n    state->m_set = 0;\n    for (i = 0; i < count; i++, track++) {{\n")],
  "set_before_count": [(OLD, NS + f"    UnkCameraTrack_8000F9E8 *track = motion->m_tracks;\n    int i = 0;\n    UnkCameraMotionState_8000F9E8 *state = {ST};\n    state->m_set = 0;\n    int count = motion->m_trackCount;\n    for (; i < count; i++, track++) {{\n")],
  "for_i_count": [(OLD, NS + f"    UnkCameraTrack_8000F9E8 *track = motion->m_tracks;\n    int i;\n    UnkCameraMotionState_8000F9E8 *state = {ST};\n    int count;\n    state->m_set = 0;\n    for (i = 0, count = motion->m_trackCount; i < count; i++, track++) {{\n")],
  "for_all": [(OLD, NS + f"    UnkCameraTrack_8000F9E8 *track;\n    int i;\n    UnkCameraMotionState_8000F9E8 *state = {ST};\n    int count;\n    state->m_set = 0;\n    for (track = motion->m_tracks, i = 0, count = motion->m_trackCount; i < count; i++, track++) {{\n")],
  "for_all_set_after": [(OLD, NS + f"    UnkCameraTrack_8000F9E8 *track;\n    int i;\n    UnkCameraMotionState_8000F9E8 *state = {ST};\n    int count;\n    track = motion->m_tracks;\n    i = 0;\n    count = motion->m_trackCount;\n    state->m_set = 0;\n    for (; i < count; i++, track++) {{\n")],
  "while_form": [(OLD, NS + f"    UnkCameraTrack_8000F9E8 *track = motion->m_tracks;\n    int i = 0;\n    UnkCameraMotionState_8000F9E8 *state = {ST};\n    int count = motion->m_trackCount;\n    state->m_set = 0;\n    for (; i < count; track++, i++) {{\n")],
  "state_first_stmt": [(OLD, NS + f"    UnkCameraTrack_8000F9E8 *track;\n    int i;\n    UnkCameraMotionState_8000F9E8 *state;\n    int count;\n    state = {ST};\n    track = motion->m_tracks;\n    i = 0;\n    count = motion->m_trackCount;\n    state->m_set = 0;\n    for (; i < count; i++, track++) {{\n")],
  "set_first": [(OLD, NS + f"    UnkCameraTrack_8000F9E8 *track;\n    int i;\n    UnkCameraMotionState_8000F9E8 *state;\n    int count;\n    state = {ST};\n    state->m_set = 0;\n    track = motion->m_tracks;\n    i = 0;\n    count = motion->m_trackCount;\n    for (; i < count; i++, track++) {{\n")],
  "set_via_out": [(OLD, NS + f"    UnkCameraTrack_8000F9E8 *track = motion->m_tracks;\n    int i = 0;\n    UnkCameraMotionState_8000F9E8 *state = {ST};\n    int count = motion->m_trackCount;\n    (({ST[:-3]})out)->m_set = 0;\n    for (; i < count; i++, track++) {{\n")],
}
