"""Round 7 for GiGameOver's constructor (wave 39): the allocator's file name in CProject's inline
operator new (Game/TGi.h) through a local."""
SCORE = ["__ct__10GiGameOverFP4Task"]
H = "Game/TGi.h"
OLD = '        return fn_803605AC()->Alloc(size, "csdBase.h", 40);'
HDR = {
  "file_local": {H: [(OLD, '        const char *file = "csdBase.h";\n        return fn_803605AC()->Alloc(size, file, 40);')]},
  "file_line_locals": {H: [(OLD, '        const char *file = "csdBase.h";\n        int line = 40;\n        return fn_803605AC()->Alloc(size, file, line);')]},
  "alloc_local": {H: [(OLD, '        UnkCsdAllocator_803605AC *alloc = fn_803605AC();\n        return alloc->Alloc(size, "csdBase.h", 40);')]},
  "result_local": {H: [(OLD, '        void *p = fn_803605AC()->Alloc(size, "csdBase.h", 40);\n        return p;')]},
}
