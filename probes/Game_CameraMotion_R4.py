"""Round 4 for fn_8000F9E8 of Game/CameraMotion.cpp (wave 39): the order of the first four
instructions (retail: tracks, i = 0, count, the hoisted 0x80000)."""
SCORE = ["fn_8000F9E8"]
SIG = 'extern "C" void fn_8000F9E8(UnkCameraMotionState_8000F9E8 *state, UnkCameraMotion_8000F9E8 *motion, float frame) {\n'
OLD = SIG + """    UnkCameraTrack_8000F9E8 *track = motion->m_tracks;
    int count = motion->m_trackCount;
    state->m_set = 0;
    for (int i = 0; i < count; i++, track++) {
"""
NS = SIG.replace("UnkCameraMotionState_8000F9E8 *state,", "void *out,")
ST = "    UnkCameraMotionState_8000F9E8 *state = (UnkCameraMotionState_8000F9E8 *)out;\n"
T = "    UnkCameraTrack_8000F9E8 *track = motion->m_tracks;\n"
F = "    for (; i < count; i++, track++) {\n"
def v(body): return [(OLD, NS + body)]
REPL = {
  "base": v(T + "    int i = 0;\n" + ST + "    int count = motion->m_trackCount;\n    state->m_set = 0;\n" + F),
  "i_long": v(T + "    s32 i = 0;\n" + ST + "    s32 count = motion->m_trackCount;\n    state->m_set = 0;\n" + F),
  "i_unsigned_cmp": v(T + "    int i = 0;\n" + ST + "    int count = motion->m_trackCount;\n    state->m_set = 0;\n    if (i < count) do {\n"),
  "state_decl_only": v(T + "    int i = 0;\n    UnkCameraMotionState_8000F9E8 *state;\n    int count = motion->m_trackCount;\n    state = (UnkCameraMotionState_8000F9E8 *)out;\n    state->m_set = 0;\n" + F),
  "set_out_cast": v(T + "    int i = 0;\n" + ST + "    int count = motion->m_trackCount;\n    static_cast<UnkCameraMotionState_8000F9E8 *>(out)->m_set = 0;\n" + F),
  "set_zero_local": v(T + "    int i = 0;\n" + ST + "    int count = motion->m_trackCount;\n    state->m_set = i;\n" + F),
  "i_from_set": v(T + "    int i;\n" + ST + "    int count = motion->m_trackCount;\n    state->m_set = i = 0;\n" + F),
  "track_idx0": v("    UnkCameraTrack_8000F9E8 *track = &motion->m_tracks[0];\n    int i = 0;\n" + ST + "    int count = motion->m_trackCount;\n    state->m_set = 0;\n" + F),
  "motion_const": [(OLD, NS.replace("UnkCameraMotion_8000F9E8 *motion", "const UnkCameraMotion_8000F9E8 *motion") + T + "    int i = 0;\n" + ST + "    int count = motion->m_trackCount;\n    state->m_set = 0;\n" + F)],
  "while_true": v(T + "    int i = 0;\n" + ST + "    int count = motion->m_trackCount;\n    state->m_set = 0;\n    while (i < count) {\n").__add__([("            break;\n        }\n    }\n}\n\n/* Reverses the bytes", "            break;\n        }\n        i++;\n        track++;\n    }\n}\n\n/* Reverses the bytes")]),
}
del REPL["i_unsigned_cmp"]
