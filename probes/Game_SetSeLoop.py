"""Wave 40: SetSeLoop::Behave, the clamped volume in r29 below this (r31) and the contact list."""
SCORE = ["Behave__9SetSeLoopFf"]
OLD = "                        fn_801D53C4(fn_80053164(), m_params.seId, 0, this, 2,\n                                    volume = *fn_8016F21C(&volume, &s_volumeLow, &s_volumeHigh), 2);\n"
F = "void SetSeLoop::Behave(float dt) {"
H = """inline s8 ToVolume_(s32 v) { return v; }
inline s32 Pass_(s32 v) { return v; }
inline const s32 &Clamp_(const s32 &v, const s32 &lo, const s32 &hi) { return *fn_8016F21C(&v, &lo, &hi); }
inline void Play_(SetSeLoop *self, u16 id, s8 volume) { fn_801D53C4(fn_80053164(), id, 0, self, 2, volume, 2); }
""" + F
C = "volume = *fn_8016F21C(&volume, &s_volumeLow, &s_volumeHigh)"
def v(new):
    return [(F, H), (OLD, new)]
REPL = {
  "tovol_arg": v("                        fn_801D53C4(fn_80053164(), m_params.seId, 0, this, 2, ToVolume_(" + C + "), 2);\n"),
  "tovol_stmt": v("                        " + C + ";\n                        fn_801D53C4(fn_80053164(), m_params.seId, 0, this, 2, ToVolume_(volume), 2);\n"),
  "pass_arg": v("                        fn_801D53C4(fn_80053164(), m_params.seId, 0, this, 2, Pass_(" + C + "), 2);\n"),
  "pass_stmt": v("                        " + C + ";\n                        fn_801D53C4(fn_80053164(), m_params.seId, 0, this, 2, Pass_(volume), 2);\n"),
  "clamp_arg": v("                        fn_801D53C4(fn_80053164(), m_params.seId, 0, this, 2, volume = Clamp_(volume, s_volumeLow, s_volumeHigh), 2);\n"),
  "clamp_stmt": v("                        volume = Clamp_(volume, s_volumeLow, s_volumeHigh);\n                        fn_801D53C4(fn_80053164(), m_params.seId, 0, this, 2, ToVolume_(volume), 2);\n"),
  "cast_stmt": v("                        " + C + ";\n                        fn_801D53C4(fn_80053164(), m_params.seId, 0, this, 2, (s8)volume, 2);\n"),
  "plain_stmt": v("                        " + C + ";\n                        fn_801D53C4(fn_80053164(), m_params.seId, 0, this, 2, volume, 2);\n"),
  "play_tovol": v("                        " + C + ";\n                        Play_(this, m_params.seId, volume);\n"),
}
