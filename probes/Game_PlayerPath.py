"""Source shapes for Player::PathImpl::MakeSearch (wave 39): retail loads the player's reach (f1)
before the 40 it adds (f0)."""
SCORE = ["MakeSearch__Q26Player8PathImplFRCQ25boost33shared_ptr<Q26Player10PlayerBase>"]
OLD = "    return UnkPathSearch_800ABE4C(chara->m_position, fn_800796DC(chara), 40.0f + chara->field_0x274);\n"
def v(pre, expr): return [(OLD, pre + "    return UnkPathSearch_800ABE4C(chara->m_position, fn_800796DC(chara), " + expr + ");\n")]
REPL = {
  "member_first": v("", "chara->field_0x274 + 40.0f"),
  "margin_local": v("    float margin = 40.0f;\n", "chara->field_0x274 + margin"),
  "margin_local_rev": v("    float margin = 40.0f;\n", "margin + chara->field_0x274"),
  "reach_local": v("    float reach = chara->field_0x274;\n", "reach + 40.0f"),
  "radius_local": v("    float radius = chara->field_0x274 + 40.0f;\n", "radius"),
  "radius_local2": v("    float radius = 40.0f + chara->field_0x274;\n", "radius"),
}
