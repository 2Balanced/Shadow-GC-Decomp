/* Effect::PJS::EffectTask and the effect draw tasks (class names and bases from RTTI): the task
 * that runs the PJS effects. EffectTask advances the effect manager every frame; the object it
 * owns sets up the effect heap and the manager's renderers and creates one task per draw pass
 * (opaque, punch-through, translucent, additive, subtractive, glare, shimmer, the lens flare,
 * blur and screen colour), each of which draws its pass in the task layer it hangs in. File name
 * from the class; all other names are inferred. Definitions are in reverse address order
 * (deferred inlining emits functions last-defined first). */
#define EFFECT_ELEMENT_POLYMORPHIC
#define RESOURCEONEFILE_POLYMORPHIC

#include "Effect/System/EffectElement.h"
#include "Game/Render.h"
#include "Game/ResourceOneFile.h"
#include "Game/Task.h"
#include "boost/scoped_ptr.hpp"
#include "size_assert.h"

namespace Effect {
namespace PJS {
class EffectManagerPJS;
} // namespace PJS
} // namespace Effect

using Effect::PJS::EffectManagerPJS;

/* The game state (as in Game/AdvBattleMode.cpp): the frames the last update took, their limit,
 * and the frame rate (names inferred). */
struct UnkGameState_8057D6A0 {
    /* 0x000 */ u8 field_0x0[0xC];
    /* 0x00C */ s32 m_frames;
    /* 0x010 */ u8 field_0x10[0x34 - 0x10];
    /* 0x034 */ s32 m_maxFrames;
    /* 0x038 */ u8 field_0x38[0x50 - 0x38];
    /* 0x050 */ s32 m_frameRate;
    /* 0x054 */ u8 field_0x54[0x150 - 0x54];
};
SIZE_ASSERT(UnkGameState_8057D6A0, 0x150);

extern UnkGameState_8057D6A0 lbl_8057D6A0;

/* The effect heap's memory (Game/DiscErrorHandler.cpp's). */
extern void *lbl_805F0B88;

/* One part of the lens flare: its texture and size, its place along the line through the screen
 * centre, its texture coordinates and its colour (layout from the table; names inferred). */
struct UnkLensFlarePart_8053C9C0 {
    /* 0x00 */ const char *m_texture;
    /* 0x04 */ s16 m_width;
    /* 0x06 */ s16 m_height;
    /* 0x08 */ float m_position;
    /* 0x0C */ float m_u0;
    /* 0x10 */ float m_u1;
    /* 0x14 */ float m_v0;
    /* 0x18 */ float m_v1;
    /* 0x1C */ u8 m_r;
    /* 0x1D */ u8 m_g;
    /* 0x1E */ u8 m_b;
    /* 0x1F */ u8 m_a;
    /* 0x20 */ u32 field_0x20;
};
SIZE_ASSERT(UnkLensFlarePart_8053C9C0, 0x24);

static UnkLensFlarePart_8053C9C0 s_lensFlareParts[7] = {
    {"ef_lensf", 0x80, 0x80, 1.0f, 0.0f, 0.5f, 0.0f, 0.5f, 0xFF, 0xFF, 0xFF, 0xFF, 0},
    {"ef_lensf", 0x80, 0x80, 0.6f, 0.5f, 1.0f, 0.0f, 0.5f, 0xFF, 0xFF, 0xFF, 0x80, 0},
    {"ef_lensf", 0x40, 0x40, 0.4f, 0.0f, 0.25f, 0.5f, 0.75f, 0xFF, 0xFF, 0xFF, 0x80, 0},
    {"ef_lensf", 0x40, 0x40, 0.2f, 0.25f, 0.5f, 0.5f, 0.75f, 0xFF, 0xFF, 0xFF, 0x80, 0},
    {"ef_lensf", 0x40, 0x40, -0.2f, 0.25f, 0.5f, 0.5f, 0.75f, 0xFF, 0xFF, 0xFF, 0x80, 0},
    {"ef_lensf", 0x40, 0x40, -0.4f, 0.0f, 0.25f, 0.75f, 1.0f, 0xFF, 0xFF, 0xFF, 0x80, 0},
    {"ef_lensf", 0x80, 0x80, -0.6f, 0.5f, 1.0f, 0.5f, 1.0f, 0xFF, 0xFF, 0xFF, 0x80, 0},
};

