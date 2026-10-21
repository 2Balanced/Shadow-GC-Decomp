/* Gong::Gong (class name from RTTI): the GONG gadget of Circus Park, a gong on a stand (GONG.BIN,
 * three models: the stand, the gong and the striker). A character that touches the striker
 * (model 2) is bounced straight up and the gong rings; one that touches the gong (model 1) is
 * launched along the stand's facing at a speed that grows with how fast it came in, scaled by the
 * "speed" parameter. The first ring also spawns a Gong::CSD::GongCSD (class name from RTTI), a
 * task that plays the "PR0302RINGANM" 2D animation over the screen; other gadgets spawn it as well
 * through fn_80136B5C. File name inferred from the class. Method names marked "(name inferred)"
 * are not original. Definitions are in reverse address order (deferred inlining emits functions
 * last-defined first). */
#include <boost/bind.hpp>
#include <boost/function.hpp>

#include "Game/TNumEdit.h"
/* This unit's .rodata is kept whole (the GongCSD constructor addresses it through one base), and
 * retail has two unreferenced constant blocks of the original headers in it: 0x1C zero bytes after
 * the TNumEdit names and a zero vector after the CharaColliBase names (their meaning and types are
 * unknown; the bytes are reproduced, as in DefenseSystem.cpp). */
static const float lbl_804BF4C4[7] = {0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f};
/* After TNumEdit.h: the order of the casts sets the order of the RTTI names in .rodata. */
#include "Game/CharaColliBaseCharInfo.h"
static const float lbl_804BF550[3] = {0.0f, 0.0f, 0.0f};
#include "Game/CharCommand.h"
#include "Game/CharInfo.h"
#include "Game/Gadget.h"
#include "Game/GadgetCharInfo.h"
#include "Game/GadgetCollision.h"
#include "Game/GadgetModelInfo.h"
#include "Game/GadgetClassDesc.h"
#include "Game/GadgetUtil.h"
#include "Game/Render.h"
#include "Game/FileQueue.h"
#include "boost/shared_ptr.hpp"
#include "boost/weak_ptr.hpp"
#include "size_assert.h"

namespace Chao {
namespace CSD {

/* CScene (class name from the RTTI of RCPtr<CScene>::RCObject<CScene>): a scene of a 2D CSD
 * project. Only its address is used here. */
class CScene;

/* RCPtr (class name from RTTI): the CSD library's reference-counted pointer. It points at a
 * counted RCObject whose first word is the object (layout from the inlined Get). */
template <class T>
class RCPtr {
  public:
    struct RCObject {
        T *m_object;
    };

    RCPtr(const RCPtr &other);
    ~RCPtr();

    T *Get() const {
        return m_object != 0 ? m_object->m_object : 0;
    }

    RCObject *m_object;
};

} // namespace CSD
} // namespace Chao

using Chao::CSD::CScene;
using Chao::CSD::RCPtr;

/* The CSD memory allocator fn_803605AC returns (name inferred). Slot 0x14 allocates size bytes,
 * recording the source file and line. */
class UnkCsdAllocator_803605AC {
  public:
    virtual void vf_0x08();
    virtual void vf_0x0C();
    virtual void vf_0x10();
    virtual void *Alloc(u32 size, const char *file, int line);
};

extern "C" UnkCsdAllocator_803605AC *fn_803605AC(void);

/* The 0x4C-byte CSD project the GongCSD plays (name inferred); fn_800BFF30 constructs it from a
 * loaded resource. Its allocation goes through the CSD allocator (the class operator new of the
 * CSD headers, csdBase.h line 40). */
class UnkCsdProject_800BFF30 {
  public:
    UnkCsdProject_800BFF30(void *resource);
    virtual ~UnkCsdProject_800BFF30();

    /* 0x04 */ u8 field_0x4[0x4C - 0x4];

    static void *operator new(u32 size) {
        return fn_803605AC()->Alloc(size, "csdBase.h", 40);
    }
};

