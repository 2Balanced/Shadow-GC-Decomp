/* Player::Shadow::Super (class name from RTTI): the task that turns Shadow into Super Shadow. The
 * player creates it (0x800A2B78) and ends it (0x800A2B44, through Finish). Its state is a separately
 * allocated object (class name unknown) that loads the Super Shadow character file (file 0xE),
 * marks the player's state bits, hands the player a Chaos Spear with 999 rounds, and once the file
 * is loaded builds the extra models: four that follow the player's motions, two placed on a
 * skeleton node, and five of which the one the motion selects is shown. It hides them with the
 * player's weapon and fades them out when the task ends. File name inferred from the class (as
 * PlayerShadowWeapon.cpp). Member and function names are inferred. Definitions are in reverse
 * address order (deferred inlining emits functions last-defined first). */
#include "Game/CharInfo.h"
#include "Game/CharacterFiles.h"
#include "Game/MathTypes.h"
#include "Game/Player.h"
#include "Game/PlayerWeaponBase.h"
#include "Game/Task.h"
#include "Game/WeaponBase.h"
#include "boost/scoped_ptr.hpp"
#include "boost/shared_ptr.hpp"
#include "boost/weak_ptr.hpp"
#include "size_assert.h"

/* The RenderWare types used here, declared locally (layouts as in rw/rwcore.h, reduced to the
 * members used; as in Game/BossRender.cpp). */
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

struct _stMtnPlayWork;

/* An object that keeps a clump in step with a motion (class name unknown; as in
 * Game/DevilDoomCtrlUnit.h). */
class UnkClumpUpdater_80015A7C {
  public:
    UnkClumpUpdater_80015A7C(RpClump *clump, _stMtnPlayWork *motion);
    ~UnkClumpUpdater_80015A7C();

    /* Emitted out of line (0x8027E4D8) where its result feeds the inline RemoveFromWorld. */
    RpClump *GetClump() {
        return m_clump;
    }

    /* 0x00 */ RpClump *m_clump;
    /* 0x04 */ u8 field_0x4[0x1C - 0x4];
};

/* A colour fade applied to a clump's materials (class name unknown; as in
 * Game/DevilDoomCtrlUnitPsychoObject.cpp): the alpha at 0xC. */
struct UnkClumpFade_80027D5C {
    /* 0x00 */ float m_color[4];
    /* 0x10 */ u8 field_0x10[0x2C - 0x10];
};

/* The world of each stage area (layout as in Game/DevilDoomCtrlUnit.cpp). */
struct UnkWorldEntry_8051D208 {
    /* 0x00 */ u8 field_0x0[0x10];
    /* 0x10 */ void *m_world;
    /* 0x14 */ u8 field_0x14[0x18 - 0x14];
};
extern UnkWorldEntry_8051D208 lbl_8051D208[];

/* The game state (struct name unknown; see Game/StageTable.cpp). */
struct UnkGameState_8057D6A0 {
    /* 0x000 */ u8 field_0x0[0xA8];
    /* 0x0A8 */ s32 m_stageId;
};
extern UnkGameState_8057D6A0 lbl_8057D6A0;

namespace Player {

/* The slots of PlayerMotion's vtable called here, with the arguments their bodies take
 * (vf_0x34 at 0x800AA748 clears two words, vf_0x38 at 0x800AA740 returns 0; Player::Shadow's
 * motion fills them). Only a view: PlayerMotion declares them without arguments. */
class UnkMotionSlots_800AA748 : public Task {
  public:
    virtual ~UnkMotionSlots_800AA748();
    virtual void vf_0x10();
    virtual void vf_0x14();
    virtual void vf_0x18();
    virtual void vf_0x1C();
    virtual void vf_0x20();
    virtual void vf_0x24();
    virtual void vf_0x28();
    virtual void vf_0x2C();
    virtual void vf_0x30();
    /* The motion works of the two extra parts (meaning inferred). */
    virtual void GetPartWorks(_stMtnPlayWork **first, _stMtnPlayWork **second);
    /* Which of the five extra models to show (meaning inferred). */
    virtual int GetModelIndex();
};

} // namespace Player

