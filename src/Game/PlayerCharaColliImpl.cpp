/* Player::PlayerCharaColliImpl (class name from the RTTI of its bound attack callback): a player's
 * collision state, kept behind a small handle at word 0x248 of the player (UnkPlayerPart_80095BD4;
 * class name unknown). It owns the player's body collider and its attack collision, answers the
 * attacks that reach the player, and every frame records which special objects the player touches
 * and the nearest character it should take note of. Also here: the player's collision against the
 * world (fn_8009728C and its helpers), which the behaviors call. File, member and function names
 * are inferred. Definitions are in reverse address order (deferred inlining emits functions
 * last-defined first).
 *
 * NonMatching: every function and every section but .sbss match except five functions, all of them
 * register allocation or instruction order that no source shape tried changed under GC/2.7:
 * - Update (0x80096008), AcceptHit (0x80099088) and fn_800986F0 (the sweep): retail gives the
 *   locals the high saved registers and the parameters lower ones; GC/2.7 does the reverse (the
 *   parameter-colouring difference of Game/VehicleColliMove.cpp's Sweep; for AcceptHit a local
 *   copy of the candidate and a separate assignment of the filter were tried).
 * - fn_80098224: the registers of the loop counter and the hoisted filter address are swapped
 *   (as in Game/VehicleColliMove.cpp's Settle).
 * - the range constructor of the list's storage (0x800993CC): its two argument copies are in the
 *   other order (init(i, last), init(first, last), copies of last and of i in several orders were
 *   tried).
 * .sbss: retail places the compiler's iterator-tag object before the bound callbacks' temporaries.
 * The player is read through boost::shared_ptr::px where the game loads it at each use: get() or
 * operator-> are inline calls, which the compiler evaluates before the statement. */
/* The const iterator and the range insertion of std::list (see the MSL stand-in). */
#define MSL_LIST_RANGE_INSERT
/* The player in CollisionEventArgs, and calling the player's collision callback. */
#define PLAYER_COLLISION_EVENT_SOURCE
/* boost::function's operator() puts a bad_function_call on the stack (see the MSL's stdexcept). */
#define MSL_RUNTIME_ERROR_STRING
#include <list>

#include "boost/shared_ptr.hpp"
#include <boost/bind.hpp>
#include <boost/function.hpp>

#include "Game/CharCommand.h"
#include "Game/CharInfo.h"
#include "Game/CharaColli.h"
#include "Game/CharaColliAttack.h"
#include "Game/CharaColliBaseCharInfo.h"
#include "Game/GeometryLine.h"
#include "Game/MathTypes.h"
#include "Game/Player.h"
#include "Game/PlayerMotionEvent.h"
#include "boost/scoped_ptr.hpp"
#include "boost/weak_ptr.hpp"
#include "size_assert.h"

using Player::PlayerBase;
using SonicteamUSA::System::CharaColli::AttackCallback;
using SonicteamUSA::System::CharaColli::AttackCallbackParam;
using SonicteamUSA::System::CharaColli::CharaColliAttack;
using SonicteamUSA::System::CharaColli::CharaColliBase;
using SonicteamUSA::System::CharaColli::CharaColliBaseCharInfo;
using Sonicteam::System::Geometry::Line;

/* The contact list of a collider and an iterator over it (see Game/BirdBase.cpp). */
struct UnkColliContacts_8001EE04;
struct UnkColliContactNode_800587A0;
struct UnkColliContactIter_8001EDFC {
    UnkColliContactIter_8001EDFC(UnkColliContactNode_800587A0 *node) : m_node(node) {}

    UnkColliContactNode_800587A0 *m_node;
};

/* A zero vector in .rodata (as lbl_805E07A0 in Game/MathTypes.h). */
extern const Vec3f lbl_805E07B8;

extern "C" {
void fn_80014E1C(u32 *flags, int bit);
void fn_8006127C(u32 *flags, int bit);
bool fn_8006CD78(u32 *bits, int bit);
float fn_8040D180(const void *a, const void *b); /* the distance between two positions */
Vec2f fn_8040F760(const Vec3f &direction);
float fn_8040CF50(const Vec3f *vector); /* length */
Vec3f fn_8040CDD0(const Vec3f *a, const Vec3f *b); /* a + b */
Vec3f fn_8040CDA8(const Vec3f *vector, float scale);
/* The collider manager (see Game/BirdBase.cpp and Game/AttackObject.cpp). */
void *fn_80021B08(void);
boost::weak_ptr<CharInfo> fn_800C4568(void *manager, boost::weak_ptr<CharaColliBase> collider);
boost::weak_ptr<CharaColliAttack> fn_800C4374(void *manager, boost::shared_ptr<CharaColliBase> collider);
void fn_800C78B0(void *manager, boost::shared_ptr<CharaColliBase> *collider);
boost::shared_ptr<CharaColliBase> fn_800C799C(void *manager, const Vec3f *position,
                                              const Vec3f *arg2, int arg3, int arg4, int arg5,
                                              int arg6, const boost::weak_ptr<CharInfo> &owner,
                                              const Vec3f *offset, const Vec3f *rotation,
                                              int arg10, float arg11, float arg12);
UnkColliContactNode_800587A0 *fn_8001EE04(void *contacts);
UnkColliContactNode_800587A0 *fn_8001EDEC(void *contacts);
UnkColliContactIter_8001EDFC fn_80027374(UnkColliContactIter_8001EDFC *it, int);
boost::weak_ptr<CharaColliBase> fn_800587A0(UnkColliContactNode_800587A0 *node);
/* Tells a character it was hurt by something (meaning of the argument unknown). */
void fn_8020642C(boost::shared_ptr<CharInfo> *chara, int arg1);
/* Sets an attack collision's mode and its attack callback. */
void fn_80096B64(CharaColliAttack *attack, int mode, const AttackCallback &callback);
}

/* fn_8040CD74 (a - b) called as returning its result by value (as in
 * Game/PlayerControlApproach.cpp). */
typedef Vec3f (*SubtractFn)(const Vec3f *a, const Vec3f *b);
#define Subtract ((SubtractFn)fn_8040CD74)

/* The player's members used here (struct name unknown; Player::PlayerBase leaves them
 * unmapped). */
struct UnkPlayerFields_800959B0 {
    /* 0x000 */ u8 field_0x0[0x5C];
    /* 0x05C */ s32 m_team;
    /* 0x060 */ u8 field_0x60[0x64 - 0x60];
    /* 0x064 */ s32 m_character;
    /* 0x068 */ u8 field_0x68[0xA4 - 0x68];
    /* 0x0A4 */ u32 m_touched; /* bits 5 to 9: the special objects touched this frame */
    /* 0x0A8 */ u32 m_stateBits[2];
    /* 0x0B0 */ u8 field_0xB0[0xD0 - 0xB0];
    /* 0x0D0 */ Vec3f m_lastPosition;
    /* 0x0DC */ u8 field_0xDC[0x274 - 0xDC];
    /* 0x274 */ float m_radius;
};

#define Fields(player) reinterpret_cast<UnkPlayerFields_800959B0 *>(player)

/* The player's collision radius (name inferred). An inline function in the original: the game
 * reads it before the other arguments of the calls it is passed to. */
inline float RadiusOf(PlayerBase *player) {
    return Fields(player)->m_radius;
}

/* vector * scale (an inline wrapper: the scale is read before the call that makes the vector, as
 * in Game/PlayerBehaviorGrind.cpp). */
inline Vec3f Scale(const Vec3f &vector, float scale) {
    return fn_8040CDA8(&vector, scale);
}

/* The characters that belong to a player (Player::PlayerBase's word 0x254; see Player.h): two
 * slots it never collides with (meaning inferred). */
struct UnkCharaSlots_800AADA4Fields {
    bool Contains(const boost::shared_ptr<CharInfo> &chara) {
        if (m_slots[0] == chara) {
            return true;
        }
        return (int)(m_slots[1] == chara);
    }

    /* 0x00 */ u8 field_0x0[0x8];
    /* 0x08 */ boost::shared_ptr<CharInfo> m_slots[2];
};

#define Slots(player) reinterpret_cast<UnkCharaSlots_800AADA4Fields *>((player)->m_charaSlots)

/* The parts of AttackCallbackParam read here (layout as in Game/CharaColliAttack.h). */
inline Vec3f &AttackDirection(AttackCallbackParam &param) {
    return *reinterpret_cast<Vec3f *>(param.field_0x8 + (0x34 - 0x8));
}

