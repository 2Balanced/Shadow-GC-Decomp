# Round 2 for src/RenderWare/RtFSyst.c: fread/fwrite through an inline file-read function.
import itertools
SYMBOLS = {
    "fn_8047A3E0": "u32 fn_8047A3E0(void *buffer, u32 size, u32 count, RtFile *file) {",
    "fn_8047A428": "u32 fn_8047A428(const void *buffer, u32 size, u32 count, RtFile *file) {",
    "fn_80479A50": "RwBool fn_80479A50(s32 maxNbFS) {",
}
XTRA = {
    "fn_8047A3E0": """static u32 FileRead(RtFile *file, void *buffer, u32 size) {
    return file->fs->fileRead(file, buffer, size);
}

""",
    "fn_8047A428": """static u32 FileWrite(RtFile *file, const void *buffer, u32 size) {
    return file->fs->fileWrite(file, buffer, size);
}

""",
}
BODIES = {
    "fn_8047A3E0": {
        "read_inl": """
    return FileRead(file, buffer, size * count) / size;
""",
        "read_inl2": """
    u32 n = FileRead(file, buffer, size * count);
    return n / size;
""",
    },
    "fn_8047A428": {
        "write_inl": """
    return FileWrite(file, buffer, size * count) / size;
""",
    },
    "fn_80479A50": {},
}
stmts = ["lbl_805E2A40.numFS = 0;", "lbl_805F1548 = 0;", "lbl_805E2A40.maxNbFS = maxNbFS;",
         "lbl_805E2A40.fsList = 0;", "lbl_805E2A40.isOpen = 1;", "lbl_805E2A40.errorCallback = 0;"]
for i, perm in enumerate(itertools.permutations(stmts)):
    BODIES["fn_80479A50"]["open_p%03d" % i] = "\n    " + "\n    ".join(perm) + "\n    return 1;\n"
