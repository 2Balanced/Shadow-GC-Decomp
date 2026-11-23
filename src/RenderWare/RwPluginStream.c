/* RenderWare core: plugin data chunks in streams (0x804815F0-0x80481B98).
 * _rwPluginRegistryAddPluginStream and the always and rights callbacks, then reading, invoking
 * rights on, sizing, writing and skipping the extension chunks of an object. No data.
 * Boundary: a file of its own between the stream module and the plugin registry. Compiled with the
 * stream module (automatic inlining) _rwPluginRegistryReadDataChunks and SkipDataChunks would
 * inline RwStreamSkip, which retail calls; the registry file after it opens with its module open
 * and close like the other core modules. C, cflags_rw_core (configure.py). */
#include "Game/rw/r6.h"

RwBool fn_8047CDB4(RwStream *stream, u32 *type, u32 *length, u32 *version, u32 *build);
RwStream *fn_8047CEF0(RwStream *stream, u32 type, s32 size, u32 version, u32 build);
RwBool fn_8047CF50(RwStream *stream, u32 type, u32 *length, u32 *version);
RwStream *fn_804810FC(RwStream *stream, u32 offset);

static inline RwPluginRegistryEntry *FindEntry(const RwPluginRegistry *reg, u32 pluginID) {
    RwPluginRegistryEntry *entry = reg->firstRegEntry;
    while (entry) {
        if (entry->pluginID == pluginID) {
            break;
        }
        entry = entry->nextRegEntry;
    }
    return entry;
}

/* _rwPluginRegistryAddPluginStream. */
s32 fn_804815F0(RwPluginRegistry *reg, u32 pluginID, RwPluginDataChunkReadCallBack readCB,
                RwPluginDataChunkWriteCallBack writeCB,
                RwPluginDataChunkGetSizeCallBack getSizeCB) {
    RwPluginRegistryEntry *entry = FindEntry(reg, pluginID);
    if (entry) {
        entry->readCB = readCB;
        entry->writeCB = writeCB;
        entry->getSizeCB = getSizeCB;
        return entry->offset;
    }
    return -1;
}

/* _rwPluginRegistryAddPlgnStrmlwysCB. */
s32 fn_80481634(RwPluginRegistry *reg, u32 pluginID, RwPluginDataChunkAlwaysCallBack alwaysCB) {
    RwPluginRegistryEntry *entry = FindEntry(reg, pluginID);
    if (entry) {
        entry->alwaysCB = alwaysCB;
        return entry->offset;
    }
    return -1;
}

/* _rwPluginRegistryAddPlgnStrmRightsCB. */
s32 fn_80481670(RwPluginRegistry *reg, u32 pluginID, RwPluginDataChunkRightsCallBack rightsCB) {
    RwPluginRegistryEntry *entry = FindEntry(reg, pluginID);
    if (entry) {
        entry->rightsCB = rightsCB;
        return entry->offset;
    }
    return -1;
}

/* _rwPluginRegistryReadDataChunks. */
const RwPluginRegistry *fn_804816AC(const RwPluginRegistry *reg, RwStream *stream, void *object) {
    u32 length;
    u32 version;

    if (!fn_8047CF50(stream, 3, &length, &version)) {
        return NULL;
    }
    if (version >= 0x35000 && version <= 0x37002) {
        RwPluginRegistryEntry *entry;
        while (length > 0) {
            u32 type;
            u32 chunkLength;
            if (!fn_8047CDB4(stream, &type, &chunkLength, NULL, NULL)) {
                return NULL;
            }
            entry = reg->firstRegEntry;
            while (entry) {
                if (entry->pluginID == type) {
                    break;
                }
                entry = entry->nextRegEntry;
            }
            if (entry && entry->readCB) {
                if (!entry->readCB(stream, chunkLength, object, entry->offset, entry->size)) {
                    return NULL;
                }
            } else {
                if (!fn_804810FC(stream, chunkLength)) {
                    return NULL;
                }
            }
            length -= chunkLength + 12;
        }
        entry = reg->firstRegEntry;
        while (entry) {
            if (entry->alwaysCB) {
                if (!entry->alwaysCB(object, entry->offset, entry->size)) {
                    return NULL;
                }
            }
            entry = entry->nextRegEntry;
        }
        return reg;
    }
    RWERROR((0x80000004));
    return NULL;
}

/* _rwPluginRegistryInvokeRights. */
const RwPluginRegistry *fn_80481870(const RwPluginRegistry *reg, u32 pluginID, void *object,
                                    u32 extra) {
    RwPluginRegistryEntry *entry = FindEntry(reg, pluginID);
    if (entry && entry->rightsCB) {
        if (entry->rightsCB(object, entry->offset, entry->size, extra)) {
            return reg;
        }
        return NULL;
    }
    return NULL;
}

/* _rwPluginRegistryGetSize. */
s32 fn_804818FC(const RwPluginRegistry *reg, const void *object) {
    s32 total = 0;
    RwPluginRegistryEntry *entry = reg->firstRegEntry;

    while (entry) {
        if (entry->getSizeCB) {
            s32 size = entry->getSizeCB(object, entry->offset, entry->size);
            if (size > 0) {
                total = size + total;
                total += 12;
            }
        }
        entry = entry->nextRegEntry;
    }
    return total;
}

/* _rwPluginRegistryWriteDataChunks. */
const RwPluginRegistry *fn_80481980(const RwPluginRegistry *reg, RwStream *stream,
                                    const void *object) {
    RwPluginRegistryEntry *entry;
    s32 total = fn_804818FC(reg, object);

    if (!fn_8047CEF0(stream, 3, total, 0x37002, 0x37)) {
        return NULL;
    }
    entry = reg->firstRegEntry;
    while (entry) {
        if (entry->getSizeCB && entry->writeCB) {
            s32 size = entry->getSizeCB(object, entry->offset, entry->size);
            if (size > 0) {
                if (!fn_8047CEF0(stream, entry->pluginID, size, 0x37002, 0x37)) {
                    return NULL;
                }
                if (!entry->writeCB(stream, size, object, entry->offset, entry->size)) {
                    return NULL;
                }
            }
        }
        entry = entry->nextRegEntry;
    }
    return reg;
}

/* _rwPluginRegistrySkipDataChunks. */
const RwPluginRegistry *fn_80481AD4(const RwPluginRegistry *reg, RwStream *stream) {
    u32 length;

    if (!fn_8047CF50(stream, 3, &length, NULL)) {
        return NULL;
    }
    while (length > 0) {
        u32 chunkLength;
        if (!fn_8047CDB4(stream, NULL, &chunkLength, NULL, NULL)) {
            return NULL;
        }
        if (!fn_804810FC(stream, chunkLength)) {
            return NULL;
        }
        length -= chunkLength + 12;
    }
    return reg;
}
