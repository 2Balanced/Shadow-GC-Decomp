/* Player::Behavior::Fall (class name from RTTI): behavior 2 of Shadow, falling. Each frame it pulls
 * the player down and lets it steer, and every so often looks for an edge to grab (behavior 0xC,
 * Player::Behavior::InitializeArgsEdgeHang); it lands (behavior 3) when the player touches the
 * ground. Coming from some behaviors it starts the character's fall effect, which follows the
 * player's motion and hides with its weapon; after Chaos Control it waits for the stage block to be
 * ready. File name inferred from the class (as Game/PlayerBehaviorSuper.cpp); all other names are
 * inferred. Definitions are in reverse address order (deferred inlining emits functions
 * last-defined first). */
/* Declares Player::Behavior::Behavior's slots with their arguments (see Game/PlayerBehavior.h). */
#define PLAYER_BEHAVIOR_SLOT_ARGS
/* Defines InitializeArgsEdgeHang's inline functions, whose out-of-line copies this unit emits. */
#define PLAYER_BEHAVIOR_ARGS_INLINE

#include "Effect/System/EffectElement.h"
#include "Game/CharCommand.h"
#include "Game/MathTypes.h"
#include "Game/NpcPartModel.h"
#include "Game/Player.h"
#include "Game/PlayerBehavior.h"
#include "Game/PlayerCharaParam.h"
#include "Game/PlayerWeaponBase.h"
#include "Game/TBlockManage.h"
#include "boost/shared_ptr.hpp"
#include "boost/weak_ptr.hpp"
#include <dolphin/types.h>
#include "size_assert.h"

using Effect::System::EffectElement;

/* boost/shared_ptr.hpp only declares shared_ptr::reset, which the game has out of line; this unit
 * emits the copy for the stage block check (boost 1.32's definition; as in Game/AdvFolder.cpp). */
namespace boost {
template <> inline void shared_ptr<TBlockManage::forBlockCheck>::reset() {
    this_type().swap(*this);
}
} // namespace boost

/* The object at 0x278 of the player (struct name unknown; as in
 * Game/PlayerBehaviorEdgeHang.cpp). */
struct UnkObject_8007FE50 {
    /* 0x00 */ u8 field_0x0[0xC];
    /* 0x0C */ Vec3f field_0xC;
    /* 0x18 */ u32 field_0x18;
    /* 0x1C */ u8 field_0x1C[0x20 - 0x1C];
    /* 0x20 */ float field_0x20;
    /* 0x24 */ u8 field_0x24;
};

/* The player's members read here (struct name unknown; Player::PlayerBase leaves them
 * unmapped). */
struct UnkPlayerFields_80080828 {
    /* 0x000 */ u8 field_0x0[0x64];
    /* 0x064 */ s32 m_kind; /* the character (inferred) */
    /* 0x068 */ u8 field_0x68[0xB8 - 0x68];
    /* 0x0B8 */ Vec3f field_0xB8;
    /* 0x0C4 */ u8 field_0xC4[0x278 - 0xC4];
    /* 0x278 */ UnkObject_8007FE50 *field_0x278;
};

/* The range an edge is looked for in (struct name unknown). */
struct UnkEdgeRange_80097EB0 {
    float m_up;
    float m_down;
};

/* Degrees to radians (0x8000AB90). */
float DegToRad(float degrees);

extern "C" {
/* The player's flag word and state bits (see Game/PlayerShadowSuper.cpp). */
void fn_80014E1C(u32 *flags, int bit);
void fn_8006127C(u32 *flags, int bit);
bool fn_8006CD78(u32 *bits, int bit);
void fn_80076CA8(u32 *bits, int bit);
void fn_80076CC8(u32 *bits, int bit);
/* Called with the player's object at 0x234 (behaviour unknown). */
void fn_8007792C(void *object);
void fn_80077950(void *object, int arg);
/* The player's height (the word at 0x274; kept out of line among Player::Behavior::DriveVehicle's
 * functions, behaviour inferred). */
float fn_8007E514(Player::PlayerBase *player);
/* Player updates run before the behavior's own (behaviour unknown; as in FirstMeet). */
void fn_80079720(Player::PlayerBase *player);
void fn_8009728C(boost::shared_ptr<Player::PlayerBase> *player, int arg1);
/* Turning the player (behaviour inferred; as in Super). */
void fn_80093AF8(boost::shared_ptr<Player::PlayerBase> *player, float rate);
/* The player's speed towards the ground (behaviour inferred). */
float fn_8009325C(boost::shared_ptr<Player::PlayerBase> *player);
/* Looks for an edge in reach: where to hang, the edge's normal and its character (behaviour
 * inferred). */
bool fn_80097EB0(Vec3f *position, Vec3f *normal, boost::weak_ptr<CharInfo> *chara,
                 boost::shared_ptr<Player::PlayerBase> *player, const UnkEdgeRange_80097EB0 *range);
/* Starts effect id of the player's effects at 0x23C following a matrix (behaviour inferred). */
boost::weak_ptr<EffectElement> fn_800A2814(void *effects, const Mtx34f *matrix, int id);
/* Switches a player's behavior (behaviour inferred). */
void fn_800A7F14(Player::Behavior::UnkBehaviorExecuter_800A7F14 *executer,
                 const Player::Behavior::InitializeArgs *args);

Vec3f fn_800091A4(const Vec3f *vector, float scale);  /* vector * scale */
void fn_8040CFF8(Vec3f *vector, const Vec3f *value);  /* vector += value */
float fn_8040CF90(const Vec3f *a, const Vec3f *b);    /* dot product */
Mtx34f fn_8040B1B0(float radians);                    /* a rotation matrix */
Mtx34f fn_8001E044(const Mtx34f *a, const Mtx34f *b); /* product */
}

