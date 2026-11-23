"""GX setup fn_804655D4 of Game/RwCpp27.cpp (wave 40, agent r3): argument order of
GXSetChanCtrl."""
D = """    s32 colorEnable;
    s32 alphaEnable;
    s32 colorMatSrc;
    s32 alphaMatSrc;
    s32 colorAmbSrc;
    s32 alphaAmbSrc;
"""
REPL = {
    "base": [],
    "bool_en": [(D, D.replace("s32 colorEnable;", "GXBool colorEnable;").replace("s32 alphaEnable;", "GXBool alphaEnable;"))],
    "u8_all": [(D, D.replace("s32 ", "u8 "))],
    "u32_all": [(D, D.replace("s32 ", "u32 "))],
    "src_enum": [(D, D.replace("s32 colorMatSrc;", "GXColorSrc colorMatSrc;").replace("s32 alphaMatSrc;", "GXColorSrc alphaMatSrc;").replace("s32 colorAmbSrc;", "GXColorSrc colorAmbSrc;").replace("s32 alphaAmbSrc;", "GXColorSrc alphaAmbSrc;"))],
    "bool_src": [(D, D.replace("s32 colorEnable;", "GXBool colorEnable;").replace("s32 alphaEnable;", "GXBool alphaEnable;").replace("s32 colorMatSrc;", "GXColorSrc colorMatSrc;").replace("s32 alphaMatSrc;", "GXColorSrc alphaMatSrc;").replace("s32 colorAmbSrc;", "GXColorSrc colorAmbSrc;").replace("s32 alphaAmbSrc;", "GXColorSrc alphaAmbSrc;"))],
}
SCORE = ["fn_804655D4"]