inline s32 &AttackMode(CharaColliAttack *attack) {
    return *reinterpret_cast<s32 *>(attack->field_0x3C);
}

/* A character's team (CharInfo's word 0x5C; name inferred). An inline function in the original:
 * retail calls the shared_ptr's operator-> out of line before it. */
inline s32 TeamOf(CharInfo *chara) {
    return chara->field_0x5C;
}

namespace Player {

class PlayerCharaColliImpl {
  public:
    PlayerCharaColliImpl(const boost::shared_ptr<PlayerBase> &player);
    ~PlayerCharaColliImpl();

    void OnAttack(AttackCallbackParam &param);
    boost::weak_ptr<CharaColliAttack> GetAttack() const;
    boost::shared_ptr<CharaColliBase> GetBody() const;
    boost::weak_ptr<CharInfo> GetNearest() const;
    void ClearTarget();
    void SetTarget(const boost::weak_ptr<CharInfo> &target);
    void Update();
    int GetAttackMode() const;
    void SetAttacking(bool attacking);
    void TurnOff();
    void TurnOn();

    /* 0x00 */ boost::shared_ptr<PlayerBase> m_player;
    /* 0x08 */ boost::weak_ptr<CharInfo> m_nearest; /* the character found by Update */
    /* 0x10 */ boost::weak_ptr<CharInfo> m_target;  /* the character Update prefers */
    /* 0x18 */ boost::shared_ptr<CharaColliBase> m_body;
    /* 0x20 */ boost::weak_ptr<CharaColliAttack> m_attack;
    /* 0x28 */ s32 m_attacking; /* the number of attacks in progress */
};
SIZE_ASSERT(PlayerCharaColliImpl, 0x2C);

} // namespace Player

/* The player's collision handle (class name unknown; Game/BossSonic.cpp makes one for its NPC). */
struct UnkPlayerPart_80095BD4 {
    UnkPlayerPart_80095BD4(const boost::shared_ptr<PlayerBase> &player);
    ~UnkPlayerPart_80095BD4();

    /* 0x0 */ boost::scoped_ptr<Player::PlayerCharaColliImpl> m_impl;
};
SIZE_ASSERT(UnkPlayerPart_80095BD4, 0x4);

using Player::PlayerCharaColliImpl;

/* Deletes the state (name inferred; see checked_delete below). */
inline void DeleteImpl(PlayerCharaColliImpl *impl) {
    delete impl;
}

/* Retail keeps boost::checked_delete<Impl> out of line and ~Impl at its own place: the
 * specialization calls an inline helper (as in Game/PlayerCharaColliMove.cpp; see
 * docs/research-notes.md). */
namespace boost {
template <> inline void checked_delete<PlayerCharaColliImpl>(PlayerCharaColliImpl *x) {
    DeleteImpl(x);
}
} // namespace boost

struct UnkPlayerColliFilter_80097E74;
struct UnkColliMoveContact_803334A4;
struct UnkCopyVector3_800091EC;
struct UnkEdgeRange_80097EB0;

/* The functions of the world collision below, defined in reverse address order, and the handle's
 * GetBody (defined at the end of the file). */
extern "C" {
boost::shared_ptr<CharaColliBase> fn_800959E0(UnkPlayerPart_80095BD4 *colli);
bool fn_80096D3C(boost::shared_ptr<PlayerBase> *player, float height);
bool fn_80096E20(Vec3f *out, boost::shared_ptr<PlayerBase> *player);
bool fn_80097050(const Vec3f *position);
void fn_8009728C(boost::shared_ptr<PlayerBase> *player,
                 const std::list<boost::shared_ptr<CharaColliBase> > *ignore);
float fn_800979A0(boost::shared_ptr<PlayerBase> *player, const void *line,
                  const std::list<boost::shared_ptr<CharaColliBase> > *ignore, u32 mask, u32 exclude, int arg5);
float fn_800979E8(boost::shared_ptr<PlayerBase> *player, Vec3f *position, Vec3f *normal,
                  boost::weak_ptr<CharaColliBaseCharInfo> *collider, u32 *attributes, u32 *kind,
                  const void *line, const std::list<boost::shared_ptr<CharaColliBase> > *ignore, u32 mask,
                  u32 exclude, int arg10);
bool fn_80097EB0(Vec3f *position, Vec3f *normal, boost::weak_ptr<CharInfo> *chara,
                 boost::shared_ptr<PlayerBase> *player, const UnkEdgeRange_80097EB0 *range);
UnkCopyVector3_800091EC fn_80098224(boost::shared_ptr<PlayerBase> *player, const Vec3f *position);
void fn_800984DC(boost::shared_ptr<PlayerBase> *player, const Vec3f *position);
bool fn_800986F0(Vec3f *normal, u32 groundMask, u32 mask, u32 exclude, boost::shared_ptr<PlayerBase> *player,
                 Vec3f *position, Vec3f *velocity, const std::list<boost::shared_ptr<CharaColliBase> > *ignore,
                 Vec3f *lastMove, UnkColliMoveContact_803334A4 *contact, int count);
}

/* ---- The player's collision against the world (0x80096D3C-0x80099658) ---- */

typedef std::list<boost::shared_ptr<CharaColliBase> > ColliderList;

/* A length (name inferred). As a call it is not a constant expression, so the constants below are
 * stored by a static initializer, as in retail (see Game/VehicleColliMove.cpp). */
inline float Span(float length) {
    return length;
}

/* How far the collision pushes away from what it touched, and the time after which a player may
 * stand on the ground again (names and meaning of the second inferred). */
static float s_skin = Span(0.020000001f);
static float s_standTime = Span(60.0f);

/* The constants of the attribute mask (namespace scope, so that they are loaded from .sdata2). */
const u32 kNotWater = 0xFFFBFFFF;
const u32 kWater = 0x8;

/* An attribute the walls carry (a global word of another unit; see Game/PlayerBehaviorWallJump.cpp). */
extern u32 lbl_805E4514;

/* What the player's collision tests skip and accept (struct name unknown; constructor 0x80097E74
 * and destructor 0x80097E18): hits whose attributes share a bit with m_exclude are skipped, as are
 * hits on the colliders in m_ignore and, unless m_player is unset, attacks of rival characters
 * (names inferred). Other hits count when their attributes share a bit with m_mask. */
struct UnkPlayerColliFilter_80097E74 {
    UnkPlayerColliFilter_80097E74() {}
    ~UnkPlayerColliFilter_80097E74() {}

    /* 0x00 */ u32 m_mask;
    /* 0x04 */ u32 m_exclude;
    /* 0x08 */ ColliderList m_ignore;
    /* 0x14 */ boost::shared_ptr<PlayerBase> m_player;
};
SIZE_ASSERT(UnkPlayerColliFilter_80097E74, 0x1C);

/* What a collision test passes its filter for each candidate hit (see Game/VehicleColliMove.cpp). */
struct UnkPlayerColliCandidate_80099088 {
    /* 0x00 */ u8 field_0x0[0x8];
    /* 0x08 */ CharaColliBase *m_collider;
    /* 0x0C */ u32 m_attributes;
    /* 0x10 */ const UnkPlayerColliFilter_80097E74 *m_filter;
};

typedef bool (*UnkPlayerColliFilterFn_80099088)(UnkPlayerColliCandidate_80099088 *candidate);

/* A sphere: centre and radius (built by fn_80021C04; see Game/VehicleColliMove.cpp). */
struct UnkSphere_80021C04 {
    Vec3f &Center() {
        return *reinterpret_cast<Vec3f *>(center);
    }

    /* 0x00 */ float center[3];
    /* 0x0C */ float radius;
};

/* A segment set up by fn_80020A20 (see Game/PlayerBehaviorWallJump.cpp). */
struct UnkSegment_80020A20 {
    /* 0x00 */ u8 field_0x0[0x18];
};

/* A world object a hit refers to (class name unknown): its distance from the start of the test
 * and its kind (names inferred). */
struct UnkHitObject_80097D0C {
    /* 0x00 */ u8 field_0x0[0x18];
    /* 0x18 */ float m_distance;
    /* 0x1C */ u8 field_0x1C[0x40 - 0x1C];
    /* 0x40 */ s32 m_kind;
};

inline float DistanceOf(const UnkHitObject_80097D0C *object) {
    return object->m_distance;
}

