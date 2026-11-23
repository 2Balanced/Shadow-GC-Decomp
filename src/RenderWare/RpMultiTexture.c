/* RenderWare multi-texture plugin (plugin id 0x12C), 0x804A869C..0x804A9BF8: the engine plugin
 * (open/close of the effect dictionaries), the per-platform material extension (constructor,
 * destructor, copy and stream callbacks, registered for a platform by fn_804A8EFC), the MTData
 * accessors, the table of effect types (fn_804A912C registers a type's stream callbacks) and the
 * effects and effect dictionaries themselves.
 * Data: .bss 0x805E32D8 (one MTPlatformInfo per platform, then the effect type table), .sdata
 * 0x805EDF68 (the plugin's module record) and .sbss 0x805F16D0 (the default dictionary).
 * Built with cflags_rw_r5 (C, automatic inlining in definition order): every call here goes to a
 * function defined later except fn_804A9674's, which expands fn_804A9584 (and, through it,
 * itself) as retail does.  Functions keep their fn_ symbols; comments say what they do.
 * Status: every function matches except fn_804A87D8 and fn_804A8B08 (the registers of MTDataCreate's
 * definition, count and size are permuted where it is expanded) and fn_804A9914 (registers of the
 * expanded dictionary search). */
#include <dolphin/types.h>
#include "Game/rw/r8.h"

/* A link of RenderWare's linked lists. */
typedef struct MTLink {
    struct MTLink *next;
    struct MTLink *prev;
} MTLink;

/* An effect: a type (the index of its entry in the effect type table), a reference count, a name
 * and its link in a dictionary; the type's data follows (GcnMatInstance for type 6). */
typedef struct MTEffect {
    s32 type;      /* 0x00 */
    u32 refCount;  /* 0x04 */
    char name[32]; /* 0x08 */
    MTLink link;   /* 0x28 */
} MTEffect;

#define MTEFFECTFROMLINK(l) ((MTEffect *)((u8 *)(l) - 0x28))

/* A dictionary of effects; dictionaries are linked into the plugin's globals through `link`. */
typedef struct MTEffectDict {
    MTLink effects; /* 0x00 */
    MTLink link;    /* 0x08 */
} MTEffectDict;

#define MTDICTFROMLINK(l) ((MTEffectDict *)((u8 *)(l) - 8))

/* The plugin's engine globals. */
typedef struct MTGlobals {
    MTLink dicts;             /* 0x00 */
    MTEffectDict *current;    /* 0x08 */
    u32 pathSize;             /* 0x0C */
    char *buffer;             /* 0x10: 0x220 bytes */
    char *path;               /* 0x14: buffer + 0x100 */
} MTGlobals;

/* One entry of the effect type table. */
typedef struct MTEffectType {
    u32 type;                                                    /* 0x00 */
    void (*destroy)(MTEffect *effect);                           /* 0x04 */
    MTEffect *(*read)(RwStream *stream, u32 type, u32 version, u32 length); /* 0x08 */
    void *write;                                                 /* 0x0C */
    void *getSize;                                               /* 0x10 */
} MTEffectType;

typedef struct MTModule {
    s32 globalsOffset;
    s32 numInstances;
} MTModule;

/* The name of a stream chunk header (RwStreamReadChunkHeaderInfo fills it in). */
typedef struct MTChunkHeader {
    u8 data[0x18];
} MTChunkHeader;

s32 fn_80485D6C(void); /* RwEngineGetVersion */
RwStream *fn_804A0C50(RwStream *stream, const RwTexture *texture);
RwStream *fn_804A0CDC(RwStream *stream, RwTexture **texture);
s32 fn_804A0DB8(const RwTexture *texture);
s32 fn_80482F64(const void *object);
const void *fn_80482FA4(const void *object, RwStream *stream);
RwStream *fn_80481368(s32 type, s32 access, const void *data); /* RwStreamOpen */
s32 fn_8048124C(RwStream *stream, void *data);                 /* RwStreamClose */
void fn_8047D254(void *data, u32 size);
u32 fn_80483060(void *header, RwStream *stream);

