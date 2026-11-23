"""_rpMeshRead (fn_8045C770) of Game/RwCpp22.cpp: retail runs the index copy loop on the chunk
count itself; ours copies it to r3 first (wave 40, agent r3)."""
L = """                    remaining -= count;
                    while (count != 0) {
                        *indices++ = *source++;
                        count--;
                    }
"""
REPL = {
    "base": [],
    "postdec": [(L, """                    remaining -= count;
                    while (count--) {
                        *indices++ = *source++;
                    }
""")],
    "postdec_ne": [(L, """                    remaining -= count;
                    while (count-- != 0) {
                        *indices++ = *source++;
                    }
""")],
    "for_gt": [(L, """                    remaining -= count;
                    for (; count > 0; count--) {
                        *indices++ = *source++;
                    }
""")],
    "for_ne": [(L, """                    remaining -= count;
                    for (; count != 0; count--) {
                        *indices++ = *source++;
                    }
""")],
    "sub_after": [(L, """                    while (count != 0) {
                        *indices++ = *source++;
                        count--;
                    }
""")],
    "dowhile": [(L, """                    remaining -= count;
                    if (count != 0) {
                        do {
                            *indices++ = *source++;
                        } while (--count);
                    }
""")],
}
SCORE = ["fn_8045C770"]
LOOPHEAD = """                while (remaining != 0) {
                    source = buffer;
                    count = 256;
"""
REPL.update({
    "count_s32": [("    u32 count;\n    u32 numMeshes;", "    s32 count;\n    u32 numMeshes;")],
    "count_inner": [("    u32 count;\n    u32 numMeshes;", "    u32 numMeshes;"),
                    (LOOPHEAD, "                while (remaining != 0) {\n                    u32 count;\n                    source = buffer;\n                    count = 256;\n")],
    "count_inner_ne": [("    u32 count;\n    u32 numMeshes;", "    u32 numMeshes;"),
                    (LOOPHEAD, "                while (remaining != 0) {\n                    u32 count;\n                    source = buffer;\n                    count = 256;\n"),
                    (L, "                    remaining -= count;\n                    while (count-- != 0) {\n                        *indices++ = *source++;\n                    }\n")],
    "src_inner": [("    u32 *source;\n    u32 size;", "    u32 size;"),
                  (LOOPHEAD, "                while (remaining != 0) {\n                    u32 *source = buffer;\n                    count = 256;\n")],
    "i_loop": [(L, """                    remaining -= count;
                    {
                        u32 i;
                        for (i = 0; i < count; i++) {
                            *indices++ = *source++;
                        }
                    }
""")],
})
REPL.update({
    "j_dec": [(L, """                    remaining -= count;
                    {
                        u32 j = count;
                        while (j--) {
                            *indices++ = *source++;
                        }
                    }
""")],
    "idx_src": [(L, """                    remaining -= count;
                    while (count != 0) {
                        *indices = *source;
                        indices++;
                        source++;
                        count--;
                    }
""")],
    "rem_first_cond": [(L, """                    remaining -= count;
                    while (count > 0) {
                        *indices++ = (u16)*source++;
                        count--;
                    }
""")],
})