extern "C" {
/* The effect manager (see Game/EffectManagerPJS.h). */
EffectManagerPJS *fn_800A31F0(void);
/* Advances the effects by dt (behaviour inferred). */
void fn_80425294(EffectManagerPJS *manager, float dt);
/* Set up and release the manager's renderers (behaviour inferred). */
void fn_804325E8(EffectManagerPJS *manager);
void fn_80430A08(EffectManagerPJS *manager, int arg);
void fn_8042E1BC(EffectManagerPJS *manager, int arg);
void fn_804389F0(EffectManagerPJS *manager, int arg);
void fn_804322E0(EffectManagerPJS *manager, int arg);
void fn_80437088(EffectManagerPJS *manager, int arg);
void fn_8042554C(EffectManagerPJS *manager);
void fn_8043237C(EffectManagerPJS *manager);
void fn_80430944(EffectManagerPJS *manager);
void fn_8042E148(EffectManagerPJS *manager);
void fn_80438974(EffectManagerPJS *manager);
void fn_80432278(EffectManagerPJS *manager);
void fn_80437044(EffectManagerPJS *manager);
void fn_80432578(EffectManagerPJS *manager);
/* The draw passes (behaviour inferred from the tasks' names). */
void fn_80437018(EffectManagerPJS *manager, float dt);
void fn_80436FEC(EffectManagerPJS *manager);
void fn_80430918(EffectManagerPJS *manager);
void fn_804308EC(EffectManagerPJS *manager);
void fn_8042E0F0(EffectManagerPJS *manager);
void fn_8042E11C(EffectManagerPJS *manager);
void fn_80438948(EffectManagerPJS *manager);
void fn_80432148(EffectManagerPJS *manager, void *textures, UnkLensFlarePart_8053C9C0 *parts,
                 int count);
void fn_8043224C(EffectManagerPJS *manager);
void fn_80432220(EffectManagerPJS *manager, int arg);
void fn_8002774C(EffectManagerPJS *manager);
void fn_80027728(EffectManagerPJS *manager);
void fn_80027704(EffectManagerPJS *manager);
void fn_800276E0(EffectManagerPJS *manager);
void fn_800276BC(EffectManagerPJS *manager);
void fn_80027640(EffectManagerPJS *manager);
void fn_8002761C(EffectManagerPJS *manager);
void fn_804323F8(EffectManagerPJS *manager);

/* The kage effect's task (Game/EffectKage.cpp): creates and ends it. */
void fn_803566D4(void);
void fn_803566B0(void);

/* Allocates the effect heap's memory, and frees it (behaviour inferred). */
void *fn_80049B90(u32 size, int arg);
void fn_80049B34(void);

/* The resource loader (see Game/EffectPJS.cpp). */
s32 fn_800128D8(void *loader, ResourceOneFile *resource);
s32 fn_80012408(void *loader, s32 slot);

/* The task tree and its layer n (see Game/EffectPJS.cpp). */
Task *fn_8001C4F8(void);
Task *fn_801EB304(Task *root, int layer);
}

/* The render owner, whose words are the task layers of the draw passes (name inferred). */
inline UnkRenderOwner_80046178 *RenderOwner() {
    return fn_80046178();
}

/* Task layer n of the task tree (name inferred). */
inline Task *TaskLayer(int layer) {
    return fn_801EB304(fn_8001C4F8(), layer);
}

