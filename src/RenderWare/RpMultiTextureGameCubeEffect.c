/* RenderWare multi-texture effects, GameCube effect data, 0x804A55CC..0x804A6128: the stream
 * size, write and read of the GameCube effect (resource type 6 of the multi-texture plugin's
 * effect table, registered by fn_804A5F08), its creation and two accessors used by the render file.
 * The file has no data.  Built with cflags_rw_r5 (C, automatic inlining in definition order): the
 * size callback fn_804A55CC is expanded in the write callback after it, while fn_804A60D0 and
 * fn_804A60D8 are calls in RpMultiTextureGameCubeRender.c and fn_804A6C30 / fn_804A5F08 calls in
 * RpMultiTextureGameCube.c, which puts file boundaries at 0x804A6128 and 0x804A6C74.
 * Functions keep their fn_ symbols.
 * Status: fn_804A55CC, fn_804A560C, fn_804A5A94, fn_804A5F08, fn_804A60D0 and fn_804A60D8 match;
 * fn_804A5F48 is near (GC/2.7 reassociates the size sums' +3 onto the products). */
#include <dolphin/types.h>
#include "Game/rw/r8.h"

s32 fn_804A912C(u32 index, void *read, void *write, void *getSize, u32 flags);
void *fn_804A932C(void *object, s32 type);
GcnMatInstance *fn_804A5F48(u32 numTevStages, u32 numTexGens, u32 numC, u32 numIndStages,
                            u32 numE);
void fn_8047D254(void *data, u32 size); /* the stream's 32-bit words to native order */
RwStream *fn_8047CEF0(RwStream *stream, u32 type, u32 size, u32 version, u32 buildNum);
void fn_8047D1C8(void *data, u32 size); /* native 32-bit words to the stream's order */
void fn_8047D158(void *data, u32 size);

#define RWALIGN4(x) (((x) + 3) & ~3)

/* The bytes the effect data takes in a stream. */
u32 fn_804A55CC(const GcnMatInstance *instance) {
    u32 size = instance->body.numC * 0x34 + 0x38;
    size += instance->body.numTexGens * 0x0C;
    size += instance->body.numE * 0x1C;
    size += instance->body.numIndStages << 3;
    size += instance->body.numTevStages * 0x1C;
    return size;
}

