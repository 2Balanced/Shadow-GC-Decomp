/* RenderWare dictionary toolkit (RtDict), 0x80479040..0x80479598: dictionary schemas, finding a
 * named entry, destroying a dictionary and reading one from a stream. No data of its own.
 *
 * Written from the retail code; the fn_ symbols are kept because game code calls them by address,
 * and each function says which public RenderWare API it is. C, built with cflags_rw_r5 (-O4,p, no
 * stmw/lmw, automatic inlining in definition order). */
#include "Game/rw/r5.h"

void *fn_8047E0E0(RwSList *list);              /* the first entry */
void *fn_8047E0E8(RwSList *list);              /* past the last entry */
RwSList *fn_8047DF9C(RwSList *list, s32 count); /* drops the last `count` entries */

/* RtDictSchemaInit. */
RtDictSchema *fn_80479040(RtDictSchema *schema) {
    schema->dictionaries = fn_8047DC90(sizeof(RtDict *), schema->dictChunkType | 0x40000);
    if (schema->dictionaries == 0) {
        return 0;
    }
    schema->current = 0;
    return schema;
}

/* RtDictSchemaDestruct. */
RwBool fn_8047909C(RtDictSchema *schema) {
    fn_8047DFC8(schema->dictionaries);
    return 1;
}

/* RtDictSchemaGetCurrentDict. */
RtDict *fn_804790C4(RtDictSchema *schema) {
    return schema->current;
}

/* RtDictSchemaSetCurrentDict. */
RtDictSchema *fn_804790CC(RtDictSchema *schema, RtDict *dict) {
    schema->current = dict;
    return schema;
}

/* RtDictFindNamedEntry. */
void *fn_804790D4(RtDict *dict, const char *name) {
    void **entry;
    void *e;
    void **end;
    const char *(*getName)(void *entry);

    getName = dict->schema->getNameCB;
    end = (void **)fn_8047E0E8(dict->entries);
    for (entry = (void **)fn_8047E0E0(dict->entries); entry != end; entry++) {
        e = *entry;
        if (RWSRCGLOBAL(vecStrcmp)(getName(e), name) == 0) {
            return e;
        }
    }
    return 0;
}

/* Takes a dictionary off the schema's list (only inlined in retail). */
static RtDictSchema *SchemaRemoveDict(RtDictSchema *schema, RtDict *dict) {
    RtDict **entry = (RtDict **)fn_8047E0E0(schema->dictionaries);
    RtDict **end = (RtDict **)fn_8047E0E8(schema->dictionaries);

    for (; entry != end; entry++) {
        if (dict == *entry) {
            *entry = *(RtDict **)fn_8047E0CC(schema->dictionaries, fn_8047E03C(schema->dictionaries) - 1);
            fn_8047DF9C(schema->dictionaries, 1);
            break;
        }
    }
    return schema;
}

/* RtDictDestroy. */
RwBool fn_80479174(RtDict *dict) {
    if (dict == dict->schema->current) {
        dict->schema->current = 0;
    }
    SchemaRemoveDict(dict->schema, dict);
    {
        void **end = (void **)fn_8047E0E8(dict->entries);
        void **entry = (void **)fn_8047E0E0(dict->entries);

        while (entry != end) {
            dict->schema->destroyCB(*entry++);
        }
    }
    fn_8047DFC8(dict->entries);
    RWSRCGLOBAL(rwfree)(dict);
    return 1;
}

/* Adds a dictionary to the schema's list (only inlined in retail). */
static RtDictSchema *SchemaAddDict(RtDictSchema *schema, RtDict *dict) {
    RtDict **entry = (RtDict **)fn_8047DDBC(schema->dictionaries, schema->dictChunkType | 0x30000);

    if (entry == 0) {
        return 0;
    }
    *entry = dict;
    return schema;
}

/* Sets a new dictionary up (only inlined in retail). */
static RtDict *DictInit(RtDict *dict, RtDictSchema *schema) {
    dict->entries = fn_8047DC90(sizeof(void *), schema->dictChunkType | 0x30000);
    if (dict->entries == 0) {
        return 0;
    }
    dict->schema = schema;
    if (SchemaAddDict(schema, dict) == 0) {
        fn_8047DFC8(dict->entries);
        return 0;
    }
    return dict;
}

/* RtDictSchemaCreateDict (only inlined in retail). */
static RtDict *CreateDict(RtDictSchema *schema) {
    RtDict *dict = (RtDict *)RWSRCGLOBAL(rwmalloc)(sizeof(RtDict), schema->dictChunkType | 0x30000);

    if (dict == 0) {
        R5ERROR(0x1BF, (0x80000013, sizeof(RtDict)));
        return 0;
    }
    if (DictInit(dict, schema) == 0) {
        RWSRCGLOBAL(rwfree)(dict);
        return 0;
    }
    return dict;
}

/* RtDictSchemaStreamReadDict. */
RtDict *fn_80479284(RtDictSchema *schema, RwStream *stream) {
    u32 length;
    u32 version;
    u32 numEntries;
    RtDict *dict;

    if (!fn_8047CF50(stream, 1, &length, &version)) {
        return 0;
    }
    if (version < schema->compatibilityVersion) {
        return schema->streamReadCompatibilityCB(stream);
    }
    if (fn_80480D4C(stream, &numEntries, length) == 0) {
        return 0;
    }
    fn_8047D254(&numEntries, 4);
    dict = CreateDict(schema);
    if (dict == 0) {
        return 0;
    }
    while (numEntries--) {
        void *entry;

        if (!fn_8047CF50(stream, schema->elementChunkType, &length, &version) ||
            (entry = schema->streamReadCB(stream)) == 0) {
            fn_80479174(dict);
            return 0;
        }
        *(void **)fn_8047DDBC(dict->entries, dict->schema->dictChunkType | 0x30000) = entry;
        dict->schema->addRefCB(entry);
        schema->destroyCB(entry);
    }
    return dict;
}
