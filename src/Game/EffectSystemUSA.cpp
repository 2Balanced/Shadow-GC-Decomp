/* SonicteamUSA::System::Effect::EffectSystemUSA (class name and bases from RTTI): the game's effect
 * system, which gives the effect library its memory from a Sonicteam::System::Heap on a block it is
 * handed and draws with the screen's camera. The unit also holds the fog setting accessors of
 * EffectManagerUSA, its vf18 override (and so its vtable and inline destructor), and the accessors
 * of two game-wide objects that other units call by their fn_ names (the renderer, fn_800289B0, and
 * the node pool of Game/PooledList.h, fn_80028B30), with the first copies of the pool node's
 * shared_ptr members. File name from the class; member names are inferred. Definitions are in
 * reverse address order (deferred inlining emits functions last-defined first). */
#define UNK_FOG_PARAM_CONSTRUCTOR
#include "Effect/System/EffectManager.h"
#include "Effect/System/EffectSystem.h"
#include "Game/MathTypes.h"
#include "Game/PooledList.h"
#include "boost/shared_ptr.hpp"
#include <dolphin/types.h>
#include "size_assert.h"

using SonicteamUSA::System::Effect::EffectManagerUSA;
using SonicteamUSA::System::Effect::EffectSystemUSA;

struct RwRGBA {
    u8 red;
    u8 green;
    u8 blue;
    u8 alpha;
};

/* The screen (see Game/ScreenHolder.h; only its camera is used here). */
class UnkScreenHolder_8000DCC0 {
  public:
    /* 0x0 */ void *m_camera;
};

/* The renderer behind fn_800289B0 (class name unknown; its constructor and destructor are out of
 * line). */
class UnkRenderer_800289B0 {
  public:
    UnkRenderer_800289B0();
    ~UnkRenderer_800289B0();

    /* The renderer's address once created (name inferred). */
    static UnkRenderer_800289B0 *s_cached;

    /* 0x000 */ u8 field_0x0[0x460];
};
SIZE_ASSERT(UnkRenderer_800289B0, 0x460);

namespace Sonicteam {
namespace System {

/* The heap the effect library allocates from (class name from RTTI; only what is used here). */
class Heap {
  public:
    Heap(void *memory, u32 size);
    virtual void vf8();
    virtual void *Allocate(u32 alignment, u32 size);
    virtual void vf10();
    virtual void vf14();
    virtual void Free(void *memory);

    /* 0x4 */ void *field_0x4;
};

} // namespace System
} // namespace Sonicteam

/* The object behind EffectManagerUSA's fog setting (class name unknown). */
struct UnkFogHolder_80028AE8 {
    UnkFogParam_8002893C Get();
    void Set(const UnkFogParam_8002893C &fog);

    /* 0x0 */ u32 field_0x0;
    /* 0x4 */ UnkFogParam_8002893C m_fog;
};

extern "C" {
UnkScreenHolder_8000DCC0 *fn_8000DCC0(void);
void fn_8040EE90(void);
/* Sets the colour of clump's materials (see Game/DebugDispShape.h). */
void fn_80037B84(RpClump *clump, const RwRGBA &color);
}

UnkRenderer_800289B0 *UnkRenderer_800289B0::s_cached;
/* Unused (the linker drops it; it leaves the gap retail has after s_cached). */
u32 lbl_805EE094;

namespace SonicteamUSA {
namespace System {
namespace Effect {

void EffectManagerUSA::vf18(RpClump *clump, bool arg2, int count, void *arg4) {
    if (count > 1) {
        RwRGBA color = {0, 0, 0, 0x80};
        color.blue = color.green = color.red = 0x100 / count;
        fn_80037B84(clump, color);
    } else {
        RwRGBA color = {0xFF, 0xFF, 0xFF, 0x80};
        fn_80037B84(clump, color);
    }
    EffectManagerRW3::vf18(clump, arg2, count, arg4);
}

} // namespace Effect
} // namespace System
} // namespace SonicteamUSA

/* Never called (the linker drops it): constructs an array of vectors, which emits the first copy
 * of Vec3f's constructor here, as retail has it. */
struct UnkVectors_80028BA0 {
    UnkVectors_80028BA0() {}

    Vec3f m_vectors[2];
};

