/* Player::Behavior::Super (class name from RTTI): behavior 0x22 of Shadow, flying as Super Shadow.
 * Entering it loads the Super Shadow character file (file 0xE; see Game/PlayerShadowSuper.cpp) and
 * marks the player's flags; once the file is loaded it plays the flight motion and each frame
 * pushes the player along its up vector (faster the more it flies the way it faces, or the faster
 * it goes), pulls it back towards a point, and turns it at a rate that falls with its speed. File
 * name inferred from the class (as Game/PlayerBehaviorFirstMeet.cpp); all other names are inferred.
 * Definitions are in reverse address order (deferred inlining emits functions last-defined
 * first). */
#include "Game/CharCommand.h"
#include "Game/CharacterFiles.h"
#include "Game/MathTypes.h"
#include "Game/Player.h"
#include "Game/PlayerBehavior.h"
#include "Game/PlayerWeaponBase.h"
#include "boost/shared_ptr.hpp"
#include <dolphin/types.h>
#include "size_assert.h"

/* The player's members read here (struct name unknown; Player::PlayerBase leaves them unmapped). */
struct UnkPlayerFields_802EB0C4 {
    /* 0x000 */ u8 field_0x0[0xB8];
    /* 0x0B8 */ Vec3f m_up; /* the direction the player is pushed along */
    /* 0x0C4 */ u8 field_0xC4[0xDC - 0xC4];
    /* 0x0DC */ Vec3f m_anchor; /* the point it is pulled back towards */
    /* 0x0E8 */ u8 field_0xE8[0x1C8 - 0xE8];
    /* 0x1C8 */ Vec3f m_normal;
};

extern "C" {
CharacterFileManager *fn_80078C88(void);
/* The player's flag word and state bits (see Game/PlayerShadowSuper.cpp). */
void fn_80014E1C(u32 *flags, int bit);
void fn_8006127C(u32 *flags, int bit);
bool fn_8006CD78(u32 *bits, int bit);
void fn_80076CC8(u32 *bits, int bit);
/* Player updates run before the behavior's own (behaviour unknown; as in FirstMeet). */
void fn_80079720(Player::PlayerBase *player);
void fn_8009728C(boost::shared_ptr<Player::PlayerBase> *player, int arg1);
/* The direction the player faces, and turning it (behaviour inferred). */
Vec3f fn_800796DC(Player::PlayerBase *player);
Vec3f fn_80093FC0(boost::shared_ptr<Player::PlayerBase> *player);
void fn_80093BA0(boost::shared_ptr<Player::PlayerBase> *player, Vec3f *direction, float rate);
void fn_80093AF8(boost::shared_ptr<Player::PlayerBase> *player, float rate);

const float &fn_800704AC(const float &a, const float &b); /* the smaller */
float fn_8000FEE4(float value);                           /* square root */
Vec3f fn_8040CEE8(const Vec3f *vector);                   /* normalized */
float fn_8040CF90(const Vec3f *a, const Vec3f *b);        /* dot product */
Vec3f fn_800091A4(const Vec3f *vector, float scale);
void fn_8040CFF8(Vec3f *vector, const Vec3f *value); /* vector += value */
void fn_8040F990(Vec3f *along, Vec3f *across, const Vec3f *vector, const Vec3f *normal);
}

/* The player's members above (a macro: an inline function taking m_player.get() would call get()
 * out of line). */
#define FieldsOf(player) reinterpret_cast<UnkPlayerFields_802EB0C4 *>(player)

/* fn_8040CD74 returns lhs - rhs through its first argument, as a function returning a Vec3f does;
 * called this way the result is a temporary, as in the game. */
typedef Vec3f (*VectorSubtract)(const Vec3f *lhs, const Vec3f *rhs);
#define Subtract(lhs, rhs) (reinterpret_cast<VectorSubtract>(fn_8040CD74)(lhs, rhs))

inline float Dot(const Vec3f &a, const Vec3f &b) {
    return fn_8040CF90(&a, &b);
}

inline void AddVector(Vec3f &vector, const Vec3f &value) {
    fn_8040CFF8(&vector, &value);
}

namespace Player {
namespace Behavior {

class Super : public Behavior {
  public:
    Super(const boost::shared_ptr<PlayerBase> &player);
    virtual ~Super();
    virtual bool vf_0x0C();
    virtual void Enter();
    virtual void Leave();
    virtual void vf_0x18(CharCommand *command);
    virtual void Update(float dt);

    /* The factory record's functions (names inferred). */
    static Behavior *Create(const boost::shared_ptr<PlayerBase> &player);
    static void Release();

    void Start();
    void WaitLoad();
    void Fly(float dt);
    void UpdateSpeed();

    static const char *s_name;