inline s32 KindOf(const UnkHitObject_80097D0C *object) {
    return object->m_kind;
}

/* The distance and the kind of a hit's world object. Other units call them by these names
 * (Game/VehicleColliMove.cpp, Game/SweepMove.cpp, Game/PJSCameraCollisionQuery.cpp); each calls
 * another inline function, so they stay out of line, and this unit is the first to emit them. */
extern "C" inline float fn_80097D0C(void *object) {
    return DistanceOf(static_cast<UnkHitObject_80097D0C *>(object));
}

extern "C" inline s32 fn_80099080(void *object) {
    return KindOf(static_cast<UnkHitObject_80097D0C *>(object));
}

/* The hits of a test (class name unknown; constructor 0x800270CC, destructor 0x80026FB4 and its
 * clear 0x80026A40; see Game/VehicleColliMove.cpp). */
struct CollisionResultList;

extern "C" {
CollisionResultList *fn_800270CC(CollisionResultList *list);
void fn_80026FB4(CollisionResultList *list, int flags);
void fn_80026A40(CollisionResultList *list);
}

struct UnkHitList_800270CC {
    UnkHitList_800270CC() {
        fn_800270CC(reinterpret_cast<CollisionResultList *>(this));
    }
    ~UnkHitList_800270CC() {
        fn_80026FB4(reinterpret_cast<CollisionResultList *>(this), -1);
    }

    /* 0x00 */ u8 field_0x0[0x1C];
};

/* A hit (see Game/VehicleColliMove.cpp): its collision object at 0xC and its world object at
 * 0x14. */
namespace SonicteamUSA {
namespace System {
namespace Range {
class RangeBase;
} // namespace Range
} // namespace System
} // namespace SonicteamUSA

extern "C" u32 fn_80026588(UnkColliContactNode_800587A0 *node);

struct UnkColliContactNode_800587A0 {
    /* The hit's attributes and its kind, the low 28 and the high 4 bits of its flags (names
     * inferred). */
    u32 GetAttributes() {
        return fn_80026588(this) & 0x0FFFFFFF;
    }
    u32 GetKind() {
        return fn_80026588(this) >> 28;
    }

    /* 0x00 */ u8 field_0x0[0xC];
    /* 0x0C */ SonicteamUSA::System::Range::RangeBase *m_collider;
    /* 0x10 */ u8 field_0x10[0x14 - 0x10];
    /* 0x14 */ void *m_object;
};

/* The first hit of a list (see Game/VehicleColliMove.cpp). */
struct UnkLineHit_8002735C {
    /* 0x00 */ u8 field_0x0[0x14];
    /* 0x14 */ void *m_object;
};

/* What a mover stands on (see Game/VehicleColliMove.cpp, whose unit holds these members). */
struct UnkColliMoveContact_803334A4 {
    UnkColliMoveContact_803334A4();
    ~UnkColliMoveContact_803334A4();

    void Set(const boost::weak_ptr<CharInfo> &charInfo, const boost::weak_ptr<CharaColliBase> &collider,
             SonicteamUSA::System::Range::RangeBase *object);
    void Reset();

    /* 0x00 */ u8 m_isCollider;
    /* 0x04 */ boost::weak_ptr<CharInfo> m_chara;
    /* 0x0C */ boost::weak_ptr<CharaColliBase> m_collider;
    /* 0x14 */ SonicteamUSA::System::Range::RangeBase *m_object;
};
SIZE_ASSERT(UnkColliMoveContact_803334A4, 0x18);

extern "C" void fn_80014E04(u32 *flags);

/* The flag word of the test of fn_80097050 (see Game/VehicleColliMove.cpp). */
struct UnkColliMoveFlags {
    UnkColliMoveFlags() {
        fn_80014E04(&m_bits);
    }

    u32 m_bits;
};

/* The reach of the edge search of fn_80097EB0: ahead of the player and above it (names inferred;
 * see Game/PlayerBehaviorFall.cpp). */
struct UnkEdgeRange_80097EB0 {
    float ahead;
    float above;
};

/* What the player's state 0x278 holds for fn_80096E20 (struct name unknown; names inferred). */
struct UnkPlayerGround_80096E20 {
    /* 0x00 */ u8 field_0x0[0x20];
    /* 0x20 */ float m_time;
    /* 0x24 */ u8 m_active;
};

/* The player's members the world collision uses (struct name unknown; names inferred). */
struct UnkPlayerWorldFields_8009728C {
    /* 0x000 */ Vec3f m_position;
    /* 0x00C */ u8 field_0xC[0x18 - 0xC];
    /* 0x018 */ Vec3f m_velocity;
    /* 0x024 */ u8 field_0x24[0xA4 - 0x24];
    /* 0x0A4 */ u32 m_touched;
    /* 0x0A8 */ u32 m_stateBits[2];
    /* 0x0B0 */ u8 field_0xB0[0xD0 - 0xB0];
    /* 0x0D0 */ Vec3f m_lastPosition;
    /* 0x0DC */ u8 field_0xDC[0xE8 - 0xDC];
    /* 0x0E8 */ Vec3f m_up;
    /* 0x0F4 */ Vec3f m_groundNormal;
    /* 0x100 */ u8 field_0x100[0x1C8 - 0x100];
    /* 0x1C8 */ Vec3f m_gravityUp;
    /* 0x1D4 */ u8 field_0x1D4[0x22C - 0x1D4];
    /* 0x22C */ void *m_behaviors;
    /* 0x230 */ u8 field_0x230[0x23C - 0x230];
    /* 0x23C */ void *m_effects;
    /* 0x240 */ u8 field_0x240[0x248 - 0x240];
    /* 0x248 */ UnkPlayerPart_80095BD4 *m_colli;
    /* 0x24C */ u8 field_0x24C[0x274 - 0x24C];
    /* 0x274 */ float m_radius;
    /* 0x278 */ UnkPlayerGround_80096E20 *m_ground;
};

#define World(player) reinterpret_cast<UnkPlayerWorldFields_8009728C *>(player)

extern "C" {
/* The collision world, a sphere from a centre and a radius, the sweep of a sphere, and the hits of
 * a sphere and of a line (see Game/VehicleColliMove.cpp). */
void *fn_80020D40(void);
void fn_80021C04(UnkSphere_80021C04 *sphere, const Vec3f *center, float radius);
CollisionResultList *fn_80021934(void *world, UnkSphere_80021C04 *sphere, Vec3f *velocity, int arg3,
                                 int arg4, CollisionResultList *previous,
                                 UnkPlayerColliFilterFn_80099088 filter,
                                 const UnkPlayerColliFilter_80097E74 *self, int arg8);
CollisionResultList *fn_80021CA4(void *world, const UnkSphere_80021C04 *sphere, int arg2, int arg3,
                                 UnkPlayerColliFilterFn_80099088 filter,
                                 const UnkPlayerColliFilter_80097E74 *self, UnkHitList_800270CC *list);
UnkColliContactNode_800587A0 *fn_80021E24(void *world, const void *line, int arg2, int arg3,
                                          UnkPlayerColliFilterFn_80099088 filter,
                                          const UnkPlayerColliFilter_80097E74 *self);
void fn_800265A4(UnkColliContactNode_800587A0 *hit, int flags);
void fn_80020A20(UnkSegment_80020A20 *segment, const Vec3f *start, const Vec3f *end);
UnkLineHit_8002735C *fn_8002735C(CollisionResultList *hits);
/* A world object's position, normal and push-out direction (behaviour inferred). */
const Vec3f *fn_8001E9BC(void *object);
const Vec3f *fn_800266D4(void *object);
const Vec3f *fn_8001E9C4(void *object);
/* The character a collider is attached to (see Game/VehicleColliMove.cpp). */
boost::weak_ptr<CharInfo> fn_800586EC(CharaColliBaseCharInfo *collider);
/* A 32-bit flag word (see Game/VehicleColliMove.cpp). */
void fn_80014E04(u32 *flags);
void fn_80014E10(u32 *flags);
bool fn_80014E34(u32 *flags, int bit);
void fn_80076CA8(u32 *bits, int bit);
void fn_80076CC8(u32 *bits, int bit);
/* Vector helpers (see Game/VehicleColliMove.cpp). */
Vec3f fn_800091A4(const Vec3f *vector, float scale);
void fn_8040CE04(Vec3f *vector, const Vec3f *value); /* -= */
void fn_8040CE60(Vec3f *vector);                     /* = 0 */
float fn_8040CE74(Vec3f *vector);                    /* normalizes */
Vec3f fn_8040CEE8(const Vec3f *vector);              /* normalized */
float fn_8040CF90(const Vec3f *a, const Vec3f *b);   /* dot product */
void fn_8040CFF8(Vec3f *vector, const Vec3f *value); /* += */
void fn_8040F990(Vec3f *along, Vec3f *across, const Vec3f *vector, const Vec3f *normal);
Vec3f fn_80093230(const Vec3f *vector);
bool fn_8000A950(float value); /* whether the value is zero */
float fn_8005AE20(float metres);
/* The player's up, its front and its update of the ground state (behaviour inferred). */
Vec3f fn_80079698(PlayerBase *player);
Vec3f fn_800796DC(PlayerBase *player);
void fn_80079860(PlayerBase *player);
/* Whether the player's behaviors allow standing (behaviour inferred). */
bool fn_800A7EEC(void *behaviors, int arg1);
/* Records the ground the player stands on and its normal (behaviour inferred). */
void fn_80078E00(PlayerBase *player, const UnkColliMoveContact_803334A4 *contact);
void fn_8009339C(boost::shared_ptr<PlayerBase> *player, const Vec3f *normal);
/* The footstep effect of the player's effects at 0x23C (behaviour inferred). */
void fn_800A299C(void *effects, u32 kind, const Vec3f *position, const Vec3f *normal);
}

