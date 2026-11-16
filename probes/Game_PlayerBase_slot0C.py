"""Game/PlayerBase.cpp (wave 39): CharInfo's slot 0x0C as a real inline virtual taking the pad, and
UnkWords_8007A4D4 as a class template instance with a non-inline constructor; the question is the
order of the unit's last functions."""
import importlib.util, os
_p = os.path.join(os.path.dirname(__file__), "Game_PlayerBase_tmpl.py")
_s = importlib.util.spec_from_file_location("t", _p); _m = importlib.util.module_from_spec(_s); _s.loader.exec_module(_m)
SCORE = ["vf_0x0C__Q26Player10PlayerBaseFi"]
CI = ("Game/CharInfo.h", [("    virtual void vf_0x0C();", "    virtual void vf_0x0C(int pad) {\n        field_0x54 = pad;\n    }")])
PL = ("Game/Player.h", [("     * 0x80079918). */\n    virtual void vf_0x0C();", "     * 0x80079918). */\n    virtual void vf_0x0C(int pad);")])
CPP = [("""extern "C" void vf_0x0C__8CharInfoFv(CharInfo *info, int pad) {
    info->field_0x54 = pad;
}
""", ""),
 ("""extern "C" void vf_0x0C__Q26Player10PlayerBaseFv(PlayerBase *player, int pad) {
    player->field_0x54 = pad;
    if (player->m_charaSlots) {
        fn_800AAD44(player->m_charaSlots, pad);
    }
}""", """void PlayerBase::vf_0x0C(int pad) {
    field_0x54 = pad;
    if (m_charaSlots) {
        fn_800AAD44(m_charaSlots, pad);
    }
}""")]
HDR = {
  "slot_only": dict([CI, PL]),
  "slot_tmpl": dict([CI, PL, ("Game/UnkWords_8007A4D4.h", _m.HDR["tmpl_noinline"]["Game/UnkWords_8007A4D4.h"])]),
}
REPL = {"slot_only": CPP, "slot_tmpl": CPP}
