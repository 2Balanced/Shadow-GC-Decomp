"""Wave 41, RenderWare/RpPTankGameCube round 2: fn_80448850 with inline helpers and void* object."""
SCORE = ["fn_80448850"]
SIG = "u32 fn_80448850(R2Atomic *atomic, s32 maxParticles, u32 unused, u32 dataFlags) {\n"
VSIG = "u32 fn_80448850(void *object, s32 maxParticles, u32 unused, u32 dataFlags) {\n    R2Atomic *atomic = (R2Atomic *)object;\n"
DECL = """    u32 local[2];
    R2SurfaceProperties surface;
    R2MeshHeader *header;
    R2Mesh *mesh;
    R2Material *material;
    R2Geometry *geometry;
    R2VertexDesc *desc;
    u32 geometryFlags;
"""
MAT = """    material = fn_8045B13C();
    surface.ambient = 0.3f;
    surface.specular = 1.0f;
    surface.diffuse = 1.0f;
    material->surfaceProps = surface;
"""
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
HELP_MAT = """static inline R2Material *MakeMaterial(void) {
    R2SurfaceProperties surface;
    R2Material *material = fn_8045B13C();
    surface.ambient = 0.3f;
    surface.specular = 1.0f;
    surface.diffuse = 1.0f;
    material->surfaceProps = surface;
    return material;
}

"""
HELP_MH = """static inline R2MeshHeader *MakeHeader(R2Material *material) {
    R2MeshHeader *header;
    R2Mesh *mesh;
""" + MH.replace("    geometry->mesh = header;\n", "    return header;\n") + "}\n\n"
RES = """    atomic = fn_80456F0C(atomic, geometry, 0);
    fn_8045985C(geometry);
    fn_8045B1F4(material);
    fn_80448658(atomic);
    atomic->pipeline = ptankPipeline;
"""
RES_OBJ = """    object = fn_80456F0C(atomic, geometry, 0);
    fn_8045985C(geometry);
    fn_8045B1F4(material);
    fn_80448658(object);
    ((R2Atomic *)object)->pipeline = ptankPipeline;
"""
REPL = {
    "base": [],
    "mat": [(SIG, HELP_MAT + SIG), (MAT, "    material = MakeMaterial();\n")],
    "mh": [(SIG, HELP_MH + SIG), (MH, "    geometry->mesh = MakeHeader(material);\n")],
    "mat_mh": [(SIG, HELP_MAT + HELP_MH + SIG), (MAT, "    material = MakeMaterial();\n"), (MH, "    geometry->mesh = MakeHeader(material);\n")],
    "v": [(SIG, VSIG)],
    "v_mat": [(SIG, HELP_MAT + VSIG), (MAT, "    material = MakeMaterial();\n")],
    "v_mh": [(SIG, HELP_MH + VSIG), (MH, "    geometry->mesh = MakeHeader(material);\n")],
    "v_mat_mh": [(SIG, HELP_MAT + HELP_MH + VSIG), (MAT, "    material = MakeMaterial();\n"), (MH, "    geometry->mesh = MakeHeader(material);\n")],
    "v_obj": [(SIG, VSIG), (RES, RES_OBJ)],
    "v_obj_mat_mh": [(SIG, HELP_MAT + HELP_MH + VSIG), (MAT, "    material = MakeMaterial();\n"), (MH, "    geometry->mesh = MakeHeader(material);\n"), (RES, RES_OBJ)],
    "v_order": [(SIG, VSIG), (DECL, """    u32 local[2];
    R2SurfaceProperties surface;
    u32 geometryFlags;
    R2VertexDesc *desc;
    R2Geometry *geometry;
    R2Material *material;
    R2MeshHeader *header;
    R2Mesh *mesh;
""")],
}
