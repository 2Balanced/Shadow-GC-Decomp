/* BossRender (class name from RTTI): the task that shows a boss's model. BossCtrl creates it with
 * the boss's other tasks (0x8017A108). Its state is a separately allocated object (class name
 * unknown) that loads the model from the boss's tables into the world of its area, places it on
 * the character while no motion drives it, and fades it out while the boss is hidden and back in
 * otherwise. Member and function names are inferred. Definitions are in reverse address order
 * (deferred inlining emits functions last-defined first). */
#include "Game/BossCtrlUnit.h"
#include "Game/EnemyCharInfo.h"
#include "Game/MathTypes.h"
#include "Game/Task.h"
#include "boost/scoped_ptr.hpp"
#include "size_assert.h"

/* The RenderWare types used here, declared locally (layouts as in rw/rwcore.h, reduced to the
 * members used; as in Game/DevilDoomCtrlUnitPsychoObject.cpp). */
extern "C" {
typedef struct RwV3d {
    float x;
    float y;
    float z;
} RwV3d;

typedef struct RwMatrix {
    RwV3d right;
    u32 flags;
    RwV3d up;
    u32 pad1;
    RwV3d at;
    u32 pad2;
    RwV3d pos;
    u32 pad3;
} RwMatrix;

typedef struct RwObject {
    u8 type;
    u8 subKind;
    u8 flags;
    u8 privateFlags;
    void *parent;
} RwObject;

typedef struct RwFrame {
    /* 0x00 */ RwObject object;
    /* 0x08 */ u8 inDirtyListLink[0x8];
    /* 0x10 */ RwMatrix modelling;
} RwFrame;

typedef struct RpClump {
    /* 0x00 */ RwObject object; /* the parent is the clump's frame */
} RpClump;
}

/* The tables a boss's BossVarParam points to (struct name unknown; see Game/DevilDoom.cpp). */
struct UnkBossDef {
    /* 0x00 */ s32 m_clump; /* the boss's clump (slot 0x16 of the loader) */
};

/* A clump's colour fade (struct name unknown; see Game/DevilDoomCtrlUnitPsychoObject.cpp). */
struct UnkClumpFade_80027D5C {
    /* 0x00 */ float m_color[4];
    /* 0x10 */ u8 field_0x10[0x24 - 0x10];
    /* 0x24 */ u8 m_mode;
    /* 0x25 */ u8 field_0x25[0x2C - 0x25];
};
SIZE_ASSERT(UnkClumpFade_80027D5C, 0x2C);

/* The game state (struct name unknown; see Game/BossMotion.cpp): the frame time at 0x20. */
struct UnkGameState_8057D6A0 {
    /* 0x000 */ u8 field_0x0[0x20];
    /* 0x020 */ float m_frameTime;
    /* 0x024 */ u8 field_0x24[0x150 - 0x24];
};
extern UnkGameState_8057D6A0 lbl_8057D6A0;

/* The world of each stage area (see Game/DevilDoomCtrlUnitPsychoObject.cpp). */
struct UnkWorldEntry_8051D208 {
    /* 0x00 */ u8 field_0x0[0x10];
    /* 0x10 */ void *m_world;
    /* 0x14 */ u8 field_0x14[0x18 - 0x14];
};
extern UnkWorldEntry_8051D208 lbl_8051D208[];

extern "C" {
/* The resource loader (see Game/DevilDoomCtrlUnit.cpp). */
void *fn_800122D4(void);
void *fn_8001277C(void *loader, s32 slot, int arg2);

/* RenderWare clumps and frames (see Game/DevilDoomCtrlUnitPsychoObject.cpp). */
void *fn_8046256C(RpClump *clump);
void fn_804623EC(void *world, RpClump *clump);
void fn_804622FC(void *world, RpClump *clump);
RpClump *fn_80457090(void *data);
void fn_80457468(RpClump *clump);
void *fn_80169364(RpClump *clump);
int fn_80169350(void *frame);
void fn_8003A840(int area);
void fn_8047EBA0(RwMatrix *matrix);
RwFrame *fn_8048722C(RwFrame *frame);

/* The clump's fade (see Game/DevilDoomCtrlUnitPsychoObject.cpp). */
UnkClumpFade_80027D5C *fn_80027D5C(UnkClumpFade_80027D5C *fade);
void fn_80038198(RpClump *clump, UnkClumpFade_80027D5C *fade);
}

/* Puts clump in world (name inferred; as in Game/DevilDoomCtrlUnitPsychoObject.cpp). */
inline void SetClumpWorld(RpClump *clump, void *world) {
    void *current = fn_8046256C(clump);
    if (current != world) {
        if (current != 0) {
            fn_804623EC(current, clump);
        }
        fn_804622FC(world, clump);
    }
}

class BossRender : public Task {
  public:
    class Impl;

