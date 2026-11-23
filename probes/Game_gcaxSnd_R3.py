"""Wave 41, Game/gcaxSnd.c round 3: fn_8043E9C4 with the null check or the memory choice in inline
functions."""
SCORE = ["fn_8043E9C4"]
NL = chr(10)
FHEAD = "/* Takes the library's memory (allocated from the current heap when `memory` is NULL) and sets" + NL
CHECK = """    if (memory == 0) {
        OSReport("AX SDLIB: Panic: cannot allocate memory (%d)\\n", size);
        while (1) {
        }
    }
"""
IFELSE = """    if (memory != 0) {
        lbl_805F1408 = 1;
    } else {
        memory = (u32)OSAllocFromHeap(__OSCurrHeap, size);
    }
"""
INL_CHECK = """static inline void CheckMemory(u32 memory, u32 size) {
    if (memory == 0) {
        OSReport("AX SDLIB: Panic: cannot allocate memory (%d)\\n", size);
        while (1) {
        }
    }
}

"""
INL_TAKE = """static inline u32 TakeMemory(u32 memory, u32 size) {
    if (memory != 0) {
        lbl_805F1408 = 1;
    } else {
        memory = (u32)OSAllocFromHeap(__OSCurrHeap, size);
    }
    return memory;
}

"""
INL_BOTH = """static inline u32 TakeMemory(u32 memory, u32 size) {
    if (memory != 0) {
        lbl_805F1408 = 1;
    } else {
        memory = (u32)OSAllocFromHeap(__OSCurrHeap, size);
    }
    if (memory == 0) {
        OSReport("AX SDLIB: Panic: cannot allocate memory (%d)\\n", size);
        while (1) {
        }
    }
    return memory;
}

"""
REPL = {
    "base": [],
    "inl_check": [(CHECK, "    CheckMemory(memory, size);" + NL), (FHEAD, INL_CHECK + FHEAD)],
    "inl_take": [(IFELSE, "    memory = TakeMemory(memory, size);" + NL), (FHEAD, INL_TAKE + FHEAD)],
    "inl_both": [(CHECK, ""), (IFELSE, "    memory = TakeMemory(memory, size);" + NL), (FHEAD, INL_BOTH + FHEAD)],
}

INL_RET = """static inline u32 TakeMemory(u32 memory, u32 size) {
    if (memory != 0) {
        lbl_805F1408 = 1;
        return memory;
    }
    return (u32)OSAllocFromHeap(__OSCurrHeap, size);
}

"""
INL_SWAP = """static inline u32 TakeMemory(u32 memory, u32 size) {
    if (memory == 0) {
        memory = (u32)OSAllocFromHeap(__OSCurrHeap, size);
    } else {
        lbl_805F1408 = 1;
    }
    return memory;
}

"""
INL_TERN = """static inline u32 TakeMemory(u32 memory, u32 size) {
    return memory != 0 ? (lbl_805F1408 = 1, memory) : (u32)OSAllocFromHeap(__OSCurrHeap, size);
}

"""
INL_FLAG = """static inline void SetUserMemory(void) {
    lbl_805F1408 = 1;
}

"""
REPL.update({
    "inl_ret": [(IFELSE, "    memory = TakeMemory(memory, size);" + NL), (FHEAD, INL_RET + FHEAD)],
    "inl_swap": [(IFELSE, "    memory = TakeMemory(memory, size);" + NL), (FHEAD, INL_SWAP + FHEAD)],
    "inl_tern": [(IFELSE, "    memory = TakeMemory(memory, size);" + NL), (FHEAD, INL_TERN + FHEAD)],
    "inl_flag": [(IFELSE, IFELSE.replace("        lbl_805F1408 = 1;", "        SetUserMemory();")), (FHEAD, INL_FLAG + FHEAD)],
})
