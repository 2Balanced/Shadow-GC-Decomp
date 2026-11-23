"""Wave 40 (r7): fn_8049BC4C (pipeline node set-up): address forms of the output and sort
data in the node block, and the output loop."""
NL = "\n"
SYM = "fn_8049BC4C"
SYMBOLS = {SYM: "RwBool fn_8049BC4C(RwDlPipeline *pipeline, RwDlPipelineNode *node, RwDlNodeDefinition *nodeDef) {"}
HEAD = """    RwBool ok = 1;
    RwUInt32 numOutputs = nodeDef->numOutputs;
    memset(node, 0, sizeof(RwDlPipelineNode));
    if (numOutputs > 32) {
        RW_ERROR(0x29);
        ok = 0;
    }
    if (nodeDef->numClustersOfInterest > 32) {
        RW_ERROR(0x28);
        ok = 0;
    }
    if (numOutputs >= RW_PIPE_GLOBAL(RwUInt32, 0x38)) {
        RW_ERROR(0x2A);
        ok = 0;
    }
    if (ok) {
"""
TAIL = """        sort->word[0] = 0;
        sort->word[1] = 0;
        sort->word[2] = 0;
        node->topSortData = sort;
        node->initializationData = NULL;
        node->initializationDataSize = 0;
        node->nodeDef = nodeDef;
        pipeline->numNodes++;
    }
    return ok;
"""
MAX = "RW_PIPE_GLOBAL(RwUInt32, 0x38)"
OUT = {
    "o1": "        outputs = (RwUInt32 *)(pipeline->nodes + " + MAX + ") + pipeline->numNodes * 32;" + NL,
    "o2": "        outputs = &((RwUInt32 *)(pipeline->nodes + " + MAX + "))[pipeline->numNodes * 32];" + NL,
    "o3": "        outputs = (RwUInt32 *)((u8 *)(pipeline->nodes + " + MAX + ") + pipeline->numNodes * 128);" + NL,
}
SORT = {
    "s1": "        sort = (RwDlTopSortData *)((RwUInt32 *)(pipeline->nodes + " + MAX + ") + " + MAX + " * 32) + pipeline->numNodes;" + NL,
    "s2": "        sort = &((RwDlTopSortData *)((RwUInt32 *)(pipeline->nodes + " + MAX + ") + " + MAX + " * 32))[pipeline->numNodes];" + NL,
}
LOOP = {
    "l1": "        node->outputs = outputs;" + NL + "        node->numOutputs = numOutputs;" + NL + "        for (i = 0; i < node->numOutputs; i++) outputs[i] = (RwUInt32)-1;" + NL,
    "l2": "        node->outputs = outputs;" + NL + "        node->numOutputs = numOutputs;" + NL + "        for (i = 0; i < node->numOutputs; i++) *outputs++ = (RwUInt32)-1;" + NL,
}
BODIES = {SYM: {}}
for on, o in OUT.items():
    for sn, sv in SORT.items():
        for ln, l in LOOP.items():
            BODIES[SYM][on + sn + ln] = HEAD + "        RwUInt32 i;" + NL + "        RwUInt32 *outputs;" + NL + "        RwDlTopSortData *sort;" + NL + o + l + sv + TAIL
XTRA = {SYM: "static inline RwUInt32 *RwDlNodeBlockEnd(RwDlPipeline *pipeline) {" + NL +
        "    return (RwUInt32 *)(pipeline->nodes + " + MAX + ");" + NL + "}" + NL + NL}
for ln, l in LOOP.items():
    BODIES[SYM]["inl_" + ln] = (HEAD + "        RwUInt32 i;" + NL + "        RwUInt32 *outputs;" + NL + "        RwDlTopSortData *sort;" + NL +
        "        outputs = RwDlNodeBlockEnd(pipeline) + pipeline->numNodes * 32;" + NL + l +
        "        sort = (RwDlTopSortData *)(RwDlNodeBlockEnd(pipeline) + " + MAX + " * 32) + pipeline->numNodes;" + NL + TAIL)
