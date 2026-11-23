"""HeavyDogCtrlUnit_RocketLauncher's constructor (wave 40, round 2): fn_800C7F7C's last parameter
is a u16 (Game/CharaColliManager.cpp defines it so; Game/BossCharaColli.cpp matched with it)."""
SCORE = ["__ct__31HeavyDogCtrlUnit_RocketLauncherFP16HeavyDogVarParami"]
D = """                                              const UnkBossColliShape *shapes, _stMtnPlayWork *motion,
                                              int arg4, int arg5);
"""
REPL = {
  "u16": [(D, D.replace("int arg4, int arg5", "int arg4, u16 arg5"))],
  "u16_type": [(D, D.replace("int arg4, int arg5", "int type, u16 typeArg"))],
}
