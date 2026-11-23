/* RenderWare GameCube driver: the immediate mode (RwIm3D) pipelines and their two nodes,
 * 0x8049F8AC..0x804A076C.  The transform pipeline has one node, "ImmInstance.csl", that only
 * remembers the first word of its parameter block; the render pipeline has one node,
 * "SubmitNoLight.csl", installed for all five primitive types.
 * Data: .data 0x8056EC40..0x8056ED00 (the render node's table of vertex-format offsets, then
 * each node's name and definition), .sbss 0x805F16A0 (the word the transform node keeps).
 * The node names are writable strings in .data, so the file is built without readonly strings.
 * Functions keep their fn_ symbols; comments say what they do.
 * Status: everything matches except fn_8049FB18 (the render node body, 3144 bytes), not
 * written yet. */
#include <dolphin/types.h>
#include "Game/rw/r8.h"

RxNodeDefinition *fn_8049FB0C(void);
RxNodeDefinition *fn_804A0760(void);
s32 fn_8049FB18(void *self, const void *params);

/* The offsets the render node's body reads by vertex format. */
static u32 Im3DVertexOffsets[7] = {0, 0xA8, 0xB0, 0x90, 0x98, 0xA0, 0xB8};

/* Creates the Im3D transform pipeline (one node) and makes it current. */
s32 fn_8049F8AC(RxPipeline **pipelines) {
    RxPipeline *pipeline = fn_8049B76C();
    if (pipeline != NULL) {
        RxPipeline *locked;
        pipeline->pluginId = 1;
        locked = fn_8049CCB8(pipeline);
        if (locked != NULL) {
            pipeline = fn_8049C1E0(fn_8049D0D0(locked, NULL, fn_8049FB0C(), NULL));
            if (pipeline != NULL) {
                pipelines[0] = pipeline;
                fn_8049AC90(pipeline);
                return 1;
            }
        }
        fn_8049B800(pipeline);
    }
    return 0;
}

void fn_8049F948(RxPipeline **pipelines) {
    fn_8049AC90(NULL);
    if (pipelines[0] != NULL) {
        fn_8049B800(pipelines[0]);
        pipelines[0] = NULL;
    }
}

/* Drops the Im3D render pipelines: one pipeline shared by the five primitive types. */
void fn_8049F990(RxPipeline **pipelines) {
    fn_8049ACE0(NULL, 3);
    fn_8049ACE0(NULL, 5);
    fn_8049ACE0(NULL, 4);
    fn_8049ACE0(NULL, 1);
    fn_8049ACE0(NULL, 2);
    if (pipelines[0] != NULL) {
        fn_8049B800(pipelines[0]);
    }
    pipelines[0] = NULL;
    pipelines[1] = NULL;
    pipelines[2] = NULL;
    pipelines[3] = NULL;
    pipelines[4] = NULL;
}

/* Creates the Im3D render pipeline and installs it for every primitive type. */
s32 fn_8049FA1C(RxPipeline **pipelines) {
    RxPipeline *pipeline = fn_8049B76C();
    if (pipeline != NULL) {
        RxPipeline *locked;
        pipeline->pluginId = 1;
        locked = fn_8049CCB8(pipeline);
        if (locked != NULL) {
            pipeline = fn_8049C1E0(fn_8049D0D0(locked, NULL, fn_804A0760(), NULL));
            if (pipeline != NULL) {
                pipelines[0] = pipeline;
                pipelines[1] = pipeline;
                pipelines[2] = pipeline;
                pipelines[3] = pipeline;
                pipelines[4] = pipeline;
                fn_8049ACE0(pipeline, 3);
                fn_8049ACE0(pipeline, 5);
                fn_8049ACE0(pipeline, 4);
                fn_8049ACE0(pipeline, 1);
                fn_8049ACE0(pipeline, 2);
                return 1;
            }
        }
        fn_8049B800(pipeline);
    }
    return 0;
}

/* The word the transform node keeps from its parameter block; the render node reads it. */
static u32 Im3DTransformParam;

/* Transform node: remembers the word its parameter block starts with. */
s32 fn_8049FAFC(void *self, const u32 *params) {
    Im3DTransformParam = params[0];
    return 1;
}

static RxNodeDefinition ImmInstanceNode = {"ImmInstance.csl", {(void *)fn_8049FAFC}};

RxNodeDefinition *fn_8049FB0C(void) {
    return &ImmInstanceNode;
}

/* fn_8049FB18 (0xC48 bytes), the body of the Im3D render node, is not decompiled yet. */

static RxNodeDefinition SubmitNoLightNode = {"SubmitNoLight.csl", {(void *)fn_8049FB18}};

RxNodeDefinition *fn_804A0760(void) {
    return &SubmitNoLightNode;
}