s32 fn_804A9158(void);
s32 fn_804A9274(void);
void fn_804A9674(MTEffect *effect);
MTData *fn_804A8FCC(MTData *data, MTEffect *effect);
MTEffect *fn_804A9028(MTData *data);
MTData *fn_804A9030(MTData *data, s32 index, RwTexture *texture);
RwTexture *fn_804A90A0(MTData *data, s32 index);
MTData *fn_804A90B0(MTData *data, s32 index, u8 coords);
u8 fn_804A90BC(MTData *data, s32 index);
s32 fn_804A90F8(void);
MTEffectDict *fn_804A93B0(void);
void fn_804A9468(MTEffectDict *dict);
MTEffectDict *fn_804A9500(MTEffectDict *dict, MTEffect *effect);
MTEffect *fn_804A9584(MTEffect *effect);
MTEffect *fn_804A95D4(void);
MTEffect *fn_804A9914(const char *name);
MTEffect *fn_804A9BA4(MTEffect *effect, const char *name);
void fn_804A9BE8(MTEffect *effect);

/* The registered platforms (indexed by platform id) and effect types (indexed by type). */
static MTPlatformInfo MTPlatforms[10];
static MTEffectType MTEffectTypes[10];

static MTModule MTModuleInfo = {0, 0};

/* The dictionary the engine open creates. */
static MTEffectDict *MTDefaultDict;

#define MTGLOBALS() ((MTGlobals *)((u8 *)RwEngineInstance + MTModuleInfo.globalsOffset))
#define MTDATA(object, offset) (*(MTData **)((u8 *)(object) + (offset)))

/* Engine open callback. */
void *fn_804A869C(void *instance, s32 offset, s32 size) {
    MTModuleInfo.numInstances++;
    fn_804A9158();
    return instance;
}

/* Engine close callback. */
void *fn_804A86DC(void *instance, s32 offset, s32 size) {
    fn_804A9274();
    MTModuleInfo.numInstances--;
    return instance;
}

/* Constructor of the material extension. */
void *fn_804A871C(void *object, s32 offset, s32 size) {
    MTDATA(object, offset) = NULL;
    return object;
}

/* Frees an MTData with its textures and effect. */
static inline void MTDataDestroy(MTData *data) {
    u32 i;
    for (i = 0; i < data->numTextures; i++) {
        if (data->textures[i] != NULL) {
            fn_8048CD8C(data->textures[i]);
            data->textures[i] = NULL;
        }
    }
    if (data->effect != NULL) {
        fn_804A9674((MTEffect *)data->effect);
        data->effect = NULL;
    }
    RwEngineInstance->memFree(data);
}

/* Destructor of the material extension. */
void *fn_804A8728(void *object, s32 offset, s32 size) {
    MTData *data = MTDATA(object, offset);

    if (data != NULL) {
        MTDataDestroy(data);
        MTDATA(object, offset) = NULL;
    }
    return object;
}

/* Allocates an MTData for a platform with room for the platform's extra data. */
static inline MTData *MTDataCreate(MTPlatformInfo *def, u32 numTextures) {
    u32 size = def->extraSize + sizeof(MTData);
    MTData *data = (MTData *)RwEngineInstance->memAlloc(size, 0x103012C);
    if (data == NULL) {
        RWERROR(0x120, (0x80000013, size));
        return NULL;
    }
    memset(data, 0, size);
    data->def = def;
    data->numTextures = numTextures;
    if (def->extraSize != 0) {
        data->extra = data + 1;
    }
    return data;
}

