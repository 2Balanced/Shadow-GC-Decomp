/* Player::Behavior::EdgeHang (class name from RTTI): behavior 0xC of Shadow, hanging from an edge
 * (the edge comes with Player::Behavior::InitializeArgsEdgeHang). Entering it stops the player,
 * plays the hang motion and has the player's collision move carry the edge along with the
 * character it belongs to (through a bound callback). Each frame it pulls the player to the edge
 * and turns it towards the wall; the jump command climbs up (behavior 1 of this one, then 2 or 3),
 * the drop command or a missing wall lets go (behavior 2). File name inferred from the class (as
 * Game/PlayerBehaviorSuper.cpp); all other names are inferred. Definitions are in reverse address
 * order (deferred inlining emits functions last-defined first). */
/* Declares Player::Behavior::Behavior's slots with their arguments (see Game/PlayerBehavior.h). */
#define PLAYER_BEHAVIOR_SLOT_ARGS
#include <boost/bind.hpp>
#include <boost/function.hpp>

#include "Game/CharCommand.h"
#include "Game/GeometryLine.h"
#include "Game/MathTypes.h"
#include "Game/Player.h"
#include "Game/PlayerBehavior.h"
#include "Game/PlayerSoundBase.h"
#include "Game/PlayerWeaponBase.h"
#include "Game/Vibration.h"
#include "boost/shared_ptr.hpp"
#include "boost/weak_ptr.hpp"
#include <dolphin/types.h>
#include "size_assert.h"

using Sonicteam::System::Geometry::Line;

namespace SonicteamUSA {
namespace System {
namespace CharaColli {
class CharaColliBase;
} // namespace CharaColli
} // namespace System
} // namespace SonicteamUSA

/* A collision object of the world (see Game/VehicleColliMove.cpp). */
class UnkColliObject_80028CA4;

/* What a mover stands on (as declared in Game/VehicleCharaColliMove.cpp; its functions are in
 * Game/VehicleColliMove.cpp's range). */
struct UnkColliMoveContact_803334A4 {
    UnkColliMoveContact_803334A4();
    ~UnkColliMoveContact_803334A4();

    /* Stands on character chara (0x803333A4). */
    void SetCharInfo(const boost::weak_ptr<CharInfo> &chara);

    /* 0x00 */ u8 m_isCollider;
    /* 0x04 */ boost::weak_ptr<CharInfo> m_chara;
    /* 0x0C */ boost::weak_ptr<SonicteamUSA::System::CharaColli::CharaColliBase> m_collider;
    /* 0x14 */ UnkColliObject_80028CA4 *m_object;
};
SIZE_ASSERT(UnkColliMoveContact_803334A4, 0x18);

/* The placement taken from a contact (as declared in Game/VehicleCharaColliMove.cpp). */
struct UnkColliMovePlacement_80333098 {
    /* 0x00 */ Vec3f m_position;
    /* 0x0C */ Vec3f m_rotation;
    /* 0x18 */ u32 m_kind;
};
SIZE_ASSERT(UnkColliMovePlacement_80333098, 0x1C);

/* CharaColliMoveArgs (class name from RTTI; as declared in Game/VehicleCharaColliMove.cpp): what
 * the collision move hands its callback: the contact, the moved character, and where the ground is
 * now and was before. */
struct CharaColliMoveArgs {
    /* 0x00 */ UnkColliMoveContact_803334A4 *m_contact;
    /* 0x04 */ boost::shared_ptr<CharInfo> m_chara;
    /* 0x0C */ UnkColliMovePlacement_80333098 m_current;
    /* 0x28 */ UnkColliMovePlacement_80333098 m_previous;
};
SIZE_ASSERT(CharaColliMoveArgs, 0x44);

typedef boost::function1<void, CharaColliMoveArgs &> CharaColliMoveCallback;

/* The object at 0x278 of the player (struct name unknown; only the members read here). */
struct UnkObject_8007FE50 {
    /* 0x00 */ u8 field_0x0[0x20];
    /* 0x20 */ float field_0x20;
    /* 0x24 */ u8 field_0x24;
};

/* The player's members read here (struct name unknown; Player::PlayerBase leaves them
 * unmapped). */
struct UnkPlayerFields_8007F11C {
    /* 0x000 */ u8 field_0x0[0xB8];
    /* 0x0B8 */ Vec3f field_0xB8;
    /* 0x0C4 */ u8 field_0xC4[0xF4 - 0xC4];
    /* 0x0F4 */ Vec3f m_up;
    /* 0x100 */ u8 field_0x100[0x278 - 0x100];
    /* 0x278 */ UnkObject_8007FE50 *field_0x278;
};

/* a - b (0x8000AD64). */
Vec3f operator-(const Vec3f &a, const Vec3f &b);

