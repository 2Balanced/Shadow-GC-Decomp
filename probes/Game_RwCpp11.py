"""Wave 40, RwCpp11: the size sum of fn_8044BB18 (retail shifts the two counts separately)."""
SCORE = ["fn_8044BB18"]
O = "skin->numBones + ((split->numMeshes << 1) + (split->numRLE << 1))"
REPL = {
  "sizeof": [(O, "skin->numBones * sizeof(u8) + (split->numMeshes * sizeof(u16) + split->numRLE * sizeof(u16))")],
  "flat": [(O, "skin->numBones + split->numMeshes * 2 + split->numRLE * 2")],
  "rev": [(O, "split->numMeshes * 2 + split->numRLE * 2 + skin->numBones")],
  "s32": [(O, "skin->numBones + ((s32)split->numMeshes * 2 + (s32)split->numRLE * 2)")],
  "sizeof_rev": [(O, "sizeof(u16) * split->numMeshes + sizeof(u16) * split->numRLE + skin->numBones")],
  "sz3": [(O, "skin->numBones + (sizeof(u16) * split->numMeshes + sizeof(u16) * split->numRLE)")],
  "skinacc": [(O, "skin->numBones + (skin->split.numMeshes * 2 + skin->split.numRLE * 2)")],
}
