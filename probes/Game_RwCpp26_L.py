"""Atomic lighting callback fn_80463428 of Game/RwCpp26.cpp (wave 40, agent r3): declaration
order of the list walkers, and where the geometry flags are read."""
SYMBOLS = {"fn_80463428": "void *fn_80463428(void *object, RxGCInstanceData *data) {"}

CLAMP = """    if (data->ambientSet != 0) {
        if (data->ambient.red > 1.0f) {
            data->ambient.red = 1.0f;
        }
        if (data->ambient.green > 1.0f) {
            data->ambient.green = 1.0f;
        }
        if (data->ambient.blue > 1.0f) {
            data->ambient.blue = 1.0f;
        }
    }
    return object;
"""


def gen(decls, ends, flagsfirst):
    lines = ["    RpAtomic *atomic = (RpAtomic *)object;"]
    cond = "(atomic->geometry->flags & rpGEOMETRYLIGHT)"
    if flagsfirst == 1:
        lines.append("    RpGeometry *geometry = atomic->geometry;")
        cond = "(geometry->flags & rpGEOMETRYLIGHT)"
    elif flagsfirst == 2:
        lines.append("    u32 flags = atomic->geometry->flags;")
        cond = "(flags & rpGEOMETRYLIGHT)"
    lines += ["    %s;" % d for d in decls]
    oe = "end" if ends else "&atomic->llWorldSectorsInAtomic.link"
    ie = "endLight" if ends else "&sector->lightsInWorldSector.link"
    lines += [
        "",
        "    data->lightMask = 0;",
        "    data->ambient.red = 0.0f;",
        "    data->ambient.green = 0.0f;",
        "    data->ambient.blue = 0.0f;",
        "    data->ambient.alpha = 1.0f;",
        "    data->ambientSet = 0;",
        "    data->numLights = 0;",
        "    if (" + cond + " && RWSRCGLOBAL(curWorld) != NULL) {",
        "        fn_804641CC(1, data);",
        "        RWSRCGLOBAL(lightFrame)++;",
    ]
    if ends:
        lines.append("        end = &atomic->llWorldSectorsInAtomic.link;")
    lines += [
        "        for (cur = atomic->llWorldSectorsInAtomic.link.next; cur != " + oe + "; cur = cur->next) {",
        "            sector = ((RpTie *)((u8 *)cur - (u32)&((RpTie *)0)->lWorldSectorInAtomic))->worldSector;",
    ]
    if ends:
        lines.append("            endLight = &sector->lightsInWorldSector.link;")
    lines += [
        "            for (curLight = sector->lightsInWorldSector.link.next; curLight != " + ie + ";",
        "                 curLight = curLight->next) {",
        "                light = ((RpLightTie *)curLight)->light;",
        "                if (light != NULL && light->lightFrame != RWSRCGLOBAL(lightFrame) &&",
        "                    (light->object.object.flags & rpLIGHTLIGHTATOMICS)) {",
        "                    light->lightFrame = RWSRCGLOBAL(lightFrame);",
        "                    ltm = RwFrameGetLTM((RwFrame *)light->object.object.parent);",
        "                    sphere = fn_804566D0(atomic);",
        "                    d.x = sphere->center.x - ltm->pos.x;",
        "                    d.y = sphere->center.y - ltm->pos.y;",
        "                    d.z = sphere->center.z - ltm->pos.z;",
        "                    r = sphere->radius + light->radius;",
        "                    if (d.x * d.x + d.y * d.y + d.z * d.z < r * r) {",
        "                        fn_804643E8(((RpLightTie *)curLight)->light, data);",
        "                    }",
        "                }",
        "            }",
        "        }",
        "    }",
    ]
    return "\n" + "\n".join(lines) + "\n" + CLAMP


BASE = ["RwLLLink *cur", "RwLLLink *end", "RpWorldSector *sector", "RwLLLink *curLight",
        "RwLLLink *endLight", "RpLight *light", "RwMatrix *ltm", "RwSphere *sphere", "RwV3d d", "f32 r"]
NOEND = [x for x in BASE if "end" not in x.lower()]
LTMF = ["RwMatrix *ltm", "RwLLLink *cur", "RwLLLink *curLight", "RpLight *light",
        "RpWorldSector *sector", "RwSphere *sphere", "RwV3d d", "f32 r"]
LTME = ["RwMatrix *ltm", "RwLLLink *endLight", "RwLLLink *cur", "RwLLLink *end", "RwLLLink *curLight",
        "RpLight *light", "RpWorldSector *sector", "RwSphere *sphere", "RwV3d d", "f32 r"]
V = {
    "g_base": (BASE, True, 0),
    "g_noend": (NOEND, False, 0),
    "g_noend_f1": (NOEND, False, 1),
    "g_noend_f2": (NOEND, False, 2),
    "g_base_f1": (BASE, True, 1),
    "g_ltmf": (LTMF, False, 0),
    "g_ltmf_f1": (LTMF, False, 1),
    "g_ltme": (LTME, True, 0),
    "g_ltme_f1": (LTME, True, 1),
}
BODIES = {"fn_80463428": {k: gen(*v) for k, v in V.items()}}
import itertools
CORE = ["RwLLLink *cur", "RwLLLink *curLight", "RpLight *light", "RwMatrix *ltm"]
REST = ["RpWorldSector *sector", "RwSphere *sphere", "RwV3d d", "f32 r"]
for perm in itertools.permutations(range(4)):
    name = "p" + "".join(str(i) for i in perm)
    BODIES["fn_80463428"][name] = gen([CORE[i] for i in perm] + REST, False, 2)