extern "C" {
/* Advances and draws the project (behaviour inferred). */
void fn_8036DA1C(UnkCsdProject_800BFF30 *project);
void fn_8036D94C(UnkCsdProject_800BFF30 *project);
/* Releases the project's scenes (behaviour inferred). */
void fn_8036C248(UnkCsdProject_800BFF30 *project);
/* The project's scene name, and a new scene name played kind times (behaviour inferred). */
RCPtr<CScene> fn_8036D660(UnkCsdProject_800BFF30 *project, const char *name);
RCPtr<CScene> fn_8036D890(UnkCsdProject_800BFF30 *project, const char *name, int arg2, int arg3);
/* A scene's end frame and current frame (behaviour inferred). */
float fn_80366728(CScene *scene);
float fn_80366738(CScene *scene);

/* The resource manager, the language-specific path of a file, and the loaded resource (behaviour
 * inferred, see CircusBall.cpp). */
void *fn_800122D4(void);
void *fn_8003F8D4(void);
const char *fn_8004237C(void *arg0, const char *name, int arg2);
void *fn_80012700(void *manager, int arg1, const char *fileName);
}

/* The render owner fn_80046178 returns, as far as this unit reads it (the word at 0x38 is passed
 * when creating the Render; meaning unknown). */
struct UnkRenderOwnerGong_80046178 {
    /* 0x00 */ u8 field_0x0[0x38];
    /* 0x38 */ void *field_0x38;
};

extern "C" {
/* A 32-bit flag word: fn_80014E04 constructs it clear, fn_80014E1C sets bit, fn_8006127C clears
 * bit and fn_80014E34 tests bit (behaviour inferred from their bodies). */
void fn_80014E04(u32 *flags);
void fn_80014E1C(u32 *flags, int bit);
void fn_8006127C(u32 *flags, int bit);
bool fn_80014E34(u32 *flags, int bit);

/* Loads model file fileName (behaviour inferred; arg0 unknown). */
boost::shared_ptr<GadgetModelInfo> fn_8015F800(int arg0, const char *fileName);
/* Attaches info to the model's variant (behaviour inferred, see BreakObj.cpp). */
bool fn_80161FC8(GadgetModelInfo *model, int variant, const boost::weak_ptr<CharInfo> &info);
/* A model call taking a float (behaviour unknown). */
void fn_80161308(GadgetModelInfo *model, float arg1);
/* Builds the matrix that places the character info (behaviour inferred). */
void fn_8015CEA0(GadgetCharInfo *charInfo, Mtx34f *matrix);
/* Like fn_8015B004: calls callback with every character in the collision event (behaviour
 * inferred). */
void fn_8015AA84(ColliDetectArgs &args, void (*callback)(boost::shared_ptr<CharInfo> hit, void *user),
                 void *user);
/* Tells the character it was launched (behaviour inferred; see FireRing.cpp). */
void fn_8015F6EC(boost::shared_ptr<CharInfo> character, int arg1, int arg2, int arg3);
void fn_80136B5C(int kind);
}

namespace Gong {

namespace CSD {

/* The 2D ring animation task (name from RTTI). */
class GongCSD : public Task {
  public:
    GongCSD(Task *parent, int kind);
    virtual ~GongCSD();
    virtual void Tick(float dt);

    /* The Render's draw callback (name inferred). */
    void Draw(RednerDrawArgs &args);

    /* 0x28 */ boost::shared_ptr<Render> m_render;
    /* 0x30 */ UnkCsdProject_800BFF30 *m_project;
    /* 0x34 */ float m_timer; /* the time to the next tick sound (inferred) */
    /* 0x38 */ u32 m_tickFlags; /* bit 0: tick (inferred) */
    /* 0x3C */ u32 pad_0x3C;  /* the class size is a multiple of 8 */
};

} // namespace CSD

/* The parameter the owner supplies (name from the parameter table). */
struct GongParams {
    GongParams() : speed(0.0f) {}

    /* 0x0 */ float speed;
};

class Gong : public Gadget {
  public:
    Gong(Task *parent, UnkPlacement_800C9CD0 *owner);
    virtual ~Gong();
    virtual void Behave(float dt);
    virtual void ReloadPlacement(UnkPlacement_800C9CD0 *owner);

