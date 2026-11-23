"""Wave 41, RenderWare/RpUVAnim: fn_80453FB0 (material stream read), its two inlined helpers."""
SCORE = ["fn_80453FB0"]
LOOP_OLD = """    slot = slots;
    for (i = 0; i < 8; i++) {"""
LOOP_NEW = """    for (i = 0, slot = slots; i < 8; i++, slot++) {"""
INC_OLD = """            }
        }
        slot++;
    }"""
INC_NEW = """            }
        }
    }"""
H_OLD = """    if (dict != 0) {
        anim = fn_804790D4(dict, (u32 *)name);
        if (anim != NULL) {
            fn_80454618(anim);
        } else {
            anim = UVAnimCreateDefault((u32 *)name);
        }
    } else {
        anim = UVAnimCreateDefault((u32 *)name);
    }
    *animOut = anim;
    return anim != NULL ? stream : NULL;"""
H_OUT = """    if (dict != 0) {
        *animOut = fn_804790D4(dict, (u32 *)name);
        if (*animOut != NULL) {
            fn_80454618(*animOut);
        } else {
            *animOut = UVAnimCreateDefault((u32 *)name);
        }
    } else {
        *animOut = UVAnimCreateDefault((u32 *)name);
    }
    return *animOut != NULL ? stream : NULL;"""
H_IF = """    if (dict != 0) {
        anim = fn_804790D4(dict, (u32 *)name);
        if (anim != NULL) {
            fn_80454618(anim);
        } else {
            anim = UVAnimCreateDefault((u32 *)name);
        }
    } else {
        anim = UVAnimCreateDefault((u32 *)name);
    }
    *animOut = anim;
    if (anim == NULL) {
        return NULL;
    }
    return stream;"""
H_NEST = """    anim = NULL;
    if (dict != 0) {
        anim = fn_804790D4(dict, (u32 *)name);
        if (anim != NULL) {
            fn_80454618(anim);
        }
    }
    if (anim == NULL) {
        anim = UVAnimCreateDefault((u32 *)name);
    }
    *animOut = anim;
    return anim != NULL ? stream : NULL;"""
REPL = {
    "base": [],
    "loop": [(LOOP_OLD, LOOP_NEW), (INC_OLD, INC_NEW)],
    "out": [(H_OLD, H_OUT)],
    "loop_out": [(LOOP_OLD, LOOP_NEW), (INC_OLD, INC_NEW), (H_OLD, H_OUT)],
    "loop_if": [(LOOP_OLD, LOOP_NEW), (INC_OLD, INC_NEW), (H_OLD, H_IF)],
    "loop_nest": [(LOOP_OLD, LOOP_NEW), (INC_OLD, INC_NEW), (H_OLD, H_NEST)],
}
H_OUT2 = H_OUT.replace("return *animOut != NULL ? stream : NULL;", "return *animOut == NULL ? NULL : stream;")
H_OUT3 = H_OUT.replace("return *animOut != NULL ? stream : NULL;", "if (*animOut == NULL) {\n        return NULL;\n    }\n    return stream;")
H_OUT4 = H_OUT.replace("return *animOut != NULL ? stream : NULL;", "if (*animOut != NULL) {\n        return stream;\n    }\n    return NULL;")
REPL.update({
    "loop_out2": [(LOOP_OLD, LOOP_NEW), (INC_OLD, INC_NEW), (H_OLD, H_OUT2)],
    "loop_out3": [(LOOP_OLD, LOOP_NEW), (INC_OLD, INC_NEW), (H_OLD, H_OUT3)],
    "loop_out4": [(LOOP_OLD, LOOP_NEW), (INC_OLD, INC_NEW), (H_OLD, H_OUT4)],
})
