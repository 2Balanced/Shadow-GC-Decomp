"""Wave 41, RenderWare/RpPTankGameCube round 3: fn_80448850 with void* object and block scopes."""
SCORE = ["fn_80448850"]
SRC = open("src/RenderWare/RpPTankGameCube.c", newline="").read().replace("\r\n", "\n")


def _span(text, start, end, after=0):
    """The text from start up to and including end (searched from start); a marker when the
    anchor is gone because a variant of this table was kept in the source."""
    try:
        a = text.index(start)
        b = text.index(end, a) + len(end) + after
    except ValueError:
        return "<<anchor gone: a variant of this table is in the source>>"
    return text[a:b]

OLD = _span(SRC, "u32 fn_80448850(R2Atomic *atomic, s32 maxParticles, u32 unused, u32 dataFlags) {", "\n}\n", 3 - len("\n}\n"))
BODY_GEO = """    desc = fn_8046301C();
    geometryFlags = 0x2;
    fn_80462F7C(desc, 4, 0);
    if ((dataFlags & 0x10) == 0x10) {
        geometryFlags |= 0x30;
        fn_80462F88(desc, 4, 0);
    }
    if (dataFlags & 0x40042) {
        geometryFlags |= 0x8;
        fn_80462FA4(desc, 5);
    }
    if (dataFlags & 0x180180) {
        geometryFlags |= 0x4;
        fn_80462F94(desc, 1, 4, 0);
    }

    geometry = fn_80458D24(1.0f);
    geometry->flags = geometryFlags;
    geometry->numVertices = 1;
    fn_80463104(geometry, desc);
    fn_804630C0(desc);
"""
MAT = """    material = fn_8045B13C();
    surface.ambient = 0.3f;
    surface.specular = 1.0f;
    surface.diffuse = 1.0f;
    material->surfaceProps = surface;
    fn_8045905C(geometry, local, material);
"""
HDR_CODE = """    header = fn_8045BF30(sizeof(R2MeshHeader) + sizeof(R2Mesh));
    memset(header, 0, sizeof(R2MeshHeader) + sizeof(R2Mesh));
    header->flags = (header->flags & ~0xFF) |
                    ((R2MeshGlobals *)((u8 *)lbl_805F15B4 + lbl_805F14F0))->meshFlagsDefault;
    mesh = (R2Mesh *)(header + 1);
    mesh->indices = (u16 *)mesh;
    mesh->material = material;
    header->numMeshes = 1;
    header->serialNum = fn_8045CA20();
    header->firstMeshOffset = 0;
    header->totalIndicesInMesh = 0;
    geometry->mesh = header;
"""
TAIL = """    atomic = fn_80456F0C(atomic, geometry, 0);
    fn_8045985C(geometry);
    fn_8045B1F4(material);
    fn_80448658(atomic);
    atomic->pipeline = ptankPipeline;
    return 1;
}
"""
VSIG = "u32 fn_80448850(void *object, s32 maxParticles, u32 unused, u32 dataFlags) {\n    R2Atomic *atomic = (R2Atomic *)object;\n"
def ind(t):
    return "".join("    " + l + "\n" if l else "\n" for l in t.rstrip("\n").split("\n"))
V = {}
# header and mesh in a block of their own
V["blk_hdr"] = VSIG + """    u32 local[2];
    R2SurfaceProperties surface;
    R2Material *material;
    R2Geometry *geometry;
    R2VertexDesc *desc;
    u32 geometryFlags;

""" + BODY_GEO + "\n" + MAT + "\n    {\n        R2MeshHeader *header;\n        R2Mesh *mesh;\n\n" + ind(HDR_CODE) + "    }\n\n" + TAIL
V["blk_mat_hdr"] = VSIG + """    R2Geometry *geometry;
    R2VertexDesc *desc;
    u32 geometryFlags;

""" + BODY_GEO + "\n    {\n        u32 local[2];\n        R2SurfaceProperties surface;\n        R2Material *material;\n\n" + ind(MAT) + "\n        {\n            R2MeshHeader *header;\n            R2Mesh *mesh;\n\n" + ind(ind(HDR_CODE)) + "        }\n\n" + ind(TAIL.replace("}\n", "")) + "    }\n}\n"
V["blk_geo"] = VSIG + """    u32 local[2];
    R2SurfaceProperties surface;
    R2MeshHeader *header;
    R2Mesh *mesh;
    R2Material *material;
    R2Geometry *geometry;

    {
        R2VertexDesc *desc;
        u32 geometryFlags;

""" + ind(BODY_GEO) + "    }\n\n" + MAT + "\n" + HDR_CODE + "\n" + TAIL
REPL = {"base": []}
for k, t in V.items():
    REPL[k] = [(OLD, t)]
