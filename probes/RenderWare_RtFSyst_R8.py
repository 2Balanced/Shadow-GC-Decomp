# Round 8 for src/RenderWare/RtFSyst.c: declaration orders of RtFSManagerGetFileSystemFromFileName.
import itertools
SYMBOLS = {"fn_8047A044": "RtFileSystem *fn_8047A044(const char *name) {"}
decls = ["const char *p;", "u32 length;", "RtFileSystem *fs;", "char *q;", "u32 i;"]
BODY = """
    if (lbl_805E2A40.numFS != 0) {
        char device[100];
        %s

        length = RWSRCGLOBAL(vecStrlen)(name);
        p = name;
        q = device;
        for (i = 0; i < length; i++, p++, q++) {
            if (*p == ':') {
                fn_803ACABC(device, name, i + 1);
                q[1] = 0;
                fs = fn_80479FD0(device);
                if (fs != 0) {
                    return fs;
                }
            }
        }
        if (lbl_805F1548 != 0) {
            return lbl_805F1548;
        }
    }
    if (lbl_805E2A40.errorCallback != 0) {
        lbl_805E2A40.errorCallback(6);
    }
    return 0;
"""
BODIES = {"fn_8047A044": {}}
for k, perm in enumerate(itertools.permutations(decls)):
    BODIES["fn_8047A044"]["dec%03d" % k] = BODY % "\n        ".join(perm)
