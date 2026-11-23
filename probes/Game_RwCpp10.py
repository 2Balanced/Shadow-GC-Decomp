"""Wave 40, RwCpp10 (RpPTank GameCube): register choice of fn_80448850 and the float registers of
the rotated axes in fn_80448A24."""
SCORE = ["fn_80448850", "fn_80448A24"]
A0 = """            f32 s = (f32)sin(tank->cRotate);
            f32 c = (f32)cos(tank->cRotate);
"""
AX = """            axes[0].x = c * ltm->right.x + -s * ltm->up.x;
            axes[0].y = c * ltm->right.y + -s * ltm->up.y;
            axes[0].z = c * ltm->right.z + -s * ltm->up.z;
"""
def ax(f): return "".join(f.format(a=a) for a in "xyz")
D0 = """    R2SurfaceProperties surface;
    u32 local;
"""
R0 = "    atomic = fn_80456F0C(atomic, geometry, 0);\n"
REPL = {
  "base": [(A0, A0)],
  "ns_var": [(A0, A0 + "            f32 ns = -s;\n"), (AX, ax("            axes[0].{a} = c * ltm->right.{a} + ns * ltm->up.{a};\n"))],
  "ns_first": [(A0, "            f32 s = (f32)sin(tank->cRotate);\n            f32 ns = -s;\n            f32 c = (f32)cos(tank->cRotate);\n"), (AX, ax("            axes[0].{a} = c * ltm->right.{a} + ns * ltm->up.{a};\n"))],
  "c_first": [(A0, "            f32 c;\n            f32 s = (f32)sin(tank->cRotate);\n            c = (f32)cos(tank->cRotate);\n")],
  "minus": [(AX, ax("            axes[0].{a} = c * ltm->right.{a} - s * ltm->up.{a};\n"))],
  "dbl": [(A0, "            f32 s = sin(tank->cRotate);\n            f32 c = cos(tank->cRotate);\n")],
  "s_decl_swap": [(A0, "            f32 c;\n            f32 s;\n            s = (f32)sin(tank->cRotate);\n            c = (f32)cos(tank->cRotate);\n")],
  "loc_swap": [(D0, "    u32 local;\n    R2SurfaceProperties surface;\n")],
  "newvar": [(R0, "    result = fn_80456F0C(atomic, geometry, 0);\n"), (D0, D0 + "    R2Atomic *result;\n"),
             ("    fn_80448658(atomic);\n    atomic->pipeline", "    fn_80448658(result);\n    result->pipeline")],
  "noassign": [(R0, "    fn_80456F0C(atomic, geometry, 0);\n")],
}
