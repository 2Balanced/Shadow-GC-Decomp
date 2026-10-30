/* The manager of the texture animations (.ADB files) and of the TTextureChange tasks that play them
 * (class name unknown; fn_80013BC4, Game/ResourceOneFile.cpp, returns the one instance). It keeps a
 * list of the loaded animations and the clumps whose materials it has already matched. Matching a
 * clump's (or a world's) materials against the animations fills a table of material and animation
 * pairs, from which a TTextureChange task is created (Game/TTextureChange.cpp). Other units call
 * the functions by their fn_ names. The file name and all names but TTextureChange's are inferred.
 * Definitions are in reverse address order (deferred inlining emits functions last-defined
 * first). */
#define MSL_ALGORITHM_OUTLINE
#define MSL_VECTOR_DESTROY_ALL_LATE
#define MSL_VECTOR_DESTROY_ALL_FIRST
#define MSL_VECTOR_MIDDLE
#define UNKHANDLE_DEFAULT_CTOR
#include <vector>
#include <string.h>

#include "Game/HandleUser.h"
#include "Game/Task.h"
#include "Game/UnkHandleTable.h"
#include <dolphin/types.h>
#include "size_assert.h"

struct RwTexture {
    /* 0x00 */ u8 field_0x0[0x10];
    /* 0x10 */ char m_name[0x20];
};

struct RpMaterial {
    /* 0x0 */ RwTexture *m_texture;
};

struct RpGeometry {
    /* 0x00 */ u8 field_0x0[0x20];
    /* 0x20 */ RpMaterial **m_materials;
    /* 0x24 */ s32 m_numMaterials;
};

struct RpAtomic {
    /* 0x00 */ u8 field_0x0[0x18];
    /* 0x18 */ RpGeometry *m_geometry;
};

struct RpClump;
struct RpWorld;

typedef RpAtomic *(*RpAtomicCallBack)(RpAtomic *atomic, void *data);
typedef RpMaterial *(*RpMaterialCallBack)(RpMaterial *material, void *data);

/* A texture animation as the .ADB file holds it (big-endian; struct name unknown; see
 * Game/TTextureChange.cpp). */
struct UnkTextureFrame_8004F0A4 {
    /* 0x0 */ s32 m_time;
    /* 0x4 */ s32 m_index;
};

struct UnkTextureAnim_8004F0A4 {
    /* 0x00 */ s32 m_length;
    /* 0x04 */ char m_baseName[0x20];
    /* 0x24 */ char m_name[0x20];
    /* 0x44 */ s32 field_0x44;
    /* 0x48 */ s32 m_count;
    /* 0x4C */ UnkTextureFrame_8004F0A4 m_frames[1];
};

/* A node of the list of loaded animations. */
struct UnkTextureAnimHolder_8004F22C {
    /* 0x0 */ UnkTextureAnimHolder_8004F22C *m_next;
    /* 0x4 */ UnkTextureAnim_8004F0A4 *m_anim;
};

/* A material and its animation, as TTextureChange takes them. */
struct UnkTextureChangeParam_8004F22C {
    /* 0x0 */ RpMaterial *m_material;
    /* 0x4 */ UnkTextureAnimHolder_8004F22C *m_holder;
};

class TTextureChange : public Task, public HandleUser {
  public:
    virtual ~TTextureChange();
};

extern "C" {
void *fn_80049B60(u32 size, s32 count, int heap);
void fn_80049B34(void *block);
/* Swaps the bytes of a value in place. */
void fn_80043D0C(void *value, int size);
/* RpClumpForAllAtomics, RpWorldForAllMaterials. */
RpClump *fn_80456C54(RpClump *clump, RpAtomicCallBack callBack, void *data);
RpWorld *fn_8046066C(RpWorld *world, RpMaterialCallBack callBack, void *data);
HandleUser *fn_80045A98(const UnkHandle_80045A98 &handle);
UnkHandle_80045A98 fn_8004F678(void *params, s32 count, const char *name, Task *parent);
}

/* The frame time all animations show when not negative (Game/TTextureChange.cpp reads it). */
float lbl_805E39A0 = -1.0f;

