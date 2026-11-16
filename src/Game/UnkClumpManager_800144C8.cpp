/* What loaded clumps are handed to (class name unknown; fn_800144C8, Game/ResourceOneFile.cpp,
 * returns the one instance): two presets of the 0x18-byte, reference-counted settings record that
 * the RenderWare layer attaches to a geometry (fn_80463104 stores the record's address in the
 * geometry's plugin data and counts the reference), and the function that gives every atomic of a
 * clump one of them. No RTTI or string names the class and it has no data of its own; the unit is
 * named after the placeholder Game/ResourceOneFile.cpp already uses for the class. The callers
 * (Game/ResourceOneFile.cpp, Game/ResourceOneFileReq.cpp) reach the members through their
 * mangled names. Member names are inferred. Definitions are in reverse address order (deferred
 * inlining emits functions last-defined first). */
#include <dolphin/types.h>
#include "size_assert.h"

struct RpGeometry;
struct RpClump;

struct RpAtomic {
    /* 0x00 */ u8 field_0x0[0x18];
    /* 0x18 */ RpGeometry *m_geometry;
};

typedef RpAtomic *(*RpAtomicCallBack)(RpAtomic *atomic, void *data);

/* The settings record of a geometry (struct name unknown; fn_80462FAC fills it with its
 * defaults). */
struct UnkGeometrySettings_80462FAC {
    /* 0x00 */ u8 field_0x0[0x16];
    /* 0x16 */ u16 m_refCount;
};
SIZE_ASSERT(UnkGeometrySettings_80462FAC, 0x18);

extern "C" {
/* RenderWare: calls callback for every atomic of clump. */
RpClump *fn_80456C54(RpClump *clump, RpAtomicCallBack callback, void *data);
/* The settings record (Game/RwCpp26.cpp): its defaults, the setters of its byte pairs and of the
 * byte at 0xA, and attaching one to a geometry (behaviour inferred). */
void fn_80462FAC(UnkGeometrySettings_80462FAC *settings);
void fn_80462F7C(UnkGeometrySettings_80462FAC *settings, u8 first, u8 second);
void fn_80462F88(UnkGeometrySettings_80462FAC *settings, u8 first, u8 second);
void fn_80462F94(UnkGeometrySettings_80462FAC *settings, u32 offset, u8 first, u8 second);
void fn_80462FA4(UnkGeometrySettings_80462FAC *settings, u8 value);
void fn_80463104(RpGeometry *geometry, void *settings);
}

class UnkClumpManager_800144C8 {
  public:
    UnkClumpManager_800144C8();
    ~UnkClumpManager_800144C8();

    /* Gives every atomic of clump the preset of index. */
    void SetSettings(RpClump *clump, int index);

    static RpAtomic *SetAtomicSettings(RpAtomic *atomic, void *data);

    /* 0x00 */ UnkGeometrySettings_80462FAC m_settings[2];
};
SIZE_ASSERT(UnkClumpManager_800144C8, 0x30);

UnkClumpManager_800144C8::UnkClumpManager_800144C8() {
    fn_80462FAC(&m_settings[0]);
    fn_80462F7C(&m_settings[0], 4, 6);
    fn_80462F88(&m_settings[0], 1, 0);
    fn_80462F94(&m_settings[0], 1, 4, 6);
    fn_80462FA4(&m_settings[0], 3);
    UnkGeometrySettings_80462FAC *settings = &m_settings[1];
    fn_80462FAC(settings);
    fn_80462F7C(settings, 4, 0);
    fn_80462F88(settings, 1, 0);
    fn_80462F94(settings, 1, 4, 6);
    fn_80462FA4(settings, 3);
}

UnkClumpManager_800144C8::~UnkClumpManager_800144C8() {}

void UnkClumpManager_800144C8::SetSettings(RpClump *clump, int index) {
    if (clump != 0) {
        fn_80456C54(clump, SetAtomicSettings, &m_settings[index]);
    }
}

RpAtomic *UnkClumpManager_800144C8::SetAtomicSettings(RpAtomic *atomic, void *data) {
    fn_80463104(atomic->m_geometry, data);
    return atomic;
}
