"""Wave 41: IsAllIgnored shapes that match (copy propagation off) measured with the unit's @N numbering
and function order (build/t/nprobe.py), with the order fixes of the unit."""
OLD = "                    if (entry->first == hit->field_0x5C && entry->second == GetKind(hit.get())) {\n"
ANCH = "bool GadgetCharCollision::IsAllIgnored(UnkColliContacts_8001EE04 *contacts) {\n"
NEXT = "/* Wraps collider in a new handle. */\n"
PRAG = [(ANCH, "#pragma opt_propagation off\n" + ANCH), (NEXT, "#pragma opt_propagation reset\n" + NEXT)]
GETID = "inline s32 GetId(CharInfo *info) {\n    return info->field_0x5C;\n}\n"
ORD = [("#define GADGETCHARCOLLISION_IMPL\n", "#define GADGETCHARCOLLISION_IMPL\n#define MSL_VECTOR_DESTROY_ALL_LATE\n"),
       ("GadgetColliDetectArgs::~GadgetColliDetectArgs() {}\n", ""),
       (NEXT, "#include <vector_impl>\n" + NEXT)]
VEC = [('#include "Game/GadgetCharCollision.h"\n', '#include <utility>\n#include <vector>\n#include "Game/GadgetCharCollision.h"\n')]
def v(s=None, pre=None, extra=(), prag=True):
    r = []
    if s:
        r.append((OLD, s))
    r += list(extra)
    if pre:
        r.append((ANCH, pre + "\n" + ANCH))
    return r + (PRAG if prag else [])
GI = "                    if (entry->first == GetId(hit.get()) && entry->second == GetKind(hit.get())) {\n"
DT = {"Game/GadgetColliDetectArgs.h": [("    virtual ~GadgetColliDetectArgs();\n", "    virtual ~GadgetColliDetectArgs() {}\n")]}
REPL = {
 "prag": v(),
 "getid_noprag": v(GI, GETID, prag=False),
 "prag_getid": v(GI, GETID),
 "prag_getid_ord": v(GI, GETID, ORD),
 "prag_getid_ord_vec": v(GI, GETID, ORD + VEC),
 "ord_only": v(extra=ORD, prag=False),
 "ord_vec_only": v(extra=ORD + VEC, prag=False),
}
HDR = {"prag_getid_ord": DT, "prag_getid_ord_vec": DT, "ord_only": DT, "ord_vec_only": DT}
SCORE = ["IsAllIgnored__19GadgetCharCollisionFP25UnkColliContacts_8001EE04"]
LATE = [("#define GADGETCHARCOLLISION_IMPL\n", "#define GADGETCHARCOLLISION_IMPL\n#define MSL_VECTOR_DESTROY_ALL_LATE\n"),
        (NEXT, "#include <vector_impl>\n" + NEXT)]
NODT = [("GadgetColliDetectArgs::~GadgetColliDetectArgs() {}\n", "")]
INLDT_CPP = [("GadgetColliDetectArgs::~GadgetColliDetectArgs() {}\n", "inline GadgetColliDetectArgs::~GadgetColliDetectArgs() {}\n")]
REPL.update({
 "late_only": v(extra=LATE, prag=False),
 "vec_only": v(extra=VEC, prag=False),
 "late_vec": v(extra=LATE + VEC, prag=False),
 "dt_hdr_only": v(extra=NODT, prag=False),
 "dt_inl_cpp": v(extra=INLDT_CPP, prag=False),
 "dt_inl_hdr_out": v(extra=NODT, prag=False),
})
DT_OUT = {"Game/GadgetColliDetectArgs.h": [("    virtual ~GadgetColliDetectArgs();\n};\n", "    virtual ~GadgetColliDetectArgs();\n};\n\ninline GadgetColliDetectArgs::~GadgetColliDetectArgs() {}\n")]}
HDR.update({"dt_hdr_only": DT, "dt_inl_hdr_out": DT_OUT})
