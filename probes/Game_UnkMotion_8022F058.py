"""Wave 40 shapes for ProcessEvents (Game/UnkMotion_8022F058.cpp): retail keeps this in r30, ahead
of the event's data (r29) and of work (r28); GC/2.7 gives this r28."""
PE = "ProcessEvents__Q218UnkMotion_8022F0584ImplFP14_stMtnPlayWork"
SYMBOLS = {PE: "void UnkMotion_8022F058::Impl::ProcessEvents(_stMtnPlayWork *work) {"}
SW = """        switch (event) {
        case 0x5003: {
            s32 *value = data;
            int index = value[0];
            if (s_sounds[index].m_sound >= 0) {
                fn_801D54F4(fn_80053164(), s_sounds[index].m_sound, SELF->m_owner, 0, 1);
            }
            if (s_sounds[value[0]].m_rumble != -1 && !SELF->m_owner->GetRider().expired()) {
                fn_8034DF48(s_sounds[value[0]].m_rumble, SELF->m_owner->GetRider().lock()->field_0x54);
            }
            break;
        }
        case 0xD004: {
            int index = data[0];
            if (s_sounds[index].m_sound >= 0) {
                fn_801D53C4(fn_80053164(), s_sounds[index].m_sound, 0, SELF, 2, 0, 5);
            }
            break;
        }
        }
        if (SELF->m_callback.IsSet()) {
            Vehicle::MotionEventArgs args;
            args.m_event = event;
            args.m_value = data;
            SELF->m_callback.Call(args);
        }
"""
def body(decl, sw=SW, self="this", pre="", loop="    while (fn_80415854(work, 0, &event, &data, 0) != 0) {\n        work = 0;\n"):
    return decl + pre + loop + sw.replace("SELF", self) + "    }\n"
D = "    u16 event;\n    s32 *data;\n"
BODIES = {PE: {
    "base": body(D),
    "data_first": body("    s32 *data;\n    u16 event;\n"),
    "self_local": body("    Impl *self = this;\n" + D, self="self"),
    "self_local_last": body(D + "    Impl *self = this;\n", self="self"),
    "self_const": body("    Impl *const self = this;\n" + D, self="self"),
    "self_void": body("    void *me = this;\n" + D, self="((Impl *)me)"),
    "owner_self": body(D, sw=SW.replace("SELF, 2, 0, 5", "(Impl *)this, 2, 0, 5")),
    "value_top": body(D + "    s32 *value;\n", sw=SW.replace("s32 *value = data;", "value = data;")),
    "value_top_first": body("    s32 *value;\n" + D, sw=SW.replace("s32 *value = data;", "value = data;")),
    "no_value": body(D, sw=SW.replace("            s32 *value = data;\n", "").replace("value[0]", "data[0]")),
    "for_loop": body(D, loop="    for (int more = fn_80415854(work, 0, &event, &data, 0); more != 0; more = fn_80415854(0, 0, &event, &data, 0)) {\n"),
    "do_work2": body(D + "    _stMtnPlayWork *w = work;\n", loop="    while (fn_80415854(w, 0, &event, &data, 0) != 0) {\n        w = 0;\n"),
}}
SC = "    Impl *const self = this;\n"
B = BODIES[PE]
B["sc_value_const"] = body(SC + D, self="self", sw=SW.replace("s32 *value = data;", "s32 *const value = data;"))
B["sc_last"] = body(D + SC, self="self")
B["sc_ref"] = body("    Impl &self = *this;\n" + D, self="(&self)")
B["sc_inloop"] = body(D, self="self", loop="    while (fn_80415854(work, 0, &event, &data, 0) != 0) {\n        Impl *const self = this;\n        work = 0;\n")
B["sc_value_top"] = body(SC + D + "    s32 *value;\n", self="self", sw=SW.replace("s32 *value = data;", "value = data;"))
B["sc_owner"] = body("    Vehicle::TVehicleBase *const owner = m_owner;\n" + D, sw=SW.replace("SELF->m_owner", "owner"))
B["sc_index_only"] = body(SC + D, self="self", sw=SW.replace("            s32 *value = data;\n            int index = value[0];", "            int index = data[0];").replace("value[0]", "data[0]"))
B["value_const"] = body(D, sw=SW.replace("s32 *value = data;", "s32 *const value = data;"))
B["value_cconst"] = body(D, sw=SW.replace("s32 *value = data;", "const s32 *const value = data;"))
B["value_ref"] = body(D, sw=SW.replace("s32 *value = data;", "s32 *&value = data;"))
B["sc_value_cconst"] = body(SC + D, self="self", sw=SW.replace("s32 *value = data;", "const s32 *value = data;"))
VC = SW.replace("s32 *value = data;", "s32 *const value = data;")
B["vc_sc_last"] = body(D + SC, self="self", sw=VC)
B["vc_self_plain"] = body("    Impl *self = this;\n" + D, self="self", sw=VC)
B["vc_sc_inloop"] = body(D, self="self", sw=VC, loop="    while (fn_80415854(work, 0, &event, &data, 0) != 0) {\n        Impl *const self = this;\n        work = 0;\n")
B["vc_sc_inloop2"] = body(D, self="self", sw=VC, loop="    while (fn_80415854(work, 0, &event, &data, 0) != 0) {\n        work = 0;\n        Impl *const self = this;\n")
B["vc_sc_cc"] = body("    const Impl *const self = this;\n" + D, self="const_cast<Impl *>(self)", sw=VC)
B["vc_sc_work"] = body(SC + "    _stMtnPlayWork *w = work;\n" + D, self="self", sw=VC, loop="    while (fn_80415854(w, 0, &event, &data, 0) != 0) {\n        w = 0;\n")
B["vc_sc_k"] = body(SC + D + "    const int kSound = 0xD004;\n", self="self", sw=VC.replace("case 0xD004", "case kSound"))
B["vc_sc_dowhile"] = body(SC + D, self="self", sw=VC, loop="    for (;;) {\n        if (fn_80415854(work, 0, &event, &data, 0) == 0) {\n            break;\n        }\n        work = 0;\n")
WL = "    while (fn_80415854(w, 0, &event, &data, 0) != 0) {\n        w = 0;\n"
W = "    _stMtnPlayWork *w = work;\n"
B["w_last"] = body(SC + D + W, self="self", sw=VC, loop=WL)
B["w_mid"] = body(SC + W + D, self="self", sw=VC, loop=WL)
B["w_first"] = body(W + SC + D, self="self", sw=VC, loop=WL)
B["w_last_noself"] = body(D + W, sw=VC, loop=WL)
B["w_last_plain"] = body(D + W, loop=WL)
B["w_const_ptr"] = body(SC + D + "    _stMtnPlayWork *const w = work;\n", self="self", sw=VC, loop="    for (int more = fn_80415854(w, 0, &event, &data, 0); more != 0; more = fn_80415854(0, 0, &event, &data, 0)) {\n")
B["w_scope"] = body(SC + D + "    {\n" + W, self="self", sw=VC, loop=WL) + "    }\n"
B["first_then_loop"] = body(SC + D + "    int more = fn_80415854(work, 0, &event, &data, 0);\n", self="self", sw=VC, loop="    while (more != 0) {\n").replace("            self->m_callback.Call(args);\n        }\n", "            self->m_callback.Call(args);\n        }\n        more = fn_80415854(0, 0, &event, &data, 0);\n")
