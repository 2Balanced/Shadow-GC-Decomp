# Probe table for src/RenderWare/RtCharset.c: RtCharsetPrint's declaration order.
import itertools
OLD = """    s32 length = strlen(string);
    s32 vertexSize;
    s32 indexSize;
    RwIm2DVertex *vertices;
    u16 *indices;
    s32 count;
"""
items = ["s32 vertexSize;", "s32 indexSize;", "RwIm2DVertex *vertices;", "u16 *indices;", "s32 count;"]
REPL = {}
for i, perm in enumerate(itertools.permutations(items)):
    for lf in (0, 1):
        if lf == 0:
            new = "    s32 length = strlen(string);\n" + "".join("    " + p + "\n" for p in perm)
        else:
            new = "".join("    " + p + "\n" for p in perm) + "    s32 length = strlen(string);\n"
            # declarations after an initialised one are fine in C89 only before statements
        REPL["p%03d_%d" % (i, lf)] = [(OLD, new)]
SCORE = ["fn_80478700"]
