"""Wave 41, RenderWare/RpCollision.c: fn_80440920 (retail keeps the tree size in one register
through its steps)."""
SCORE = ["fn_80440920"]
NL = chr(10)
A = "    size = RwCpp05NodeSize(value, count);" + NL
REPL = {
    "base": [],
    "direct": [(A, "    size = (u16)count != 0 ? ((u16)count << 4) + 0x37 : 0x28;" + NL + "    size += (u16)value << 1;" + NL + "    size += 1;" + NL)],
    "direct2": [(A, "    if ((u16)count != 0) {" + NL + "        size = ((u16)count << 4) + 0x37;" + NL + "    } else {" + NL + "        size = 0x28;" + NL + "    }" + NL + "    size += (u16)value << 1;" + NL + "    size++;" + NL)],
    "direct3": [(A, "    size = 0x28;" + NL + "    if ((u16)count != 0) {" + NL + "        size = ((u16)count << 4) + 0x37;" + NL + "    }" + NL + "    size = size + ((u16)value << 1) + 1;" + NL)],
    "direct4": [(A, "    size = 0x28;" + NL + "    if ((u16)count != 0) {" + NL + "        size = ((u16)count << 4) + 0x37;" + NL + "    }" + NL + "    size += ((u16)value << 1);" + NL + "    size += 1;" + NL)],
}