namespace Effect {
namespace PJS {

/* The draw tasks. Each one draws its pass of the effects every frame. */
class EffectDrawOpaqTask : public Task {
  public:
    EffectDrawOpaqTask() : Task((Task *)RenderOwner()->field_0x8) {
        m_name = "EffectDrawOpaq";
        m_flags |= 0x100;
    }
    virtual ~EffectDrawOpaqTask() {}
    virtual void Tick(float dt) {
        fn_8002774C(fn_800A31F0());
    }
};

class EffectDrawPunchTask : public Task {
  public:
    EffectDrawPunchTask() : Task(RenderOwner()->field_0x10) {
        m_name = "EffectDrawPunch";
        m_flags |= 0x100;
    }
    virtual ~EffectDrawPunchTask() {}
    virtual void Tick(float dt) {
        fn_80027728(fn_800A31F0());
    }
};

class EffectDrawTransTask : public Task {
  public:
    EffectDrawTransTask() : Task((Task *)RenderOwner()->field_0x18) {
        m_name = "EffectDrawTrans";
        m_flags |= 0x100;
    }
    virtual ~EffectDrawTransTask() {}
    virtual void Tick(float dt) {
        fn_80027704(fn_800A31F0());
    }
};

class EffectDrawAddTask : public Task {
  public:
    EffectDrawAddTask() : Task(RenderOwner()->field_0x20) {
        m_name = "EffectDrawAdd";
        m_flags |= 0x100;
    }
    virtual ~EffectDrawAddTask() {}
    virtual void Tick(float dt) {
        fn_800276E0(fn_800A31F0());
    }
};

class EffectDrawSubTask : public Task {
  public:
    EffectDrawSubTask() : Task(RenderOwner()->field_0x20) {
        m_name = "EffectDrawSub";
        m_flags |= 0x100;
    }
    virtual ~EffectDrawSubTask() {}
    virtual void Tick(float dt) {
        fn_800276BC(fn_800A31F0());
    }
};

class EffectDrawGlareTask : public Task {
  public:
    EffectDrawGlareTask() : Task(RenderOwner()->field_0x24) {
        m_name = "EffectDrawGlare";
        m_flags |= 0x100;
    }
    virtual ~EffectDrawGlareTask() {}
    virtual void Tick(float dt) {
        fn_80027640(fn_800A31F0());
    }
};

class EffectDrawShimmerTask : public Task {
  public:
    EffectDrawShimmerTask() : Task(RenderOwner()->field_0x2C) {
        m_name = "EffectDrawShimmer";
        m_flags |= 0x100;
    }
    virtual ~EffectDrawShimmerTask() {}
    virtual void Tick(float dt) {
        fn_8002761C(fn_800A31F0());
    }
};

/* Frees the effects' resources at the end of each frame (name inferred). */
class EffectDeleteResourceTask : public Task {
  public:
    EffectDeleteResourceTask() : Task((Task *)RenderOwner()->field_0x0) {
        m_name = "EffectDeleteResourceTask";
        m_flags |= 0x100;
    }
    virtual ~EffectDeleteResourceTask() {}
    virtual void Tick(float dt) {
        fn_804323F8(fn_800A31F0());
    }
};

class EffectPreGlareTask : public Task {
  public:
    EffectPreGlareTask() : Task(RenderOwner()->field_0x24) {
        m_name = "PreGlare";
        m_flags |= 0x100;
    }
    virtual ~EffectPreGlareTask() {}
    virtual void Tick(float dt) {
        fn_80430918(fn_800A31F0());
    }
};

class EffectPostGlareTask : public Task {
  public:
    EffectPostGlareTask() : Task(RenderOwner()->field_0x2C) {
        m_name = "PostGlare";
        m_flags |= 0x100;
    }
    virtual ~EffectPostGlareTask() {}
    virtual void Tick(float dt) {
        fn_804308EC(fn_800A31F0());
    }
};

class EffectSaveScreenTask : public Task {
  public:
    EffectSaveScreenTask() : Task(RenderOwner()->field_0x58) {
        m_name = "SaveScreen";
        m_flags |= 0x100;
    }
    virtual ~EffectSaveScreenTask() {}
    virtual void Tick(float dt) {
        fn_8042E0F0(fn_800A31F0());
    }
};

/* Blurs the screen saved by its EffectSaveScreenTask. */
class EffectDrawBlurTask : public Task {
  public:
    EffectDrawBlurTask() : Task(RenderOwner()->field_0x2C) {
        m_name = "DrawBlur";
        m_flags |= 0x100;
        m_saveScreen = new EffectSaveScreenTask();
    }
    virtual ~EffectDrawBlurTask() {
        if (m_saveScreen != 0) {
            m_saveScreen->m_flags |= 1;
        }
    }
    virtual void Tick(float dt) {
        fn_8042E11C(fn_800A31F0());
    }