/* The player's members above (a macro, as in Game/PlayerBehaviorSuper.cpp). */
#define FieldsOf(player) reinterpret_cast<UnkPlayerFields_80080828 *>(player)

/* vector += value (an inline wrapper, which orders the call's arguments as the game does). */
inline void AddVector(Vec3f &vector, const Vec3f &value) {
    fn_8040CFF8(&vector, &value);
}

/* fn_8040B034 (Game/MathTypes.h) builds a translation matrix through its first argument, as a
 * function returning a Mtx34f does; called as one, the matrix is a temporary, as in the game (name
 * inferred). */
typedef Mtx34f (*TranslationMtxFn)(const Vec3f *offset);
#define TranslationMtx(offset) (reinterpret_cast<TranslationMtxFn>(fn_8040B034)(offset))

/* m = value, through the game's out-of-line matrix copy (name inferred). */
inline void AssignMtx(Mtx34f &m, const Mtx34f &value) {
    fn_8040C148(&m, &value);
}

namespace Player {
namespace Behavior {

class Fall : public Behavior {
  public:
    Fall(const boost::shared_ptr<PlayerBase> &player);
    virtual ~Fall();
    virtual bool vf_0x0C(int prevId, const InitializeArgs &args);
    virtual void Enter(int prevId, const InitializeArgs &args);
    virtual void Leave(int nextId);
    virtual void vf_0x18(CharCommand *command);
    virtual void Update(float dt);

    /* The factory record's functions (names inferred). */
    static Behavior *Create(const boost::shared_ptr<PlayerBase> &player);
    static void Release();

    void UpdateEffectMatrix();
    void StartEffect();
    void StopEffect();

    static const char *s_name;