class UnkAdbManager_80013BC4 {
  public:
    UnkAdbManager_80013BC4();
    ~UnkAdbManager_80013BC4();

    /* Takes node out of the list and frees it with its animation. */
    bool Remove(UnkTextureAnimHolder_8004F22C *node) {
        UnkTextureAnimHolder_8004F22C *n = m_head;
        UnkTextureAnimHolder_8004F22C *prev = 0;
        for (; n != 0; n = n->m_next) {
            if (n == node) {
                fn_80049B34(n->m_anim);
                if (prev != 0) {
                    prev->m_next = n->m_next;
                } else {
                    m_head = n->m_next;
                }
                fn_80049B34(node);
                return true;
            }
            prev = n;
        }
        return false;
    }

    /* 0x0 */ UnkTextureAnimHolder_8004F22C *m_head;
    /* 0x4 */ std::vector<RpClump *> m_clumps;
};
SIZE_ASSERT(UnkAdbManager_80013BC4, 0x10);

static UnkTextureChangeParam_8004F22C s_params[32];
static s32 s_paramCount;

/* No frame pinned. */
inline float NoTextureChangeTime() {
    return -1.0f;
}

/* Calling an inline function keeps it out of line (deferred inlining is one level deep), as in
 * retail. */
inline void ResetTextureChangeTime() {
    lbl_805E39A0 = NoTextureChangeTime();
}

UnkAdbManager_80013BC4::UnkAdbManager_80013BC4() {
    m_head = 0;
    m_clumps.reserve(0x40);
}

UnkAdbManager_80013BC4::~UnkAdbManager_80013BC4() {}

extern "C" void fn_8004FF84(UnkAdbManager_80013BC4 *manager) {
    while (manager->m_head != 0) {
        manager->Remove(manager->m_head);
    }
    manager->m_clumps.clear();
    ResetTextureChangeTime();
}

/* Adds an animation (byte-swapped in place) to the list. */
extern "C" UnkTextureAnimHolder_8004F22C *fn_8004FE88(UnkAdbManager_80013BC4 *manager, void *data) {
    UnkTextureFrame_8004F0A4 *frame;
    UnkTextureAnim_8004F0A4 *anim = (UnkTextureAnim_8004F0A4 *)data;
    UnkTextureAnimHolder_8004F22C *last;
    s32 i;
    UnkTextureFrame_8004F0A4 *frames;
    UnkTextureAnimHolder_8004F22C *node;
    if (anim != 0) {
        fn_80043D0C(&anim->m_length, 4);
        fn_80043D0C(&anim->field_0x44, 4);
        fn_80043D0C(&anim->m_count, 4);
        frames = anim->m_frames;
        for (i = 0; i < anim->m_count; i++) {
            frame = &frames[i];
            fn_80043D0C(&frame->m_time, 4);
            fn_80043D0C(&frame->m_index, 4);
        }
        last = manager->m_head;
        if (last == 0) {
            manager->m_head = (UnkTextureAnimHolder_8004F22C *)fn_80049B60(8, 1, 1);
            manager->m_head->m_next = 0;
            node = manager->m_head;
        } else {
            while (last->m_next != 0) {
                last = last->m_next;
            }
            node = last->m_next = (UnkTextureAnimHolder_8004F22C *)fn_80049B60(8, 1, 1);
        }
        node->m_anim = anim;
        return node;
    }
    return 0;
}

/* Takes node out of the list (Remove written out). */
extern "C" bool fn_8004FE00(UnkAdbManager_80013BC4 *manager, UnkTextureAnimHolder_8004F22C *node) {
    UnkTextureAnimHolder_8004F22C *n = manager->m_head;
    UnkTextureAnimHolder_8004F22C *prev = 0;
    for (; n != 0; n = n->m_next) {
        if (n == node) {
            fn_80049B34(n->m_anim);
            if (prev != 0) {
                prev->m_next = n->m_next;
            } else {
                manager->m_head = n->m_next;
            }
            fn_80049B34(node);
            return true;
        }
        prev = n;
    }
    return false;
}

