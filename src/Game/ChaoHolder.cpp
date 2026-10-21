/* ChaoHolder (class name from RTTI): the Chao gadget. It owns a Chao (class name from the RTTI of
 * its shared_ptr control block), a Chao or a Cheez by its "0:CHAO 1:CHEEZ" parameter, that walks a
 * circle of "MOVE_RADIUS m" around the owner's position at "MOVE_SPPED m/sec" and chirps now and
 * then. Touching it makes it turn to face the nearest player; for a Cheez the first touch also
 * turns on the switch whose id the owner supplies. An attack (that passes fn_8015D4E4) knocks it
 * over for good. Its state lives in a ChaoData (class name from RTTI) kept by the owner record, so
 * it survives the gadget being recreated. File name inferred from the class. Names marked "(name inferred)" are not
 * original. Definitions are in reverse address order (deferred inlining emits functions
 * last-defined first). */
#include <boost/bind.hpp>
#include <boost/function.hpp>
#include <cmath>

#include "Game/CharInfo.h"
#include "Game/CharaColli.h"
#include "Game/CharaColliAttack.h"
#include "Game/Gadget.h"
#include "Game/GadgetCharInfo.h"
#include "Game/GadgetCollision.h"
#include "Game/GadgetModelInfo.h"
#include "Game/GadgetClassDesc.h"
#include "Game/GadgetUtil.h"
#include "Game/Mission.h"
#include "Game/SetAdapterState.h"
#include "Game/UnkSwitchBank_800CB69C.h"
#include "Game/FileQueue.h"
#include "Game/Random.h"
#include "boost/shared_ptr.hpp"
#include "boost/weak_ptr.hpp"
#include "size_assert.h"

using SonicteamUSA::System::CharaColli::AttackCallback;
using SonicteamUSA::System::CharaColli::AttackCallbackParam;
using SonicteamUSA::System::CharaColli::CharaColliAttack;
using SonicteamUSA::System::CharaColli::CharaColliBase;

/* Target (class name from RTTI): a point other objects can aim at (see CircusBall.cpp). */
class Target {
  public:
    /* Moves the target to position (name inferred). */
    void fn_800CCE38(const Vec3f *position);
};

/* The positioned object fn_802B4254 creates (see CityBombBig.cpp; it may be a sound source). */
class UnkSound_802B4254;

class UnkPlayerManager_8006D6A8;
struct _stMtnPlayWork;
class UnkHintManager_801FB180;

/* The out-of-line copy of the inline accessor HintRing.cpp defines (0x80109D64). */
UnkHintManager_801FB180 *GetHintManager();

