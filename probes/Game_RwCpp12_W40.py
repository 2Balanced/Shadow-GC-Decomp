"""Wave 40 (r2), RwCpp12: fn_8044D344 with the skin read through a pointer to const (retail hoists
skin->usedBones out of the loop once per use and strength-reduces each use into a pointer of its
own: lwz r31,8(skin); mr r25,r31; mr r27,r31)."""
SCORE = ["fn_8044D344"]
S0 = "void fn_8044D344(R2Matrix *out, R2Skin *skin, R2Matrix *ltm, R2HAnimHierarchy *hierarchy) {\n"
L0 = """            usedBones = skin->usedBones;
            matrices = skin->matrices;
            numUsedBones = skin->numUsedBones;
            for (i = 0; i < numUsedBones; i++) {
                boneToWorld.flags = 0;
                boneLTM = RwFrameGetLTM(hierarchy->nodeInfo[usedBones[i]].frame);
                fn_8047EBB8(&boneToWorld, &matrices[usedBones[i]], boneLTM);
                fn_8047EBB8(&out[usedBones[i]], &boneToWorld, last);
            }
"""
L1 = """            for (i = 0; i < skin->numUsedBones; i++) {
                boneToWorld.flags = 0;
                boneLTM = RwFrameGetLTM(hierarchy->nodeInfo[skin->usedBones[i]].frame);
                fn_8047EBB8(&boneToWorld, &skin->matrices[skin->usedBones[i]], boneLTM);
                fn_8047EBB8(&out[skin->usedBones[i]], &boneToWorld, last);
            }
"""
REPL = {
 "base": [(S0, S0)],
 "const": [(S0, S0.replace("R2Skin *skin", "const R2Skin *skin"))],
 "const_direct": [(S0, S0.replace("R2Skin *skin", "const R2Skin *skin")), (L0, L1)],
}
