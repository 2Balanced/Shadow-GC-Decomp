"""Wave 40 (r7): RxPipelineExecute (fn_8049B674): retail has the context in r31, the node
parameters in r30, data in r29 and the pipeline in r28."""
import itertools
NL = "\n"
SYM = "fn_8049B674"
SYMBOLS = {SYM: "RwDlPipeline *fn_8049B674(RwDlPipeline *pipeline, void *data, RwBool heapReset) {"}
HEAP = "    if (heapReset && *(RwInt32 *)((u8 *)lbl_805F169C + 0x18) != 0) fn_8049ECFC(lbl_805F169C);" + NL
REST = """    pipeline->embeddedPacketState = 0;
    if (((RwDlNodeBody)pipeline->nodes->nodeDef->nodeMethods[0])(pipeline->nodes, params) == NULL) {
        exec->result = 0;
    }
    if (pipeline->embeddedPacketState > 1) {
        pipeline->embeddedPacketState = 2;
        fn_8049B14C(pipeline->embeddedPacket);
    }
    *params = NULL;
    lbl_805E3260.pipeline = NULL;
    exec->heap = NULL;
    if (exec->result) return pipeline;
    return NULL;
"""
ST = {
    "r": "    exec->result = 1;",
    "p": "    exec->pipeline = pipeline;",
    "d": "    exec->dataParam = data;",
    "h": "    exec->heap = lbl_805F169C;",
}
BODIES = {SYM: {}}
for decl_name, decl in (("ep", "    RwDlExecution *exec;" + NL + "    void **params;" + NL),
                        ("pe", "    void **params;" + NL + "    RwDlExecution *exec;" + NL)):
    for order in ("rpdh", "rdph", "dprh", "pdrh", "rhpd"):
        body = decl + HEAP + "    exec = &lbl_805E3260;" + NL + "    params = &exec->dataParam;" + NL
        body += NL.join(ST[c] for c in order) + NL + REST
        BODIES[SYM][decl_name + "_" + order] = body
    for order in ("rpdh", "rdph"):
        body = decl + "    exec = &lbl_805E3260;" + NL + "    params = &exec->dataParam;" + NL + HEAP
        body += NL.join(ST[c] for c in order) + NL + REST
        BODIES[SYM][decl_name + "_early_" + order] = body
