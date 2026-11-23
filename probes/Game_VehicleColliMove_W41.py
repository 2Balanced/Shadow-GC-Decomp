"""Wave 41: Sweep and Settle with copy propagation off (the wave-40 lever)."""
SW = "Sweep__FP5Vec3fPQ27Vehicle12TVehicleBaseP5Vec3fP5Vec3fP15CollisionEffectP28UnkColliMoveContact_803334A4PC21UnkColliList_80095314i"
ST = "Settle__FRC5Vec3fRC5Vec3ff"
SCORE = [SW, ST]
FS = "static bool Sweep(Vec3f *normal, TVehicleBase *vehicle, Vec3f *position, Vec3f *velocity,"
FT = "static UnkCopyVector3_800091EC Settle(const Vec3f &position, const Vec3f &direction,"
FN = "UnkVehicleColliMove_8033377C::UnkVehicleColliMove_8033377C(TVehicleBase *vehicle) : m_vehicle(vehicle) {"
CS = "/* Moves position out of the world objects within distance below it (name inferred; direction is"
REPL = {
  "prag_sweep": [(FS, "#pragma opt_propagation off\n" + FS), (CS, "#pragma opt_propagation reset\n" + CS)],
  "prag_settle": [(FT, "#pragma opt_propagation off\n" + FT), (FN, "#pragma opt_propagation reset\n" + FN)],
}
