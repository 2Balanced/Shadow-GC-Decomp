"""Source shapes for fn_800AA0D4 of Game/PlayerMotion.cpp (wave 39): retail switches on the event's
first word in r4; GC/2.7 had it in r0."""
SCORE = ["fn_800AA0D4"]
OLD = "            switch (static_cast<s32 *>(data)[0]) {\n"
F = "void fn_800AA0D4(Player::PlayerMotion *motion) {"
HELP = "inline s32 FirstWord_(void *d) { return static_cast<s32 *>(d)[0]; }\nstruct UnkEventWords_ { s32 a; s32 b; };\n"
REPL = {
  "local": [(OLD, "            s32 kind = static_cast<s32 *>(data)[0];\n            switch (kind) {\n")],
  "inline": [(F, HELP + F), (OLD, "            switch (FirstWord_(data)) {\n")],
  "deref": [(OLD, "            switch (*static_cast<s32 *>(data)) {\n")],
  "struct": [(F, HELP + F), (OLD, "            switch (static_cast<UnkEventWords_ *>(data)->a) {\n")],
  "int_cast": [(OLD, "            switch (static_cast<int *>(data)[0]) {\n")],
  "u32": [(OLD, "            switch (static_cast<u32 *>(data)[0]) {\n")],
  "local_int": [(OLD, "            int kind = static_cast<s32 *>(data)[0];\n            switch (kind) {\n")],
  "args_value": [(OLD, "            switch (*args.m_value) {\n")],
}
