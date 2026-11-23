"""Wave 41: EffectObjectUSA's constructor with the description as a pointer to non-const (the
wave 40 constructor lever), on top of the masked alpha local."""
import importlib.util, os
spec = importlib.util.spec_from_file_location("w41", os.path.join("probes", "Game_EffectObjectUSA_W41.py"))
w41 = importlib.util.module_from_spec(spec); spec.loader.exec_module(w41)
CT = "__ct__Q412SonicteamUSA6System6Effect15EffectObjectUSAFPCQ36Effect6System22UnkEffectDesc_801D0AFCPQ36Effect6System16EffectLaunchInfoPQ36Effect6System13EffectManager"
SCORE = [(CT, CT.replace("FPCQ36Effect6System22", "FPQ36Effect6System22"))]
HDR = w41.HDR
REPL = {"cm_nc": w41.REPL["cm_nc"]}
