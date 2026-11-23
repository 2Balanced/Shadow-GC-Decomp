"""Wave 41: TTextureChange::Setup with copy propagation off (the wave-40 lever) and named copies of
the animation in the frame loop."""
SCORE = ["Setup__14TTextureChangeFP30UnkTextureChangeParam_8004F22Cl"]
F = "bool TTextureChange::Setup(UnkTextureChangeParam_8004F22C *params, s32 count) {"
END = "void TTextureChange::FreeTextures() {"
P = [(F, "#pragma opt_propagation off\n" + F), (END, "#pragma opt_propagation reset\n" + END)]
LOOP = """    for (s32 j = 0; j < entry->m_anim->m_count; j++) {
        char name[0x40];
        sprintf(name, "%s.%d", entry->m_anim->m_name, entry->m_anim->m_frames[j].m_index);
        entry->m_textures[j] = fn_8048CFFC(dict, name);
    }
"""
REPL = {
  "prag": P,
  "prag_anim": P + [(LOOP, """    for (s32 j = 0; j < entry->m_anim->m_count; j++) {
        char name[0x40];
        UnkTextureAnim_8004F0A4 *anim = entry->m_anim;
        sprintf(name, "%s.%d", anim->m_name, anim->m_frames[j].m_index);
        entry->m_textures[j] = fn_8048CFFC(dict, name);
    }
""")],
  "prag_frame": P + [(LOOP, """    for (s32 j = 0; j < entry->m_anim->m_count; j++) {
        char name[0x40];
        UnkTextureFrame_8004F0A4 *frame = &entry->m_anim->m_frames[j];
        sprintf(name, "%s.%d", entry->m_anim->m_name, frame->m_index);
        entry->m_textures[j] = fn_8048CFFC(dict, name);
    }
""")],
  "prag_index": P + [(LOOP, """    for (s32 j = 0; j < entry->m_anim->m_count; j++) {
        char name[0x40];
        s32 index = entry->m_anim->m_frames[j].m_index;
        sprintf(name, "%s.%d", entry->m_anim->m_name, index);
        entry->m_textures[j] = fn_8048CFFC(dict, name);
    }
""")],
}

def lp(body):
    return P + [(LOOP, "    for (s32 j = 0; j < entry->m_anim->m_count; j++) {\n        char name[0x40];\n" + body + "        entry->m_textures[j] = fn_8048CFFC(dict, name);\n    }\n")]
REPL.update({
  "f_anim_frame": lp("        UnkTextureAnim_8004F0A4 *anim = entry->m_anim;\n        UnkTextureFrame_8004F0A4 *frame = &anim->m_frames[j];\n        sprintf(name, \"%s.%d\", anim->m_name, frame->m_index);\n"),
  "f_frame_ptradd": lp("        UnkTextureFrame_8004F0A4 *frame = entry->m_anim->m_frames + j;\n        sprintf(name, \"%s.%d\", entry->m_anim->m_name, frame->m_index);\n"),
  "f_frames_j": lp("        UnkTextureFrame_8004F0A4 *frames = entry->m_anim->m_frames;\n        sprintf(name, \"%s.%d\", entry->m_anim->m_name, frames[j].m_index);\n"),
  "f_frame_ref": lp("        UnkTextureFrame_8004F0A4 &frame = entry->m_anim->m_frames[j];\n        sprintf(name, \"%s.%d\", entry->m_anim->m_name, frame.m_index);\n"),
  "f_frame_idx": lp("        UnkTextureFrame_8004F0A4 *frame = &entry->m_anim->m_frames[j];\n        s32 index = frame->m_index;\n        sprintf(name, \"%s.%d\", entry->m_anim->m_name, index);\n"),
  "f_bytes": lp("        u8 *base = (u8 *)entry->m_anim + j * 8;\n        sprintf(name, \"%s.%d\", entry->m_anim->m_name, *(s32 *)(base + 0x50));\n"),
  "f_bytes2": lp("        UnkTextureFrame_8004F0A4 *frame = (UnkTextureFrame_8004F0A4 *)((u8 *)entry->m_anim + j * 8);\n        sprintf(name, \"%s.%d\", entry->m_anim->m_name, frame[0x4C / 8].m_index);\n"),
})

REPL.update({
  "f_b3": lp("        u8 *frame = (u8 *)entry->m_anim->m_frames + j * sizeof(UnkTextureFrame_8004F0A4);\n        sprintf(name, \"%s.%d\", entry->m_anim->m_name, ((UnkTextureFrame_8004F0A4 *)frame)->m_index);\n"),
  "f_b4": lp("        UnkTextureAnim_8004F0A4 *anim = (UnkTextureAnim_8004F0A4 *)((UnkTextureFrame_8004F0A4 *)entry->m_anim + j);\n        sprintf(name, \"%s.%d\", entry->m_anim->m_name, anim->m_frames[0].m_index);\n"),
  "f_b5": lp("        u8 *base = (u8 *)entry->m_anim + j * sizeof(UnkTextureFrame_8004F0A4);\n        sprintf(name, \"%s.%d\", entry->m_anim->m_name, ((UnkTextureAnim_8004F0A4 *)base)->m_frames[0].m_index);\n"),
  "f_b6": lp("        char *base = (char *)entry->m_anim + j * 8;\n        sprintf(name, \"%s.%d\", entry->m_anim->m_name, *(s32 *)(base + 0x50));\n"),
})

REPL["np_b3"] = [(LOOP, "    for (s32 j = 0; j < entry->m_anim->m_count; j++) {\n        char name[0x40];\n        u8 *frame = (u8 *)entry->m_anim->m_frames + j * sizeof(UnkTextureFrame_8004F0A4);\n        sprintf(name, \"%s.%d\", entry->m_anim->m_name, ((UnkTextureFrame_8004F0A4 *)frame)->m_index);\n        entry->m_textures[j] = fn_8048CFFC(dict, name);\n    }\n")]