Vec3f operator-(const Vec3f &a, const Vec3f &b);

inline void AssignVector(Vec3f &vector, const Vec3f &value) {
    fn_800091D0(&vector, &value);
}
inline void CopyVector(Vec3f &vector, const Vec3f &value) {
    fn_800091EC(&vector, &value);
}
inline void AddVector(Vec3f &a, const Vec3f &b) {
    fn_8040CFF8(&a, &b);
}
inline void SubVector(Vec3f &a, const Vec3f &b) {
    fn_8040CE04(&a, &b);
}
inline float Dot(const Vec3f &a, const Vec3f &b) {
    return fn_8040CF90(&a, &b);
}
inline float LengthSq(const Vec3f &v) {
    return v.x * v.x + v.y * v.y + v.z * v.z;
}

/* A vector returned through the out-of-line copy constructor (fn_800091EC). */
struct UnkCopyVector3_800091EC {
    UnkCopyVector3_800091EC() {}
    UnkCopyVector3_800091EC(const UnkCopyVector3_800091EC &v) {
        fn_800091EC(reinterpret_cast<Vec3f *>(this), reinterpret_cast<const Vec3f *>(&v));
    }

    float x;
    float y;
    float z;
};

/* boost/weak_ptr.hpp only declares weak_ptr::reset, which the game has out of line; this unit
 * emits the copy for CharaColliBaseCharInfo (boost 1.32's definition; as in Game/BaseDoor.cpp). */
namespace boost {
template <> inline void weak_ptr<CharaColliBaseCharInfo>::reset() {
    this_type().swap(*this);
}
} // namespace boost

/* Accepts a candidate hit of a collision test (name inferred). */
static bool AcceptHit(UnkPlayerColliCandidate_80099088 *candidate) {
    const UnkPlayerColliFilter_80097E74 *filter = candidate->m_filter;
    if (candidate->m_attributes & filter->m_exclude) {
        return false;
    }
    if (candidate->m_collider != 0) {
        CharaColliBase *collider;
        ColliderList::const_iterator it = filter->m_ignore.begin();
        collider = candidate->m_collider;
        for (; it != filter->m_ignore.end(); it++) {
            if (collider == (*it).get()) {
                return false;
            }
        }
        if (filter->m_player.px != 0) {
            CharaColliBaseCharInfo *charCollider = dynamic_cast<CharaColliBaseCharInfo *>(candidate->m_collider);
            if (charCollider != 0) {
                boost::shared_ptr<CharInfo> owner = fn_800586EC(charCollider).lock();
                if (owner.get() != 0) {
                    s32 team = owner->field_0x5C;
                    if (team == 1 || team == 2) {
                        s32 own = filter->m_player.px->field_0x5C;
                        if (own != team && own != 3) {
                            return false;
                        }
                        if (fn_8006CD78(World(filter->m_player.px)->m_stateBits, 8)) {
                            return false;
                        }
                    }
                }
            }
        }
    }
    return (candidate->m_attributes & filter->m_mask) != 0;
}

/* Sweeps the player's sphere from position along velocity through the world, sliding along what
 * it touches; normal receives the average ground normal and contact what the player stands on
 * (name inferred). Returns whether the player stands on the ground. */
