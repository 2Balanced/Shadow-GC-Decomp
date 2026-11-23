"""TTextureChange::Setup (wave 40, round 4): the inner loop inlined from a function (the lever of
EffectShimmerRW3's InitStrip: a loop inlined from a function starts its strength-reduced offset
with a copy of the zeroed counter, `mr`, where a loop written in place gets a second `li`)."""
SCORE = ["Setup__14TTextureChangeFP30UnkTextureChangeParam_8004F22Cl"]
LOOP = """                for (j = 0; j < entry->m_anim->m_count; j++) {
                    char name[0x40];
                    sprintf(name, "%s.%d", entry->m_anim->m_name, entry->m_anim->m_frames[j].m_index);
                    entry->m_textures[j] = fn_8048CFFC(dict, name);
                }
"""
ANCHOR = "bool TTextureChange::Setup(UnkTextureChangeParam_8004F22C *params, s32 count) {\n"
H1 = """/* The textures of the animation's frames. */
inline void LoadFrames(UnkTextureChangeEntry_8004F0A4 *entry, RwTexDictionary *dict) {
    for (s32 j = 0; j < entry->m_anim->m_count; j++) {
        char name[0x40];
        sprintf(name, "%s.%d", entry->m_anim->m_name, entry->m_anim->m_frames[j].m_index);
        entry->m_textures[j] = fn_8048CFFC(dict, name);
    }
}

"""
H2 = H1.replace("    for (s32 j = 0;", "    s32 j;\n    for (j = 0;")
H3 = """/* The textures of the animation's frames. */
inline void LoadFrames(UnkTextureChangeEntry_8004F0A4 *entry, RwTexDictionary *dict) {
    for (s32 j = 0; j < entry->m_anim->m_count; j++) {
        char name[0x40];
        UnkTextureAnim_8004F0A4 *anim = entry->m_anim;
        sprintf(name, "%s.%d", anim->m_name, anim->m_frames[j].m_index);
        entry->m_textures[j] = fn_8048CFFC(dict, name);
    }
}

"""
H4 = """/* The textures of the animation's frames. */
inline void LoadFrames(RwTexDictionary *dict, UnkTextureChangeEntry_8004F0A4 *entry) {
    for (s32 j = 0; j < entry->m_anim->m_count; j++) {
        char name[0x40];
        sprintf(name, "%s.%d", entry->m_anim->m_name, entry->m_anim->m_frames[j].m_index);
        entry->m_textures[j] = fn_8048CFFC(dict, name);
    }
}

"""
DECL_J = "    s32 i;\n    s32 j;\n"
REPL = {
  "inl": [(ANCHOR, H1 + ANCHOR), (LOOP, "                LoadFrames(entry, dict);\n"), (DECL_J, "    s32 i;\n")],
  "inl_keepj": [(ANCHOR, H1 + ANCHOR), (LOOP, "                LoadFrames(entry, dict);\n")],
  "inl_jtop": [(ANCHOR, H2 + ANCHOR), (LOOP, "                LoadFrames(entry, dict);\n"), (DECL_J, "    s32 i;\n")],
  "inl_anim": [(ANCHOR, H3 + ANCHOR), (LOOP, "                LoadFrames(entry, dict);\n"), (DECL_J, "    s32 i;\n")],
  "inl_rev": [(ANCHOR, H4 + ANCHOR), (LOOP, "                LoadFrames(dict, entry);\n"), (DECL_J, "    s32 i;\n")],
}
SPR = '        sprintf(name, "%s.%d", entry->m_anim->m_name, entry->m_anim->m_frames[j].m_index);\n'
def H(spr, pre=""):
    return [(ANCHOR, H1.replace(SPR, pre + spr) + ANCHOR), (LOOP, "                LoadFrames(entry, dict);\n"), (DECL_J, "    s32 i;\n")]
