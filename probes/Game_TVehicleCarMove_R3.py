"""Wave 40: the handling table read directly instead of through the inline Param(), one function
at a time (d<k> = the k-th `param` reference of the file)."""
P = "Q27Vehicle15TVehicleCarMove"
SCORE = ["UpdateModel__%sFv" % P, "Move__%sFf" % P, "SetPlacement__%sFPC5Vec3fPC5Vec3f" % P, "UpdateBoost__%sFf" % P, "UpdateMotion__%sFf" % P, "ReadInput__%sFf" % P, "Sweep__FPQ27Vehicle12TVehicleBaseUlP5Vec3fP5Vec3fbP5Vec3fPf"]
A = "    const CarParam &param = Param();"
SKIP = (A, "    const CarParam &param = Param() ;")
REPL = {"base": []}
for k in range(18):
    REPL["d%02d" % k] = [SKIP] * k + [(A, "    const CarParam &param = s_params[m_type];")]