extern "C" bool fn_800986F0(Vec3f *normal, u32 groundMask, u32 mask, u32 exclude,
                            boost::shared_ptr<PlayerBase> *player, Vec3f *position, Vec3f *velocity,
                            const ColliderList *ignore, Vec3f *lastMove,
                            UnkColliMoveContact_803334A4 *contact, int count) {
    void *world = fn_80020D40();
    CollisionResultList *previous = 0;
    UnkSphere_80021C04 sphere;
    fn_80021C04(&sphere, position, World(player->px)->m_radius);
    Vec3f move;
    CopyVector(move, *velocity);
    UnkPlayerColliFilter_80097E74 filter;
    filter.m_mask = mask;
    filter.m_exclude = exclude;
    filter.m_player = *player;
    if (player->px != 0 && World(player->px)->m_colli != 0) {
        filter.m_ignore.push_back(fn_800959E0(World(player->px)->m_colli));
    }
    if (ignore != 0) {
        filter.m_ignore.insert(filter.m_ignore.end(), ignore->begin(), ignore->end());
    }
    float grounds = 0.0f;
    Vec3f best;
    CopyVector(best, lbl_805E07A0);
    float bestGrounds = 0.0f;
    if (contact != 0) {
        contact->Reset();
    }
    CollisionResultList *hits;
    bool again;
    bool onGround;
    do {
        again = false;
        onGround = false;
        if (lastMove != 0) {
            AssignVector(*lastMove, move);
        }
        Vec3f start;
        CopyVector(start, move);
        hits = fn_80021934(world, &sphere, &move, 3, 2, previous, AcceptHit, &filter, 0);
        if (previous != 0) {
            fn_80026FB4(previous, 1);
            previous = 0;
        }
        fn_8040CFF8(reinterpret_cast<Vec3f *>(sphere.center), &move);
        if (hits != 0) {
            fn_8040CE60(normal);
            grounds = 0.0f;
            Vec3f left;
            CopyVector(left, Subtract(&start, &move));
            Vec3f remain;
            CopyVector(remain, left);
            Vec3f push(0.0f, 0.0f, 0.0f);
            float pushes = 0.0f;
            UnkColliContactNode_800587A0 *node;
            for (UnkColliContactIter_8001EDFC it(fn_8001EE04(hits)); (node = it.m_node) != fn_8001EDEC(hits);
                 fn_80027374(&it, 0)) {
                void *object = node->m_object;
                bool stopped = false;
                Vec3f hitNormal;
                CopyVector(hitNormal, *fn_800266D4(object));
                Vec3f pushOut;
                CopyVector(pushOut, *fn_8001E9C4(object));
                if (Dot(hitNormal, remain) <= 0.0f) {
                    SubVector(remain, fn_80093230(&hitNormal));
                }
                if (!(remain.x * remain.x + remain.y * remain.y + remain.z * remain.z < 0.0001f) &&
                    Dot(remain, *velocity) < 0.0f) {
                    fn_8040CE60(&remain);
                    stopped = true;
                }
                if (fn_8040CF90(&hitNormal, &World(player->px)->m_velocity) <= 0.0f) {
                    if (!fn_8006CD78(World(player->px)->m_stateBits, 2) &&
                        fn_8040CF90(&World(player->px)->m_velocity, &World(player->px)->m_up) < 0.0f) {
                        float speed = fn_8040CF50(&World(player->px)->m_velocity);
                        if (!fn_8000A950(speed)) {
                            fn_8040F990(0, &World(player->px)->m_velocity, &World(player->px)->m_velocity,
                                        &hitNormal);
                            fn_800091D0(&World(player->px)->m_velocity, &(const Vec3f &)fn_8040CDA8(&fn_8040CEE8(&World(player->px)->m_velocity), speed));
                        }
                    } else {
                        fn_8040F990(0, &World(player->px)->m_velocity, &World(player->px)->m_velocity,
                                    &hitNormal);
                    }
                }
                AddVector(reinterpret_cast<Vec3f &>(sphere.center), fn_800091A4(&hitNormal, s_skin));
                if (fn_80099080(object) == 1) {
                    Vec3f offset;
                    CopyVector(offset, Subtract(reinterpret_cast<Vec3f *>(sphere.center), fn_8001E9BC(object)));
                    float distance = Dot(offset, hitNormal);
                    float reach = sphere.radius + s_skin;
                    if (distance >= 0.0f && fn_8040CF2C(&offset) < reach * reach) {
                        Vec3f out;
                        CopyVector(out, fn_8040CDA8(&hitNormal, reach - distance));
                        fn_8040CFF8(reinterpret_cast<Vec3f *>(sphere.center), &out);
                        fn_8040CFF8(&push, &out);
                        pushes += 1.0f;
                    }
                }
                u32 attributes = node->GetAttributes();
                if (!stopped && (attributes & groundMask) &&
                    fn_8040CF90(&pushOut, &World(player->px)->m_gravityUp) >= 0.17364818f) {
                    fn_8040CFF8(normal, &pushOut);
                    grounds += 1.0f;
                }
                count--;
                if (attributes & 0x8000) {
                    fn_80014E1C(&World(player->px)->m_touched, 0);
                }
                if (attributes & 0x4) {
                    fn_80014E1C(&World(player->px)->m_touched, 1);
                }
                if (attributes & 0x20) {
                    fn_80014E1C(&World(player->px)->m_touched, 2);
                }
                if (attributes & 0x400) {
                    fn_80014E1C(&World(player->px)->m_touched, 4);
                }
                boost::shared_ptr<CharaColliBase> collider = fn_800587A0(node).lock();
                boost::weak_ptr<CharInfo> owner;
                if (collider.get() != 0) {
                    boost::shared_ptr<CharaColliBaseCharInfo> charCollider =
                        boost::dynamic_pointer_cast<CharaColliBaseCharInfo>(collider);
                    owner = fn_800586EC(charCollider.get());
                }
                if (contact != 0 && !owner.expired()) {
                    contact->Set(owner, fn_800587A0(node), node->m_collider);
                }
                if (PlayerCollisionEventCallback(player->px).IsSet()) {
                    Player::CollisionEventArgs args;
                    args.m_player = *player;
                    args.m_contact = node;
                    args.m_chara = owner;
                    PlayerCollisionEventCallback(player->px).Call(args);
                }
            }
            if (pushes > 1.0f) {
                Vec3f average;
                CopyVector(average, fn_8040CDA8(&push, 1.0f / pushes));
                fn_8040CE04(reinterpret_cast<Vec3f *>(sphere.center), &average);
            }
            if (pushes > 0.0f) {
                float limit = 0.48f * sphere.radius;
                if (fn_8040CF2C(&push) > limit * limit && fn_8040CF2C(&remain) > limit * limit) {
                    AssignVector(remain, fn_8040CDA8(&fn_8040CEE8(&remain), limit));
                }
            }
            if (fn_8040CF2C(&remain) > 0.001f) {
                again = true;
                AssignVector(move, remain);
            }
        }
        if (again) {
            previous = hits;
            hits = 0;
        }
        if (grounds > 0.0f) {
            AssignVector(best, *normal);
            bestGrounds = grounds;
        }
    } while (again && count > 0);
    if (hits != 0) {
        fn_80026FB4(hits, 1);
    }
    if (previous != 0) {
        fn_80026FB4(previous, 1);
    }
    if (grounds < 1.0f) {
        AssignVector(*normal, best);
        grounds = bestGrounds;
    }
    if (grounds >= 1.0f) {
        onGround = true;
        fn_8040CE74(normal);
    }
    AssignVector(*position, reinterpret_cast<Vec3f &>(sphere.center));
    return onGround;
}

/* Tests the ground below position: what it touches and where (the footstep effect and the
 * flags of the ground; name inferred). */
extern "C" void fn_800984DC(boost::shared_ptr<PlayerBase> *player, const Vec3f *position) {
    u32 attributes = 0;
    u32 kind = 0;
    Line line;
    fn_800091D0(&line.m_start, &(const Vec3f &)fn_8040CDD0(position, &fn_8040CDA8(&World(player->px)->m_gravityUp,
                                                                     World(player->px)->m_radius - 0.1f)));
    fn_800091D0(&line.m_end, &(const Vec3f &)Subtract(position, &fn_8040CDA8(&World(player->px)->m_gravityUp,
                                                             World(player->px)->m_radius - 0.1f)));
    Vec3f hitPosition;
    Vec3f hitNormal;
    float distance = fn_800979E8(player, &hitPosition, &hitNormal, 0, &attributes, &kind, &line, 0, 0x24,
                                 0x40000, 2);
    if (distance <= 0.0f) {
        fn_800091D0(&line.m_start, &World(player->px)->m_lastPosition);
        AssignVector(line.m_end, *position);
        if (!(LengthSq(line.m_start - line.m_end) < 0.0001f)) {
            distance = fn_800979E8(player, &hitPosition, &hitNormal, 0, &attributes, &kind, &line, 0, 0x24,
                                   0x40000, 2);
        }
    }
    if (distance >= 0.0f) {
        if (attributes & 0x4) {
            fn_80014E1C(&World(player->px)->m_touched, 1);
        }
        if (attributes & 0x20) {
            fn_80014E1C(&World(player->px)->m_touched, 3);
        }
        if (World(player->px)->m_effects != 0) {
            fn_800A299C(World(player->px)->m_effects, kind, &hitPosition, &hitNormal);
        }
    }
}

/* Moves position out of the world objects within the player's radius of it (name inferred). */
extern "C" UnkCopyVector3_800091EC fn_80098224(boost::shared_ptr<PlayerBase> *player,
                                               const Vec3f *position) {
    void *world = fn_80020D40();
    UnkPlayerColliFilter_80097E74 filter;
    filter.m_mask = 0x1A93;
    filter.m_exclude = 0x40000;
    filter.m_player = *player;
    UnkCopyVector3_800091EC point;
    fn_800091EC(reinterpret_cast<Vec3f *>(&point), position);
    float radius = World(player->px)->m_radius;
    float reach = radius + 1.0f;
    float lift = radius + s_skin;
    const UnkPlayerColliFilter_80097E74 *self = &filter;
    for (int i = 3; i > 0; i--) {
        UnkHitList_800270CC list;
        UnkSphere_80021C04 sphere;
        fn_80021C04(&sphere, reinterpret_cast<Vec3f *>(&point), reach);
        CollisionResultList *hits = fn_80021CA4(world, &sphere, 1, 2, AcceptHit, self, &list);
        if (hits != 0) {
            void *object = fn_8002735C(hits)->m_object;
            if (fn_80097D0C(object) < radius) {
                AssignVector(reinterpret_cast<Vec3f &>(point),
                             fn_8040CDD0(fn_8001E9BC(object), &fn_8040CDA8(fn_800266D4(object), lift)));
                fn_80026FB4(hits, 1);
                fn_80026A40(reinterpret_cast<CollisionResultList *>(&list));
                continue;
            }
            fn_80026FB4(hits, 1);
        }
        bool clear = true;
        UnkColliContactNode_800587A0 *node;
        for (UnkColliContactIter_8001EDFC it(fn_8001EE04(&list)); (node = it.m_node) != fn_8001EDEC(&list);
             fn_80027374(&it, 0)) {
            void *object = node->m_object;
            Line line;
            AssignVector(line.m_start, reinterpret_cast<Vec3f &>(point));
            AssignVector(line.m_end, *fn_8001E9BC(object));
            UnkColliContactNode_800587A0 *blocked = fn_80021E24(world, &line, 1, 2, AcceptHit, self);
            if (blocked != 0) {
                fn_800265A4(blocked, 1);
            } else {
                AssignVector(reinterpret_cast<Vec3f &>(point),
                             fn_8040CDD0(fn_8001E9BC(object), &fn_8040CDA8(fn_800266D4(object), lift)));
                clear = false;
                break;
            }
        }
        fn_80026A40(reinterpret_cast<CollisionResultList *>(&list));
        if (clear) {
            break;
        }
    }
    return point;
}