extern "C" {
/* A 32-bit flag word: fn_8000AAEC constructs one from a value, fn_80014E10 clears it, fn_80014E1C
 * sets bit and fn_80014E34 tests bit (behaviour from their bodies). */
u32 *fn_8000AAEC(u32 *flags, u32 value);
void fn_80014E10(u32 *flags);
void fn_80014E1C(u32 *flags, int bit);
bool fn_80014E34(u32 *flags, int bit);
/* Loads a gadget model (see GadgetUtil.h). */
boost::shared_ptr<GadgetModelInfo> fn_8015F800(int arg0, const char *fileName);
/* The motion work of a model's motion, the motion library's calls on it, and the model's motion
 * calls (behaviour inferred; see Fan.cpp and BreakRoad.cpp). */
boost::weak_ptr<_stMtnPlayWork> fn_801602B4(GadgetModelInfo *model, int motion);
int fn_80416F28(_stMtnPlayWork *work, int arg1, int arg2, int arg3);
void fn_804165D8(_stMtnPlayWork *work, int arg1, int arg2, int arg3);
void fn_80416148(_stMtnPlayWork *work, int arg1, float arg2);
void fn_80416DC8(_stMtnPlayWork *work, int arg1, float speed);
/* Stops motion of the model (behaviour inferred). */
void fn_80160670(GadgetModelInfo *model, int motion);
void fn_80160A84(GadgetModelInfo *model, int motion, int mode);
/* The collider manager, a sphere collider for charInfo, and the attack collision of a collider
 * (behaviour inferred; see StretchGrass.cpp and Bomb.cpp). */
void *fn_80021B08(void);
boost::shared_ptr<CharaColliBase> fn_800C7DEC(void *manager, const boost::weak_ptr<CharInfo> &charInfo,
                                              int flags, int arg3, int arg4, const Vec3f *offset,
                                              float radius);
boost::weak_ptr<CharaColliAttack> fn_800C4374(void *manager, boost::shared_ptr<CharaColliBase> collider);
/* Whether a character touches the collider (behaviour inferred). */
bool fn_8015D2EC(UnkColliContacts_8001EE04 *contacts);
/* Sets callback as the attack callback of an attack collision (see BreakRoad.cpp). */
void fn_80096B64(CharaColliAttack *attack, int arg1, const AttackCallback &callback);
/* A test on the attack (meaning unknown), and its attacker (behaviour inferred). */
bool fn_8015D4E4(AttackCallbackParam &param);
boost::shared_ptr<CharInfo> fn_8015D570(AttackCallbackParam &param);
/* Gives the attacker the reward for breaking the object (behaviour inferred). */
void fn_8020642C(const boost::shared_ptr<CharInfo> &attacker, int reward);
/* Creates a target with the given flags (see CircusBall.cpp). */
boost::shared_ptr<Target> fn_801E72DC(int flags);
/* The positioned object (see CityBombBig.cpp). */
boost::shared_ptr<UnkSound_802B4254> fn_802B4254(int kind, int arg1);
void fn_802B4498(UnkSound_802B4254 *sound, float range);
void fn_802B4510(UnkSound_802B4254 *sound, u8 flag);
void fn_802B44C8(UnkSound_802B4254 *sound, const Vec3f *position);
void fn_800CC978(boost::shared_ptr<UnkSound_802B4254> *sound);
/* Plays sound id at the character info's position (behaviour inferred). */
void fn_801D54F4(void *manager, int id, GadgetCharInfo *charInfo, int arg3, int arg4);
/* Sets switch id (behaviour and meaning of the last argument inferred). */
void fn_8016907C(UnkSwitchBank_800CB69C *manager, int id, int arg2);
/* Tells the owner that the switch fired (behaviour inferred; see ElecBomb.cpp). */
void fn_800C985C(UnkPlacement_800C9CD0 *owner);
/* The player manager and its entry (see CityBombBig.cpp). */
UnkPlayerManager_8006D6A8 *fn_8006D6A8(void);
boost::weak_ptr<Mission::Mission> fn_800740E4(UnkPlayerManager_8006D6A8 *manager, int index);
/* A hint message id (behaviour inferred), and showing a message (see HintRing.cpp). */
s32 fn_801FAAC4(int arg0);
s32 fn_801FAF00(UnkHintManager_801FB180 *manager, s32 id, s32 branch, int arg3, int arg4);
/* The character id (see Helicopter.cpp) and the character with an id (behaviour inferred). */
extern int lbl_805EE588;
boost::weak_ptr<CharInfo> fn_800A8B74(UnkCharaList_80074194 *manager, int id);
/* Whether |value| is below a small tolerance, and whether value is zero (from their bodies). */
bool fn_8000B044(float value);
bool fn_8000A950(float value);
/* Vector helpers (behaviour inferred): a + b, vector turned about the y axis, normalising (the
 * former length is returned), vector += value, a x b, the angle between two unit vectors, and the
 * pitch and yaw of a direction. */
Vec3f fn_8040CDD0(const Vec3f *a, const Vec3f *b);
Vec3f fn_8040D0AC(const Vec3f *vector, float angle);
float fn_8040CE74(Vec3f *vector);
void fn_8040CFF8(Vec3f *vector, const Vec3f *value);
Vec3f fn_8040CFB8(const Vec3f *a, const Vec3f *b);
float fn_8040F8FC(const Vec3f *a, const Vec3f *b);
void fn_8003B9E8(const Vec3f *direction, float *pitch, float *yaw);
}