    /* Creates the gong under task layer 11 (name inferred). */
    static void Instantiate(UnkPlacement_800C9CD0 *owner);

    /* Copies the owner's position, rotation and parameter (name inferred). */
    void LoadFromOwner(UnkPlacement_800C9CD0 *owner);
    /* Places the three models (name inferred). */
    void RebuildTransform();
    /* The collision callbacks of the gong and of the striker (names inferred). */
    void OnGongColliDetect(ColliDetectArgs &args);
    void OnStrikerColliDetect(ColliDetectArgs &args);

    static const char *s_taskName;
    static const char *s_fileName;

    /* 0x30 */ boost::shared_ptr<GadgetCharInfo> m_chara;
    /* 0x38 */ boost::shared_ptr<GadgetModelInfo> m_stand;
    /* 0x40 */ boost::shared_ptr<GadgetModelInfo> m_gong;
    /* 0x48 */ boost::shared_ptr<GadgetModelInfo> m_striker;
    /* 0x50 */ boost::shared_ptr<UnkHitList_801695B8> m_hitList;
    /* 0x58 */ GongParams m_params;
    /* 0x5C */ s32 m_state;
    /* 0x60 */ s32 m_gongHits;   /* the times the gong was hit (inferred) */
    /* 0x64 */ u32 m_rangFlags;  /* bit 0: the ring animation was spawned (inferred) */
};

/* What the collision callbacks pass to the per-character hit functions (name inferred). */
struct GongHitContext {
    GongHitContext() : m_params(0), m_gongHits(0), m_rangFlags(0), m_hit(false) {}

    /* 0x00 */ boost::shared_ptr<UnkHitList_801695B8> m_hitList;
    /* 0x08 */ boost::weak_ptr<GadgetCharInfo> m_chara;
    /* 0x10 */ GongParams *m_params;
    /* 0x14 */ Vec3f m_position;
    /* 0x20 */ s32 m_gongHits;
    /* 0x24 */ u32 m_rangFlags;
    /* 0x28 */ bool m_hit;
};

} // namespace Gong

SIZE_ASSERT(Gong::CSD::GongCSD, 0x40);
SIZE_ASSERT(Gong::Gong, 0x68);
SIZE_ASSERT(Gong::GongHitContext, 0x2C);