REPL["frame_ptr"] = H('        sprintf(name, "%s.%d", entry->m_anim->m_name, frame->m_index);\n', "        UnkTextureFrame_8004F0A4 *frame = &entry->m_anim->m_frames[j];\n")
REPL["frame_add"] = H('        sprintf(name, "%s.%d", entry->m_anim->m_name, (entry->m_anim->m_frames + j)->m_index);\n')
REPL["frame_ptr_after"] = H('        UnkTextureFrame_8004F0A4 *frame = &entry->m_anim->m_frames[j];\n        sprintf(name, "%s.%d", entry->m_anim->m_name, frame->m_index);\n')
REPL["frame_ref"] = H('        sprintf(name, "%s.%d", entry->m_anim->m_name, frame.m_index);\n', "        UnkTextureFrame_8004F0A4 &frame = entry->m_anim->m_frames[j];\n")
REPL["frame_star"] = H('        sprintf(name, "%s.%d", entry->m_anim->m_name, (*(entry->m_anim->m_frames + j)).m_index);\n')
REPL["frame_idx_inl"] = [(ANCHOR, "inline s32 FrameIndex(const UnkTextureAnim_8004F0A4 *anim, s32 j) { return anim->m_frames[j].m_index; }\n" + H1.replace(SPR, '        sprintf(name, "%s.%d", entry->m_anim->m_name, FrameIndex(entry->m_anim, j));\n') + ANCHOR), (LOOP, "                LoadFrames(entry, dict);\n"), (DECL_J, "    s32 i;\n")]
REPL["name_first"] = H('        const char *base = entry->m_anim->m_name;\n        sprintf(name, "%s.%d", base, entry->m_anim->m_frames[j].m_index);\n')
REPL["index_first"] = H('        s32 index = entry->m_anim->m_frames[j].m_index;\n        sprintf(name, "%s.%d", entry->m_anim->m_name, index);\n')
REPL["frames_u8"] = H('        sprintf(name, "%s.%d", entry->m_anim->m_name, ((UnkTextureFrame_8004F0A4 *)((u8 *)entry->m_anim + j * 8))->m_index + 0 * 0 + ((s32 *)0)[0] * 0);\n')
REPL["frames_bytes"] = H('        sprintf(name, "%s.%d", entry->m_anim->m_name, *(s32 *)((u8 *)entry->m_anim + j * sizeof(UnkTextureFrame_8004F0A4) + 0x50));\n')
REPL["frames_bytes2"] = H('        sprintf(name, "%s.%d", entry->m_anim->m_name, ((s32 *)((u8 *)entry->m_anim + j * sizeof(UnkTextureFrame_8004F0A4)))[0x50 / 4]);\n')
REPL["frames_ptr_j"] = H('        sprintf(name, "%s.%d", entry->m_anim->m_name, (&entry->m_anim->m_frames[0] + j)->m_index);\n')
ENT = """    /* 0x10 */ s32 m_current;
};
"""
MEM = """    /* 0x10 */ s32 m_current;

    void LoadFrames(RwTexDictionary *dict) {
        for (s32 j = 0; j < m_anim->m_count; j++) {
            char name[0x40];
            sprintf(name, "%s.%d", m_anim->m_name, m_anim->m_frames[j].m_index);
            m_textures[j] = fn_8048CFFC(dict, name);
        }
    }
};
"""
REPL["member"] = [(ENT, MEM), (LOOP, "                entry->LoadFrames(dict);\n"), (DECL_J, "    s32 i;\n")]
H5 = """inline void LoadFrames(UnkTextureChangeEntry_8004F0A4 *entry, RwTexDictionary *dict, RwTexture **textures) {
    for (s32 j = 0; j < entry->m_anim->m_count; j++) {
        char name[0x40];
        sprintf(name, "%s.%d", entry->m_anim->m_name, entry->m_anim->m_frames[j].m_index);
        textures[j] = fn_8048CFFC(dict, name);
    }
}

"""
REPL["tex_param"] = [(ANCHOR, H5 + ANCHOR), (LOOP, "                LoadFrames(entry, dict, entry->m_textures);\n"), (DECL_J, "    s32 i;\n")]
H6 = """inline void LoadFrames(UnkTextureAnim_8004F0A4 **anim, RwTexDictionary *dict, RwTexture ***textures) {
    for (s32 j = 0; j < (*anim)->m_count; j++) {
        char name[0x40];
        sprintf(name, "%s.%d", (*anim)->m_name, (*anim)->m_frames[j].m_index);
        (*textures)[j] = fn_8048CFFC(dict, name);
    }
}

"""
REPL["anim_pp"] = [(ANCHOR, H6 + ANCHOR), (LOOP, "                LoadFrames(&entry->m_anim, dict, &entry->m_textures);\n"), (DECL_J, "    s32 i;\n")]