/* fn_8040CD74 called as returning a - b by value (it stores the result through its first argument;
 * Game/MathTypes.h declares it that way; as in Helicopter.cpp). */
typedef Vec3f (*VectorSubtractFn)(const Vec3f *a, const Vec3f *b);

/* The contacts of a collider (the object holds them 0xC bytes in; name inferred). */
inline UnkColliContacts_8001EE04 *GetContacts(CharaColliBase *collider) {
    return reinterpret_cast<UnkColliContacts_8001EE04 *>(reinterpret_cast<u8 *>(collider) + 0xC);
}

/* Converts the parameters' metres to world units (name inferred). */
inline float MetresToUnits(float metres) {
    return 10.0f * metres;
}

/* A length in world units (name inferred). As a call it is not a constant expression, so the
 * constants below are stored by a static initializer, as in retail (see CityBombBig.cpp). */
inline float Span(float length) {
    return length;
}

/* The model's height above the ground, the collider's radius, the height of the positioned object,
 * its range, and the walking speed at which the walk motion plays at its normal rate (names
 * inferred). */
static float s_modelHeight = Span(2.0f);
static float s_colliRadius = Span(2.0f);
static float s_soundHeight = Span(8.0f);
static float s_soundRange = Span(80.0f);
static float s_walkSpeed = Span(7.0f);

/* The sounds a Chao chirps while walking (name inferred). */
static const u16 s_chirps[2] = {0x7447, 0x745D};

/* A 32-bit flag word (class name unknown). */
struct ChaoFlags {
    ChaoFlags(u32 value) {
        fn_8000AAEC(&m_bits, value);
    }
    void Clear() {
        fn_80014E10(&m_bits);
    }
    void Set(int bit) {
        fn_80014E1C(&m_bits, bit);
    }
    bool Test(int bit) {
        return fn_80014E34(&m_bits, bit);
    }

    u32 m_bits;
};

/* ChaoData (class name from RTTI): the Chao's state that the owner record keeps (member names
 * inferred). */
class ChaoData : public SetAdapterState {
  public:
    ChaoData() : m_flags(0) {}

    /* 0x04 */ ChaoFlags m_flags; /* bit 0 touched, bit 1 knocked over */
    /* 0x08 */ Vec3f m_position;
};
SIZE_ASSERT(ChaoData, 0x14);

/* A motion of a Chao type: the model's motion and the mode it plays in (names inferred). */
struct ChaoMotion {
    /* 0x0 */ s32 id;
    /* 0x4 */ s32 mode;
};

/* A Chao type: its model file and its motions for walking, touched, knocked over and lying (names
 * inferred). */
struct ChaoType {
    /* 0x00 */ const char *fileName;
    /* 0x04 */ ChaoMotion motions[4];
};
SIZE_ASSERT(ChaoType, 0x24);

/* The parameters the owner supplies (names from the parameter table). */
struct ChaoParams {
    /* 0x0 */ u32 type; /* 0 Chao, 1 Cheez */
    /* 0x4 */ float radius;
    /* 0x8 */ float speed;
};
SIZE_ASSERT(ChaoParams, 0xC);

class Chao;

class ChaoHolder : public Gadget {
  public:
    ChaoHolder(Task *parent, UnkPlacement_800C9CD0 *owner);
    virtual ~ChaoHolder();
    virtual void Behave(float dt);
    virtual void ReloadPlacement(UnkPlacement_800C9CD0 *owner);

    /* Creates the gadget under task layer 11 (name inferred). */
    static void Instantiate(UnkPlacement_800C9CD0 *owner);

    /* Method names are inferred. */
    /* Copies the owner's parameters. */
    void LoadFromOwner(UnkPlacement_800C9CD0 *owner);
    /* Creates the Chao of the type parameter. */
    void CreateChao(UnkPlacement_800C9CD0 *owner);
    ChaoParams &GetParams();

    static const char *s_taskName;