    /* 0x1C */ s32 m_state; /* 0 loading, 1 flying */
    /* 0x20 */ s32 m_step;
    /* 0x24 */ float m_speed;
    /* 0x28 */ float m_turnRate;
};
SIZE_ASSERT(Super, 0x2C);

const char *Super::s_name = "Super";

static float s_one = 1.0f;
/* The speed at which the push is fastest, and the distance from the anchor beyond which the player
 * is pulled back (names inferred). The game sets them in __sinit rather than in .sdata, as an
 * initializer that is not a constant expression does; the inline call reproduces that. */
inline float DynamicFloat(float value) {
    return value;
}
static float s_maxSpeed = DynamicFloat(750.0f);
static float s_pullRange = DynamicFloat(10.0f);

Super::Super(const boost::shared_ptr<PlayerBase> &player) : Behavior(player, s_name, 0x22) {}

Super::~Super() {}

bool Super::vf_0x0C() {
    return true;
}

void Super::Enter() {
    fn_80078C88()->Load(0xE);
    fn_80014E1C(&m_player.get()->m_flags, 2);
    fn_80014E1C(&m_player.get()->m_flags, 7);
    fn_80014E1C(&m_player.get()->m_flags, 9);
    m_speed = 0.0f;
    m_turnRate = 0.0f;
    m_state = 0;
}

void Super::Leave() {
    fn_80078C88()->Unload(0xE);
    fn_8006127C(&m_player.get()->m_flags, 2);
    fn_8006127C(&m_player.get()->m_flags, 7);
    fn_8006127C(&m_player.get()->m_flags, 9);
}

void Super::vf_0x18(CharCommand *command) {
    if (m_state == 0) {
        command->Flags() |= 2;
    }
}

void Super::Update(float dt) {
    switch (m_state) {
    case 0:
        WaitLoad();
        break;
    case 1:
        Fly(dt);
        break;
    }
}

void Super::Start() {
    m_state = 1;
    m_step = 0;
}

void Super::WaitLoad() {
    if (fn_80078C88()->IsLoaded(0xE)) {
        Start();
    }
}

void Super::Fly(float dt) {
    switch (m_step) {
    case 0:
        m_player.get()->m_motion->vf_0x10(0x2B, 10.0f);
        m_step++;
    }

    Vec3f up;
    fn_800091EC(&up, &FieldsOf(m_player.get())->m_up);
    if (!fn_8006CD78(m_player.get()->m_stateBits, 0x1B)) {
        UpdateSpeed();
    } else if (fn_8006CD78(m_player.get()->m_stateBits, 0x3C)) {
        m_speed = 250.0f;
    } else if (fn_8040CF2C(&FieldsOf(m_player.get())->m_up) < 0.9f * 0.9f) {
        m_speed = 250.0f;
    } else {
        float dot = Dot(fn_800796DC(m_player.get()), fn_8040CEE8(&up));
        float t;
        if (dot < 0.766f) {
            t = 0.0f;
        } else {
            t = (dot - 0.766f) / (1.0f - 0.766f);
        }
        m_speed = 150.0f * t + 250.0f;
    }
    AddVector(m_player->field_0x24, fn_800091A4(&up, m_speed));

    Vec3f toAnchor;
    fn_800091EC(&toAnchor, &(const Vec3f &)Subtract(&FieldsOf(m_player.get())->m_anchor,
                                                     &m_player.get()->m_position));
    Vec3f along;
    fn_8040F990(&along, 0, &toAnchor, &FieldsOf(m_player.get())->m_normal);
    if (fn_8040CF2C(&along) > s_pullRange * s_pullRange) {
        AddVector(m_player->field_0x24,
                  fn_800091A4(&(const Vec3f &)fn_8040CEE8(&along), 25.0f));
    }

    fn_80079720(m_player.get());
    fn_80076CC8(m_player.get()->m_stateBits, 2);
    fn_8009728C(&m_player, 0);
    if (fn_8006CD78(m_player.get()->m_stateBits, 0x3C)) {
        m_turnRate = 15.0f;
        fn_80093BA0(&m_player, &(Vec3f &)fn_80093FC0(&m_player), m_turnRate * dt);
    } else {
        float speedSq = fn_8040CF2C(&m_player.get()->field_0x18);
        if (speedSq <= 22500.0f) {
            m_turnRate = 4.0f;
        } else if (speedSq > 90000.0f) {
            m_turnRate = 2.0f;
        } else {
            float speed = fn_8000FEE4(speedSq);
            m_turnRate = 2.0f * ((speed - 300.0f) / -150.0f) + 2.0f;
        }
        if (fn_8006CD78(m_player.get()->m_stateBits, 0x1B)) {
            fn_80093AF8(&m_player, m_turnRate * dt);
        }
    }
}

void Super::UpdateSpeed() {
    float ratio = fn_8040CF50(&m_player.get()->field_0x18) / s_maxSpeed;
    ratio = fn_800704AC(ratio, s_one);
    m_speed = 150.0f * ratio + 250.0f;
}

Behavior *Super::Create(const boost::shared_ptr<PlayerBase> &player) {
    return new Super(player);
}

void Super::Release() {}

} // namespace Behavior
} // namespace Player

/* The record the behavior factory table finds Super by (keeps its lbl_ name). */
extern const Player::Behavior::UnkBehaviorDesc_804EC024 lbl_80507B50 = {
    Player::Behavior::Super::Create, Player::Behavior::Super::Release, 0};
