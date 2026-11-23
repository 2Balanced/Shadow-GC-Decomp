"""Wave 40, RwCpp10 round 2: fn_80448850's saved registers (retail: atomic r31, geometry flags r30,
data flags then geometry r29, descriptor then material r28)."""
SCORE = ["fn_80448850"]
D0 = """    R2SurfaceProperties surface;
    u32 local;
"""
SIG = "u32 fn_80448850(R2Atomic *atomic, s32 maxParticles, u32 unused, u32 dataFlags) {\n"
I0 = "    desc = fn_8046301C();\n    geometryFlags = 0x2;\n"
REPL = {
  "init_first": [(I0, "    geometryFlags = 0x2;\n    desc = fn_8046301C();\n")],
  "flags_copy": [(SIG, SIG.replace("dataFlags", "dataFlagsIn")), (D0, D0 + "    u32 dataFlags = dataFlagsIn;\n")],
  "atomic_copy": [(SIG, SIG.replace("*atomic", "*atomicIn")), (D0, D0 + "    R2Atomic *atomic = atomicIn;\n")],
  "local_arr": [(D0, "    R2SurfaceProperties surface;\n    u32 local[1];\n"), ("&local,", "local,")],
  "local_first_arr": [(D0, "    u32 local[2];\n    R2SurfaceProperties surface;\n"), ("&local,", "local,")],
  "surf_ptr": [("    material->surfaceProps = surface;\n", "    {\n        R2SurfaceProperties *sp = &surface;\n        material->surfaceProps = *sp;\n    }\n")],
  "mesh_gone": [("    mesh = (R2Mesh *)(header + 1);\n    mesh->indices = (u16 *)mesh;\n    mesh->material = material;\n", "    ((R2Mesh *)(header + 1))->indices = (u16 *)(header + 1);\n    ((R2Mesh *)(header + 1))->material = material;\n")],
  "s32_flags": [(SIG, SIG.replace("u32 dataFlags", "s32 dataFlags"))],
  "ret_int": [(SIG, SIG.replace("u32 fn_80448850", "s32 fn_80448850"))],
  "geomflag_s32": [("    u32 geometryFlags;\n", "    s32 geometryFlags;\n")],
}