    BossRender(Task *parent, BossVarParam *param);
    virtual ~BossRender();
    virtual void Tick(float dt);

    /* 0x28 */ boost::scoped_ptr<Impl> m_impl;
    /* 0x2C */ u32 pad_0x2C; /* the allocation is 0x30 bytes */
};

class BossRender::Impl {
  public:
    Impl(BossVarParam *param);
    ~Impl() {
        if (m_clump) {
            void *world = fn_8046256C(m_clump);
            if (world != 0) {
                fn_804623EC(world, m_clump);
            }
            fn_80457468(m_clump);
            m_clump = 0;
            m_frame = 0;
            m_param->m_clump = 0;
        }
        if (m_fade) {
            delete m_fade;
        }
    }
    void Update(float dt);

    /* 0x0 */ RpClump *m_clump;
    /* 0x4 */ RwFrame *m_frame;
    /* 0x8 */ UnkClumpFade_80027D5C *m_fade;
    /* 0xC */ BossVarParam *m_param;
};
SIZE_ASSERT(BossRender::Impl, 0x10);
SIZE_ASSERT(BossRender, 0x30);

BossRender::Impl::Impl(BossVarParam *param) : m_clump(0), m_frame(0), m_fade(0), m_param(param) {
    void *data = fn_8001277C(fn_800122D4(), 0x16, m_param->m_def->m_clump);
    if (data != 0) {
        m_clump = fn_80457090(data);
    }
    if (m_clump != 0) {
        int area = fn_80169350(fn_80169364(m_clump));
        fn_8003A840(area);
        SetClumpWorld(m_clump, lbl_8051D208[area].m_world);
        m_frame = (RwFrame *)m_clump->object.parent;
        UnkClumpFade_80027D5C *fade =
            (UnkClumpFade_80027D5C *)operator new(sizeof(UnkClumpFade_80027D5C));
        if (fade != 0) {
            fade = fn_80027D5C(fade);
        }
        m_fade = fade;
        m_param->m_alpha = 1.0f;
        if (m_fade != 0) {
            m_fade->m_mode = 8;
            m_fade->m_color[3] = m_param->m_alpha;
            fn_80038198(m_clump, m_fade);
        }
    }
    m_param->m_clump = m_clump;
    m_param->m_fade = m_fade;
}

void BossRender::Impl::Update(float dt) {
    if (!m_param->m_motion && m_frame) {
        Mtx34f matrix;
        fn_8040C148(&matrix, &lbl_805E0720);
        fn_8040B59C(&matrix, &m_param->m_chara->m_position);
        fn_8040B80C(&matrix, m_param->m_chara->m_rotation.z);
        fn_8040B848(&matrix, m_param->m_chara->m_rotation.y);
        fn_8040B884(&matrix, m_param->m_chara->m_rotation.x);
        RwMatrix *modelling = &m_frame->modelling;
        modelling->right.x = matrix.values[0][0];
        modelling->right.y = matrix.values[1][0];
        modelling->right.z = matrix.values[2][0];
        modelling->up.x = matrix.values[0][1];
        modelling->up.y = matrix.values[1][1];
        modelling->up.z = matrix.values[2][1];
        modelling->at.x = matrix.values[0][2];
        modelling->at.y = matrix.values[1][2];
        modelling->at.z = matrix.values[2][2];
        modelling->pos.x = matrix.values[0][3];
        modelling->pos.y = matrix.values[1][3];
        modelling->pos.z = matrix.values[2][3];
        modelling->pad3 = 0;
        modelling->pad2 = 0;
        modelling->pad1 = 0;
        modelling->flags = 0;
        fn_8047EBA0(modelling);
        fn_8048722C(m_frame);
    }
    if (m_fade) {
        if (m_param->m_hidden) {
            m_param->m_alpha -= dt;
            if (m_param->m_alpha < 0.0f) {
                m_param->m_alpha = 0.0f;
                void *world = fn_8046256C(m_clump);
                if (world != 0) {
                    fn_804623EC(world, m_clump);
                }
            }
        } else {
            m_param->m_alpha += dt;
            if (m_param->m_alpha > 1.0f) {
                m_param->m_alpha = 1.0f;
            }
            if (!fn_8046256C(m_clump)) {
                int area = fn_80169350(fn_80169364(m_clump));
                fn_8003A840(area);
                SetClumpWorld(m_clump, lbl_8051D208[area].m_world);
            }
        }
        m_fade->m_color[3] = m_param->m_alpha;
    }
}

BossRender::BossRender(Task *parent, BossVarParam *param) : Task(parent), m_impl(new Impl(param)) {
    m_name = "Render";
}

BossRender::~BossRender() {}

void BossRender::Tick(float dt) {
    m_impl->Update(lbl_8057D6A0.m_frameTime);
}
