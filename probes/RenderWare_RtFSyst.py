# Probe table for src/RenderWare/RtFSyst.c (tools/pprobe.py -u RenderWare/<Unit> -p <table>).
SYMBOLS = {
    "fn_80479848": "RwBool fn_80479848(RtFile *file) {",
    "fn_80479A50": "RwBool fn_80479A50(s32 maxNbFS) {",
    "fn_8047A3E0": "u32 fn_8047A3E0(void *buffer, u32 size, u32 count, RtFile *file) {",
    "fn_8047A428": "u32 fn_8047A428(const void *buffer, u32 size, u32 count, RtFile *file) {",
}
BODIES = {
    "fn_80479848": {
        "eof_pos": """
    if (file->position == -1) {
        return 1;
    }
    return file->position >= file->length;
""",
        "eof_pos2": """
    RwBool eof;
    if (file->position == -1) {
        return 1;
    }
    eof = file->position >= file->length;
    return eof;
""",
        "eof_pos3": """
    if (file->position != -1) {
        return file->position >= file->length;
    }
    return 1;
""",
    },
    "fn_80479A50": {
        "open_a": """
    lbl_805E2A40.maxNbFS = maxNbFS;
    lbl_805E2A40.numFS = 0;
    lbl_805E2A40.errorCallback = 0;
    lbl_805F1548 = 0;
    lbl_805E2A40.fsList = 0;
    lbl_805E2A40.isOpen = 1;
    return 1;
""",
        "open_b": """
    lbl_805E2A40.numFS = 0;
    lbl_805E2A40.maxNbFS = maxNbFS;
    lbl_805F1548 = 0;
    lbl_805E2A40.fsList = 0;
    lbl_805E2A40.errorCallback = 0;
    lbl_805E2A40.isOpen = 1;
    return 1;
""",
        "open_c": """
    lbl_805E2A40.numFS = 0;
    lbl_805F1548 = 0;
    lbl_805E2A40.maxNbFS = maxNbFS;
    lbl_805E2A40.fsList = 0;
    lbl_805E2A40.errorCallback = 0;
    lbl_805E2A40.isOpen = 1;
    return 1;
""",
        "open_d": """
    lbl_805E2A40.errorCallback = 0;
    lbl_805E2A40.numFS = 0;
    lbl_805F1548 = 0;
    lbl_805E2A40.maxNbFS = maxNbFS;
    lbl_805E2A40.fsList = 0;
    lbl_805E2A40.isOpen = 1;
    return 1;
""",
        "open_e": """
    lbl_805E2A40.fsList = 0;
    lbl_805E2A40.numFS = 0;
    lbl_805F1548 = 0;
    lbl_805E2A40.maxNbFS = maxNbFS;
    lbl_805E2A40.errorCallback = 0;
    lbl_805E2A40.isOpen = 1;
    return 1;
""",
    },
    "fn_8047A3E0": {
        "read_direct": """
    return file->fs->fileRead(file, buffer, size * count) / size;
""",
        "read_local2": """
    u32 total = size * count;
    return file->fs->fileRead(file, buffer, total) / size;
""",
    },
    "fn_8047A428": {
        "write_direct": """
    return file->fs->fileWrite(file, buffer, size * count) / size;
""",
    },
}
