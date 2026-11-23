"""Wave 40 (r2), RwCpp16 round 7: fn_8045345C's float registers (retail: lowerLeft f8, lowerRight f7,
m11 f6, m01 f5, m00 f4, m10 f3, inverse f2, upperRight f1)."""
import itertools
SCORE = ["fn_8045345C"]
B0 = """        float inverse = lbl_805FB0DC / determinant;
        float upperRight = -from[3] * inverse;
        float lowerLeft = -from[4] * inverse;
        float lowerRight = from[5] * inverse;
        float m10 = out[4];
        float m11 = out[5];
        float m00 = out[2];
        float m01 = out[3];
"""
A = """        inverse = lbl_805FB0DC / determinant;
        upperRight = -from[3] * inverse;
        lowerLeft = -from[4] * inverse;
        lowerRight = from[5] * inverse;
        m10 = out[4];
        m11 = out[5];
        m00 = out[2];
        m01 = out[3];
"""
names = ["inverse", "upperRight", "lowerLeft", "lowerRight", "m10", "m11", "m00", "m01"]
def decl(order):
    return "".join("        float %s;\n" % n for n in order) + A
REPL = {"base": [(B0, B0)]}
REPL["o1"] = [(B0, decl(["inverse", "upperRight", "m10", "m00", "m01", "m11", "lowerRight", "lowerLeft"]))]
REPL["o2"] = [(B0, decl(["lowerLeft", "lowerRight", "m11", "m01", "m00", "m10", "inverse", "upperRight"]))]
REPL["o3"] = [(B0, decl(["upperRight", "inverse", "m10", "m00", "m01", "m11", "lowerRight", "lowerLeft"]))]
REPL["o4"] = [(B0, decl(["lowerLeft", "lowerRight", "m11", "m01", "m00", "m10", "upperRight", "inverse"]))]
REPL["o5"] = [(B0, decl(["m10", "m00", "m01", "m11", "lowerRight", "lowerLeft", "inverse", "upperRight"]))]
REPL["o6"] = [(B0, decl(["inverse", "upperRight", "lowerLeft", "lowerRight", "m10", "m00", "m01", "m11"]))]
