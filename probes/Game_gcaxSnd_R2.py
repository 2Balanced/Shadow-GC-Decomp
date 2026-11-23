"""Wave 41, Game/gcaxSnd.c round 2: fn_8043E9C4 (retail keeps memory in r30 and the interrupt
level in r29, and branches from the `memory != 0` arm to the shared null check)."""
SCORE = ["fn_8043E9C4"]
NL = chr(10)
HEAD = """void fn_8043E9C4(u32 memory, u32 size) {
    u32 interrupt = OSDisableInterrupts();
    u32 heap;
    if (memory != 0) {
        lbl_805F1408 = 1;
    } else {
        memory = (u32)OSAllocFromHeap(__OSCurrHeap, size);
    }
    if (memory == 0) {
"""
def head(decl, cond):
    return "void fn_8043E9C4(u32 memory, u32 size) {" + NL + decl + cond + "    if (memory == 0) {" + NL
IFELSE = """    if (memory != 0) {
        lbl_805F1408 = 1;
    } else {
        memory = (u32)OSAllocFromHeap(__OSCurrHeap, size);
    }
"""
ELSEIF = """    if (memory == 0) {
        memory = (u32)OSAllocFromHeap(__OSCurrHeap, size);
    } else {
        lbl_805F1408 = 1;
    }
"""
TERN = """    if (memory != 0) {
        lbl_805F1408 = 1;
    }
    if (memory == 0) {
        memory = (u32)OSAllocFromHeap(__OSCurrHeap, size);
    }
"""
D1 = "    u32 interrupt = OSDisableInterrupts();" + NL + "    u32 heap;" + NL
D2 = "    u32 heap;" + NL + "    u32 interrupt = OSDisableInterrupts();" + NL
D3 = "    u32 heap;" + NL + "    u32 interrupt;" + NL + "    interrupt = OSDisableInterrupts();" + NL
REPL = {
    "base": [],
    "elseif": [(HEAD, head(D1, ELSEIF))],
    "d2": [(HEAD, head(D2, IFELSE))],
    "d2_elseif": [(HEAD, head(D2, ELSEIF))],
    "d3": [(HEAD, head(D3, IFELSE))],
    "tern": [(HEAD, head(D1, TERN))],
    "d2_tern": [(HEAD, head(D2, TERN))],
}