void UnkVectors_80028BA0_Construct(void *memory) {
    new (memory) UnkVectors_80028BA0;
}

/* The pool behind fn_80028B30 (inlined into it). */
inline PooledListPool &PooledListPool_Instance() {
    static PooledListPool s_pool(0x200);
    return s_pool;
}

extern "C" PooledListPool *fn_80028B30(void) {
    if (PooledListPool::s_cached == 0) {
        PooledListPool::s_cached = &PooledListPool_Instance();
    }
    return PooledListPool::s_cached;
}

void UnkFogHolder_80028AE8::Set(const UnkFogParam_8002893C &fog) {
    m_fog = fog;
}

UnkFogParam_8002893C UnkFogHolder_80028AE8::Get() {
    return m_fog;
}

/* The game's copy of the pool node's shared_ptr reset, which other units call by its fn_ name
 * (see Game/PooledList.h): a friend of a class template, so that GC/2.7 treats it as the template
 * instance it stands for (the node's value type is unknown). */
template <class T> struct UnkPoolNodeReset_80028A1C {
    friend void fn_80028A1C(void *node) {
        boost::shared_ptr<T>().swap(*static_cast<boost::shared_ptr<T> *>(node));
    }
};
template struct UnkPoolNodeReset_80028A1C<void>;

/* Never called (the linker drops it): gives a node back to the pool, which emits the reset above
 * and its members here, as retail has them. */
void UnkPoolRelease_80028A1C(PooledListPoolNode *node) {
    fn_80028B30()->Push(node);
}

/* The renderer behind fn_800289B0 (inlined into it). */
inline UnkRenderer_800289B0 &UnkRenderer_800289B0_Instance() {
    static UnkRenderer_800289B0 s_renderer;
    return s_renderer;
}

extern "C" UnkRenderer_800289B0 *fn_800289B0(void) {
    if (UnkRenderer_800289B0::s_cached == 0) {
        UnkRenderer_800289B0::s_cached = &UnkRenderer_800289B0_Instance();
    }
    return UnkRenderer_800289B0::s_cached;
}

static inline UnkFogHolder_80028AE8 *GetFogHolder(EffectManagerUSA *manager) {
    return *reinterpret_cast<UnkFogHolder_80028AE8 **>(manager->field_0x4 + 0x50);
}

extern "C" void fn_80028984(EffectManagerUSA *manager, const UnkFogParam_8002893C *param) {
    UnkFogHolder_80028AE8 *holder = GetFogHolder(manager);
    if (holder != 0) {
        holder->Set(*param);
    }
}

extern "C" UnkFogParam_8002893C fn_800288D8(EffectManagerUSA *manager) {
    UnkFogHolder_80028AE8 *holder = GetFogHolder(manager);
    if (holder != 0) {
        return holder->Get();
    }
    UnkFogParam_8002893C fog;
    return fog;
}

namespace SonicteamUSA {
namespace System {
namespace Effect {

void *EffectSystemUSA::Initialize(void *heap, u32 size) {
    m_heap = heap;
    m_allocator = new Sonicteam::System::Heap(heap, size);
    field_0x4 = 0;
}

void *EffectSystemUSA::Finalize() {
    void *heap = m_heap;
    delete static_cast<Sonicteam::System::Heap *>(m_allocator);
    m_allocator = 0;
    m_heap = 0;
    return heap;
}

void *EffectSystemUSA::Allocate(u32 size) {
    return static_cast<Sonicteam::System::Heap *>(m_allocator)->Allocate(0x10, size);
}

void EffectSystemUSA::Free(void *memory) {
    static_cast<Sonicteam::System::Heap *>(m_allocator)->Free(memory);
}

void EffectSystemUSA::Fill(void *memory, char value, u32 size) {
    memset(memory, value, size);
}

void EffectSystemUSA::Copy(void *to, const void *from, u32 size) {
    memcpy(to, from, size);
}

void EffectSystemUSA::Flush(void *memory) {
    fn_8040EE90();
}

void *EffectSystemUSA::GetCamera() {
    return fn_8000DCC0()->m_camera;
}

} // namespace Effect
} // namespace System
} // namespace SonicteamUSA

PooledListPool *PooledListPool::s_cached;
