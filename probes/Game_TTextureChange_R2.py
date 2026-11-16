"""TTextureChange::GetFrame (wave 39): the tidy form of the declaration order that matches."""
SYMBOLS = {"GetFrame__14TTextureChangeFP30UnkTextureChangeEntry_8004F0A4": "s32 TTextureChange::GetFrame(UnkTextureChangeEntry_8004F0A4 *entry) {"}
BODIES = {"GetFrame__14TTextureChangeFP30UnkTextureChangeEntry_8004F0A4": {"tidy": """    /* The frame table is declared after the counter: GC/2.7 hands out the registers in declaration
     * order, and retail keeps the animation in r5, its length in r6, the counter in r7 and the
     * table in r8 (wave 39; probes/Game_TTextureChange.py). */
    UnkTextureAnim_8004F0A4 *anim = entry->m_anim;
    float time;
    s32 length;
    s32 count;
    s32 loops;
    s32 i;
    UnkTextureFrame_8004F0A4 *frames = anim->m_frames;
    if (lbl_805E39A0 >= 0.0f) {
        time = lbl_805E39A0;
    } else {
        time = m_time;
    }
    length = anim->m_length;
    count = anim->m_count;
    loops = time / length;
    time -= length * loops;
    for (i = 0; i < count; i++) {
        if (time < frames[i].m_time) {
            break;
        }
    }
    return i - 1;
"""}}