extern "C" {
/* The resource loader (see Game/BossRender.cpp). */
void *fn_800122D4(void);
void *fn_8001277C(void *loader, s32 slot, int index);
s32 fn_80012608(void *loader, s32 slot, int index);

/* RenderWare clumps and frames (see Game/BossRender.cpp). */
void *fn_8046256C(RpClump *clump);
void fn_804623EC(void *world, RpClump *clump);
void fn_804622FC(void *world, RpClump *clump);
RpClump *fn_80457090(void *data);
void fn_80457468(RpClump *clump);
void *fn_80169364(RpClump *clump);
int fn_80169350(void *frame);
void fn_8003A840(int area);
RwFrame *fn_8048722C(RwFrame *frame);

/* The clump fade, show, hide and animate (see Game/NpcEggMonitorMotion.cpp). */
UnkClumpFade_80027D5C *fn_80027D5C(UnkClumpFade_80027D5C *fade);
void fn_80038198(RpClump *clump, UnkClumpFade_80027D5C *fade);
void fn_80038F94(RpClump *clump);
void fn_80038FC0(RpClump *clump);
void fn_80039B94(RpClump *clump, float dt, float arg2);
void fn_800159AC(UnkClumpUpdater_80015A7C *updater);

/* The larger of two values (see Game/Mission.cpp). */
const float *fn_80070494(const float *a, const float *b);

/* The player's state bits (see Game/PlayerShadowWeapon.cpp), a count of what keeps bit 0x3F set
 * (fn_80079334 counts up or down), the motion work, the motion's skeleton node and its fade
 * (fn_800AA58C; behaviour inferred). */
void fn_80076CA8(u32 *bits, int bit);
void fn_80076CC8(u32 *bits, int bit);
void fn_80079334(Player::PlayerBase *player, bool add);
_stMtnPlayWork *fn_800AA758(Player::PlayerMotion *motion);
Player::UnkMotionNode_800AA760 *fn_800AA760(Player::PlayerMotion *motion, int index);
void fn_800AA58C(Player::PlayerMotion *motion, float fade);
boost::weak_ptr<CharInfo> fn_800AADA4(Player::UnkCharaSlots_800AADA4 *slots, int index);

/* Takes and drops a reference to an effect set (behaviour inferred). */
void fn_801D2DE8(int set);
void fn_801D2D14(int set);

/* Creates a Weapon::ChaosSpear::ChaosSpear for owner (0x802F0740; name inferred). */
boost::shared_ptr<Weapon::WeaponGunBase> fn_802F0740(const boost::weak_ptr<CharInfo> &owner);
}

/* The models of the parts that follow the motions, and of the five selectable models (-1: none). */
static const s32 s_partModels[4] = {3, 4, 5, 6};
static const s32 s_models[5] = {-1, 9, 10, 11, 12};

/* The lowest alpha (retail keeps it in .sdata, hence explicit_zero_data). */
#pragma explicit_zero_data on
static float s_minAlpha = 0.0f;
#pragma explicit_zero_data reset

/* Passes a returned owner on by reference, so that the local copies it (as retail does). */
inline const boost::weak_ptr<CharInfo> &Ref(const boost::weak_ptr<CharInfo> &owner) {
    return owner;
}

/* Removes clump from its world (name inferred). */
inline void RemoveFromWorld(RpClump *clump) {
    if (clump) {
        void *world = fn_8046256C(clump);
        if (world) {
            fn_804623EC(world, clump);
        }
    }
}

namespace Player {
namespace Shadow {

/* The task (vtable 0x8054DEFC); the player keeps a pointer to it at slot, which the task clears
 * when it ends. */
class Super : public Task {
  public:
    class Impl;

    Super(Task *parent, const boost::shared_ptr<PlayerBase> &player, Super **slot);
    virtual ~Super();
    virtual void Tick(float dt);

    /* Ends Super Shadow: the models fade out, then the task ends. */
    void Finish();

    /* 0x28 */ boost::scoped_ptr<Impl> m_impl;
    /* 0x2C */ u32 pad_0x2C; /* the allocation is 0x30 bytes */
};
SIZE_ASSERT(Super, 0x30);

/* The task's state (class name unknown; size from its allocation). */
class Super::Impl {
  public:
    Impl(const boost::shared_ptr<PlayerBase> &player);
    ~Impl();
    bool Update(float dt);
    void Load();
    void ResetFade();
    void FadeOut(float dt);
    void CreateModels();
    void DestroyModels();
    void UpdateModels(float dt);
    void UpdateVisibility();
    void AddAllToWorld();
    void RemoveAllFromWorld();
    void AddToWorld(RpClump *clump);