/* Whether clump was matched already; if not, it is now. */
extern "C" bool fn_8004FCC8(UnkAdbManager_80013BC4 *manager, RpClump *clump) {
    for (RpClump **it = manager->m_clumps.begin(); it != manager->m_clumps.end(); it++) {
        if (*it == clump) {
            return true;
        }
    }
    RpClump *added = clump;
    manager->m_clumps.push_back(added);
    return false;
}

/* Adds the material and every animation of the list that replaces its texture to the table. */
static RpMaterial *AddMaterial(RpMaterial *material, void *data) {
    RwTexture *texture = material->m_texture;
    if (texture == 0) {
        return material;
    }
    UnkTextureAnimHolder_8004F22C *node = (UnkTextureAnimHolder_8004F22C *)data;
    for (; node != 0; node = node->m_next) {
        if (strcmp(texture->m_name, node->m_anim->m_baseName) == 0 && s_paramCount < 31) {
            s_params[s_paramCount].m_material = material;
            s_params[s_paramCount].m_holder = node;
            s_paramCount++;
        }
    }
    return material;
}

extern "C" UnkHandle_80045A98 fn_8004F930(UnkAdbManager_80013BC4 *manager, const char *name, Task *parent);

extern "C" UnkHandle_80045A98 fn_8004FB78(UnkAdbManager_80013BC4 *manager, RpWorld *world, void *data, const char *name, u8 once, Task *parent) {
    s_paramCount = 0;
    if (once && fn_8004FCC8(manager, (RpClump *)world)) {
        return UnkHandle_80045A98();
    }
    if (data != 0) {
        fn_8046066C(world, AddMaterial, data);
    } else {
        fn_8046066C(world, AddMaterial, manager->m_head);
    }
    return fn_8004F930(manager, name, parent);
}

static RpAtomic *AddAtomic(RpAtomic *atomic, void *data) {
    RpGeometry *geometry = atomic->m_geometry;
    if (geometry == 0) {
        return atomic;
    }
    s32 count = geometry->m_numMaterials;
    for (s32 i = 0; i < count; i++) {
        RpMaterial *material = geometry->m_materials[i];
        if (material != 0) {
            RwTexture *texture = material->m_texture;
            if (texture != 0) {
                UnkTextureAnimHolder_8004F22C *node = (UnkTextureAnimHolder_8004F22C *)data;
                for (; node != 0; node = node->m_next) {
                    if (strcmp(texture->m_name, node->m_anim->m_baseName) == 0 && s_paramCount < 31) {
                        s_params[s_paramCount].m_material = material;
                        s_params[s_paramCount].m_holder = node;
                        s_paramCount++;
                    }
                }
            }
        }
    }
    return atomic;
}

extern "C" UnkHandle_80045A98 fn_8004F9E8(UnkAdbManager_80013BC4 *manager, RpClump *clump, void *data,
                                          const char *name, u8 once, Task *parent) {
    if (once && fn_8004FCC8(manager, clump)) {
        return UnkHandle_80045A98();
    }
    s_paramCount = 0;
    if (data != 0) {
        fn_80456C54(clump, AddAtomic, data);
    } else {
        fn_80456C54(clump, AddAtomic, manager->m_head);
    }
    return fn_8004F930(manager, name, parent);
}

/* Ends the task of a handle. */
extern "C" void fn_8004F99C(UnkAdbManager_80013BC4 *manager, UnkHandle_80045A98 handle) {
    HandleUser *user = fn_80045A98(handle);
    if (user != 0) {
        dynamic_cast<TTextureChange *>(user)->m_flags |= 1;
    }
}

/* Creates a TTextureChange task for the table. */
extern "C" UnkHandle_80045A98 fn_8004F930(UnkAdbManager_80013BC4 *manager, const char *name, Task *parent) {
    if (s_paramCount != 0) {
        return fn_8004F678(s_params, s_paramCount, name, parent);
    }
    const UnkHandle_80045A98 &none = UnkHandle_80045A98();
    return none;
}

#include <vector_impl>
