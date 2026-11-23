"""Wave 40, round 3: the 0x5003 case (or the whole event) in an inline member, so that this and the
event's value are the inline function's parameters."""
_ns = {}
exec(compile(open("probes/Game_UnkMotion_8022F058.py", encoding="utf-8").read(), "r1", "exec"), _ns)
PE = _ns["PE"]; body = _ns["body"]; D = _ns["D"]; SW = _ns["SW"]; VC = _ns["VC"]
OLD_DECL = "    void ProcessEvents(_stMtnPlayWork *work);"
_src = open("src/Game/UnkMotion_8022F058.cpp", encoding="utf-8").read()
_a = _src.index("void UnkMotion_8022F058::Impl::ProcessEvents(_stMtnPlayWork *work) {")
OLD_HEAD = _src[_a:_src.index("void UnkMotion_8022F058::Impl::LoadParts(")]
SCORE = [PE]
C1 = """            int index = value[0];
            if (s_sounds[index].m_sound >= 0) {
                fn_801D54F4(fn_80053164(), s_sounds[index].m_sound, m_owner, 0, 1);
            }
            if (s_sounds[value[0]].m_rumble != -1 && !m_owner->GetRider().expired()) {
                fn_8034DF48(s_sounds[value[0]].m_rumble, m_owner->GetRider().lock()->field_0x54);
            }
"""
C2 = """            int index = data[0];
            if (s_sounds[index].m_sound >= 0) {
                fn_801D53C4(fn_80053164(), s_sounds[index].m_sound, 0, this, 2, 0, 5);
            }
"""
CB = """        if (m_callback.IsSet()) {
            Vehicle::MotionEventArgs args;
            args.m_event = event;
            args.m_value = data;
            m_callback.Call(args);
        }
"""
HEAD = "void UnkMotion_8022F058::Impl::ProcessEvents(_stMtnPlayWork *work) {\n" + D + "    while (fn_80415854(work, 0, &event, &data, 0) != 0) {\n        work = 0;\n"
def v(inl, loopbody):
    return [(OLD_HEAD, inl + HEAD + loopbody + "    }\n}\n\n")]
DECL1 = "inline void UnkMotion_8022F058::Impl::OnSound(s32 *value) {\n" + C1 + "}\n\n"
DECL2 = "inline void UnkMotion_8022F058::Impl::OnSound2(s32 *data) {\n" + C2 + "}\n\n"
SWI = "        switch (event) {\n        case 0x5003:\n            OnSound(data);\n            break;\n        case 0xD004: {\n" + C2 + "            break;\n        }\n        }\n"
SWI2 = "        switch (event) {\n        case 0x5003:\n            OnSound(data);\n            break;\n        case 0xD004:\n            OnSound2(data);\n            break;\n        }\n"
MEM = [(OLD_DECL, OLD_DECL + "\n    void OnSound(s32 *value);\n    void OnSound2(s32 *data);\n    void OnEvent(u16 event, s32 *data);\n    void OnEvent2(u16 &event, s32 *&data);")]
ALL = "inline void UnkMotion_8022F058::Impl::OnEvent(u16 event, s32 *data) {\n" + SW.replace("SELF->", "").replace("SELF", "this") + "}\n\n"
ALL2 = "inline void UnkMotion_8022F058::Impl::OnEvent2(u16 &event, s32 *&data) {\n" + SW.replace("SELF->", "").replace("SELF", "this") + "}\n\n"
REPL = {
    "inl_5003": MEM + v(DECL1, SWI + CB),
    "inl_both": MEM + v(DECL1 + DECL2, SWI2 + CB),
    "inl_event": MEM + v(ALL, "        OnEvent(event, data);\n"),
    "inl_event_ref": MEM + v(ALL2, "        OnEvent2(event, data);\n"),
}
