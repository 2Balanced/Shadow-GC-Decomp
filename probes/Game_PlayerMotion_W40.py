"""fn_800AA0D4 (wave 40): the event's first word read in a loop of one pass (3 rows each), then
the words and the kind as two named locals in declaration order."""
SCORE = ["fn_800AA0D4"]
OLD = "            switch (static_cast<s32 *>(data)[0]) {\n"
W = "for (int n = 0; n < 1; n++) {\n"
E = "                fn_800A29C0(Fields(motion->m_player.get())->m_effect);\n                break;\n            }\n"
REPL = {
  "for_kind": [(OLD, "            s32 kind;\n            " + W + "                kind = static_cast<s32 *>(data)[0];\n            }\n            switch (kind) {\n")],
  "for_words": [(OLD, "            s32 *words;\n            " + W + "                words = static_cast<s32 *>(data);\n            }\n            switch (words[0]) {\n")],
  "two": [(OLD, "            {\n            s32 *words = static_cast<s32 *>(data);\n            s32 kind = words[0];\n            switch (kind) {\n"), (E, E + "            }\n")],
  "two_char": [(OLD, "            {\n            s32 *words = static_cast<s32 *>(data);\n            s32 kind;\n            switch (kind = words[0]) {\n"), (E, E + "            }\n")],
  "two_top": [("    u16 event;\n    void *data;\n", "    u16 event;\n    void *data;\n    s32 *words;\n    s32 kind;\n"), (OLD, "            words = static_cast<s32 *>(data);\n            kind = words[0];\n            switch (kind) {\n")],
  "two_top_rev": [("    u16 event;\n    void *data;\n", "    u16 event;\n    void *data;\n    s32 kind;\n    s32 *words;\n"), (OLD, "            words = static_cast<s32 *>(data);\n            kind = words[0];\n            switch (kind) {\n")],
  "two_first": [("    _stMtnPlayWork *work = motion->m_work;\n", "    s32 *words;\n    s32 kind;\n    _stMtnPlayWork *work = motion->m_work;\n"), (OLD, "            words = static_cast<s32 *>(data);\n            kind = words[0];\n            switch (kind) {\n")],
  "kind_assign": [(OLD, "            {\n            s32 *words = static_cast<s32 *>(data);\n            s32 kind;\n            kind = *words;\n            switch (kind) {\n"), (E, E + "            }\n")],
}
