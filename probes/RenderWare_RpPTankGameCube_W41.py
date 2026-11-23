"""Wave 41, RenderWare/RpPTankGameCube: fn_80448850 saved registers (declaration orders, void* object)."""
import itertools
SCORE = ["fn_80448850"]
SIG = "u32 fn_80448850(R2Atomic *atomic, s32 maxParticles, u32 unused, u32 dataFlags) {\n"
DECL = """    u32 local[2];
    R2SurfaceProperties surface;
    R2MeshHeader *header;
    R2Mesh *mesh;
    R2Material *material;
    R2Geometry *geometry;
    R2VertexDesc *desc;
    u32 geometryFlags;
"""
D = {"header": "    R2MeshHeader *header;\n", "material": "    R2Material *material;\n",
     "geometry": "    R2Geometry *geometry;\n", "desc": "    R2VertexDesc *desc;\n",
     "flags": "    u32 geometryFlags;\n"}
REPL = {"base": []}
for perm in itertools.permutations(["header", "material", "geometry", "desc", "flags"]):
    decl = "    u32 local[2];\n    R2SurfaceProperties surface;\n    R2Mesh *mesh;\n" + "".join(D[p] for p in perm)
    name = "p_" + "_".join(p[:2] for p in perm)
    REPL[name] = [(DECL, decl)]
    REPL["v" + name] = [(SIG, "u32 fn_80448850(void *object, s32 maxParticles, u32 unused, u32 dataFlags) {\n    R2Atomic *atomic = (R2Atomic *)object;\n"), (DECL, decl)]