    /* Member names are inferred. */
    /* 0x30 */ boost::shared_ptr<Chao> m_chao;
    /* 0x38 */ ChaoParams m_params;
    /* 0x44 */ u32 pad_0x44; /* the class size is a multiple of 8 */
};
SIZE_ASSERT(ChaoHolder, 0x48);

class Chao {
  public:
    Chao(ChaoHolder *holder, UnkPlacement_800C9CD0 *owner);
    ~Chao();

    /* Method names are inferred. */
    void Tick(float dt);
    /* Re-reads the owner's position, rotation and switch id. */
    void Reset(UnkPlacement_800C9CD0 *owner);
    /* Puts the Chao back at the start of its circle. */
    void ResetPosition();
    /* Walks the circle, or turns to the nearest player. */
    void Move(float dt);
    void RebuildTransform();
    void SetMotion(const ChaoMotion *motion, bool restart);
    void OnAttack(AttackCallbackParam &param);

    /* Member names are inferred. */
    /* 0x00 */ ChaoHolder *m_holder;
    /* 0x04 */ const ChaoType *m_type;
    /* 0x08 */ ChaoData *m_data;
    /* 0x0C */ Vec3f m_center;
    /* 0x18 */ Vec3f m_rotation;
    /* 0x24 */ boost::shared_ptr<GadgetCharInfo> m_chara;
    /* 0x2C */ boost::shared_ptr<Target> m_target;
    /* 0x34 */ boost::shared_ptr<UnkSound_802B4254> m_sound;
    /* 0x3C */ boost::shared_ptr<CharaColliBase> m_collider;
    /* 0x44 */ boost::shared_ptr<GadgetModelInfo> m_model;
    /* 0x4C */ s32 m_state; /* 0 walking, 1 touched, 2 knocked over, 3 lying */
    /* 0x50 */ float m_chirpTimer;
    /* 0x54 */ bool m_touched;
    /* 0x55 */ u8 m_switchId;
    /* 0x58 */ const ChaoMotion *m_motion;
};
SIZE_ASSERT(Chao, 0x5C);

/* Deletes a Chao (name inferred; see checked_delete below). */
inline void DeleteChao(Chao *chao) {
    delete chao;
}

/* Retail keeps boost::checked_delete<Chao> out of line (0x8012DCE4) and ~Chao at its own place
 * (0x8012EABC); this specialization calls an inline helper, as in Rocket.cpp (see
 * docs/research-notes.md). */
namespace boost {
template<> inline void checked_delete<Chao>(Chao *x) {
    DeleteChao(x);
}
} // namespace boost

/* The two Chao types. */
static const ChaoType lbl_804BE710[2] = {
    {"CHAO.BIN", {{0, 2}, {0, 1}, {1, 2}, {1, 1}}},
    {"CHEEZ.BIN", {{0, 2}, {0, 1}, {1, 2}, {1, 1}}},
};

Chao::Chao(ChaoHolder *holder, UnkPlacement_800C9CD0 *owner)
    : m_holder(holder), m_type(&lbl_804BE710[m_holder->m_params.type]), m_rotation(0.0f, 0.0f, 0.0f),
      m_touched(false), m_motion(0) {
    m_chara = fn_8015CD90(0x3C, 4);
    fn_800C9F14(owner, &m_center);
    fn_800C9ED4(owner, &m_rotation);
    fn_800C9E60(owner, &m_switchId);
    if (owner->m_adapterState == 0) {
        m_data = new ChaoData;
        m_data->m_flags.Clear();
        ResetPosition();
        fn_800C9930(owner, m_data);
    } else {
        m_data = dynamic_cast<ChaoData *>(owner->m_adapterState);
    }
    if (m_holder->m_params.type == 1) {
        if (m_data->m_flags.Test(0)) {
            m_state = 1;
        } else {
            if (m_data->m_flags.Test(1)) {
                m_state = 3;
            } else {
                m_state = 0;
            }
            m_sound = fn_802B4254(2, 0);
            fn_802B4498(m_sound.get(), s_soundRange);
            fn_802B4510(m_sound.get(), 1);
        }
    } else {
        if (m_data->m_flags.Test(1)) {
            m_state = 3;
        } else if (m_data->m_flags.Test(0)) {
            m_state = 1;
        } else {
            m_state = 0;
        }
    }
    m_model = fn_8015F800(1, m_type->fileName);
    SetMotion(&m_type->motions[m_state], false);
    m_collider = fn_800C7DEC(fn_80021B08(), m_chara, 2, 7, 0, &lbl_805E07A0, s_colliRadius);
    boost::weak_ptr<CharaColliAttack> attack = fn_800C4374(fn_80021B08(), m_collider);
    boost::shared_ptr<CharaColliAttack> collision = attack.lock();
    fn_80096B64(collision.get(), 1, boost::bind(&Chao::OnAttack, this, _1));
    m_target = fn_801E72DC(0x2000);
    RebuildTransform();
    m_chirpTimer = fn_8040EC80(0.5f, 6.0f);
}

