"""Does a const vfPara_0x5C keep EggShadowAndroid's lwzu after the link register's store?"""
SCORE = [("vfPara_0x5C__Q216EggShadowAndroid16EggShadowAndroidFv", "vfPara_0x5C__Q216EggShadowAndroid16EggShadowAndroidCFv")]
REPL = {"const": [("int EggShadowAndroid::vfPara_0x5C() {", "int EggShadowAndroid::vfPara_0x5C() const {")]}
HDR = {"const": {"Game/EggShadowAndroid.h": [("    virtual int vfPara_0x5C();", "    virtual int vfPara_0x5C() const;")],
                 "Game/Enemy.h": [("    virtual int vfStat_0x74();", "    virtual int vfStat_0x74() const;")]}}
