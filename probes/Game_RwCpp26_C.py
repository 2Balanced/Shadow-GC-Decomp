"""World light loader fn_804641CC of Game/RwCpp26.cpp (wave 40, agent r3): retail keeps the
colour below the argument copy on the stack."""
C = """                color.r = (u8)(255.0f * light->color.red);
                color.g = (u8)(255.0f * light->color.green);
                color.b = (u8)(255.0f * light->color.blue);
                color.a = 0;
                GXInitLightColor(lightObj, color);"""
H = """/* Loads the world's ambient and directional lights"""
HELP = """static GXColor RwCpp26GXColor(const RwRGBAReal *c) {
    GXColor color;
    color.r = (u8)(255.0f * c->red);
    color.g = (u8)(255.0f * c->green);
    color.b = (u8)(255.0f * c->blue);
    color.a = 0;
    return color;
}

/* Loads the world's ambient and directional lights"""
REPL = {
    "help": [(H, HELP), (C, "                GXInitLightColor(lightObj, RwCpp26GXColor(&light->color));"), ("                GXColor color;\n", "")],
    "top": [("                GXColor color;\n", ""), ("    s32 n;\n    RwLLLink *cur;", "    GXColor color;\n    s32 n;\n    RwLLLink *cur;")],
    "block": [("                GXColor color;\n", ""), (C, "                {\n                GXColor color;\n" + C + "\n                }")],
    "first": [("                GXColor color;\n", ""), ("                RwMatrix *ltm;\n", "                GXColor color;\n                RwMatrix *ltm;\n")],
}
SCORE = ["fn_804641CC"]