/* Stream write of the instance data (resource type 6). */
const GcnMatInstance *fn_804A560C(const GcnMatInstance *instance, RwStream *stream) {
    u8 header[0x38];
    u8 tevBuffer[0x1C];
    u8 texGenBuffer[0xC];
    u8 indBuffer[8];
    u8 cBuffer[4];
    u8 eBuffer[4];
    u32 i;

    if (!fn_8047CEF0(stream, 3, fn_804A55CC(instance), 0x37002, 0x37)) {
        return NULL;
    }
    header[2] = instance->body.unk00;
    header[1] = instance->body.numTevStages;
    header[0] = instance->body.numTexGens;
    header[0x34] = instance->body.numC;
    header[0x35] = instance->body.numE;
    header[0x36] = instance->body.numIndStages;
    header[3] = ((instance->body.unk54 & 0xF) << 4) | (instance->body.unk58 & 0xF);
    memcpy(&header[0x24], instance->body.bytes2E, 0x10);
    memcpy(&header[4], instance->body.bytes0E, 0x20);
    fn_8047D1C8(&header[4], 0x20);
    if (!fn_80480ED0(stream, header, sizeof(header))) return NULL;
    for (i = 0; i < instance->body.numTexGens; i++) {
        const GcnMatTexGen *texGen = &instance->body.texGens[i];
        texGenBuffer[0] = texGen->flags;
        texGenBuffer[1] = texGen->type;
        texGenBuffer[2] = texGen->src;
        texGenBuffer[3] = texGen->unk0C;
        texGenBuffer[4] = texGen->unk10;
        texGenBuffer[5] = 0;
        texGenBuffer[6] = 0;
        texGenBuffer[7] = 0;
        *(u16 *)&texGenBuffer[8] = texGen->unk14;
        *(u16 *)&texGenBuffer[10] = texGen->unk16;
        fn_8047D1C8(&texGenBuffer[8], 4);
        if (!fn_80480ED0(stream, texGenBuffer, sizeof(texGenBuffer))) return NULL;
    }
    for (i = 0; i < instance->body.numTevStages; i++) {
        const GcnMatTevStage *stage = &instance->body.tevStages[i];
        tevBuffer[0] = stage->unk00[0];
        tevBuffer[1] = stage->unk00[1];
        tevBuffer[2] = stage->unk00[2];
        tevBuffer[3] = stage->unk00[3];
        tevBuffer[4] = stage->unk00[4];
        tevBuffer[5] = stage->unk00[5];
        tevBuffer[6] = stage->unk00[6];
        tevBuffer[7] = stage->unk00[7];
        tevBuffer[8] = stage->unk00[8];
        tevBuffer[9] = stage->unk00[9];
        tevBuffer[10] = stage->unk00[10];
        tevBuffer[11] = stage->unk2C;
        tevBuffer[12] = stage->unk30[0];
        tevBuffer[13] = stage->unk30[1];
        tevBuffer[14] = stage->unk30[2];
        tevBuffer[15] = stage->unk30[3];
        tevBuffer[16] = stage->unk40;
        tevBuffer[17] = stage->unk44[0];
        tevBuffer[18] = stage->unk44[1];
        tevBuffer[19] = stage->unk44[2];
        tevBuffer[20] = stage->unk44[3];
        tevBuffer[21] = stage->unk44[4];
        tevBuffer[22] = stage->unk44[5];
        tevBuffer[23] = stage->unk44[6];
        *(u32 *)&tevBuffer[24] = stage->indirect;
        fn_8047D158(&tevBuffer[24], 4);
        if (stage->unk44[4] & 0x100) {
            tevBuffer[23] |= 0x80;
        }
        if (!fn_80480ED0(stream, tevBuffer, sizeof(tevBuffer))) return NULL;
    }
    for (i = 0; i < instance->body.numC; i++) {
        const GcnMatElementC *element = &instance->body.arrayC[i];
        cBuffer[0] = element->unk00[0];
        cBuffer[1] = element->unk00[1];
        cBuffer[2] = element->unk00[2];
        cBuffer[3] = 0;
        if (!fn_80480ED0(stream, cBuffer, sizeof(cBuffer)) ||
            !fn_8047D350(stream, element->matrix, sizeof(element->matrix))) {
            return NULL;
        }
    }
    for (i = 0; i < instance->body.numE; i++) {
        const GcnMatElementE *element = &instance->body.arrayE[i];
        eBuffer[0] = element->unk00[0];
        eBuffer[1] = element->unk00[1];
        eBuffer[2] = element->unk00[2];
        eBuffer[3] = element->unk24;
        if (!fn_80480ED0(stream, eBuffer, sizeof(eBuffer)) ||
            !fn_8047D350(stream, element->values, sizeof(element->values))) {
            return NULL;
        }
    }
    for (i = 0; i < instance->body.numIndStages; i++) {
        const GcnMatIndStage *stage = &instance->body.indStages[i];
        indBuffer[0] = stage->unk00[0];
        indBuffer[1] = stage->unk00[1];
        indBuffer[2] = stage->unk00[2];
        indBuffer[3] = stage->unk00[3];
        indBuffer[4] = stage->unk00[4];
        indBuffer[5] = 0;
        indBuffer[6] = 0;
        indBuffer[7] = 0;
        if (!fn_80480ED0(stream, indBuffer, sizeof(indBuffer))) return NULL;
    }
    return instance;
}

