"""Source shapes for TTextureChange::GetFrame (wave 39): retail keeps the animation in r5, its
length in r6, the frame counter in r7 and the frame table in r8 - declaration order, for GC/2.7."""
import itertools
SYMBOLS = {"GetFrame__14TTextureChangeFP30UnkTextureChangeEntry_8004F0A4": "s32 TTextureChange::GetFrame(UnkTextureChangeEntry_8004F0A4 *entry) {"}
D = {"a": "    UnkTextureAnim_8004F0A4 *anim;\n", "f": "    UnkTextureFrame_8004F0A4 *frames;\n", "l": "    s32 length;\n", "c": "    s32 count;\n", "i": "    s32 i;\n", "t": "    float time;\n", "o": "    s32 loops;\n"}
REST = """    anim = entry->m_anim;
    frames = anim->m_frames;
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
"""
B = {}
for order in ["alicfto", "alifcto", "alcifto", "atlicfo", "atlcifo", "atlifco", "alictfo", "alcfito", "atlocif", "atloicf", "alifcot", "ailfcto", "alciofs"]:
    order = order.replace("s", "")
    if len(order) < 7: order += "".join(x for x in "aflcito" if x not in order)
    B["d_" + order] = "".join(D[x] for x in order) + REST
BODIES = {"GetFrame__14TTextureChangeFP30UnkTextureChangeEntry_8004F0A4": B}