    /* 0x00 */ Super **m_slot;
    /* 0x04 */ boost::shared_ptr<PlayerBase> m_player;
    /* 0x0C */ UnkClumpUpdater_80015A7C *m_parts[4];
    /* 0x1C */ UnkMotionNode_800AA760 *m_node;
    /* 0x20 */ RpClump *m_nodeModels[2];
    /* 0x28 */ s32 m_modelIndex;
    /* 0x2C */ RpClump *m_models[5];
    /* 0x40 */ UnkClumpFade_80027D5C m_fade;
    /* 0x6C */ float m_alpha;
    /* 0x70 */ bool m_hidden;
    /* 0x74 */ s32 m_state; /* 0 loading, 1 shown, 2 fading out */
};
SIZE_ASSERT(Super::Impl, 0x78);

/* Deletes the state (name inferred; see checked_delete below). */
inline void DeleteImpl(Super::Impl *impl) {
    delete impl;
}

} // namespace Shadow
} // namespace Player

/* Retail keeps boost::checked_delete<Super::Impl> out of line (0x8027E2AC) and ~Impl at its own
 * place (0x8027EF5C). As in Game/Rocket.cpp, this specialization calls an inline helper, which
 * gives retail's code and order under GC/2.7 (see docs/research-notes.md). */
namespace boost {
template<> inline void checked_delete<Player::Shadow::Super::Impl>(Player::Shadow::Super::Impl *x) {
    Player::Shadow::DeleteImpl(x);
}
} // namespace boost