/* Stream read of the instance data (resource type 6). */
GcnMatInstance *fn_804A5A94(RwStream *stream) {
    u8 header[0x38];
    u8 tevBuffer[0x1C];
    u8 texGenBuffer[0xC];
    u8 indBuffer[8];
    u8 cBuffer[4];
    u8 eBuffer[4];
    GcnMatInstance *instance;
    GcnMatInstanceBody *body;
    u32 i;

    if (fn_80480D4C(stream, header, sizeof(header)) != sizeof(header)) return NULL;
    instance = fn_804A5F48(header[1], header[0], header[0x34], header[0x36], header[0x35]);
    if (instance == NULL) return NULL;
    body = (GcnMatInstanceBody *)fn_804A60D0((u8 *)instance);
    body->unk00 = header[2];
    body->unk54 = (header[3] >> 4) & 0xF;
    body->unk58 = header[3] & 0xF;
    memcpy(body->bytes2E, &header[0x24], 0x10);
    fn_8047D254(&header[4], 0x20);
    memcpy(body->bytes0E, &header[4], 0x20);
    for (i = 0; i < body->numTexGens; i++) {
        GcnMatTexGen *texGen = &body->texGens[i];
        if (fn_80480D4C(stream, texGenBuffer, sizeof(texGenBuffer)) != sizeof(texGenBuffer)) {
            RwEngineInstance->memFree(instance);
            return NULL;
        }
        texGen->flags = texGenBuffer[0];
        texGen->type = texGenBuffer[1];
        texGen->src = texGenBuffer[2];
        texGen->unk0C = texGenBuffer[3];
        texGen->unk10 = texGenBuffer[4];
        fn_8047D254(&texGenBuffer[8], 4);
        texGen->unk14 = *(u16 *)&texGenBuffer[8];
        texGen->unk16 = *(u16 *)&texGenBuffer[10];
    }
    for (i = 0; i < body->numTevStages; i++) {
        GcnMatTevStage *stage = &body->tevStages[i];
        if (fn_80480D4C(stream, tevBuffer, sizeof(tevBuffer)) != sizeof(tevBuffer)) {
            RwEngineInstance->memFree(instance);
            return NULL;
        }
        stage->unk00[0] = tevBuffer[0];
        stage->unk00[1] = tevBuffer[1];
        stage->unk00[2] = tevBuffer[2];
        stage->unk00[3] = tevBuffer[3];
        stage->unk00[4] = tevBuffer[4];
        stage->unk00[5] = tevBuffer[5];
        stage->unk00[6] = tevBuffer[6];
        stage->unk00[7] = tevBuffer[7];
        stage->unk00[8] = tevBuffer[8];
        stage->unk00[9] = tevBuffer[9];
        stage->unk00[10] = tevBuffer[10];
        stage->unk2C = tevBuffer[11];
        stage->unk30[0] = tevBuffer[12];
        stage->unk30[1] = tevBuffer[13];
        stage->unk30[2] = tevBuffer[14];
        stage->unk30[3] = tevBuffer[15];
        stage->unk40 = tevBuffer[16];
        stage->unk44[0] = tevBuffer[17];
        stage->unk44[1] = tevBuffer[18];
        stage->unk44[2] = tevBuffer[19];
        stage->unk44[3] = tevBuffer[20];
        stage->unk44[4] = tevBuffer[21];
        stage->unk44[5] = tevBuffer[22];
        stage->unk44[6] = tevBuffer[23];
        fn_8047D254(&tevBuffer[24], 4);
        stage->indirect = *(u32 *)&tevBuffer[24];
        if (stage->unk44[6] & 0x80) {
            stage->unk44[4] |= 0x100;
            stage->unk44[6] &= ~0x80;
        }
    }
    for (i = 0; i < body->numC; i++) {
        GcnMatElementC *element = &body->arrayC[i];
        if (fn_80480D4C(stream, cBuffer, sizeof(cBuffer)) != sizeof(cBuffer) ||
            !fn_8047D784(stream, element->matrix, sizeof(element->matrix))) {
            RwEngineInstance->memFree(instance);
            return NULL;
        }
        element->unk00[0] = cBuffer[0];
        element->unk00[1] = cBuffer[1];
        element->unk00[2] = cBuffer[2];
    }
    for (i = 0; i < body->numE; i++) {
        GcnMatElementE *element = &body->arrayE[i];
        if (fn_80480D4C(stream, eBuffer, sizeof(eBuffer)) != sizeof(eBuffer) ||
            !fn_8047D784(stream, element->values, sizeof(element->values))) {
            RwEngineInstance->memFree(instance);
            return NULL;
        }
        element->unk00[0] = eBuffer[0];
        element->unk00[1] = eBuffer[1];
        element->unk00[2] = eBuffer[2];
        element->unk24 = (s8)eBuffer[3];
    }
    for (i = 0; i < body->numIndStages; i++) {
        GcnMatIndStage *stage = &body->indStages[i];
        if (fn_80480D4C(stream, indBuffer, sizeof(indBuffer)) != sizeof(indBuffer)) {
            RwEngineInstance->memFree(instance);
            return NULL;
        }
        stage->unk00[0] = indBuffer[0];
        stage->unk00[1] = indBuffer[1];
        stage->unk00[2] = indBuffer[2];
        stage->unk00[3] = indBuffer[3];
        stage->unk00[4] = indBuffer[4];
    }
    return instance;
}