extern "C" {
/* The player's flag word and state bits (see Game/PlayerShadowSuper.cpp). */
void fn_80014E1C(u32 *flags, int bit);
void fn_8006127C(u32 *flags, int bit);
bool fn_8006CD78(u32 *bits, int bit);
void fn_80076CA8(u32 *bits, int bit);
void fn_80076CC8(u32 *bits, int bit);
/* The player's collision move: its callback and its contact, and clearing them (behaviour
 * inferred). */
void fn_80078E58(Player::PlayerBase *player, const CharaColliMoveCallback *callback);
void fn_80078E00(Player::PlayerBase *player, const UnkColliMoveContact_803334A4 *contact);
void fn_80078E2C(Player::PlayerBase *player);
void fn_80078DD4(Player::PlayerBase *player);
/* The player's front axis (behaviour inferred). */
Vec3f fn_800796DC(Player::PlayerBase *player);
/* Player updates run before the behavior's own (behaviour unknown; as in FirstMeet). */
void fn_80079720(Player::PlayerBase *player);
void fn_8009728C(boost::shared_ptr<Player::PlayerBase> *player, int arg1);
/* Turning the player towards a direction (behaviour inferred; as in Super). */
void fn_80093BA0(boost::shared_ptr<Player::PlayerBase> *player, Vec3f *direction, float rate);
/* The player's input along a direction (behaviour inferred). */
int fn_80093C60(boost::shared_ptr<Player::PlayerBase> *player, const Vec3f *direction);
/* Casts a line through the world; negative when nothing is hit (behaviour inferred). */
float fn_800979A0(boost::shared_ptr<Player::PlayerBase> *player, const Line *line, int arg2,
                  int arg3, int arg4, int arg5);
/* Whether the player has landed (see Game/PlayerBehaviorDamage.cpp). */
bool fn_800B7B58(boost::shared_ptr<Player::PlayerBase> *player);
/* Switches a player's behavior (behaviour inferred). */
void fn_800A7F14(Player::Behavior::UnkBehaviorExecuter_800A7F14 *executer,
                 const Player::Behavior::InitializeArgs *args);
/* Rumbles the controller (see Game/VibrationRequest.cpp). */
boost::weak_ptr<Vibration::RequestHandle> fn_8034DF48(int pattern, int pad);

void fn_8040CE60(Vec3f *vector);                       /* vector = 0 */
Vec3f fn_80016284(const Vec3f *v);                     /* -v */
Vec3f fn_800091A4(const Vec3f *vector, float scale);   /* vector * scale */
Vec3f fn_8040CDA8(const Vec3f *vector, float scale);   /* vector * scale */
Vec3f fn_8040CDD0(const Vec3f *a, const Vec3f *b);     /* a + b */
void fn_8040CFF8(Vec3f *vector, const Vec3f *value);   /* vector += value */
Vec3f fn_8040B618(const Mtx34f *matrix, const Vec3f *vector); /* the vector turned */
Vec3f fn_8040B67C(const Mtx34f *matrix, const Vec3f *point);  /* the point transformed */
/* The matrix turned by angles in an axis order, and back (see Game/VehicleCharaColliMove.cpp). */
Mtx34f fn_8040FFBC(const Mtx34f *matrix, const Vec3f *angles, int order);
Mtx34f fn_8040FEBC(const Mtx34f *matrix, const Vec3f *angles, int order);
}

/* The player's members above (a macro, as in Game/PlayerBehaviorSuper.cpp). */
#define FieldsOf(player) reinterpret_cast<UnkPlayerFields_8007F11C *>(player)

inline void AddVector(Vec3f &vector, const Vec3f &value) {
    fn_8040CFF8(&vector, &value);
}

inline Vec3f Scale(const Vec3f &vector, float scale) {
    return fn_8040CDA8(&vector, scale);
}

namespace Player {
namespace Behavior {

class EdgeHang : public Behavior {
  public:
    EdgeHang(const boost::shared_ptr<PlayerBase> &player);
    virtual ~EdgeHang();
    virtual bool vf_0x0C(int prevId, const InitializeArgs &args);
    virtual void Enter(int prevId, const InitializeArgs &args);
    virtual void Leave(int nextId);
    virtual void vf_0x18(CharCommand *command);
    virtual void Update(float dt);
    virtual void vf_0x20();

    /* The factory record's functions (names inferred). */
    static Behavior *Create(const boost::shared_ptr<PlayerBase> &player);
    static void Release();

    void Hang(float dt);
    void ClimbUp(float dt);
    void OnColliMove(CharaColliMoveArgs &args);
    void Hold(float dt);
    bool CheckWall();

    static const char *s_name;

