"""Wave 40 (r7): fn_80493C84: how the video data's base register comes to the prologue."""

DEF = "static RwDlVideoData lbl_8056E348 = {"
FN = "void fn_80493C84(RwDlRenderMode *mode, RwInt32 format) {"
REPL = {
    "pool_on_def": [(DEF, "#pragma pool_data on\n" + DEF), ("    \"Invalid TV format\",\n};", "    \"Invalid TV format\",\n};\n#pragma pool_data off")],
    "pool_on_all": [(DEF, "#pragma pool_data on\n" + DEF)],
    "global_pool": [(DEF, "#pragma pool_data on\nRwDlVideoData lbl_8056E348 = {")],
    "local_ptr": [(FN, FN + "\n    RwDlVideoData *data = &lbl_8056E348;"),
                  ("lbl_8056E348.current.", "data->current."),
                  ("lbl_8056E348.invalid", "data->invalid")],
}
SCORE = ["fn_80493C84"]
