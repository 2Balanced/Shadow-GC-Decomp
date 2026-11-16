"""MotionHand::GetNodeMatrix (wave 39): li r29,0 / mr r31,r29 (the counter copied into the offset)."""
SCORE = ["GetNodeMatrix__Q36Player6Shadow10MotionHandFi"]
O = """    MotionHandImpl *impl = m_impl.get();
    for (u32 i = 0; i < 5; i++) {
        if (impl->m_anims[i] != 0) {
            Mtx34f *matrix = fn_80415E70(impl->m_anims[i], node);
            if (matrix != 0) {
                return matrix;
            }
        }
    }
    return 0;
"""
def b(s):
    return [(O, s)]
REPL = {
 "ofs_var": b("""    MotionHandImpl *impl = m_impl.get();
    u32 i = 0;
    u32 offset = i;
    for (; i < 5; i++, offset += 4) {
        _stMtnPlayWork *anim = *(_stMtnPlayWork **)((u8 *)impl->m_anims + offset);
        if (anim != 0) {
            Mtx34f *matrix = fn_80415E70(anim, node);
            if (matrix != 0) {
                return matrix;
            }
        }
    }
    return 0;
"""),
 "do_while": b("""    MotionHandImpl *impl = m_impl.get();
    u32 i = 0;
    do {
        if (impl->m_anims[i] != 0) {
            Mtx34f *matrix = fn_80415E70(impl->m_anims[i], node);
            if (matrix != 0) {
                return matrix;
            }
        }
        i++;
    } while (i < 5);
    return 0;
"""),
 "two_idx": b("""    MotionHandImpl *impl = m_impl.get();
    u32 i;
    u32 j;
    for (i = j = 0; i < 5; i++, j++) {
        if (impl->m_anims[j] != 0) {
            Mtx34f *matrix = fn_80415E70(impl->m_anims[j], node);
            if (matrix != 0) {
                return matrix;
            }
        }
    }
    return 0;
"""),
 "two_idx2": b("""    MotionHandImpl *impl = m_impl.get();
    u32 i = 0;
    u32 j = i;
    for (; i < 5; i++, j++) {
        if (impl->m_anims[j] != 0) {
            Mtx34f *matrix = fn_80415E70(impl->m_anims[j], node);
            if (matrix != 0) {
                return matrix;
            }
        }
    }
    return 0;
"""),
 "int_i": b("""    MotionHandImpl *impl = m_impl.get();
    for (int i = 0; (u32)i < 5; i++) {
        if (impl->m_anims[i] != 0) {
            Mtx34f *matrix = fn_80415E70(impl->m_anims[i], node);
            if (matrix != 0) {
                return matrix;
            }
        }
    }
    return 0;
"""),
}