/* Copy callback of the material extension. */
void *fn_804A87D8(void *dstObject, const void *srcObject, s32 offset, s32 size) {
    MTData *src = MTDATA(srcObject, offset);
    MTData *dst;
    u32 i;

    if (src == NULL) return dstObject;
    dst = MTDataCreate(src->def, src->numTextures);
    if (dst == NULL) return NULL;
    MTDATA(dstObject, offset) = dst;
    for (i = 0; i < src->numTextures; i++) {
        fn_804A9030(dst, i, fn_804A90A0(src, i));
        fn_804A90B0(dst, i, fn_804A90BC(src, i));
    }
    fn_804A8FCC(dst, fn_804A9028(src));
    return dstObject;
}

/* Stream size callback of the material extension. */
s32 fn_804A8924(const void *object, s32 offset, s32 size) {
    MTData *data = MTDATA(object, offset);
    s32 total = 0;

    if (data != NULL) {
        u32 i;
        total = data->numTextures + 8;
        for (i = 0; i < data->numTextures; i++) {
            total += fn_804A0DB8(data->textures[i]);
        }
        if (data->effect != NULL) {
            total += fn_80482F64(data->effect + 8) + 12;
        }
    }
    return total;
}

/* Stream write callback of the material extension. */
RwStream *fn_804A89C4(RwStream *stream, s32 length, const void *object, s32 offset, s32 size) {
    MTData *data = MTDATA(object, offset);

    if (data != NULL) {
        s32 version = fn_80485D6C();
        u8 header[4];

        if (!fn_8047D458(stream, &version, sizeof(version))) return NULL;
        header[0] = data->def->platform;
        header[1] = data->numTextures;
        header[2] = (data->effect != NULL);
        header[3] = 0;
        if (!fn_80480ED0(stream, header, sizeof(header))) return NULL;
        if (data->numTextures != 0) {
            u32 i;
            if (!fn_80480ED0(stream, data->textureCoords, data->numTextures)) return NULL;
            for (i = 0; i < data->numTextures; i++) {
                fn_804A0C50(stream, data->textures[i]);
            }
        }
        if (data->effect != NULL) {
            if (!fn_80482FA4(data->effect + 8, stream)) return NULL;
        }
    }
    return stream;
}

/* Stream read callback of the material extension. */
RwStream *fn_804A8B08(RwStream *stream, s32 length, void *object, s32 offset, s32 size) {
    char name[0x28];
    s32 version;
    u8 header[4];
    MTData *data;

    if (!fn_8047D868(stream, &version, sizeof(version))) return NULL;
    if (!fn_80480D4C(stream, header, sizeof(header))) return NULL;
    data = MTDataCreate(&MTPlatforms[header[0]], header[1]);
    if (data == NULL) return NULL;
    if (header[1] != 0) {
        u32 i;
        if (!fn_80480D4C(stream, data->textureCoords, data->numTextures)) {
            MTDataDestroy(data);
            return NULL;
        }
        for (i = 0; i < data->numTextures; i++) {
            if (!fn_804A0CDC(stream, &data->textures[i])) {
                MTDataDestroy(data);
                return NULL;
            }
        }
    }
    if (header[2] & 1) {
        MTEffect *effect;
        if (!fn_80483060(name, stream)) {
            MTDataDestroy(data);
            return NULL;
        }
        effect = fn_804A9914(name);
        if (effect == NULL) {
            effect = fn_804A95D4();
            if (effect == NULL) {
                MTDataDestroy(data);
                return NULL;
            }
            fn_804A9BA4(effect, name);
        }
        fn_804A8FCC(data, effect);
        fn_804A9674(effect);
    }
    MTDATA(object, offset) = data;
    return stream;
}

/* Attaches the plugin: the effect type table, the platform table and the engine plugin. */
s32 fn_804A8E90(void) {
    s32 offset;
    s32 result;

    if (!fn_804A90F8()) return FALSE;
    memset(MTPlatforms, 0, sizeof(MTPlatforms));
    offset = fn_80485D78(sizeof(MTGlobals), 0x12C, fn_804A869C, fn_804A86DC);
    result = (offset < 0) ? FALSE : TRUE;
    MTModuleInfo.globalsOffset = offset;
    return result;
}