Chao::~Chao() {}

void Chao::Tick(float dt) {
    if (fn_8015D2EC(GetContacts(m_collider.get()))) {
        if (!m_touched) {
            if (m_holder->m_params.type == 1) {
                if (m_state != 1 && m_state != 2) {
                    SetMotion(&m_type->motions[1], false);
                    m_state = 1;
                    fn_801D54F4(fn_80053164(), 0x745E, m_chara.get(), 0, 1);
                    if (!m_data->m_flags.Test(0)) {
                        m_data->m_flags.Set(0);
                        {
                            boost::shared_ptr<Mission::Mission> player =
                                fn_800740E4(fn_8006D6A8(), 2).lock();
                            if (player.get() != 0) {
                                player->AddCount(1, 0);
                                if (!player->IsState(1)) {
                                    fn_801FAF00(GetHintManager(), 0x3D2, fn_801FAAC4(2), 0, 0);
                                }
                            }
                        }
                        fn_800CC978(&m_sound);
                        fn_8016907C(fn_800CB69C(), m_switchId, 1);
                        if (m_holder->m_owner != 0) {
                            fn_800C985C(m_holder->m_owner);
                        }
                    }
                }
            } else if (m_state == 0) {
                SetMotion(&m_type->motions[1], true);
                m_state = 1;
                fn_801D54F4(fn_80053164(), 0x745E, m_chara.get(), 0, 1);
                if (!m_data->m_flags.Test(0)) {
                    m_data->m_flags.Set(0);
                }
            }
            m_touched = true;
        }
    } else {
        m_touched = false;
    }

    switch (m_state) {
    case 0:
        m_chirpTimer -= dt;
        if (m_chirpTimer < 0.0f) {
            fn_801D54F4(fn_80053164(), s_chirps[fn_8040EA90(2)], m_chara.get(), 0, 1);
            m_chirpTimer = fn_8040EC80(6.0f, 12.0f);
        }
        break;
    case 1:
        break;
    case 2: {
        int done = fn_80416F28(
            fn_801602B4(m_model.get(), m_type->motions[2].id).lock().get(), 0, 3, 1);
        if (done != 0) {
            m_state = 3;
            SetMotion(&m_type->motions[3], true);
        }
        break;
    }
    }

    Move(dt);
    RebuildTransform();
    m_model->fn_80162FB0(dt);
}

void Chao::Reset(UnkPlacement_800C9CD0 *owner) {
    fn_800C9F14(owner, &m_center);
    fn_800C9ED4(owner, &m_rotation);
    fn_800C9E60(owner, &m_switchId);
    ResetPosition();
    RebuildTransform();
    m_model->fn_80162EF4();
}

ChaoParams &ChaoHolder::GetParams() {
    return m_params;
}

void Chao::ResetPosition() {
    Vec3f offset(0.0f, 0.0f, -m_holder->GetParams().radius);

    AssignVector(offset, fn_8040D0AC(&offset, m_rotation.y));
    AssignVector(m_data->m_position, fn_8040CDD0(&m_center, &offset));
    m_chara->m_rotation.y = m_rotation.y;
}

