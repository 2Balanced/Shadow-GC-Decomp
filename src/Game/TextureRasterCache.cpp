/* The texture raster cache (class name unknown; a function-local static): textures of the same
 * name loaded with different texture dictionaries share one raster. Adding a dictionary hands each
 * of its textures to the cache, which keeps the first raster of a name and gives that one to the
 * later textures (destroying their own); releasing a dictionary takes its textures out again and
 * destroys a raster once no texture uses it. The other units call the C functions at the end of
 * the file (they keep their fn_ names). File name inferred. Names marked "(name inferred)" are not
 * original. Definitions are in reverse address order (deferred inlining emits functions
 * last-defined first). */
#include <cstring>

#include <dolphin/types.h>
#include "size_assert.h"

struct RwRaster;
struct RwTexDictionary;

/* RenderWare's texture (rw/rwcore.h; only the members used are laid out). */
struct RwTexture {
    /* 0x00 */ RwRaster *raster;
    /* 0x04 */ RwTexDictionary *dict;
    /* 0x08 */ u8 lInDictionary[0x8];
    /* 0x10 */ char name[32];
};

typedef RwTexture *(*RwTextureCallBack)(RwTexture *texture, void *data);

extern "C" {
/* RwRasterDestroy, RwTextureSetRaster, RwTextureDestroy, RwTexDictionaryForAllTextures and
 * RwTexDictionaryDestroy. */
s32 fn_80489BDC(RwRaster *raster);
RwTexture *fn_8048CA00(RwTexture *texture, RwRaster *raster);
s32 fn_8048CD8C(RwTexture *texture);
RwTexDictionary *fn_8048CBE0(RwTexDictionary *dict, RwTextureCallBack callBack, void *data);
s32 fn_8048CB38(RwTexDictionary *dict);
}

/* A raster the cache keeps for a texture name (struct name inferred). */
struct UnkRasterEntry_802E0718 {
    /* 0x00 */ u32 m_hash;
    /* 0x04 */ RwRaster *m_raster;
    /* 0x08 */ s32 m_refs; /* the textures using the raster */
    /* 0x0C */ UnkRasterEntry_802E0718 *m_next;
    /* 0x10 */ char m_name[32];
};
SIZE_ASSERT(UnkRasterEntry_802E0718, 0x30);

/* A texture name's hash (name inferred). */
inline u32 HashName(const char *name) {
    u32 hash = 0;
    for (int i = 0; *name != 0; name++, i++) {
        hash += *name << (i & 0x1F);
    }
    return hash;
}

class UnkRasterCache_802E0514 {
  public:
    UnkRasterCache_802E0514() : m_entries(0), m_count(0), m_dictionaries(0) {}
    ~UnkRasterCache_802E0514();

    /* Names inferred. */
    UnkRasterEntry_802E0718 *Find(const char *name, UnkRasterEntry_802E0718 **previous);
    void Release(RwTexture *texture);
    void Add(RwTexture *texture);

    static UnkRasterCache_802E0514 *s_instance;

    /* 0x0 */ UnkRasterEntry_802E0718 *m_entries;
    /* 0x4 */ s32 m_count;
    /* 0x8 */ s32 m_dictionaries; /* the dictionaries added and not yet released */
};
SIZE_ASSERT(UnkRasterCache_802E0514, 0xC);

UnkRasterEntry_802E0718 *UnkRasterCache_802E0514::Find(const char *name,
                                                       UnkRasterEntry_802E0718 **previous) {
    u32 hash = HashName(name);
    UnkRasterEntry_802E0718 *entry = m_entries;
    UnkRasterEntry_802E0718 *last = 0;
    while (entry != 0) {
        if (hash == entry->m_hash && strcmp(name, entry->m_name) == 0) {
            if (previous != 0) {
                *previous = last;
            }
            return entry;
        }
        last = entry;
        entry = entry->m_next;
    }
    return 0;
}