/* Registers a platform's material extension: its plugin id and the size of its extra data. */
s32 fn_804A8EFC(s32 platform, u32 pluginId, u32 extraSize) {
    s32 offset = fn_8045B2CC(sizeof(MTData *), pluginId, fn_804A871C, fn_804A8728, fn_804A87D8);
    MTPlatformInfo *info;

    if (offset < 0) return FALSE;
    info = &MTPlatforms[platform];
    info->offset = offset;
    if (fn_8045B30C(pluginId, fn_804A8B08, fn_804A89C4, fn_804A8924) < 0) return FALSE;
    info->platform = platform;
    info->pluginId = pluginId;
    info->extraSize = extraSize;
    return TRUE;
}

/* Sets the effect of an MTData. */
MTData *fn_804A8FCC(MTData *data, MTEffect *effect) {
    if (data->effect != NULL) {
        fn_804A9674((MTEffect *)data->effect);
    }
    data->effect = (u8 *)effect;
    if (effect != NULL) {
        fn_804A9BE8((MTEffect *)data->effect);
    }
    return data;
}

MTEffect *fn_804A9028(MTData *data) {
    return (MTEffect *)data->effect;
}

/* Sets texture `index` of an MTData. */
MTData *fn_804A9030(MTData *data, s32 index, RwTexture *texture) {
    if (data->textures[index] != NULL) {
        fn_8048CD8C(data->textures[index]);
    }
    data->textures[index] = texture;
    if (texture != NULL) {
        texture->refCount++;
    }
    return data;
}

RwTexture *fn_804A90A0(MTData *data, s32 index) {
    return data->textures[index];
}

MTData *fn_804A90B0(MTData *data, s32 index, u8 coords) {
    data->textureCoords[index] = coords;
    return data;
}

u8 fn_804A90BC(MTData *data, s32 index) {
    return data->textureCoords[index];
}

/* A material's MTData for a platform, or NULL when the platform has not registered. */
void *fn_804A90C8(void *object, s32 platform) {
    MTPlatformInfo *info = &MTPlatforms[platform];
    if (info->pluginId != 0) {
        return MTDATA(object, info->offset);
    }
    return NULL;
}

s32 fn_804A90F8(void) {
    memset(MTEffectTypes, 0, sizeof(MTEffectTypes));
    return TRUE;
}

/* Registers an effect type's stream callbacks and destructor. */
s32 fn_804A912C(u32 type, void *read, void *write, void *getSize, void *destroy) {
    MTEffectType *entry = &MTEffectTypes[type];
    entry->type = type;
    entry->read = read;
    entry->write = write;
    entry->getSize = getSize;
    entry->destroy = destroy;
    return TRUE;
}

#define MTGLOBALSBASE() ((u8 *)RwEngineInstance + MTModuleInfo.globalsOffset)

/* Engine open: the dictionary list, the default dictionary and the path buffers. */
s32 fn_804A9158(void) {
    MTEffectDict *dict;
    char *buffer;

    *(MTLink **)(MTGLOBALSBASE()) = (MTLink *)(MTGLOBALSBASE());
    *(MTLink **)(MTGLOBALSBASE() + 4) = (MTLink *)(MTGLOBALSBASE());
    dict = fn_804A93B0();
    MTDefaultDict = dict;
    if (dict == NULL) return FALSE;
    *(MTEffectDict **)(MTGLOBALSBASE() + 8) = dict;
    buffer = (char *)RwEngineInstance->memAlloc(0x220, 0x4012C);
    if (buffer == NULL) {
        fn_804A9468(*(MTEffectDict **)(MTGLOBALSBASE() + 8));
        RWERROR(0x120, (0x80000013, 0x220));
        return FALSE;
    }
    memset(buffer, 0, 0x220);
    *(char **)(MTGLOBALSBASE() + 0x10) = buffer;
    *(char **)(MTGLOBALSBASE() + 0x14) = buffer + 0x100;
    *(u32 *)(MTGLOBALSBASE() + 0xC) = 0x100;
    return TRUE;
}