    /* 0x28 */ Task *m_saveScreen;
    /* 0x2C */ u32 field_0x2C;
};
SIZE_ASSERT(EffectDrawBlurTask, 0x30);

class EffectDrawScreenColorTask : public Task {
  public:
    EffectDrawScreenColorTask() : Task(RenderOwner()->field_0x58) {
        m_name = "DrawScreenColor";
        m_flags |= 0x100;
    }
    virtual ~EffectDrawScreenColorTask() {}
    virtual void Tick(float dt) {
        fn_80438948(fn_800A31F0());
    }
};

/* Draws the second part of the lens flare. */
class EffectDrawGlobalLensFlareAnotherTask : public Task {
  public:
    EffectDrawGlobalLensFlareAnotherTask() : Task(RenderOwner()->field_0x24) {
        m_name = "DrawGlobalLensFlare(another)";
        m_flags |= 0x100;
    }
    virtual ~EffectDrawGlobalLensFlareAnotherTask() {}
    virtual void Tick(float dt) {
        fn_80432220(fn_800A31F0(), 0);
    }
};

/* Loads the lens flare's textures, hands them to the manager with the parts of the flare, and
 * draws the flare. */
class EffectDrawGlobalLensFlareTask : public Task {
  public:
    EffectDrawGlobalLensFlareTask() : Task(RenderOwner()->field_0x24) {
        m_name = "DrawGlobalLensFlare";
        m_flags |= 0x100;
        m_textures = 0;
        m_file = new ResourceOneFile("common/LensFlare.one", false);
        m_slot = fn_800128D8(fn_800122D4(), m_file);
    }
    virtual ~EffectDrawGlobalLensFlareTask() {
        if (m_file != 0) {
            fn_800127F8(fn_800122D4(), m_slot);
        }
        if (m_another != 0) {
            m_another->m_flags |= 1;
        }
    }
    virtual void Tick(float dt) {
        if (m_textures == 0) {
            if (fn_80012408(fn_800122D4(), m_slot) != 3) {
                return;
            }
            m_textures = m_file->GetFile("LENSFLARE.TXD");
            fn_80432148(fn_800A31F0(), m_textures, s_lensFlareParts, 7);
            m_another = new EffectDrawGlobalLensFlareAnotherTask();
        }
        fn_8043224C(fn_800A31F0());
        fn_80432220(fn_800A31F0(), 1);
    }

    /* 0x28 */ s32 m_slot;
    /* 0x2C */ ResourceOneFile *m_file;
    /* 0x30 */ void *m_textures;
    /* 0x34 */ Task *m_another;
};
SIZE_ASSERT(EffectDrawGlobalLensFlareTask, 0x38);

class EffectDrawFullScreenShimmerSubTask : public Task {
  public:
    EffectDrawFullScreenShimmerSubTask() : Task(TaskLayer(0x10)) {
        m_name = "FullScreenShimmerDrawSub";
    }
    virtual ~EffectDrawFullScreenShimmerSubTask() {}
    virtual void Tick(float dt) {
        fn_80437018(fn_800A31F0(), dt);
    }
};

/* Draws the shimmer over the whole screen, with a second task in task layer 16. */
class EffectDrawFullScreenShimmerTask : public Task {
  public:
    EffectDrawFullScreenShimmerTask(Task *parent) : Task(parent) {
        m_name = "FullScreenShimmerDraw";
        m_flags |= 0x100;
        m_sub = new EffectDrawFullScreenShimmerSubTask();
    }
    virtual ~EffectDrawFullScreenShimmerTask() {}
    virtual void Tick(float dt) {
        fn_80436FEC(fn_800A31F0());
    }

    /* 0x28 */ Task *m_sub;
    /* 0x2C */ u32 field_0x2C;
};
SIZE_ASSERT(EffectDrawFullScreenShimmerTask, 0x30);

} // namespace PJS
} // namespace Effect

using namespace Effect::PJS;

/* What EffectTask owns (class name unknown): it sets up the effect heap and the manager's
 * renderers, and creates the draw tasks. */
class UnkEffectSystem_801D167C {
  public:
    UnkEffectSystem_801D167C(Task *parent, u32 heapSize);
    ~UnkEffectSystem_801D167C();

    /* 0x00 */ Task *m_opaq;
    /* 0x04 */ Task *m_punch;
    /* 0x08 */ Task *m_trans;
    /* 0x0C */ Task *m_add;
    /* 0x10 */ Task *m_sub;
    /* 0x14 */ Task *m_glare;
    /* 0x18 */ Task *m_shimmer;
    /* 0x1C */ Task *m_preGlare;
    /* 0x20 */ Task *m_postGlare;
    /* 0x24 */ Task *m_blur;
    /* 0x28 */ Task *m_screenColor;
    /* 0x2C */ Task *m_lensFlare;
    /* 0x30 */ Task *m_deleteResource;
};
SIZE_ASSERT(UnkEffectSystem_801D167C, 0x34);

/* Deletes the object (name inferred; see the checked_delete specialization below). */
inline void DeleteEffectSystem(UnkEffectSystem_801D167C *system) {
    delete system;
}

