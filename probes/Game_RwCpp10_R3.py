"""Wave 40, RwCpp10 round 3: fn_80448850 with parts as inline helpers."""
SCORE = ["fn_80448850"]
SIG = "u32 fn_80448850(R2Atomic *atomic, s32 maxParticles, u32 unused, u32 dataFlags) {\n"
MH = """    header = fn_8045BF30(sizeof(R2MeshHeader) + sizeof(R2Mesh));
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
HELP_MH = """static inline R2MeshHeader *MakeHeader(R2Material *material) {
    R2MeshHeader *header;
    R2Mesh *mesh;
""" + MH.replace("    geometry->mesh = header;\n", "    return header;\n") + "}\n\n"
HELP_MH2 = """static inline void MakeHeader(R2Geometry *geometry, R2Material *material) {
    R2MeshHeader *header;
    R2Mesh *mesh;
""" + MH + "}\n\n"
GEO = """    desc = fn_8046301C();
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
HELP_GEO = """static inline R2Geometry *MakeGeometry(u32 dataFlags) {
    R2Geometry *geometry;
    R2VertexDesc *desc;
    u32 geometryFlags;
""" + GEO + "    return geometry;\n}\n\n"
REPL = {
  "mh": [(SIG, HELP_MH + SIG), (MH, "    geometry->mesh = MakeHeader(material);\n")],
  "mh2": [(SIG, HELP_MH2 + SIG), (MH, "    MakeHeader(geometry, material);\n")],
  "geo": [(SIG, HELP_GEO + SIG), (GEO, "    geometry = MakeGeometry(dataFlags);\n")],
  "geo_mh2": [(SIG, HELP_GEO + HELP_MH2 + SIG), (GEO, "    geometry = MakeGeometry(dataFlags);\n"), (MH, "    MakeHeader(geometry, material);\n")],
}
