# Round 3 for src/RenderWare/RtFSyst.c: fread's signature and spelling.
A = "u32 fn_8047A3E0(void *buffer, u32 size, u32 count, RtFile *file) {\n    RtFileSystem *fs = file->fs;\n\n    return fs->fileRead(file, buffer, size * count) / size;\n}"
def v(sig, body):
    return [(A, sig + " {" + body + "}")]
REPL = {
    "voidptr": v("u32 fn_8047A3E0(void *buffer, u32 size, u32 count, void *fptr)",
                 "\n    RtFile *file = (RtFile *)fptr;\n    RtFileSystem *fs = file->fs;\n\n    return fs->fileRead(file, buffer, size * count) / size;\n"),
    "voidptr2": v("u32 fn_8047A3E0(void *buffer, u32 size, u32 count, void *fptr)",
                 "\n    RtFileSystem *fs = ((RtFile *)fptr)->fs;\n\n    return fs->fileRead((RtFile *)fptr, buffer, size * count) / size;\n"),
    "voidptr3": v("u32 fn_8047A3E0(void *buffer, u32 size, u32 count, void *fptr)",
                 "\n    return ((RtFile *)fptr)->fs->fileRead((RtFile *)fptr, buffer, size * count) / size;\n"),
    "bytes": v("u32 fn_8047A3E0(void *buffer, u32 size, u32 count, RtFile *file)",
                 "\n    u32 bytes = size * count;\n    RtFileSystem *fs = file->fs;\n\n    return fs->fileRead(file, buffer, bytes) / size;\n"),
    "bytes2": v("u32 fn_8047A3E0(void *buffer, u32 size, u32 count, RtFile *file)",
                 "\n    RtFileSystem *fs = file->fs;\n    u32 bytes = size * count;\n    u32 read;\n\n    read = fs->fileRead(file, buffer, bytes);\n    return read / size;\n"),
    "constbuf": v("u32 fn_8047A3E0(void *buffer, u32 size, u32 count, RtFile *const file)",
                 "\n    RtFileSystem *fs = file->fs;\n\n    return fs->fileRead(file, buffer, size * count) / size;\n"),
    "constfile": v("u32 fn_8047A3E0(void *buffer, u32 size, u32 count, const RtFile *file)",
                 "\n    RtFileSystem *fs = file->fs;\n\n    return fs->fileRead((RtFile *)file, buffer, size * count) / size;\n"),
    "fnptr": v("u32 fn_8047A3E0(void *buffer, u32 size, u32 count, RtFile *file)",
                 "\n    u32 (*read)(RtFile *file, void *buffer, u32 size) = file->fs->fileRead;\n\n    return read(file, buffer, size * count) / size;\n"),
}
SCORE = ["fn_8047A3E0"]