    /* 0x1C */ float m_wait; /* until the next look for an edge */
    /* 0x20 */ boost::weak_ptr<EffectElement> m_effect;
    /* 0x28 */ Mtx34f m_effectMatrix;
    /* 0x58 */ boost::shared_ptr<TBlockManage::forBlockCheck> m_block;
};
SIZE_ASSERT(Fall, 0x60);

const char *Fall::s_name = "Fall";

/* The range an edge is looked for in (names inferred). The game sets them in __sinit rather than in
 * .sdata, as an initializer that is not a constant expression does; the inline call reproduces
 * that (as in Game/PlayerBehaviorSuper.cpp). */
inline float DynamicFloat(float value) {
    return value;
}
static float s_edgeDown = DynamicFloat(6.0f);
static float s_edgeUp = DynamicFloat(3.0f);

Fall::Fall(const boost::shared_ptr<PlayerBase> &player) : Behavior(player, s_name, 2) {}

Fall::~Fall() {}

bool Fall::vf_0x0C(int prevId, const InitializeArgs &args) {
    return true;
}

void Fall::Enter(int prevId, const InitializeArgs &args) {
    fn_80076CC8(m_player.get()->m_stateBits, 2);
    fn_80076CA8(m_player.get()->m_stateBits, 0);
    fn_80014E1C(&m_player.get()->m_flags, 3);
    fn_80014E1C(&m_player.get()->m_flags, 7);
    m_wait = 0.5f;
    switch (prevId) {
    case 9:
        fn_80014E1C(&m_player.get()->m_flags, 2);
        fn_80014E1C(&m_player.get()->m_flags, 6);
        StartEffect();
        break;
    case 5:
        fn_80014E1C(&m_player.get()->m_flags, 6);
        m_wait = 0.0f;
        if (!fn_8006CD78(m_player.get()->m_stateBits, 0x1E)) {
            fn_80014E1C(&m_player.get()->m_flags, 2);
            StartEffect();
        }
        break;
    case 6:
        m_wait = 0.0f;
        StartEffect();
        break;
    case 0x1D:
        m_block = fn_8003A154(&m_player.get()->m_position);
        break;
    }
    if (prevId != 9) {
        m_player.get()->m_motion->vf_0x10(3, 10.0f);
    }
}

void Fall::Leave(int nextId) {
    StopEffect();
    fn_80076CC8(m_player.get()->m_stateBits, 0);
    fn_8006127C(&m_player.get()->m_flags, 3);
    fn_8006127C(&m_player.get()->m_flags, 7);
    fn_8006127C(&m_player.get()->m_flags, 6);
    fn_8006127C(&m_player.get()->m_flags, 2);
}

void Fall::vf_0x18(CharCommand *command) {}

void Fall::Update(float dt) {
    AddVector(m_player->field_0x24,
              fn_800091A4(&FieldsOf(m_player.operator->())->field_0xB8, 220.0f));
    if (m_block.get() == 0 ||
        !fn_80039EDC(boost::weak_ptr<TBlockManage::forBlockCheck>(m_block))) {
        m_block.reset();
        fn_80079720(m_player.get());
        if (fn_8006CD78(m_player.get()->m_stateBits, 0x1B) &&
            !fn_8006CD78(m_player.get()->m_stateBits, 0x3C)) {
            fn_80093AF8(&m_player, 15.0f * dt);
        }
    }
    fn_8009728C(&m_player, 0);
    UpdateEffectMatrix();
    m_wait -= dt;
    if (m_wait <= 0.0f) {
        m_wait = 0.0f;
        Vec3f position;
        Vec3f normal;
        boost::weak_ptr<CharInfo> chara;
        UnkEdgeRange_80097EB0 range;
        range.m_up = s_edgeUp;
        range.m_down = s_edgeDown;
        if (fn_80097EB0(&position, &normal, &chara, &m_player, &range)) {
            InitializeArgsEdgeHang args(position, normal, chara);
            fn_800A7F14(m_player.get()->m_behaviors, &args);
            return;
        }
    }
    if (!m_effect.expired()) {
        if (m_player.get()->IsWeaponHidden()) {
            m_effect.lock()->Hide();
        } else {
            m_effect.lock()->Show();
        }
    }
    UnkObject_8007FE50 *object = FieldsOf(m_player.get())->field_0x278;
    if (object->field_0x24 && (object->field_0x18 & 1) &&
        fn_8040CF90(&object->field_0xC, &m_player.get()->field_0x18) < 0.0f &&
        object->field_0x20 < fn_8009325C(&m_player)) {
        fn_800A7F14(m_player.get()->m_behaviors, &(const InitializeArgs &)InitializeArgs(3));
        return;
    }
    if (fn_8006CD78(m_player.get()->m_stateBits, 2)) {
        fn_800A7F14(m_player.get()->m_behaviors, &(const InitializeArgs &)InitializeArgs(3));
    }
}

/* Places the fall effect at the player's motion's root node, below the player (behaviour
 * inferred). */
void Fall::UpdateEffectMatrix() {
    UnkSkeletonNode_800AA760 *node = fn_800AA760(m_player.get()->m_motion, 0);
    Vec3f offset;
    AssignMtx(m_effectMatrix,
              fn_8001E044(&(const Mtx34f &)fn_8001E044(
                              node->m_matrix,
                              &(const Mtx34f &)TranslationMtx(fn_80008E1C(
                                  &offset, 0.0f, -fn_8007E514(m_player.operator->()), 0.0f))),
                          &(const Mtx34f &)fn_8040B1B0(DegToRad(-90.0f))));
}

void Fall::StartEffect() {
    if (lbl_804F1834[FieldsOf(m_player.get())->m_kind].m_hasFallEffect) {
        fn_80076CA8(m_player.get()->m_stateBits, 0x35);
        fn_80077950(reinterpret_cast<void *>(m_player.get()->field_0x234), 4);
        m_effect = fn_800A2814(reinterpret_cast<void *>(m_player.get()->field_0x23C), &m_effectMatrix,
                               lbl_804F1834[FieldsOf(m_player.get())->m_kind].m_fallEffect);
        UpdateEffectMatrix();
    }
}

void Fall::StopEffect() {
    if (lbl_804F1834[FieldsOf(m_player.get())->m_kind].m_hasFallEffect) {
        fn_80076CC8(m_player.get()->m_stateBits, 0x35);
        fn_8007792C(reinterpret_cast<void *>(m_player.get()->field_0x234));
        if (!m_effect.expired()) {
            m_effect.lock()->Retire();
            m_effect.reset();
        }
    }
}

Behavior *Fall::Create(const boost::shared_ptr<PlayerBase> &player) {
    return new Fall(player);
}

void Fall::Release() {}

} // namespace Behavior
} // namespace Player

/* The record the behavior factory table finds Fall by (keeps its lbl_ name). */
extern const Player::Behavior::UnkBehaviorDesc_804EC024 lbl_804AFDD0 = {
    Player::Behavior::Fall::Create, Player::Behavior::Fall::Release, 0};