namespace Gong {

const char *Gong::s_fileName = "GONG.BIN";
const char *Gong::s_taskName = "Gong";

/* Not original: an unused table that pools the rest of the unit's .rodata strings in retail's
 * order (the linker drops the table; the RTTI names and literals below reuse these copies). The
 * kept-whole .rodata also holds strings of inline code and RTTI of the CSD library headers
 * (csdBase.h, csdRCObject.h: reference-counted pointers, their deallocation functors and the
 * resource base of a project), which this unit does not reconstruct; GC/2.7 would also emit the
 * strings it does generate (bind_t, exception and Gong RTTI names) in a different order. */
static const char *s_rodataPool[] = {
    "boost::_bi::bind_t<void, boost::_mfi::mf1<void, Gong::Gong, ColliDetectArgs &>, boost::_bi::list2<boost::_bi::value<Gong::Gong *>, boost::arg<1>>>",
    "boost::_bi::bind_t<void, boost::_mfi::mf1<void, Gong::CSD::GongCSD, RednerDrawArgs &>, boost::_bi::list2<boost::_bi::value<Gong::CSD::GongCSD *>, boost::arg<1>>>",
    "Gong::CSD::GongCSD",
    "PR0302RINGANM",
    "Scene_0000",
    "Not inititalized m_eDealloctor\n",
    "Memory allocation failure",
    "csdBase.h",
    "csdRCObject.h",
    "std::exception",
    "exception",
    "boost::bad_weak_ptr",
    "Chao::CSD::RCPtr<unsigned char>::RCObject<unsigned char>::CFunctorBase",
    "Chao::CSD::CBase",
    "Chao::CSD::RCPtr<unsigned char>::RCObject<unsigned char>",
    "Chao::CSD::CResourceBase<Chao::CSD::Project>",
    "Chao::CSD::RCPtr<Chao::CSD::CTexList>::RCObject<Chao::CSD::CTexList>::CFunctorBase",
    "Chao::CSD::RCPtr<Chao::CSD::CTexList>::RCObject<Chao::CSD::CTexList>",
    "boost::detail::sp_counted_base",
    "Chao::CSD::RCPtr<Chao::CSD::CScene>::RCObject<Chao::CSD::CScene>::CFunctorBase",
    "Chao::CSD::RCPtr<Chao::CSD::CScene>::RCObject<Chao::CSD::CScene>",
    "Gong::Gong",
};

/* Launches hit off the gong along the stand's facing (name inferred). */
static void HitGong(boost::shared_ptr<CharInfo> hit, void *user) {
    Mtx34f matrix;
    Vec3f from;
    Vec3f to;
    Vec3f position;
    Vec3f velocity;
    Vec3f offset;
    GongHitContext *context = static_cast<GongHitContext *>(user);
    boost::shared_ptr<GadgetCharInfo> self = context->m_chara.lock();

    if (self.get() == 0) {
        return;
    }
    if (context->m_hitList->fn_801697CC(boost::weak_ptr<CharInfo>(hit))) {
        return;
    }

    hit->PushCommand(new Player::IgnoreInputCommand(1, 0));
    hit->PushCommand(new ClrVelocityCommand(4));
    hit->PushCommand(new Player::LaunchCommand(1, 4));

    float speed;
    if (context->m_gongHits > 0) {
        speed = 560.0f;
    } else {
        float rate = fn_8040CF50(&hit->field_0x18) / context->m_params->speed;
        if (rate >= 1.0f) {
            rate = 1.0f;
        }
        fn_800091EC(&from, &hit->m_position);
        from.y = 0.0f;
        fn_800091EC(&to, &self->m_position);
        to.y = 0.0f;
        fn_8040CD74(&offset, &from, &to);
        float distance = fn_8040CF50(&offset);
        float scale;
        if (distance <= 5.0f) {
            scale = 1.0f;
        } else if (distance >= 50.0f) {
            scale = 0.4f;
        } else {
            scale = 0.4f + (0.6f - distance / 50.0f);
        }
        speed = scale * (670.0f * rate);
        if (speed < 560.0f) {
            speed = 560.0f;
        }
    }

    fn_800091EC(&position, &context->m_position);
    position.y += 5.0f;
    hit->PushCommand(new SetPositionCommand(position, 4));

    fn_8040C148(&matrix, &lbl_805E0720);
    fn_8040B848(&matrix, self->m_rotation.y);
    fn_8040B884(&matrix, self->m_rotation.x);
    fn_8040B80C(&matrix, self->m_rotation.z);

    velocity.x = 0.0f;
    velocity.y = 1.0f;
    velocity.z = 0.0f;
    AssignVector(velocity, fn_8040B67C(&matrix, &velocity));
    fn_8040CE38(&velocity, speed);
    hit->PushCommand(new SetVelocityCommand(velocity, 4));
    fn_801D560C(fn_80053164(), 0xE00F, 0, 0);
    hit->PushCommand(new Player::LaunchCommand(1, 4));

    fn_80169BB0(boost::weak_ptr<CharInfo>(hit), 5);
    context->m_hit = true;
}

/* Bounces hit straight up off the striker (name inferred). */
static void HitStriker(boost::shared_ptr<CharInfo> hit, void *user) {
    GongHitContext *context = static_cast<GongHitContext *>(user);

    if (context != 0 && !context->m_hit) {
        boost::shared_ptr<GadgetCharInfo> self = context->m_chara.lock();
        if (self.get() == 0) {
            return;
        }

        hit->PushCommand(new Player::IgnoreInputCommand(1, 0));
        hit->PushCommand(new ClrVelocityCommand(4));
        hit->PushCommand(new SetPositionCommand(context->m_position, 4));
        Vec3f velocity(0.0f, 400.0f, 0.0f);
        hit->PushCommand(new SetVelocityCommand(velocity, 4));
        hit->PushCommand(new Player::LaunchCommand(1, 4));
        fn_801D560C(fn_80053164(), 0x743E, 0, 0);

        if ((context->m_rangFlags & 1) == 0) {
            fn_8015F6EC(hit, 0, 50, 0);
            context->m_rangFlags |= 1;
            fn_80136B5C(4);
        }

        fn_80169BB0(boost::weak_ptr<CharInfo>(hit), 1);
        context->m_hit = true;
    }
}

Gong::Gong(Task *parent, UnkPlacement_800C9CD0 *owner) : Gadget(parent, owner) {
    m_name = s_taskName;
    m_chara = fn_8015CB88(0x4A, 4, "Gong");
    LoadFromOwner(owner);

    m_stand = fn_8015F800(1, s_fileName);
    m_stand->fn_80162A68(0, 0);
    fn_80161FC8(m_stand.get(), 0, m_chara);

    m_striker = fn_8015F800(1, s_fileName);
    m_striker->fn_80162A68(2, 0);
    fn_80161FC8(m_striker.get(), 2, m_chara);
    fn_800D0B64(m_striker->field_0x4, boost::bind(&Gong::OnStrikerColliDetect, this, _1));
    m_striker->fn_80160C28(1);
    m_striker->fn_8015FF60();

    m_gong = fn_8015F800(1, s_fileName);
    m_gong->fn_80162A68(1, 0);
    fn_80161FC8(m_gong.get(), 1, m_chara);
    fn_800D0B64(m_gong->field_0x4, boost::bind(&Gong::OnGongColliDetect, this, _1));
    m_gong->fn_80160C28(0);
    m_gong->fn_8015FF60();

    RebuildTransform();
    m_hitList = fn_801695B8();
}

Gong::~Gong() {
    fn_8016952C(&m_hitList);
}

void Gong::Behave(float dt) {
    if (m_hitList.get() != 0) {
        m_hitList->fn_801697F0();
    }
    if (m_stand.get() != 0) {
        m_stand->fn_80162FB0(dt);
    }
    if (m_gong.get() != 0) {
        m_gong->fn_80162FB0(dt);
    }
    if (m_striker.get() != 0) {
        m_striker->fn_80162FB0(dt);
    }

    switch (m_state) {
    case 0: m_state = 1; break; case 1: break; case 2: break;
    }

    RebuildTransform();
}

void Gong::ReloadPlacement(UnkPlacement_800C9CD0 *owner) {
    LoadFromOwner(owner);
    RebuildTransform();
    if (m_stand.get() != 0) {
        m_stand->fn_8016108C();
    }
    if (m_gong.get() != 0) {
        m_gong->fn_80162EF4();
        m_gong->fn_8016108C();
    }
    if (m_striker.get() != 0) {
        m_striker->fn_80162EF4();
        m_striker->fn_8016108C();
    }
}

void Gong::LoadFromOwner(UnkPlacement_800C9CD0 *owner) {
    fn_800C9F14(owner, &m_chara->m_position);
    fn_800C9ED4(owner, &m_chara->m_rotation);
    fn_800C9DBC(owner, &m_params, sizeof(m_params));
    m_state = 0;
    m_gongHits = 0;
    m_rangFlags = 0;
}

void Gong::RebuildTransform() {
    if (m_stand.get() != 0 && m_gong.get() != 0 && m_striker.get() != 0) {
        Mtx34f matrix;

        fn_8015CEA0(m_chara.get(), &matrix);
        m_stand->fn_801633DC(&matrix);
        m_gong->fn_801633DC(&matrix);
        m_striker->fn_801633DC(&matrix);
    }
}

void Gong::OnGongColliDetect(ColliDetectArgs &args) {
    GongHitContext context;

    context.m_hitList = m_hitList;
    context.m_chara = m_chara;
    context.m_params = &m_params;
    fn_800091D0(&context.m_position, &m_chara->m_position);
    context.m_gongHits = m_gongHits;
    fn_8015AA84(args, HitGong, &context);
    if (context.m_hit) {
        if (m_gong.get() != 0) {
            m_gong->fn_80160B18(0);
        }
        m_gongHits++;
    }
}

void Gong::OnStrikerColliDetect(ColliDetectArgs &args) {
    GongHitContext context;

    context.m_chara = m_chara;
    context.m_params = &m_params;
    context.m_rangFlags = m_rangFlags;
    fn_800091D0(&context.m_position, &m_chara->m_position);
    context.m_position.y += 300.0f;
    fn_8015B004(args, HitStriker, &context);
    if (context.m_hit) {
        if (m_striker.get() != 0) {
            fn_80161308(m_striker.get(), 1.0f);
            m_striker->fn_80160B18(1);
        }
        if (context.m_rangFlags & 1) {
            m_rangFlags |= 1;
        }
    }
}

namespace CSD {

GongCSD::GongCSD(Task *parent, int kind) : Task(parent) {
    fn_80014E04(&m_tickFlags);
    m_render = fn_8004DFB0(
        reinterpret_cast<UnkRenderOwnerGong_80046178 *>(fn_80046178())->field_0x38, "GongCSD");
    fn_80062CD0(&m_render->m_drawCallbacks, boost::bind(&GongCSD::Draw, this, _1));
    void *manager = fn_800122D4();
    void *resource = fn_80012700(manager, 1, fn_8004237C(fn_8003F8D4(), "PR0302RINGANM", 1));
    m_project = new UnkCsdProject_800BFF30(resource);
    fn_8036D890(m_project, "Scene_0000", kind, 0);
    m_timer = 0.0f;
    if (kind == 0 || kind == 5) {
        fn_8006127C(&m_tickFlags, 0);
    } else {
        fn_80014E1C(&m_tickFlags, 0);
    }
}

GongCSD::~GongCSD() {
    if (m_project != 0) {
        fn_8036C248(m_project);
        delete m_project;
        m_project = 0;
    }
}

void GongCSD::Tick(float dt) {
    if (m_project != 0) {
        fn_8036DA1C(m_project);
        RCPtr<CScene> scene = fn_8036D660(m_project, "Scene_0000");
        CScene *current = scene.Get();
        float end = fn_80366728(scene.Get()) - 0.01f;
        if (fn_80366738(current) > end) {
            m_flags |= 1;
            fn_801D560C(fn_80053164(), 0x743A, 0, 0);
        } else if (fn_80014E34(&m_tickFlags, 0)) {
            m_timer -= dt;
            if (m_timer <= 0.0f) {
                m_timer = 0.2f;
                fn_801D560C(fn_80053164(), 0xE00B, 0, 0);
            }
        }
    }
}

void GongCSD::Draw(RednerDrawArgs &args) {
    if (m_project != 0) {
        fn_8036D94C(m_project);
    }
}

} // namespace CSD

} // namespace Gong

/* Spawns the ring animation for kind, except kind 5 (name unknown; FireRing and the shooting
 * game call it too). */
extern "C" void fn_80136B5C(int kind) {
    if (kind != 5) {
        Task *parent = fn_801EB304(fn_8001C4F8(), 11);
        new Gong::CSD::GongCSD(parent, kind);
    }
}

extern "C" void fn_80136B14(void) {
    fn_8006B474("GONG.BIN", 1);
    fn_8006B474(fn_8004237C(fn_8003F8D4(), "PR0302RINGANM", 1), 0);
}

extern "C" void fn_80136B10(void) {}

void Gong::Gong::Instantiate(UnkPlacement_800C9CD0 *owner) {
    new Gong(fn_801EB304(fn_8001C4F8(), 11), owner);
}

static GadgetParamDesc lbl_80528C44[] = {
    {4, "speed", 0, 0, 0, 0.0f, -65536.0f, 65536.0f, fn_800CBC20, 0},
    {0},
};

GadgetClassDesc lbl_80528C94 = {
    "GONG", fn_80136B14, fn_80136B10, Gong::Gong::Instantiate, 0, 0, 0xC81, 1, 0, 10, lbl_80528C44,
};
