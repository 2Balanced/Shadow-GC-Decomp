"""fn_800AA0D4 (wave 40, round 2): fn_800A29C0 (Game/PlayerEffect.cpp) takes the landing's kind as
its second parameter; retail passes the event's first word, which is why the switch keeps it in r4."""
SCORE = ["fn_800AA0D4"]
DECL = "void fn_800A29C0(void *effect);\n"
CALL = "                fn_800A29C0(Fields(motion->m_player.get())->m_effect);\n"
SW = "            switch (static_cast<s32 *>(data)[0]) {\n"
REPL = {
  "kind_arg": [(DECL, "void fn_800A29C0(void *effect, int kind);\n"),
               (CALL, "                fn_800A29C0(Fields(motion->m_player.get())->m_effect, static_cast<s32 *>(data)[0]);\n")],
  "kind_local": [(DECL, "void fn_800A29C0(void *effect, int kind);\n"),
                 (SW, "            s32 kind = static_cast<s32 *>(data)[0];\n            switch (kind) {\n"),
                 (CALL, "                fn_800A29C0(Fields(motion->m_player.get())->m_effect, kind);\n")],
}