/* Looks for an edge in reach of the player: where to hang, the edge's normal and its character
 * (behaviour inferred; see Game/PlayerBehaviorFall.cpp). */
extern "C" bool fn_80097EB0(Vec3f *position, Vec3f *normal, boost::weak_ptr<CharInfo> *chara,
                            boost::shared_ptr<PlayerBase> *player, const UnkEdgeRange_80097EB0 *range) {
    Line line;
    Vec3f front;
    fn_800091EC(&front, &(const Vec3f &)fn_800796DC(player->px));
    Vec3f up;
    fn_800091EC(&up, &World(player->px)->m_up);
    fn_800091D0(&line.m_end, &(const Vec3f &)fn_8040CDD0(&player->px->m_position,
                                         &fn_8040CDA8(&front, range->ahead + World(player->px)->m_radius)));
    AssignVector(line.m_start, fn_8040CDD0(&line.m_end, &fn_8040CDA8(&up, range->above)));
    Vec3f across;
    fn_8040F990(&across, 0, &Subtract(&player->px->m_position, &World(player->px)->m_lastPosition), &up);
    SubVector(line.m_start, across);
    Vec3f hitNormal;
    float distance = fn_800979E8(player, 0, &hitNormal, 0, 0, 0, &line, 0, 1, 0x40000, 2);
    if (Dot(hitNormal, up) < 0.866f) {
        return false;
    }
    if (distance >= 0.0f) {
        Vec3f offset;
        CopyVector(offset, Subtract(&line.m_end, &line.m_start));
        AssignVector(offset, fn_8040CDA8(&fn_8040CEE8(&offset), distance));
        Vec3f top;
        CopyVector(top, fn_8040CDD0(&line.m_start, &offset));
        fn_800091D0(&line.m_start, &player->px->m_position);
        fn_800091D0(&line.m_end, &(const Vec3f &)fn_8040CDD0(&line.m_start,
                                             &fn_8040CDA8(&front, range->ahead + World(player->px)->m_radius)));
        boost::weak_ptr<CharaColliBaseCharInfo> hit;
        distance = fn_800979E8(player, 0, normal, &hit, 0, 0, &line, 0, 0xFFFFFFFF, 0x40200, 2);
        if (chara != 0) {
            boost::shared_ptr<CharaColliBaseCharInfo> collider = hit.lock();
            if (collider.get() != 0) {
                *chara = fn_800586EC(collider.get());
            }
        }
        if (distance >= 0.0f) {
            AssignVector(offset, Subtract(&line.m_start, &line.m_end));
            float length = fn_8040CF50(&offset) - distance;
            AssignVector(offset, fn_8040CDA8(&fn_8040CEE8(&offset), length));
            AssignVector(*position, fn_8040CDD0(&top, &offset));
            return true;
        }
    }
    return false;
}

/* Casts line through the world for the player, skipping the player's own colliders and those in
 * ignore: the distance to what it hits first (negative when nothing is hit) and, where asked, the
 * hit's position, normal, collider, attributes and kind (name inferred). */
extern "C" float fn_800979E8(boost::shared_ptr<PlayerBase> *player, Vec3f *position, Vec3f *normal,
                             boost::weak_ptr<CharaColliBaseCharInfo> *collider, u32 *attributes, u32 *kind,
                             const void *line, const ColliderList *ignore, u32 mask, u32 exclude,
                             int arg10) {
    UnkPlayerColliFilter_80097E74 filter;
    filter.m_mask = mask;
    filter.m_exclude = exclude;
    filter.m_player = *player;
    if (player->px != 0 && World(player->px)->m_colli != 0) {
        filter.m_ignore.push_back(fn_800959E0(World(player->px)->m_colli));
    }
    if (ignore != 0) {
        filter.m_ignore.insert(filter.m_ignore.end(), ignore->begin(), ignore->end());
    }
    void *object;
    UnkColliContactNode_800587A0 *hit = fn_80021E24(fn_80020D40(), line, 3, arg10, AcceptHit, &filter);
    if (hit != 0) {
        object = hit->m_object;
        float distance = fn_80097D0C(object);
        if (distance <= 0.0f) {
            distance = -1.0f;
        } else {
            if (position != 0) {
                AssignVector(*position, *fn_8001E9BC(object));
            }
            if (normal != 0) {
                AssignVector(*normal, *fn_800266D4(object));
            }
            if (collider != 0) {
                collider->reset();
                boost::shared_ptr<CharaColliBase> hitCollider = fn_800587A0(hit).lock();
                if (hitCollider.get() != 0) {
                    *collider = boost::dynamic_pointer_cast<CharaColliBaseCharInfo>(hitCollider);
                }
            }
            if (attributes != 0) {
                *attributes = hit->GetAttributes();
            }
            if (kind != 0) {
                *kind = hit->GetKind();
            }
        }
        fn_800265A4(hit, 1);
        return distance;
    }
    return -1.0f;
}

/* fn_800979E8 without the hit's details (name inferred). */
extern "C" float fn_800979A0(boost::shared_ptr<PlayerBase> *player, const void *line,
                             const ColliderList *ignore, u32 mask, u32 exclude, int arg5) {
    return fn_800979E8(player, 0, 0, 0, 0, 0, line, ignore, mask, exclude, arg5);
}

/* The player's collision against the world, skipping the colliders in ignore: sweeps the player
 * from its last position to its position, finds the ground below it, keeps it out of the world
 * and records where it stands (behaviour inferred). */