void Chao::Move(float dt) {
    switch (m_state) {
    case 0:
        if (!fn_8000B044(m_holder->m_params.radius) && !fn_8000B044(m_holder->m_params.speed)) {
            float angle = dt * m_holder->m_params.speed / m_holder->m_params.radius;
            Vec3f previous;
            Vec3f offset;
            Vec3f direction;

            fn_800091EC(&previous, &m_data->m_position);
            fn_800091EC(&offset, &(const Vec3f &)((VectorSubtractFn)fn_8040CD74)(
                                     &m_data->m_position, &m_center));
            fn_8040CE74(&offset);
            AssignVector(offset, fn_8040D0AC(&offset, angle));
            fn_8040CE38(&offset, m_holder->m_params.radius);
            fn_8040CFF8(&offset, &m_center);
            fn_800091EC(&direction, &(const Vec3f &)((VectorSubtractFn)fn_8040CD74)(
                                        &offset, &m_data->m_position));
            direction.y = 0.0f;
            fn_8040CE74(&direction);
            fn_8003B9E8(&direction, &m_chara->m_rotation.x, &m_chara->m_rotation.y);
            fn_800091D0(&m_data->m_position, &offset);

            boost::shared_ptr<_stMtnPlayWork> work =
                fn_801602B4(m_model.get(), m_type->motions[m_state].id).lock();
            if (work.get() != 0) {
                float speed;
                if (fn_8000A950(dt)) {
                    speed = 0.0f;
                } else {
                    speed = fn_8040CF50(&(const Vec3f &)((VectorSubtractFn)fn_8040CD74)(
                                &previous, &m_data->m_position)) /
                            dt;
                }
                fn_80416DC8(work.get(), 0, speed / (60.0f * (s_walkSpeed / 60.0f)));
            }
        }
        break;
    case 1: {
        boost::shared_ptr<CharInfo> player = fn_800A8B74(fn_80074194(), lbl_805EE588).lock();
        if (player.get() != 0) {
            Vec3f from;
            fn_800091EC(&from, &m_data->m_position);
            Vec3f to;
            fn_800091EC(&to, &player->m_position);
            from.y = 0.0f;
            to.y = 0.0f;
            Vec3f toPlayer;
            fn_800091EC(&toPlayer, &(const Vec3f &)((VectorSubtractFn)fn_8040CD74)(&to, &from));
            Vec3f forward(0.0f, 0.0f, 1.0f);
            fn_800091D0(&forward,
                        &(const Vec3f &)fn_8040D0AC(&forward, m_chara->m_rotation.y));
            fn_8040CE74(&toPlayer);
            Vec3f cross;
            fn_800091EC(&cross, &(const Vec3f &)fn_8040CFB8(&toPlayer, &forward));
            float turn = fn_8040F8FC(&toPlayer, &forward);
            float step = dt * (270.0f * (lbl_805FAD38 / 180.0f));
            if (std::fabs(turn) > step) {
                if (cross.y > 0.0f) {
                    m_chara->m_rotation.y -= step;
                } else {
                    m_chara->m_rotation.y += step;
                }
            }
        }
        break;
    }
    }
}

void Chao::RebuildTransform() {
    fn_800091D0(&m_chara->m_position, &m_data->m_position);
    m_chara->m_position.y += s_modelHeight;

    Mtx34f matrix;
    fn_8040C148(&matrix, &lbl_805E0720);
    fn_8040B59C(&matrix, &m_data->m_position);
    fn_8040B848(&matrix, m_chara->m_rotation.y);
    m_model->fn_801633DC(&matrix);
    m_collider->SetCharInfo(m_chara.get());
    m_target->fn_800CCE38(&m_chara->m_position);
    if (m_sound.get() != 0) {
        Vec3f offset(0.0f, s_soundHeight, 0.0f);
        const Vec3f &position = fn_8040CDD0(&offset, &m_chara->m_position);
        fn_802B44C8(m_sound.get(), &position);
    }
}