/* Retail keeps boost::checked_delete<UnkEffectSystem_801D167C> out of line (0x801D20D4) and the
 * destructor at its own place (see docs/research-notes.md, EnergyCore); this specialization calls
 * an inline helper, which gives retail's code and order. */
namespace boost {
template <> inline void checked_delete<UnkEffectSystem_801D167C>(UnkEffectSystem_801D167C *x) {
    DeleteEffectSystem(x);
}
} // namespace boost

namespace Effect {
namespace PJS {

/* The task that runs the effects (task name "EffectManageTask"). */
class EffectTask : public Task {
  public:
    EffectTask(Task *parent, u32 heapSize);
    virtual ~EffectTask();
    virtual void Tick(float dt);

    /* 0x28 */ boost::scoped_ptr<UnkEffectSystem_801D167C> m_system;
};
SIZE_ASSERT(EffectTask, 0x2C);

EffectTask::EffectTask(Task *parent, u32 heapSize)
    : Task(parent), m_system(new UnkEffectSystem_801D167C(parent, heapSize)) {
    m_name = "EffectManageTask";
}

EffectTask::~EffectTask() {}

/* Advances the effects by the frames the last update took, at most m_maxFrames. */
void EffectTask::Tick(float dt) {
    s32 frames = lbl_8057D6A0.m_frames;
    if (frames > lbl_8057D6A0.m_maxFrames) {
        frames = lbl_8057D6A0.m_maxFrames;
    }
    float frameTime = 60.0f / lbl_8057D6A0.m_frameRate;
    fn_80425294(fn_800A31F0(), frameTime * frames);
}

} // namespace PJS
} // namespace Effect

/* Creates a full-screen shimmer task below parent (fn_801D1E64). */
extern "C" Task *fn_801D1E64(Task *parent) {
    return new EffectDrawFullScreenShimmerTask(parent);
}

UnkEffectSystem_801D167C::UnkEffectSystem_801D167C(Task *parent, u32 heapSize) {
    void *memory = fn_80049B90(heapSize, 1);
    lbl_805F0B88 = memory;
    Effect::System::fn_8042A5D0()->Initialize(memory, heapSize);
    fn_804325E8(fn_800A31F0());
    fn_80430A08(fn_800A31F0(), 0);
    fn_8042E1BC(fn_800A31F0(), 0);
    fn_804389F0(fn_800A31F0(), 0);
    fn_804322E0(fn_800A31F0(), 0);
    fn_80437088(fn_800A31F0(), 0);
    m_preGlare = new EffectPreGlareTask();
    m_postGlare = new EffectPostGlareTask();
    m_blur = new EffectDrawBlurTask();
    m_screenColor = new EffectDrawScreenColorTask();
    m_lensFlare = new EffectDrawGlobalLensFlareTask();
    m_opaq = new EffectDrawOpaqTask();
    m_punch = new EffectDrawPunchTask();
    m_trans = new EffectDrawTransTask();
    m_add = new EffectDrawAddTask();
    m_sub = new EffectDrawSubTask();
    m_glare = new EffectDrawGlareTask();
    m_shimmer = new EffectDrawShimmerTask();
    m_deleteResource = new EffectDeleteResourceTask();
    fn_803566D4();
}

UnkEffectSystem_801D167C::~UnkEffectSystem_801D167C() {
    fn_803566B0();
    m_opaq->m_flags |= 1;
    m_punch->m_flags |= 1;
    m_trans->m_flags |= 1;
    m_add->m_flags |= 1;
    m_sub->m_flags |= 1;
    m_glare->m_flags |= 1;
    m_shimmer->m_flags |= 1;
    m_preGlare->m_flags |= 1;
    m_postGlare->m_flags |= 1;
    m_blur->m_flags |= 1;
    m_screenColor->m_flags |= 1;
    m_lensFlare->m_flags |= 1;
    m_deleteResource->m_flags |= 1;
    fn_8042554C(fn_800A31F0());
    fn_8043237C(fn_800A31F0());
    fn_80430944(fn_800A31F0());
    fn_8042E148(fn_800A31F0());
    fn_80438974(fn_800A31F0());
    fn_80432278(fn_800A31F0());
    fn_80437044(fn_800A31F0());
    fn_80432578(fn_800A31F0());
    Effect::System::fn_8042A5D0()->Finalize();
    fn_80049B34();
}
