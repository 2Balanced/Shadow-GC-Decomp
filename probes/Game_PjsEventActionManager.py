"""Source shapes for fn_80173CD4 of Game/PjsEventActionManager.cpp (wave 39): retail reads the
script's state above the store of the saved registers, which a pointer to const allows."""
SCORE = ["fn_80173CD4"]
D = "bool fn_80173CD4(UnkEventScript_80170D8C *script, float time, float prevTime);"
S = 'extern "C" bool fn_80173CD4(UnkEventScript_80170D8C *script, float time, float prevTime) {'
H = "                script->ShowHint(cues[i].m_hint);"
REPL = {
  "const_param": [(D, D.replace("UnkEventScript_80170D8C *script", "const UnkEventScript_80170D8C *script")), (S, S.replace("UnkEventScript_80170D8C *script", "const UnkEventScript_80170D8C *script")), (H, "                const_cast<UnkEventScript_80170D8C *>(script)->ShowHint(cues[i].m_hint);")],
}