extern "C" void fn_8009728C(boost::shared_ptr<PlayerBase> *player, const ColliderList *ignore) {
    bool landed = false;
    fn_8006127C(&World(player->px)->m_touched, 0);
    fn_8006127C(&World(player->px)->m_touched, 4);
    Vec3f position;
    fn_800091EC(&position, &World(player->px)->m_lastPosition);
    Vec3f move;
    fn_800091EC(&move, &(const Vec3f &)Subtract(&player->px->m_position, &World(player->px)->m_lastPosition));
    Vec3f remain;
    Vec3f normal(0.0f, 0.0f, 0.0f);
    UnkColliMoveContact_803334A4 contact;
    u32 groundMask;
    u32 mask = 0x1A93;
    mask &= kNotWater;
    groundMask = 1;
    if (fn_8006CD78(World(player->px)->m_stateBits, 2)) {
        mask |= kWater;
    }
    if (fn_8006CD78(World(player->px)->m_stateBits, 0xD)) {
        groundMask = lbl_805E4514 | 1;
    }
    bool onGround = fn_800986F0(&normal, groundMask, mask, 0x40000, player, &position, &move, ignore, &remain,
                                0, 8);
    if (onGround) {
        if (fn_8040CF90(&normal, &World(player->px)->m_gravityUp) >= 0.17364818f && Dot(normal, move) <= 0.0f) {
            landed = true;
        } else {
            fn_800091D0(&normal, &World(player->px)->m_groundNormal);
        }
    } else {
        fn_800091D0(&normal, &World(player->px)->m_groundNormal);
    }
    if (fn_8006CD78(World(player->px)->m_stateBits, 0xD)) {
        Vec3f down;
        float length = fn_8040CF50(&move);
        fn_800091D0(&down, &(const Vec3f &)fn_8040CDA8(&World(player->px)->m_groundNormal,
                                                       -(2.37f * World(player->px)->m_radius + length)));
        UnkSegment_80020A20 segment;
        fn_80020A20(&segment, &position, &fn_8040CDD0(&position, &down));
        Vec3f hit;
        if (fn_800979E8(player, &hit, 0, 0, 0, 0, &segment, 0, mask, 0x40000, 2) >= 0.0f) {
            Vec3f start;
            CopyVector(start, position);
            Vec3f back;
            AssignVector(back, Subtract(&hit, &start));
            SubVector(back, fn_800091A4(&fn_8040CEE8(&back), fn_8005AE20(0.1f)));
            Vec3f groundNormal;
            if (fn_800986F0(&groundNormal, groundMask, mask, 0x40000, player, &start, &back, 0, 0, 0, 1)) {
                AssignVector(position, start);
                AssignVector(normal, groundNormal);
            }
        }
    } else {
        Vec3f up;
        fn_800091D0(&up, &World(player->px)->m_gravityUp);
        Vec3f down;
        fn_800091D0(&down, &(const Vec3f &)fn_8040CDA8(&up, -(fn_8040CF50(&move) + 2.37f * World(player->px)->m_radius)));
        UnkSegment_80020A20 segment;
        fn_80020A20(&segment, &position, &fn_8040CDD0(&position, &down));
        Vec3f hit;
        float distance = fn_800979E8(player, &hit, 0, 0, 0, 0, &segment, 0, mask, 0x40000, 2);
        AssignVector(down, Subtract(&hit, &position));
        SubVector(down, fn_800091A4(&fn_8040CEE8(&down), fn_8005AE20(0.1f)));
        if (distance < 0.0f) {
            distance = 100000.0f;
        }
        bool near = false;
        if (!(distance > 2.37f * World(player->px)->m_radius + fn_8040CF50(&move))) {
            if (distance <= 2.0f * World(player->px)->m_radius) {
                near = true;
            } else {
                Line line;
                float radiusSq = World(player->px)->m_radius * World(player->px)->m_radius;
                Vec3f reach;
                if (fn_8040CF2C(&remain) < radiusSq) {
                    fn_800091D0(&reach, &(const Vec3f &)Scale(fn_8040CEE8(&remain), RadiusOf(player->px)));
                } else {
                    AssignVector(reach, remain);
                }
                fn_800091D0(&line.m_start, &(const Vec3f &)Subtract(&position, &fn_8040CDA8(&up, 0.5f * (distance + World(player->px)->m_radius))));
                AssignVector(line.m_end, Subtract(&line.m_start, &reach));
                Vec3f wallHit;
                Vec3f wallNormal;
                if (fn_800979E8(player, &wallHit, &wallNormal, 0, 0, 0, &line, 0, mask, 0x40000, 2) > 0.0f &&
                    Dot(wallNormal, up) > 0.17364818f) {
                    near = true;
                }
            }
        }
        if ((fn_8006CD78(World(player->px)->m_stateBits, 2) || onGround) && near) {
            Vec3f start;
            CopyVector(start, position);
            Vec3f groundNormal;
            CopyVector(groundNormal, normal);
            if (fn_800986F0(&groundNormal, groundMask, mask, 0x40000, player, &start, &down, 0, 0, &contact, 1)) {
                if (fn_8040CF90(&groundNormal, &World(player->px)->m_gravityUp) >= 0.17364818f) {
                    AssignVector(position, start);
                    AssignVector(normal, groundNormal);
                    landed = true;
                }
            }
        } else {
            landed = false;
        }
    }
    fn_800091D0(&position, reinterpret_cast<const Vec3f *>(&fn_80098224(player, &position)));
    fn_800984DC(player, &position);
    fn_800091D0(&player->px->m_position, &position);
    if (landed) {
        fn_80076CA8(World(player->px)->m_stateBits, 2);
    } else {
        fn_80076CC8(World(player->px)->m_stateBits, 2);
    }
    if (fn_8006CD78(World(player->px)->m_stateBits, 0x36)) {
        fn_800091D0(&normal, &World(player->px)->m_gravityUp);
    }
    fn_8009339C(player, &normal);
    if (!fn_8006CD78(World(player->px)->m_stateBits, 0x37) && !fn_80096E20(0, player)) {
        fn_80078E00(player->px, &contact);
    }
}

/* Marks the special objects within reach of position (bits 5 to 7 of the flags); returns whether
 * none of kind 0x71 is near (name and meaning inferred). */
extern "C" bool fn_80097050(const Vec3f *position) {
    void *world = fn_80020D40();
    void *manager = fn_80021B08();
    UnkColliMoveFlags flags;
    fn_80014E10(&flags.m_bits);
    UnkSphere_80021C04 sphere;
    fn_80021C04(&sphere, position, 15.0f);
    CollisionResultList *hits = fn_80021CA4(world, &sphere, 2, 0, 0, 0, 0);
    if (hits != 0) {
        for (UnkColliContactIter_8001EDFC it(fn_8001EE04(hits)); it.m_node != fn_8001EDEC(hits);
             fn_80027374(&it, 0)) {
            boost::shared_ptr<CharaColliBase> collider = fn_800587A0(it.m_node).lock();
            if (collider.get() != 0) {
                boost::shared_ptr<CharInfo> chara =
                    fn_800C4568(manager, boost::weak_ptr<CharaColliBase>(collider)).lock();
                if (chara.get() != 0) {
                    switch (chara->GetKind()) {
                    case 0x6F:
                        fn_80014E1C(&flags.m_bits, 6);
                        break;
                    case 0x70:
                        fn_80014E1C(&flags.m_bits, 5);
                        break;
                    case 0x71:
                        fn_80014E1C(&flags.m_bits, 7);
                        break;
                    }
                }
            }
        }
        fn_80026FB4(hits, 1);
    }
    if (fn_80014E34(&flags.m_bits, 7)) {
        return false;
    }
    return true;
}

/* Whether the player, moving fast, passed over ground it should stand on: then out receives where
 * (behaviour inferred). */
extern "C" bool fn_80096E20(Vec3f *out, boost::shared_ptr<PlayerBase> *player) {
    Line line;
    if (!fn_800A7EEC(World(player->px)->m_behaviors, 1)) {
        return false;
    }
    if (fn_8006CD78(World(player->px)->m_stateBits, 0x2E)) {
        return false;
    }
    if (fn_80014E34(&World(player->px)->m_touched, 4)) {
        return false;
    }
    fn_80079860(player->px);
    if (!World(player->px)->m_ground->m_active || World(player->px)->m_ground->m_time >= s_standTime) {
        if (fn_8040CF2C(&World(player->px)->m_velocity) > 10000.0f) {
            Vec3f down;
            fn_800091EC(&down, &(const Vec3f &)fn_8040CDA8(&World(player->px)->m_groundNormal,
                                         -(0.2f + World(player->px)->m_radius)));
            fn_800091D0(&line.m_start, &(const Vec3f &)fn_8040CDD0(&player->px->m_position, &down));
            fn_800091D0(&line.m_end, &(const Vec3f &)fn_8040CDD0(&World(player->px)->m_lastPosition, &down));
            float distance = fn_800979E8(player, 0, 0, 0, 0, 0, &line, 0, 0x1A93, 0x40000, 2);
            if (distance > 0.0f) {
                Vec3f offset;
                CopyVector(offset, Subtract(&line.m_end, &line.m_start));
                float length = fn_8040CF50(&offset);
                distance += 1.0f;
                if (distance > length) {
                    distance = length;
                }
                AssignVector(offset, fn_8040CDA8(&fn_8040CEE8(&offset), distance));
                if (out != 0) {
                    fn_800091D0(out, &(const Vec3f &)fn_8040CDD0(&player->px->m_position, &offset));
                }
                return true;
            }
        }
    }
    return false;
}

/* Whether the player has room to stand up to height above it (behaviour inferred; see
 * Game/PlayerBehaviorSliding.cpp). */
extern "C" bool fn_80096D3C(boost::shared_ptr<PlayerBase> *player, float height) {
    Line line;
    fn_800091D0(&line.m_start, &player->px->m_position);
    fn_800091D0(&line.m_end, &(const Vec3f &)fn_8040CDD0(&player->px->m_position,
                                         &fn_8040CDA8(&fn_80079698(player->px), height)));
    float distance = fn_800979E8(player, 0, 0, 0, 0, 0, &line, 0, 0x1A93, 0x40000, 2);
    if (distance < 0.0f || distance > height) {
        return true;
    }
    return false;
}

/* Sets an attack collision's mode and its attack callback (behaviour inferred; called by many
 * gadgets). */
extern "C" void fn_80096B64(CharaColliAttack *attack, int mode, const AttackCallback &callback) {
    attack->field_0x55 = 1;
    AttackMode(attack) = mode;
    attack->m_attackCallback = callback;
}

