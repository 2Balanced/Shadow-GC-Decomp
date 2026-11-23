"""Wave 40 (r2), RwCpp16 round 15: fn_80453D08's slot test operand order and its two separate
create/destroy tests."""
SCORE = ["fn_80453D08"]
C0 = "                if (*(u32 *)((u8 *)elements + 0x20 + 4 * element) == slot) {\n"
E0 = """        if (found != 0) {
            if (object[slot] == 0) {
                object[slot] = (u32)fn_8047F994();
                if (object[slot] == 0) {
                    return 0;
                }
            }
        } else if (object[slot] != 0) {
            fn_8047F958((void *)object[slot]);
            object[slot] = 0;
        }
"""
E1 = """        if (found && object[slot] == 0) {
            object[slot] = (u32)fn_8047F994();
            if (object[slot] == 0) {
                return 0;
            }
        }
        if (!found && object[slot] != 0) {
            fn_8047F958((void *)object[slot]);
            object[slot] = 0;
        }
"""
REPL = {
 "base": [(C0, C0)],
 "both": [(C0, "                if (slot == *(u32 *)((u8 *)elements + 0x20 + 4 * element)) {\n"), (E0, E1)],
}
REPL["cmp_only"] = [(C0, "                if (slot == *(u32 *)((u8 *)elements + 0x20 + 4 * element)) {\n")]
REPL["split_only"] = [(E0, E1)]
E2 = """        if (found) {
            if (object[slot] == 0) {
                object[slot] = (u32)fn_8047F994();
                if (object[slot] == 0) {
                    return 0;
                }
            }
        }
        if (!found) {
            if (object[slot] != 0) {
                fn_8047F958((void *)object[slot]);
                object[slot] = 0;
            }
        }
"""
REPL["split2"] = [(C0, "                if (slot == *(u32 *)((u8 *)elements + 0x20 + 4 * element)) {\n"), (E0, E2)]
E3 = """        if (found && object[slot] == 0) {
            object[slot] = (u32)fn_8047F994();
            if (object[slot] == 0) {
                return 0;
            }
        } else if (!found && object[slot] != 0) {
            fn_8047F958((void *)object[slot]);
            object[slot] = 0;
        }
"""
REPL["elseif"] = [(C0, "                if (slot == *(u32 *)((u8 *)elements + 0x20 + 4 * element)) {\n"), (E0, E3)]