/* Engine close: the path buffers and the default dictionary. */
s32 fn_804A9274(void) {
    MTLink *cur;
    MTLink *list;
    MTEffectDict *target;

    if (*(char **)(MTGLOBALSBASE() + 0x10) != NULL) {
        RwEngineInstance->memFree(*(char **)(MTGLOBALSBASE() + 0x10));
        *(char **)(MTGLOBALSBASE() + 0x10) = NULL;
        *(char **)(MTGLOBALSBASE() + 0x14) = NULL;
        *(u32 *)(MTGLOBALSBASE() + 0xC) = 0;
    }
    list = (MTLink *)MTGLOBALSBASE();
    target = MTDefaultDict;
    cur = list->next;
    while (cur != list) {
        if (MTDICTFROMLINK(cur) == target) {
            fn_804A9468(target);
            MTDefaultDict = NULL;
            break;
        }
        cur = cur->next;
    }
    return TRUE;
}

/* Sets up an effect of a type; one of a real type joins the current dictionary. */
MTEffect *fn_804A932C(MTEffect *effect, s32 type) {
    memset(effect, 0, sizeof(MTEffect));
    effect->type = type;
    effect->refCount = 1;
    effect->link.prev = NULL;
    effect->link.next = NULL;
    if (type != 0) {
        MTEffectDict *dict = *(MTEffectDict **)(MTGLOBALSBASE() + 8);
        if (dict != NULL) {
            fn_804A9500(dict, effect);
        }
    }
    return effect;
}

/* Creates an effect dictionary and links it into the plugin's list. */
MTEffectDict *fn_804A93B0(void) {
    MTEffectDict *dict = (MTEffectDict *)RwEngineInstance->memAlloc(sizeof(MTEffectDict), 0x3012C);
    if (dict == NULL) {
        RWERROR(0x120, (0x80000013, sizeof(MTEffectDict)));
        return NULL;
    }
    dict->effects.next = &dict->effects;
    dict->effects.prev = &dict->effects;
    dict->link.next = *(MTLink **)(MTGLOBALSBASE());
    dict->link.prev = (MTLink *)(MTGLOBALSBASE());
    (*(MTLink **)(MTGLOBALSBASE()))->prev = &dict->link;
    *(MTLink **)(MTGLOBALSBASE()) = &dict->link;
    return dict;
}

/* Destroys an effect dictionary: its effects leave it, and it leaves the plugin's list. */
void fn_804A9468(MTEffectDict *dict) {
    MTLink *cur;

    if (dict == *(MTEffectDict **)(MTGLOBALSBASE() + 8)) {
        *(MTEffectDict **)(MTGLOBALSBASE() + 8) = NULL;
    }
    cur = dict->effects.next;
    while (cur != &dict->effects) {
        MTEffect *effect = MTEFFECTFROMLINK(cur);
        cur = cur->next;
        fn_804A9584(effect);
    }
    dict->link.prev->next = dict->link.next;
    dict->link.next->prev = dict->link.prev;
    RwEngineInstance->memFree(dict);
}

/* Adds an effect to a dictionary (taking it out of the one it was in). */
MTEffectDict *fn_804A9500(MTEffectDict *dict, MTEffect *effect) {
    if (effect->link.next != NULL) {
        effect->link.prev->next = effect->link.next;
        effect->link.next->prev = effect->link.prev;
        fn_804A9674(effect);
    }
    effect->link.next = dict->effects.next;
    effect->link.prev = &dict->effects;
    dict->effects.next->prev = &effect->link;
    dict->effects.next = &effect->link;
    fn_804A9BE8(effect);
    return dict;
}

/* Takes an effect out of its dictionary, dropping the dictionary's reference. */
MTEffect *fn_804A9584(MTEffect *effect) {
    if (effect->link.next != NULL) {
        effect->link.prev->next = effect->link.next;
        effect->link.next->prev = effect->link.prev;
        fn_804A9674(effect);
    }
    return effect;
}