void UnkRasterCache_802E0514::Release(RwTexture *texture) {
    UnkRasterEntry_802E0718 *previous;
    UnkRasterEntry_802E0718 *entry = Find(texture->name, &previous);
    if (entry != 0) {
        fn_8048CA00(texture, 0);
        if (--entry->m_refs <= 0) {
            if (previous != 0) {
                previous->m_next = entry->m_next;
            } else {
                m_entries = entry->m_next;
            }
            if (entry->m_raster != 0) {
                fn_80489BDC(entry->m_raster);
            }
            delete entry;
            m_count--;
        }
    }
    fn_8048CD8C(texture);
}

UnkRasterCache_802E0514::~UnkRasterCache_802E0514() {
    if (m_entries != 0) {
        UnkRasterEntry_802E0718 *next = m_entries;
        do {
            UnkRasterEntry_802E0718 *entry = next;
            next = entry->m_next;
            if (entry->m_raster != 0) {
                fn_80489BDC(entry->m_raster);
            }
            delete entry;
            m_count--;
        } while (next != 0);
        m_entries = 0;
    }
}

void UnkRasterCache_802E0514::Add(RwTexture *texture) {
    const char *name = texture->name;
    UnkRasterEntry_802E0718 *last = 0;
    UnkRasterEntry_802E0718 *entry = m_entries;
    u32 hash = HashName(name);
    while (entry != 0) {
        if (hash == entry->m_hash && strcmp(name, entry->m_name) == 0) {
            entry->m_refs++;
            RwRaster *raster = texture->raster;
            if (entry->m_raster != 0) {
                fn_80489BDC(raster);
                fn_8048CA00(texture, entry->m_raster);
            } else {
                entry->m_raster = raster;
            }
            return;
        }
        last = entry;
        entry = entry->m_next;
    }
    entry = new UnkRasterEntry_802E0718;
    entry->m_hash = hash;
    entry->m_raster = texture->raster;
    entry->m_refs = 1;
    entry->m_next = 0;
    strcpy(entry->m_name, name);
    if (m_entries != 0) {
        last->m_next = entry;
    } else {
        m_entries = entry;
    }
    m_count++;
}

/* The dictionary callbacks (names inferred). */
static RwTexture *AddTexture(RwTexture *texture, void *cache) {
    static_cast<UnkRasterCache_802E0514 *>(cache)->Add(texture);
    return texture;
}

static RwTexture *ReleaseTexture(RwTexture *texture, void *cache) {
    static_cast<UnkRasterCache_802E0514 *>(cache)->Release(texture);
    return texture;
}

extern "C" UnkRasterCache_802E0514 *fn_802E0514(void);

/* Adds the textures of dict (name of the function unknown). */
extern "C" void fn_802E0658(RwTexDictionary *dict) {
    UnkRasterCache_802E0514 *cache = fn_802E0514();
    fn_8048CBE0(dict, AddTexture, cache);
    cache->m_dictionaries++;
}

/* Releases the textures of dict and destroys it (name of the function unknown). */
extern "C" void fn_802E0600(RwTexDictionary *dict) {
    UnkRasterCache_802E0514 *cache = fn_802E0514();
    fn_8048CBE0(dict, ReleaseTexture, cache);
    fn_8048CB38(dict);
    cache->m_dictionaries--;
}

/* Releases texture (name of the function unknown). */
extern "C" void fn_802E05CC(RwTexture *texture) {
    fn_802E0514()->Release(texture);
}

/* The number of rasters the cache keeps (name of the function unknown). */
extern "C" s32 fn_802E05A8(void) {
    return fn_802E0514()->m_count;
}

/* The cache object (name inferred). */
static UnkRasterCache_802E0514 *GetCache() {
    static UnkRasterCache_802E0514 s_cache;
    return &s_cache;
}

UnkRasterCache_802E0514 *UnkRasterCache_802E0514::s_instance;

/* The cache (name of the function unknown). */
extern "C" UnkRasterCache_802E0514 *fn_802E0514(void) {
    if (UnkRasterCache_802E0514::s_instance == 0) {
        UnkRasterCache_802E0514::s_instance = GetCache();
    }
    return UnkRasterCache_802E0514::s_instance;
}