s32 fn_804A5F08(void) {
    return fn_804A912C(6, fn_804A5A94, fn_804A560C, fn_804A55CC, 0);
}

GcnMatInstance *fn_804A5F48(u32 numTevStages, u32 numTexGens, u32 numC, u32 numIndStages,
                            u32 numE) {
    u32 size;
    GcnMatInstance *instance;
    u8 *next;

    size = RWALIGN4(sizeof(GcnMatInstance) + numTexGens * sizeof(GcnMatTexGen));
    size = RWALIGN4(size + numTevStages * sizeof(GcnMatTevStage));
    size = RWALIGN4(size + numIndStages * sizeof(GcnMatIndStage));
    size = RWALIGN4(size + numC * sizeof(GcnMatElementC));
    size = RWALIGN4(size + numE * sizeof(GcnMatElementE));
    instance = (GcnMatInstance *)RwEngineInstance->memAlloc(size, 0x30129);
    if (instance == NULL) {
        RWERROR(0x120, (0x80000013, size));
        return NULL;
    }
    memset(instance, 0, size);
    fn_804A932C(instance, 6);
    instance->body.countTevStages = numTevStages;
    instance->body.countTexGens = numTexGens;
    instance->body.countIndStages = numIndStages;
    instance->body.countC = numC;
    instance->body.countE = numE;
    instance->body.numTevStages = instance->body.countTevStages;
    instance->body.numTexGens = instance->body.countTexGens;
    instance->body.numIndStages = instance->body.countIndStages;
    instance->body.numC = instance->body.countC;
    instance->body.numE = instance->body.countE;
    next = (u8 *)(instance + 1);
    if (numC != 0) {
        instance->body.arrayC = (GcnMatElementC *)next;
        next += numC * sizeof(GcnMatElementC);
    }
    if (numTexGens != 0) {
        instance->body.texGens = (GcnMatTexGen *)next;
        next += numTexGens * sizeof(GcnMatTexGen);
    }
    if (numE != 0) {
        instance->body.arrayE = (GcnMatElementE *)next;
        next += numE * sizeof(GcnMatElementE);
    }
    if (numIndStages != 0) {
        instance->body.indStages = (GcnMatIndStage *)next;
        next += numIndStages * sizeof(GcnMatIndStage);
    }
    if (numTevStages != 0) {
        instance->body.tevStages = (GcnMatTevStage *)next;
    }
    return instance;
}

u8 *fn_804A60D0(u8 *object) {
    return object + 0x30;
}

GcnMatSettings *fn_804A60D8(GcnMatSettings *settings, const u32 *packed) {
    u32 bits = *packed;
    settings->field0 = bits & 3;
    settings->field1 = (bits >> 2) & 3;
    settings->field2 = (bits >> 4) & 7;
    settings->field3 = (bits >> 7) & 15;
    settings->field4 = (bits >> 11) & 7;
    settings->field5 = (bits >> 14) & 7;
    settings->field6 = (bits >> 17) & 1;
    settings->field7 = (bits >> 18) & 1;
    settings->field8 = (bits >> 19) & 3;
    return settings;
}
