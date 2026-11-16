"""fn_8041575C of Game/MotionHalfKeyTrack.cpp (wave 39, second pass): retail keeps the four copies in
load-store order, which GC/2.7 does through a pointer to non-const."""
A = 'extern "C" void fn_8041575C(const float *params) {'
H = "void fn_8041575C(const float *params);"
REPL = {
 "nonconst": [(A, 'extern "C" void fn_8041575C(float *params) {')],
}
HDR = {"nonconst": {"Game/MotionHalfKeyTrack.h": [(H, "void fn_8041575C(float *params);")]}}
SCORE = ["fn_8041575C"]