namespace Player {
namespace Shadow {

Super::Impl::Impl(const boost::shared_ptr<PlayerBase> &player) : m_player(player) {
    fn_80027D5C(&m_fade);
    m_hidden = false;
    fn_80078C88()->Load(0xE);
    fn_801D2DE8(2);
    int i;
    for (i = 0; i < 4; i++) {
        m_parts[i] = 0;
    }
    m_node = 0;
    m_nodeModels[0] = 0;
    m_nodeModels[1] = 0;
    m_modelIndex = 0;
    for (i = 0; i < 5; i++) {
        m_models[i] = 0;
    }
    m_alpha = 1.0f;
    m_fade.m_color[3] = m_alpha;
    if (m_player->m_motion) {
        fn_800AA58C(m_player->m_motion, 0.0f);
    }
    fn_80076CA8(m_player->m_stateBits, 0x24);
    if (lbl_8057D6A0.m_stageId == 0x2C6) {
        fn_80076CA8(m_player->m_stateBits, 0x25);
    }
    fn_80079334(m_player.get(), true);
    {
        boost::weak_ptr<CharInfo> owner = Ref(fn_800AADA4(m_player->m_charaSlots, 1));
        m_player->m_weapon->GiveWeapon(fn_802F0740(owner), 999);
        fn_80076CA8(m_player->m_stateBits, 0x33);
    }
    m_state = 0;
}

Super::Impl::~Impl() {
    DestroyModels();
    fn_80078C88()->Unload(0xE);
    fn_801D2D14(2);
}

bool Super::Impl::Update(float dt) {
    switch (m_state) {
    case 0:
        Load();
        break;
    case 1:
        ResetFade();
        break;
    case 2:
        FadeOut(dt);
        if (m_alpha <= 0.0f) {
            RemoveAllFromWorld();
            return false;
        }
        break;
    }
    UpdateModels(dt);
    UpdateVisibility();
    return true;
}

void Super::Impl::Load() {
    if (fn_80078C88()->IsLoaded(0xE)) {
        CreateModels();
        m_state = 1;
        if (m_player->m_motion) {
            void *loader = fn_800122D4();
            s32 handle = fn_80078C88()->GetHandle(0xE);
            PlayerBase *player = m_player.get();
            player->m_motion->field_0x3E4 = fn_80012608(loader, handle, 0xE);
        }
    }
}

void Super::Impl::ResetFade() {
    m_alpha = 1.0f;
    m_fade.m_color[3] = m_alpha;
    if (m_player->m_motion) {
        fn_800AA58C(m_player->m_motion, 0.0f);
    }
}

void Super::Impl::FadeOut(float dt) {
    m_alpha -= dt / 0.3f;
    m_alpha = *fn_80070494(&m_alpha, &s_minAlpha);
    m_fade.m_color[3] = m_alpha;
    if (m_player->m_motion) {
        fn_800AA58C(m_player->m_motion, 1.0f - m_alpha);
    }
}

void Super::Impl::CreateModels() {
    void *loader = fn_800122D4();
    s32 handle = fn_80078C88()->GetHandle(0xE);
    if (m_player->m_motion) {
        _stMtnPlayWork *works[4];
        works[1] = works[0] = fn_800AA758(m_player->m_motion);
        ((UnkMotionSlots_800AA748 *)m_player->m_motion)->GetPartWorks(&works[2], &works[3]);
        for (int i = 0; i < 4; i++) {
            RpClump *clump = (RpClump *)fn_8001277C(loader, handle, s_partModels[i]);
            if (clump && works[i]) {
                clump = fn_80457090(clump);
                m_parts[i] = new UnkClumpUpdater_80015A7C(clump, works[i]);
                fn_80038198(clump, &m_fade);
            }
        }
        m_node = fn_800AA760(m_player->m_motion, 2);
        m_nodeModels[0] = (RpClump *)fn_8001277C(loader, handle, 7);
        m_nodeModels[1] = (RpClump *)fn_8001277C(loader, handle, 8);
        for (int i = 0; i < 2; i++) {
            if (m_nodeModels[i]) {
                m_nodeModels[i] = fn_80457090(m_nodeModels[i]);
                fn_80038198(m_nodeModels[i], &m_fade);
            }
        }
        for (int i = 0; i < 5; i++) {
            if (s_models[i] >= 0) {
                m_models[i] = (RpClump *)fn_8001277C(loader, handle, s_models[i]);
                if (m_models[i]) {
                    m_models[i] = fn_80457090(m_models[i]);
                    fn_80038198(m_models[i], &m_fade);
                    fn_80038FC0(m_models[i]);
                }
            }
        }
    }
    AddAllToWorld();
}

/* Destroys clump (name inferred). */
inline void DestroyClump(RpClump *&clump) {
    if (clump) {
        fn_80457468(clump);
        clump = 0;
    }
}

/* Destroys the clumps of models (name inferred). */
inline void DestroyClumps(RpClump **models, int count) {
    for (int i = 0; i < count; i++) {
        if (models[i]) {
            fn_80457468(models[i]);
            models[i] = 0;
        }
    }
}

void Super::Impl::DestroyModels() {
    RemoveAllFromWorld();
    for (int i = 0; i < 4; i++) {
        if (m_parts[i]) {
            RpClump *clump = m_parts[i]->GetClump();
            delete m_parts[i];
            m_parts[i] = 0;
            fn_80457468(clump);
        }
    }
    for (int i = 0; i < 2; i++) {
        RpClump *clump = m_nodeModels[i];
        if (clump) {
            fn_80457468(clump);
            m_nodeModels[i] = 0;
        }
    }
    for (int i = 0; i < 5; i++) {
        RpClump *clump = m_models[i];
        if (clump) {
            fn_80457468(clump);
            m_models[i] = 0;
        }
    }
}

/* The node's matrix as a RenderWare matrix (name inferred). */
inline RwMatrix ToRwMatrix(const Mtx34f *node) {
    RwMatrix matrix;
    matrix.right.x = node->values[0][0];
    matrix.right.y = node->values[1][0];
    matrix.right.z = node->values[2][0];
    matrix.up.x = node->values[0][1];
    matrix.up.y = node->values[1][1];
    matrix.up.z = node->values[2][1];
    matrix.at.x = node->values[0][2];
    matrix.at.y = node->values[1][2];
    matrix.at.z = node->values[2][2];
    matrix.pos.x = node->values[0][3];
    matrix.pos.y = node->values[1][3];
    matrix.pos.z = node->values[2][3];
    matrix.pad3 = 0;
    matrix.pad2 = 0;
    matrix.pad1 = 0;
    matrix.flags = 0;
    return matrix;
}

void Super::Impl::UpdateModels(float dt) {
    int i;
    for (i = 0; i < 4; i++) {
        if (m_parts[i]) {
            fn_80039B94(m_parts[i]->m_clump, dt, -1.0f);
            fn_800159AC(m_parts[i]);
        }
    }
    if (m_node) {
        RwMatrix modelling = ToRwMatrix(m_node->m_matrix);
        for (i = 0; i < 2; i++) {
            if (m_nodeModels[i]) {
                RwFrame *frame = (RwFrame *)m_nodeModels[i]->object.parent;
                frame->modelling = modelling;
                fn_8048722C(frame);
            }
        }
        for (i = 0; i < 5; i++) {
            if (m_models[i]) {
                RwFrame *frame = (RwFrame *)m_models[i]->object.parent;
                frame->modelling = modelling;
                fn_8048722C(frame);
            }
        }
        if (!m_hidden) {
            int index = ((UnkMotionSlots_800AA748 *)m_player->m_motion)->GetModelIndex();
            if (m_modelIndex != index) {
                if (m_models[m_modelIndex]) {
                    fn_80038FC0(m_models[m_modelIndex]);
                }
                m_modelIndex = index;
                if (m_models[m_modelIndex]) {
                    fn_80038F94(m_models[m_modelIndex]);
                }
            }
        }
    }
}

void Super::Impl::UpdateVisibility() {
    if (m_player->IsWeaponHidden()) {
        if (!m_hidden) {
            m_hidden = true;
            for (int i = 0; i < 4; i++) {
                if (m_parts[i] && m_parts[i]->m_clump) {
                    fn_80038FC0(m_parts[i]->m_clump);
                }
            }
            for (int i = 0; i < 2; i++) {
                if (m_nodeModels[i]) {
                    fn_80038FC0(m_nodeModels[i]);
                }
            }
            if (m_models[m_modelIndex]) {
                fn_80038FC0(m_models[m_modelIndex]);
            }
        }
    } else if (m_hidden) {
        m_hidden = false;
        for (int i = 0; i < 4; i++) {
            if (m_parts[i] && m_parts[i]->m_clump) {
                fn_80038F94(m_parts[i]->m_clump);
            }
        }
        for (int i = 0; i < 2; i++) {
            if (m_nodeModels[i]) {
                fn_80038F94(m_nodeModels[i]);
            }
        }
        if (m_models[m_modelIndex]) {
            fn_80038F94(m_models[m_modelIndex]);
        }
    }
}

void Super::Impl::AddAllToWorld() {
    int i;
    for (i = 0; i < 4; i++) {
        if (m_parts[i]) {
            AddToWorld(m_parts[i]->m_clump);
        }
    }
    for (i = 0; i < 2; i++) {
        if (m_nodeModels[i]) {
            AddToWorld(m_nodeModels[i]);
        }
    }
    for (i = 0; i < 5; i++) {
        if (m_models[i]) {
            AddToWorld(m_models[i]);
        }
    }
}

void Super::Impl::RemoveAllFromWorld() {
    int i;
    for (i = 0; i < 4; i++) {
        if (m_parts[i]) {
            RemoveFromWorld(m_parts[i]->GetClump());
        }
    }
    for (i = 0; i < 2; i++) {
        if (m_nodeModels[i]) {
            RemoveFromWorld(m_nodeModels[i]);
        }
    }
    for (i = 0; i < 5; i++) {
        if (m_models[i]) {
            RemoveFromWorld(m_models[i]);
        }
    }
}

void Super::Impl::AddToWorld(RpClump *clump) {
    if (clump) {
        int area = fn_80169350(fn_80169364(clump));
        fn_8003A840(area);
        void *world = lbl_8051D208[area].m_world;
        if (world) {
            void *current = fn_8046256C(clump);
            if (current != world) {
                if (current) {
                    fn_804623EC(current, clump);
                }
                fn_804622FC(world, clump);
            }
        }
    }
}

Super::Super(Task *parent, const boost::shared_ptr<PlayerBase> &player, Super **slot)
    : Task(parent), m_impl(new Impl(player)) {
    m_name = "Super";
    m_impl->m_slot = slot;
    if (m_impl->m_slot) {
        *m_impl->m_slot = this;
    }
}

Super::~Super() {
    if (m_impl->m_slot) {
        *m_impl->m_slot = 0;
    }
}

void Super::Tick(float dt) {
    if (!m_impl->Update(dt)) {
        m_flags |= 1;
    }
}

void Super::Finish() {
    if (m_impl->m_slot) {
        *m_impl->m_slot = 0;
        m_impl->m_slot = 0;
    }
    fn_80076CC8(m_impl->m_player->m_stateBits, 0x24);
    fn_80076CC8(m_impl->m_player->m_stateBits, 0x25);
    fn_80079334(m_impl->m_player.get(), false);
    fn_80076CC8(m_impl->m_player->m_stateBits, 0x33);
    if (m_impl->m_player->m_weapon) {
        m_impl->m_player->m_weapon->DropWeapon();
    }
    m_impl->m_state = 2;
}

} // namespace Shadow
} // namespace Player
