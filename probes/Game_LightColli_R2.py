"""Wave 40: the label function keeps the pooled .data address in r4 and the other addresses in r5
in retail (GC/2.7: the other way round)."""
SCORE = ["fn_801EEB2C"]
F = 'extern "C" const char *fn_801EEB2C(UnkLabelQuery_800CBC20 *query) {'
ST = """    static const char *effectNumber = "EffectNumber";
    static const char *doNotUse = "DoNotUse";
"""
IFS = """    if (s_shape < 0 || s_shape > 2) {
        s_shape = 0;
    }
"""
Q = "    Query *q = reinterpret_cast<Query *>(query);\n"
HELP = "inline const char *Pick_(const char **t, int i) { return t[i]; }\ninline const char **Tab_(const char **t) { return t; }\n"
H = (F, HELP + F)
import re
def pick(s): return s
REPL = {
  "st_first": [(IFS + ST, ST + IFS)],
  "q_first": [(IFS + ST + Q, Q + IFS + ST)],
  "q_mid": [(IFS + ST + Q, IFS + Q + ST)],
  "st_swap": [(ST, '    static const char *doNotUse = "DoNotUse";\n    static const char *effectNumber = "EffectNumber";\n')],
  "pick": [H, ("return types[q->value];", "return Pick_(types, q->value);"), ("return shapes[s_shape];", "return Pick_(shapes, s_shape);"), ("return lightFlags[q->value];", "return Pick_(lightFlags, q->value);"), ("return lights[q->value];", "return Pick_(lights, q->value);"), ("return effectFlags[q->value];", "return Pick_(effectFlags, q->value);"), ("return scaleX[s_shape];", "return Pick_(scaleX, s_shape);"), ("return scaleY[s_shape];", "return Pick_(scaleY, s_shape);"), ("return scaleZ[s_shape];", "return Pick_(scaleZ, s_shape);")],
  "tab": [H, ("return types[q->value];", "return Tab_(types)[q->value];"), ("return shapes[s_shape];", "return Tab_(shapes)[s_shape];"), ("return lightFlags[q->value];", "return Tab_(lightFlags)[q->value];"), ("return lights[q->value];", "return Tab_(lights)[q->value];"), ("return effectFlags[q->value];", "return Tab_(effectFlags)[q->value];"), ("return scaleX[s_shape];", "return Tab_(scaleX)[s_shape];"), ("return scaleY[s_shape];", "return Tab_(scaleY)[s_shape];"), ("return scaleZ[s_shape];", "return Tab_(scaleZ)[s_shape];")],
  "const_q": [(Q, "    const Query *q = reinterpret_cast<const Query *>(query);\n")],
  "ptr_st": [(ST, '    static const char *const effectNumber = "EffectNumber";\n    static const char *const doNotUse = "DoNotUse";\n')],
}

ARR = """    static const char *types[] = {"AREA", "SWITCH_ON", "SWITCH_OFF"};
    static const char *shapes[] = {"BOX", "SPHERE", "CYLINDER"};
    static const char *lights[] = {
        "LightNo:PLAYER_0", "LightNo:PLAYER_1", "LightNo:PLAYER_2", "LightNo:PLAYER_3",
        "LightNo:OBJECT_0", "LightNo:OBJECT_1", "LightNo:OBJECT_2", "LightNo:OBJECT_3",
        "LightNo:ENEMY_0",  "LightNo:ENEMY_1",  "LightNo:ENEMY_2",  "LightNo:ENEMY_3",
        "LightNo:KAGE",     "LightNo:OTHER_0",  "LightNo:OTHER_1",  "LightNo:OTHER_2",
    };
    static const char *scaleX[] = {"Scale X", "Radius", "Radius"};
    static const char *scaleY[] = {"Scale Y", "DoNotUse", "DoNotUse"};
    static const char *scaleZ[] = {"Scale Z", "DoNotUse", "Length"};
    static const char *lightFlags[] = {"Light OFF", "Light ON"};
    static const char *effectFlags[] = {"Effect OFF", "Effect ON"};
"""
INTS = """    static int s_shape;
    static int s_type;
    static int s_lightFlag;
    static int s_effectFlag;
"""
REPL = {
  "arr_file": [(ARR, ""), (F, ARR + F)],
  "arr_after_st": [(ARR, ""), (ST, ST + ARR)],
  "arr_after_ints": [(ARR, ""), (INTS, INTS + ARR)],
  "all_file": [(ARR, ""), (INTS, ""), (F, ARR + INTS + F)],
  "ints_file": [(INTS, ""), (F, INTS + F)],
}