void Chao::SetMotion(const ChaoMotion *motion, bool restart) {
    if (m_motion == 0) {
        m_model->fn_80162A68(motion->id, 0);
        m_model->fn_80160C28(motion->id);
        m_model->fn_8015FF60();
    } else if (m_motion->id != motion->id) {
        fn_80160670(m_model.get(), m_motion->id);
        m_model->fn_80162804();
        m_model->fn_80162A68(motion->id, 0);
        m_model->fn_80160C28(motion->id);
        m_model->fn_8015FF60();
    }
    boost::shared_ptr<_stMtnPlayWork> work = fn_801602B4(m_model.get(), motion->id).lock();
    if (restart && work.get() != 0) {
        fn_804165D8(work.get(), 0, -1, -1);
        fn_80416148(work.get(), 0, 15.0f);
    }
    fn_80160A84(m_model.get(), motion->id, motion->mode);
    if (work.get() != 0) {
        fn_80416DC8(work.get(), 0, 1.0f);
    }
    m_motion = motion;
}

void Chao::OnAttack(AttackCallbackParam &param) {
    if (fn_8015D4E4(param) && m_state != 3 && m_state != 2) {
        param.m_result = 1;
        m_state = 2;
        SetMotion(&m_type->motions[2], false);
        if (!m_data->m_flags.Test(1)) {
            {
                boost::shared_ptr<CharInfo> attacker = fn_8015D570(param);
                if (attacker.get() != 0) {
                    fn_8020642C(attacker, 0x2F);
                }
            }
            fn_801D54F4(fn_80053164(), 0x7448, m_chara.get(), 0, 1);
            m_data->m_flags.Set(1);
        }
    } else {
        param.m_result = 8;
    }
}

/* Defined here: retail places its string after the RTTI names the Chao code emits. */
const char *ChaoHolder::s_taskName = "ChaoHolder";

ChaoHolder::ChaoHolder(Task *parent, UnkPlacement_800C9CD0 *owner) : Gadget(parent, owner) {
    m_name = s_taskName;
    LoadFromOwner(owner);
}

ChaoHolder::~ChaoHolder() {}

extern "C" void fn_8012DE94(void) {
    fn_8006B474("CHAO.BIN", 1);
    fn_8006B474("CHEEZ.BIN", 1);
}

extern "C" void fn_8012DE90(void) {}

void ChaoHolder::Instantiate(UnkPlacement_800C9CD0 *owner) {
    ChaoHolder *holder = new ChaoHolder(fn_801EB304(fn_8001C4F8(), 11), owner);
    holder->CreateChao(owner);
}

void ChaoHolder::Behave(float dt) {
    if (m_chao.get() != 0) {
        m_chao->Tick(dt);
    }
}

void ChaoHolder::ReloadPlacement(UnkPlacement_800C9CD0 *owner) {
    ChaoParams params = m_params;

    LoadFromOwner(owner);
    if (params.type != m_params.type) {
        CreateChao(owner);
    }
    m_chao->Reset(owner);
}

void ChaoHolder::LoadFromOwner(UnkPlacement_800C9CD0 *owner) {
    fn_800C9DBC(owner, &m_params, sizeof(m_params));
    m_params.radius = MetresToUnits(m_params.radius);
    m_params.speed = MetresToUnits(m_params.speed);
}

void ChaoHolder::CreateChao(UnkPlacement_800C9CD0 *owner) {
    m_chao = boost::shared_ptr<Chao>(new Chao(this, owner));
}

static GadgetParamDesc lbl_80528538[] = {
    {2, "0:CHAO 1:CHEEZ", 0, 0, 1, 0.0f, 0.0f, 0.0f, fn_800CBC20, 0},
    {4, "MOVE_RADIUS m", 0, 0, 0, 1.0f, 0.0f, 65536.0f, fn_800CBC20, 0},
    {4, "MOVE_SPPED m/sec", 0, 0, 0, 0.5f, -65536.0f, 65536.0f, fn_800CBC20, 0},
    {0},
};

GadgetClassDesc lbl_805285D8 = {
    "Chao", fn_8012DE94, fn_8012DE90, ChaoHolder::Instantiate, 0, 0, 0xBBE, 1, 0, 10, lbl_80528538,
};