namespace Player {

PlayerCharaColliImpl::PlayerCharaColliImpl(const boost::shared_ptr<PlayerBase> &player)
    : m_player(player), m_attacking(0) {
    void *manager = fn_80021B08();
    m_body = fn_800C799C(manager, &Vec3f(0.0f, 0.0f, 0.0f), &lbl_805E07B8, 0, 0x302, 2, 0,
                         boost::weak_ptr<CharInfo>(m_player), &lbl_805E07A0, &lbl_805E07B8, 0, 10.0f,
                         1.1f * RadiusOf(m_player.get()));
    m_attack = fn_800C4374(manager, m_body);
    boost::shared_ptr<CharaColliAttack> attack = m_attack.lock();
    if (attack.get() != 0) {
        fn_80096B64(attack.get(), 0, AttackCallback(boost::bind(&PlayerCharaColliImpl::OnAttack, this, _1)));
    }
    TurnOff();
}

PlayerCharaColliImpl::~PlayerCharaColliImpl() {
    if (m_body.get() != 0) {
        fn_800C78B0(fn_80021B08(), &m_body);
    }
}

void PlayerCharaColliImpl::TurnOn() {
    m_body->TurnOn(-1);
}

void PlayerCharaColliImpl::TurnOff() {
    m_body->TurnOff(-1);
}

void PlayerCharaColliImpl::SetAttacking(bool attacking) {
    boost::shared_ptr<CharaColliAttack> attack = m_attack.lock();
    if (attack.get() != 0) {
        if (attacking) {
            m_attacking++;
        } else {
            m_attacking--;
        }
        if (m_attacking > 0) {
            AttackMode(attack.get()) = 2;
        } else {
            AttackMode(attack.get()) = 0;
        }
    }
}

int PlayerCharaColliImpl::GetAttackMode() const {
    boost::shared_ptr<CharaColliAttack> attack = m_attack.lock();
    if (attack.get() != 0) {
        return AttackMode(attack.get());
    }
    return 0;
}

void PlayerCharaColliImpl::Update() {
    UnkColliContacts_8001EE04 *contacts;
    void *manager;
    fn_8006127C(&Fields(m_player.get())->m_touched, 5);
    fn_8006127C(&Fields(m_player.get())->m_touched, 6);
    fn_8006127C(&Fields(m_player.get())->m_touched, 7);
    fn_8006127C(&Fields(m_player.get())->m_touched, 8);
    fn_8006127C(&Fields(m_player.get())->m_touched, 9);
    {
        contacts =
            reinterpret_cast<UnkColliContacts_8001EE04 *>(reinterpret_cast<u8 *>(m_body.get()) + 0xC);
        manager = fn_80021B08();
        boost::shared_ptr<CharInfo> target = m_target.lock();
        m_nearest.reset();
        float nearest;
        for (UnkColliContactIter_8001EDFC it(fn_8001EE04(contacts)); it.m_node != fn_8001EDEC(contacts);
             fn_80027374(&it, 0)) {
            boost::weak_ptr<CharaColliBase> hit = fn_800587A0(it.m_node);
            boost::shared_ptr<CharInfo> chara = fn_800C4568(manager, hit).lock();
            if (chara.get() != 0 && !Slots(m_player.get())->Contains(chara)) {
                switch (chara->GetKind()) {
                case 0x72:
                    break;
                case 0x6F:
                    fn_80014E1C(&Fields(m_player.get())->m_touched, 6);
                    break;
                case 0x70:
                    fn_80014E1C(&Fields(m_player.get())->m_touched, 5);
                    break;
                case 0x71:
                    fn_80014E1C(&Fields(m_player.get())->m_touched, 7);
                    break;
                case 0x73:
                    fn_80014E1C(&Fields(m_player.get())->m_touched, 8);
                    break;
                case 0x74:
                    fn_80014E1C(&Fields(m_player.get())->m_touched, 9);
                    break;
                default:
                    if (target.get() != 0) {
                        if (target == chara) {
                            m_nearest = chara;
                        }
                    } else {
                        float distance = fn_8040D180(chara.get(), m_player.get());
                        if (m_nearest.expired() || nearest > distance) {
                            switch (chara->GetKind()) {
                            case 0x1B:
                            case 0x33:
                            case 0x34:
                            case 0x86:
                                m_nearest = chara;
                                nearest = distance;
                                break;
                            }
                        }
                    }
                    break;
                }
            }
        }
    }
    Vec3f center;
    fn_800091EC(&center, &(const Vec3f &)fn_8040CDA8(
                             &(const Vec3f &)fn_8040CDD0(&m_player->m_position,
                                                         &Fields(m_player.get())->m_lastPosition),
                             0.5f));
    Vec3f moved;
    fn_800091EC(&moved, &(const Vec3f &)Subtract(&m_player->m_position,
                                                   &Fields(m_player.get())->m_lastPosition));
    Vec2f angles = fn_8040F760(moved);
    Vec3f rotation(angles.x, angles.y, 0.0f);
    m_body->SetCharInfo(reinterpret_cast<CharInfo *>(&center));
    m_body->SetRotation(&rotation);
    m_body->SetSize(1.1f * RadiusOf(m_player.get()), 0.0f, fn_8040CF50(&moved) / 2.0f, -2);
}

void PlayerCharaColliImpl::SetTarget(const boost::weak_ptr<CharInfo> &target) {
    m_target = target;
}

void PlayerCharaColliImpl::ClearTarget() {
    m_target.reset();
}

boost::weak_ptr<CharInfo> PlayerCharaColliImpl::GetNearest() const {
    return m_nearest;
}

boost::shared_ptr<CharaColliBase> PlayerCharaColliImpl::GetBody() const {
    return m_body;
}

boost::weak_ptr<CharaColliAttack> PlayerCharaColliImpl::GetAttack() const {
    return m_attack;
}

/* Answers an attack that reaches the player's attack collision: attacks of the player itself, of
 * the characters that belong to it and of its own team are ignored (8); otherwise the player is
 * hurt (1). */
void PlayerCharaColliImpl::OnAttack(AttackCallbackParam &param) {
    boost::shared_ptr<CharaColliAttack> attack = param.m_attacker.lock();
    if (attack.get() != 0) {
        boost::shared_ptr<CharInfo> owner = attack->m_owner.lock();
        if (owner.get() != 0) {
            if (owner == m_player || Slots(m_player.get())->Contains(owner)) {
                param.m_result = 8;
                return;
            }
            if (owner->field_0x5C == 2) {
                param.m_result = 8;
                return;
            }
            switch (TeamOf(m_player.operator->())) {
            case 2:
                if (owner->field_0x5C == 1) {
                    param.m_result = 8;
                    return;
                }
                break;
            case 3:
                if (owner->field_0x5C == 3) {
                    param.m_result = 8;
                    return;
                }
            default:
                if (GetAttackMode() == 2 && attack->m_kind == 3) {
                    param.m_result = 5;
                    return;
                }
                break;
            }
            if (Fields(m_player.get())->m_character == 0xD) {
                fn_8020642C(&owner, 0x70);
            }
        }
    }
    if (fn_8006CD78(Fields(m_player.get())->m_stateBits, 0x21) && (attack->m_flags & 0x200)) {
        param.m_result = 8;
        return;
    }
    if (param.m_damage > 0.0f) {
        m_player->PushCommand(new DamageCommand(AttackDirection(param), 4, 10.0f));
    }
    param.m_result = 1;
}

} // namespace Player

UnkPlayerPart_80095BD4::UnkPlayerPart_80095BD4(const boost::shared_ptr<PlayerBase> &player)
    : m_impl(new Player::PlayerCharaColliImpl(player)) {}

UnkPlayerPart_80095BD4::~UnkPlayerPart_80095BD4() {}

extern "C" {

void fn_80095AF4(UnkPlayerPart_80095BD4 *colli) {
    colli->m_impl->TurnOn();
}

void fn_80095AD0(UnkPlayerPart_80095BD4 *colli) {
    colli->m_impl->TurnOff();
}

void fn_80095AAC(UnkPlayerPart_80095BD4 *colli, bool attacking) {
    colli->m_impl->SetAttacking(attacking);
}

void fn_80095A88(UnkPlayerPart_80095BD4 *colli) {
    colli->m_impl->Update();
}

void fn_80095A64(UnkPlayerPart_80095BD4 *colli, const boost::weak_ptr<CharInfo> &target) {
    colli->m_impl->SetTarget(target);
}

void fn_80095A40(UnkPlayerPart_80095BD4 *colli) {
    colli->m_impl->ClearTarget();
}

boost::weak_ptr<CharInfo> fn_80095A10(UnkPlayerPart_80095BD4 *colli) {
    return colli->m_impl->GetNearest();
}

boost::shared_ptr<CharaColliBase> fn_800959E0(UnkPlayerPart_80095BD4 *colli) {
    return colli->m_impl->GetBody();
}

boost::weak_ptr<CharaColliAttack> fn_800959B0(UnkPlayerPart_80095BD4 *colli) {
    return colli->m_impl->GetAttack();
}

}