    /* 0x1C */ s32 m_state; /* 0 hanging, 1 climbing up */
    /* 0x20 */ s32 m_step;
    /* 0x24 */ s32 m_request; /* 1 climb up, 2 let go */
    /* 0x28 */ float m_wait;
    /* 0x2C */ float m_time;
    /* 0x30 */ Vec3f m_position; /* the point hung from */
    /* 0x3C */ Vec3f m_normal;   /* the wall's normal */
    /* 0x48 */ boost::weak_ptr<CharInfo> m_chara; /* the character the edge belongs to */
};
SIZE_ASSERT(EdgeHang, 0x50);

const char *EdgeHang::s_name = "EdgeHang";

/* How far below the edge the player hangs, how far out from the wall, and how far ahead the wall
 * is looked for (names inferred). The game sets them in __sinit rather than in .sdata, as an
 * initializer that is not a constant expression does; the inline call reproduces that (as in
 * Game/PlayerBehaviorSuper.cpp). */
inline float DynamicFloat(float value) {
    return value;
}
static float s_drop = DynamicFloat(-6.3999996f); /* 0xC0CCCCCC, just above -6.4f */
static float s_offset = DynamicFloat(0.25f);
static float s_reach = DynamicFloat(4.0f);

EdgeHang::EdgeHang(const boost::shared_ptr<PlayerBase> &player) : Behavior(player, s_name, 0xC) {}

EdgeHang::~EdgeHang() {}

void EdgeHang::vf_0x18(CharCommand *command) {
    switch (command->TypeId()) {
    case 0x104:
        m_request = 1;
        command->Flags() |= 2;
        break;
    case 0x105:
        break;
    case 0x106:
        m_request = 2;
        command->Flags() |= 2;
        break;
    }
}

bool EdgeHang::vf_0x0C(int prevId, const InitializeArgs &args) {
    if (fn_8006CD78(m_player.get()->m_stateBits, 0x1E)) {
        return false;
    }
    UnkObject_8007FE50 *object = FieldsOf(m_player.get())->field_0x278;
    if (object->field_0x24 && object->field_0x20 < 20.0f) {
        return false;
    }
    return true;
}

void EdgeHang::Enter(int prevId, const InitializeArgs &args) {
    const InitializeArgsEdgeHang *edge = dynamic_cast<const InitializeArgsEdgeHang *>(&args);
    fn_80076CA8(m_player.get()->m_stateBits, 0xB);
    fn_80014E1C(&m_player.get()->m_flags, 1);
    fn_8040CE60(&m_player.get()->field_0x18);
    m_player.get()->m_motion->vf_0x10(0xD, 0.0f);
    m_state = 0;
    m_step = 0;
    m_request = 0;
    m_wait = 0.8f;
    fn_800091D0(&m_position, reinterpret_cast<const Vec3f *>(&edge->m_position));
    fn_800091D0(&m_normal, reinterpret_cast<const Vec3f *>(&edge->m_normal));
    m_chara = edge->m_chara;
    {
        UnkColliMoveContact_803334A4 contact;
        contact.SetCharInfo(m_chara);
        fn_80078E58(m_player.get(), &(const CharaColliMoveCallback &)CharaColliMoveCallback(
                                        boost::bind(&EdgeHang::OnColliMove, this, _1)));
        fn_80078E00(m_player.get(), &contact);
        fn_80076CA8(m_player.get()->m_stateBits, 0x37);
    }
    m_player.get()->m_sound->StopVoice(0x12);
    fn_8034DF48(10, m_player.get()->field_0x54);
}

void EdgeHang::Leave(int nextId) {
    fn_80076CC8(m_player.get()->m_stateBits, 0);
    fn_80076CC8(m_player.get()->m_stateBits, 0xB);
    fn_8006127C(&m_player.get()->m_flags, 1);
    fn_80076CC8(m_player.get()->m_stateBits, 0x37);
    fn_80078E2C(m_player.get());
    fn_80078DD4(m_player.get());
}

void EdgeHang::Update(float dt) {
    switch (m_state) {
    case 0:
        Hang(dt);
        break;
    case 1:
        ClimbUp(dt);
        break;
    }
}

void EdgeHang::vf_0x20() {}

void EdgeHang::Hang(float dt) {
    switch (m_step) {
    case 0:
        m_wait -= dt;
        if (m_wait < 0.0f) {
            m_step++;
        }
        break;
    case 1:
        if (fn_8006CD78(m_player.get()->m_stateBits, 0x1B)) {
            switch (fn_80093C60(&m_player, &FieldsOf(m_player.get())->field_0xB8)) {
            case 0:
                m_request = 1;
                break;
            case 3:
                m_request = 2;
                break;
            }
        }
        break;
    }
    if (!CheckWall()) {
        m_request = 2;
    }
    switch (m_request) {
    case 1:
        m_state = 1;
        m_step = 0;
        break;
    case 2: {
        const Vec3f &velocity = fn_8040CDA8(
            &(const Vec3f &)fn_80016284(&(const Vec3f &)fn_800796DC(m_player.get())), 50.0f);
        fn_800091D0(&m_player.get()->field_0x18, &velocity);
        InitializeArgs args(2);
        fn_800A7F14(m_player.get()->m_behaviors, &args);
        break;
    }
    }
    Hold(dt);
    fn_80079720(m_player.get());
}

void EdgeHang::ClimbUp(float dt) {
    switch (m_step) {
    case 0:
        fn_80076CC8(m_player.get()->m_stateBits, 0x37);
        fn_80078E2C(m_player.get());
        fn_80078DD4(m_player.get());
        AddVector(m_player->field_0x18,
                  fn_800091A4(&FieldsOf(m_player.operator->())->m_up, 140.0f));
        fn_80076CA8(m_player.get()->m_stateBits, 0);
        m_player.get()->m_motion->vf_0x10(0xC, 15.0f);
        m_time = 0.0f;
        m_step++;
        break;
    case 1:
        if (fn_800B7B58(&m_player)) {
            m_step++;
        }
        break;
    case 2:
        AddVector(m_player->field_0x24,
                  fn_800091A4(&(const Vec3f &)fn_800796DC(m_player.operator->()), 220.0f));
        break;
    }
    fn_80079720(m_player.get());
    if (m_step >= 2) {
        fn_8009728C(&m_player, 0);
    }
    m_time += dt;
    if (m_time > 1.0f) {
        InitializeArgs args(2);
        fn_800A7F14(m_player.get()->m_behaviors, &args);
        return;
    }
    if (fn_8006CD78(m_player.get()->m_stateBits, 2)) {
        InitializeArgs args(3);
        fn_800A7F14(m_player.get()->m_behaviors, &args);
    }
}

/* Carries the edge (and the character on it) along with the ground the collision move stands
 * on. */
void EdgeHang::OnColliMove(CharaColliMoveArgs &args) {
    Mtx34f matrix;
    Mtx34f translation;
    fn_8040B034(&translation, &args.m_current.m_position);
    fn_8040C148(&matrix, &translation);
    fn_8040C148(&matrix, &(const Mtx34f &)fn_8040FFBC(&matrix, &args.m_current.m_rotation,
                                                      args.m_current.m_kind));
    fn_8040C148(&matrix, &(const Mtx34f &)fn_8040FEBC(&matrix, &args.m_previous.m_rotation,
                                                      args.m_previous.m_kind));
    fn_8040B59C(&matrix, &(const Vec3f &)fn_80016284(&args.m_previous.m_position));
    fn_800091D0(&m_position, &(const Vec3f &)fn_8040B67C(&matrix, &m_position));
    fn_800091D0(&m_normal, &(const Vec3f &)fn_8040B618(&matrix, &m_normal));
    const Vec3f &position = fn_8040B67C(&matrix, &args.m_chara.get()->m_position);
    fn_800091D0(&args.m_chara.get()->m_position, &position);
}

/* Pulls the player towards its place below the edge and turns it towards the wall. */
void EdgeHang::Hold(float dt) {
    Vec3f target;
    fn_800091D0(&target,
                &(const Vec3f &)fn_8040CDD0(
                    &(const Vec3f &)fn_8040CDD0(
                        &m_position, &(const Vec3f &)Scale(FieldsOf(m_player.get())->m_up, s_drop)),
                    &(const Vec3f &)Scale(m_normal, s_offset)));
    float step = 15.0f * dt;
    float rate = 1.0f;
    if (step < rate) {
        rate = step;
    }
    AddVector(m_player->m_position, fn_800091A4(&(const Vec3f &)(target - m_player->m_position), rate));
    fn_80093BA0(&m_player, &(Vec3f &)fn_80016284(&m_normal), 6.5f * dt);
}

/* Whether the wall is still in front of the player. */
bool EdgeHang::CheckWall() {
    Line line;
    fn_800091D0(&line.m_start, &m_player.get()->m_position);
    fn_800091D0(&line.m_end,
                &(const Vec3f &)fn_8040CDD0(
                    &m_player.get()->m_position,
                    &(const Vec3f &)Scale(fn_800796DC(m_player.get()),
                                          s_reach + m_player.get()->field_0x274)));
    return !(fn_800979A0(&m_player, &line, 0, 0x1A93, 0x40000, 2) < 0.0f);
}

Behavior *EdgeHang::Create(const boost::shared_ptr<PlayerBase> &player) {
    return new EdgeHang(player);
}

void EdgeHang::Release() {}

} // namespace Behavior
} // namespace Player

/* The record the behavior factory table finds EdgeHang by (keeps its lbl_ name). */
extern const Player::Behavior::UnkBehaviorDesc_804EC024 lbl_804AFCBC = {
    Player::Behavior::EdgeHang::Create, Player::Behavior::EdgeHang::Release, 0};