HEAD = """
    RpAtomic *atomic = (RpAtomic *)object;
    u32 flags = atomic->geometry->flags;

    data->lightMask = 0;
    data->ambient.red = 0.0f;
    data->ambient.green = 0.0f;
    data->ambient.blue = 0.0f;
    data->ambient.alpha = 1.0f;
    data->ambientSet = 0;
    data->numLights = 0;
    if ((flags & rpGEOMETRYLIGHT) && RWSRCGLOBAL(curWorld) != NULL) {
"""
def nested(b1, b2, b3, b4, whiles=True):
    t = HEAD + "".join("        %s;\n" % d for d in b1) + """
        fn_804641CC(1, data);
        RWSRCGLOBAL(lightFrame)++;
        cur = atomic->llWorldSectorsInAtomic.link.next;
        end = &atomic->llWorldSectorsInAtomic.link;
        while (cur != end) {
""" + "".join("            %s;\n" % d for d in b2) + """
            sector = ((RpTie *)((u8 *)cur - (u32)&((RpTie *)0)->lWorldSectorInAtomic))->worldSector;
            cur2 = sector->lightsInWorldSector.link.next;
            end2 = &sector->lightsInWorldSector.link;
            while (cur2 != end2) {
""" + "".join("                %s;\n" % d for d in b3) + """
                light = ((RpLightTie *)cur2)->light;
                if (light != NULL && light->lightFrame != RWSRCGLOBAL(lightFrame) &&
                    (light->object.object.flags & rpLIGHTLIGHTATOMICS)) {
""" + "".join("                    %s;\n" % d for d in b4) + """
                    light->lightFrame = RWSRCGLOBAL(lightFrame);
                    ltm = RwFrameGetLTM((RwFrame *)light->object.object.parent);
                    sphere = fn_804566D0(atomic);
                    d.x = sphere->center.x - ltm->pos.x;
                    d.y = sphere->center.y - ltm->pos.y;
                    d.z = sphere->center.z - ltm->pos.z;
                    r = sphere->radius + light->radius;
                    if (d.x * d.x + d.y * d.y + d.z * d.z < r * r) {
                        fn_804643E8(((RpLightTie *)cur2)->light, data);
                    }
                }
                cur2 = cur2->next;
            }
            cur = cur->next;
        }
    }
""" + CLAMP
    return t
B4 = ["RwMatrix *ltm", "RwSphere *sphere", "RwV3d d", "f32 r"]
BODIES["fn_80463428"].update({
    "n1": nested(["RwLLLink *cur", "RwLLLink *end"], ["RpWorldSector *sector", "RwLLLink *cur2", "RwLLLink *end2"], ["RpLight *light"], B4),
    "n2": nested(["RwLLLink *end", "RwLLLink *cur"], ["RpWorldSector *sector", "RwLLLink *end2", "RwLLLink *cur2"], ["RpLight *light"], B4),
    "n3": nested(["RwLLLink *cur", "RwLLLink *end"], ["RpWorldSector *sector", "RwLLLink *end2", "RwLLLink *cur2"], ["RpLight *light"], B4),
    "n4": nested(["RwLLLink *end", "RwLLLink *cur"], ["RpWorldSector *sector", "RwLLLink *cur2", "RwLLLink *end2"], ["RpLight *light"], B4),
    "n5": nested(["RwLLLink *cur", "RwLLLink *end"], ["RwLLLink *cur2", "RwLLLink *end2", "RpWorldSector *sector"], ["RpLight *light"], B4),
})
for i1, b1 in enumerate([["RwLLLink *cur", "RwLLLink *end"], ["RwLLLink *end", "RwLLLink *cur"]]):
    for i2, b2 in enumerate(itertools.permutations(["RpWorldSector *sector", "RwLLLink *cur2", "RwLLLink *end2"])):
        BODIES["fn_80463428"]["q%d%d" % (i1, i2)] = nested(b1, list(b2), ["RpLight *light"], B4)
        BODIES["fn_80463428"]["q%d%dL" % (i1, i2)] = nested(b1, list(b2) + ["RpLight *light"], [], B4)
        BODIES["fn_80463428"]["q%d%dT" % (i1, i2)] = nested(b1 + list(b2), [], ["RpLight *light"], B4)
def nested2(b1, b2, b3, b4, outer_var, inner_var):
    t = nested(b1, b2, b3, b4)
    if not outer_var:
        t = t.replace("        end = &atomic->llWorldSectorsInAtomic.link;\n", "").replace("while (cur != end)", "while (cur != &atomic->llWorldSectorsInAtomic.link)")
    if not inner_var:
        t = t.replace("            end2 = &sector->lightsInWorldSector.link;\n", "").replace("while (cur2 != end2)", "while (cur2 != &sector->lightsInWorldSector.link)")
    return t
for i1, b1 in enumerate([["RwLLLink *cur", "RwLLLink *end"], ["RwLLLink *end", "RwLLLink *cur"], ["RwLLLink *cur"]]):
    for i2, b2 in enumerate([["RpWorldSector *sector", "RwLLLink *cur2", "RwLLLink *end2"], ["RpWorldSector *sector", "RwLLLink *end2", "RwLLLink *cur2"], ["RpWorldSector *sector", "RwLLLink *cur2"]]):
        ov = "end" in " ".join(b1)
        iv = "end2" in " ".join(b2)
        BODIES["fn_80463428"]["r%d%d" % (i1, i2)] = nested2(b1, b2, ["RpLight *light"], B4, ov, iv)
