"""TTextureChange::Setup (wave 40, round 5): the frame's address (retail `add r4,r5,r23` then
`lwz r6,80(r4)`, GC/2.7 `addi r0,r23,80` then `lwzx`), with the loop already inlined from
LoadFrames."""
SCORE = ["Setup__14TTextureChangeFP30UnkTextureChangeParam_8004F22Cl"]
L = """    for (s32 j = 0; j < entry->m_anim->m_count; j++) {
        char name[0x40];
        sprintf(name, "%s.%d", entry->m_anim->m_name, entry->m_anim->m_frames[j].m_index);
        entry->m_textures[j] = fn_8048CFFC(dict, name);
    }
"""
def r(body):
    return [(L, body)]
REPL = {
  "u32_j": r(L.replace("s32 j = 0", "u32 j = 0")),
  "frames_local": r("""    for (s32 j = 0; j < entry->m_anim->m_count; j++) {
        char name[0x40];
        const UnkTextureFrame_8004F0A4 *frames = entry->m_anim->m_frames;
        sprintf(name, "%s.%d", entry->m_anim->m_name, frames[j].m_index);
        entry->m_textures[j] = fn_8048CFFC(dict, name);
    }
"""),
  "anim_then": r("""    for (s32 j = 0; j < entry->m_anim->m_count; j++) {
        char name[0x40];
        UnkTextureAnim_8004F0A4 *anim = entry->m_anim;
        const UnkTextureFrame_8004F0A4 *frame = anim->m_frames + j;
        sprintf(name, "%s.%d", anim->m_name, frame->m_index);
        entry->m_textures[j] = fn_8048CFFC(dict, name);
    }
"""),
  "while": r("""    s32 j = 0;
    while (j < entry->m_anim->m_count) {
        char name[0x40];
        sprintf(name, "%s.%d", entry->m_anim->m_name, entry->m_anim->m_frames[j].m_index);
        entry->m_textures[j] = fn_8048CFFC(dict, name);
        j++;
    }
"""),
  "tex_first": r("""    for (s32 j = 0; j < entry->m_anim->m_count; j++) {
        char name[0x40];
        RwTexture **texture = &entry->m_textures[j];
        sprintf(name, "%s.%d", entry->m_anim->m_name, entry->m_anim->m_frames[j].m_index);
        *texture = fn_8048CFFC(dict, name);
    }
"""),
  "name_outside": r("""    char name[0x40];
    for (s32 j = 0; j < entry->m_anim->m_count; j++) {
        sprintf(name, "%s.%d", entry->m_anim->m_name, entry->m_anim->m_frames[j].m_index);
        entry->m_textures[j] = fn_8048CFFC(dict, name);
    }
"""),
  "j_decl_first": r("""    s32 j;
    char name[0x40];
    for (j = 0; j < entry->m_anim->m_count; j++) {
        sprintf(name, "%s.%d", entry->m_anim->m_name, entry->m_anim->m_frames[j].m_index);
        entry->m_textures[j] = fn_8048CFFC(dict, name);
    }
"""),
}