/* Creates an effect of no type. */
MTEffect *fn_804A95D4(void) {
    MTEffect *effect = (MTEffect *)RwEngineInstance->memAlloc(sizeof(MTEffect), 0x3012C);
    if (effect == NULL) {
        RWERROR(0x120, (0x80000013, sizeof(MTEffect)));
        return NULL;
    }
    return fn_804A932C(effect, 0);
}

/* Drops a reference to an effect; the last one takes it out of its dictionary and destroys it
 * through its type's destructor, or frees it. */
void fn_804A9674(MTEffect *effect) {
    effect->refCount--;
    if (effect->refCount == 0) {
        fn_804A9584(effect);
        if (effect->type != 0) {
            MTEffectType *entry = &MTEffectTypes[(u32)effect->type];
            if (entry->destroy != NULL) {
                entry->destroy(effect);
                return;
            }
        }
        RwEngineInstance->memFree(effect);
    }
}

/* The effect of a dictionary with a name, or NULL. */
static inline MTEffect *MTEffectDictFindNamed(MTEffectDict *dict, const char *name) {
    MTLink *cur = dict->effects.next;
    while (cur != &dict->effects) {
        MTEffect *effect = MTEFFECTFROMLINK(cur);
        if (RwEngineInstance->strcmp(effect->name, name) == 0) return effect;
        cur = cur->next;
    }
    return NULL;
}

/* Finds an effect by name in the current dictionary (or in every one when there is none),
 * otherwise reads it from the file of that name in the effect path. */
MTEffect *fn_804A9914(const char *name) {
    u32 length;
    u32 version;
    u32 type;
    MTChunkHeader header;
    MTEffect *effect = NULL;
    u8 *globals = MTGLOBALSBASE();
    MTEffectDict *current = *(MTEffectDict **)(globals + 8);
    RwStream *stream;
    MTEffectType *entry;

    if (current != NULL) {
        effect = MTEffectDictFindNamed(current, name);
    } else {
        MTLink *cur = ((MTLink *)globals)->next;
        while (cur != (MTLink *)globals) {
            effect = MTEffectDictFindNamed(MTDICTFROMLINK(cur), name);
            if (effect != NULL) break;
            cur = cur->next;
        }
    }
    if (effect != NULL) {
        fn_804A9BE8(effect);
        return effect;
    }
    {
        char *path = *(char **)(MTGLOBALSBASE() + 0x14);
        RwEngineInstance->strcpy(path, *(char **)(MTGLOBALSBASE() + 0x10));
        RwEngineInstance->strncat(path, name, 31);
        stream = fn_80481368(2, 1, path);
    }
    if (stream == NULL) return NULL;
    if (!fn_8047CF50(stream, 0x20, NULL, NULL)) {
        fn_8048124C(stream, NULL);
        return NULL;
    }
    if (!fn_8047CF50(stream, 1, NULL, NULL) || !fn_80480D4C(stream, &type, sizeof(type))) {
        effect = NULL;
    } else {
        fn_8047D254(&type, sizeof(type));
        entry = &MTEffectTypes[type];
        if (entry->read == NULL) {
            effect = NULL;
        } else if (!fn_80483060(&header, stream)) {
            effect = NULL;
        } else if (!fn_8047CF50(stream, 3, &length, &version)) {
            effect = NULL;
        } else {
            effect = entry->read(stream, type, version, length);
            if (effect == NULL) {
                effect = NULL;
            } else {
                fn_804A9BA4(effect, (const char *)&header);
            }
        }
    }
    fn_8048124C(stream, NULL);
    return effect;
}

/* Names an effect. */
MTEffect *fn_804A9BA4(MTEffect *effect, const char *name) {
    RwEngineInstance->strncpy(effect->name, name, 31);
    return effect;
}

/* Adds a reference to an effect. */
void fn_804A9BE8(MTEffect *effect) {
    effect->refCount++;
}
