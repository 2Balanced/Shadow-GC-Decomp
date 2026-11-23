"""Bounding-box propagation fn_8045FBA4 of Game/RwCpp24.cpp (wave 40, agent r3): it returns the
world (r3 stays live), and the plane's axis is kept in a saved register."""
SYMBOLS = {"fn_8045FBA4": "RpWorld *fn_8045FBA4(RpWorld *world) {"}


def body(decl, init, typemode, popform):
    lines = [""] + decl + [""] + init
    lines.append("    while (depth >= 0) {")
    if typemode == 1:
        lines.append("        s32 type = sector->type;")
    if typemode == 2:
        lines.append("        type = sector->type;")
    cond = "type < 0" if typemode else "sector->type < 0"
    lines.append("        if (%s) {" % cond)
    lines.append("            ((RpWorldSector *)sector)->boundingBox = bbox;")
    lines += popform
    lines.append("        } else {")
    lines.append("            RpPlaneSector *plane = (RpPlaneSector *)sector;")
    lines.append("            depth++;")
    lines.append("            bboxStack[depth] = bbox;")
    lines.append("            stack[depth] = plane->rightSubTree;")
    ty = "type" if typemode else "plane->type"
    lines.append("            *(f32 *)((u8 *)&bboxStack[depth].inf + %s) = plane->rightValue;" % ty)
    lines.append("            *(f32 *)((u8 *)&bbox.sup + %s) = plane->leftValue;" % ty)
    lines.append("            sector = plane->leftSubTree;")
    lines.append("        }")
    lines.append("    }")
    lines.append("    return world;")
    return "\n".join(lines) + "\n"


BASE = ["    RwBBox bbox;", "    RpSector *stack[64];", "    RwBBox bboxStack[64];", "    s32 depth = 0;",
        "    RpSector *sector;"]
I1 = ["    bbox = world->boundingBox;", "    sector = world->rootSector;"]
P1 = ["            bbox = bboxStack[depth];", "            sector = stack[depth--];"]
P2 = ["            bbox = bboxStack[depth];", "            sector = stack[depth];", "            depth--;"]
BODIES = {"fn_8045FBA4": {}}
for pi, p in enumerate([P1, P2]):
    BODIES["fn_8045FBA4"]["n%d" % pi] = body(BASE, I1, 0, p)
    BODIES["fn_8045FBA4"]["s%d" % pi] = body(BASE, I1, 1, p)
    for k in range(len(BASE) + 1):
        d = BASE[:k] + ["    s32 type;"] + BASE[k:]
        BODIES["fn_8045FBA4"]["t%d%d" % (k, pi)] = body(d, I1, 2, p)
