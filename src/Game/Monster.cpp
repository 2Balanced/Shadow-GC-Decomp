/* Monster (class name from RTTI): the Monster gadget, a model that travels along a circuit path
 * (path 0x22) ahead of the player in Circus Park. A MonsterControl tells it, through its character
 * info (MonsterCharInfo, class name from RTTI), to start (command 0), to fall back (1) or to stop
 * (2). Once started it follows the path at a speed between "MIN_SPPED m/sec" and "MAX_SPPED m/sec"
 * depending on how far ahead of the player it is, changing speed by at most "ACCEL m/sec^2" or
 * "DECEL m/sec^2"; an attack with flag bit 4 set destroys it. File name inferred from the class.
 * Method names marked "(name inferred)" are not original. Definitions are in reverse address order
 * (deferred inlining emits functions last-defined first). The path is a SonicteamUSA::System::PathCtrl
 * (Game/PathCtrl.h; RTTI of its shared_ptr control block at 0x80528A1C). */
#include <boost/bind.hpp>
#include <boost/function.hpp>
#include <cmath>

#include "Game/CharInfo.h"
#include "Game/CharaColli.h"
#include "Game/CharaColliAttack.h"
#include "Game/Gadget.h"
#include "Game/GadgetCharInfo.h"
#include "Game/GadgetModelInfo.h"
#include "Game/GadgetParticle.h"
#include "Game/GadgetClassDesc.h"
#include "Game/GadgetUtil.h"
#include "Game/MonsterCharInfo.h"
#include "Game/PathCtrl.h"
#include "Game/FileQueue.h"
#include "boost/shared_ptr.hpp"
#include "boost/weak_ptr.hpp"
#include "size_assert.h"

using SonicteamUSA::System::PathCtrl;
using SonicteamUSA::System::CharaColli::AttackCallback;
using SonicteamUSA::System::CharaColli::AttackCallbackParam;
using SonicteamUSA::System::CharaColli::CharaColliAttack;
using SonicteamUSA::System::CharaColli::CharaColliBase;

struct _stMtnPlayWork;
class UnkPathManager_8001B494;
class UnkCharManager_800DDF20;
class UnkColliderManager_80021B08;
/* MonsterControl (class name from RTTI; MonsterControl.cpp): only the call the monster makes is
 * declared here. */
class MonsterControl {
  public:
    /* Stops the trigger's collisions (name inferred). */
    void Stop();
};

extern "C" {
/* The path manager: finds path id for path, and moves position along path by distance (behaviour
 * inferred, see FootingMovable.cpp). */
UnkPathManager_8001B494 *fn_8001B494(void);
bool fn_8001C7B4(UnkPathManager_8001B494 *manager, PathCtrl *path, u32 id);
bool fn_8001C91C(UnkPathManager_8001B494 *manager, PathCtrl *path, int arg2,
                 Vec3f *position, Vec3f *rotation, int loop, float distance);
/* Path helpers (behaviour inferred, see FootingMovable.cpp). */
void fn_80018304(PathCtrl *path);
Vec3f fn_80018634(PathCtrl *path);
Vec3f fn_80018434(PathCtrl *path, int arg1, int arg2);
/* A test on the path (behaviour unknown). */
bool fn_800ABDCC(PathCtrl *path);

/* The character manager: registers and unregisters a character info (behaviour inferred). */
UnkCharManager_800DDF20 *fn_800DDF20(void);
void fn_801DCD8C(UnkCharManager_800DDF20 *manager, const boost::shared_ptr<CharInfo> &info);
void fn_801DCCF4(UnkCharManager_800DDF20 *manager, const boost::shared_ptr<CharInfo> &info);
/* The player's character info (behaviour inferred, see SavePoint.cpp). */
boost::weak_ptr<CharInfo> fn_800A8B74(UnkCharaList_80074194 *manager, int id);
extern int lbl_805EE588;

/* The collider manager; a box collider, and the attack collision of a collider (behaviour
 * inferred). */
UnkColliderManager_80021B08 *fn_80021B08(void);
boost::shared_ptr<CharaColliBase> fn_800C7C34(UnkColliderManager_80021B08 *manager,
                                              const boost::weak_ptr<CharInfo> &charInfo,
                                              const Vec3f *size, int flags, int arg4,
                                              int arg5, const Vec3f *offset,
                                              const Vec3f *rotation, int arg8);
boost::weak_ptr<CharaColliAttack> fn_800C4374(UnkColliderManager_80021B08 *manager,
                                              boost::shared_ptr<CharaColliBase> collider);
boost::weak_ptr<CharaColliAttack> fn_800C42D0(UnkColliderManager_80021B08 *manager,
                                              boost::shared_ptr<CharaColliBase> collider);
/* Sets callback as the attack callback of an attack collision (see BreakRoad.cpp). */
void fn_80096B64(CharaColliAttack *attack, int arg1, const AttackCallback &callback);
/* A test on the attack (behaviour unknown). */
bool fn_8015D4E4(AttackCallbackParam &param);
/* Sends message to the character (see Bomb.cpp). */
int fn_8020642C(const boost::shared_ptr<CharInfo> &character, int message);
extern Vec3f lbl_805E07B8;

/* Owner helpers (see BlockChange.cpp and SetVehicle.cpp). */
bool fn_800C9A64(UnkPlacement_800C9CD0 *owner);
bool fn_800C996C(UnkPlacement_800C9CD0 *owner);
float fn_800C9938(UnkPlacement_800C9CD0 *owner);

/* Loads model fileName, and model helpers (behaviour inferred). */
boost::shared_ptr<GadgetModelInfo> fn_8015F800(int arg0, const char *fileName);
void fn_80160A84(GadgetModelInfo *model, int arg1, int arg2);
void fn_8016271C(GadgetModelInfo *model);
boost::weak_ptr<_stMtnPlayWork> fn_801602B4(GadgetModelInfo *model, int index);
int fn_80416F28(_stMtnPlayWork *work, int arg1, int arg2, int arg3);
int fn_80415854(_stMtnPlayWork *work, int arg1, u16 *type, void **data, int arg4);
void fn_80416DC8(_stMtnPlayWork *work, int arg1, float speed);

/* Particle helpers (see TurnOver.cpp and DefenseShield.cpp). */
void fn_8015FA78(GadgetModelInfo *model,
                 void *(*callback)(void *part, UnkParticleAttach_80169D00 *param),
                 UnkParticleAttach_80169D00 *param);
void *fn_80169D00(void *part, UnkParticleAttach_80169D00 *param);
boost::shared_ptr<GadgetParticle::GadgetParticleCtrl> fn_80166AF4(void);
void fn_80167010(GadgetParticle::GadgetParticleCtrl *ctrl, int id);
void fn_80166F88(GadgetParticle::GadgetParticleCtrl *ctrl);
void fn_80167448(GadgetParticle::GadgetParticleCtrl *ctrl, u32 id);
void fn_8016761C(GadgetParticle::GadgetParticleCtrl *ctrl, float dt);

/* Plays sound id at the character info through the sound manager (see CircusBall.cpp). */
void fn_801D54F4(void *manager, u16 id, GadgetCharInfo *charInfo, int arg3, int arg4);

/* Inverts a matrix (behaviour inferred). */
void fn_8040BCD4(Mtx34f *matrix);
/* True when value is (nearly) zero; scales a distance (behaviour inferred). */
bool fn_8000A950(float value);
float fn_8005AE20(float value);
}

/* result = a - b, returned through the caller's temporary (see Bomb.cpp). */
inline const Vec3f &Subtract(Vec3f *result, const Vec3f *a,
                                     const Vec3f *b) {
    fn_8040CD74(result, a, b);
    return *result;
}

/* A length (name inferred). As a call it is not a constant expression, so the constants below
 * are stored by the static initializer, as in retail. */
inline float Span(float length) {
    return length;
}

class Monster;


/* The four parameters the owner supplies (names from the parameter table; m/sec, stored as
 * game units). */
struct MonsterParams {
    /* 0x0 */ float minSpeed;
    /* 0x4 */ float maxSpeed;
    /* 0x8 */ float accel;
    /* 0xC */ float decel;
};

class Monster : public Gadget {
  public:
    Monster(Task *parent, UnkPlacement_800C9CD0 *owner);
    virtual ~Monster();
    virtual void Behave(float dt);
    virtual void ReloadPlacement(UnkPlacement_800C9CD0 *owner);
    virtual bool HasEnded();

    /* Creates the monster under task layer 11 (name inferred). */
    static void Instantiate(UnkPlacement_800C9CD0 *owner);

    /* Reacts to a MonsterControl command: 0 start, 1 fall back, 2 stop (name inferred). */
    void Issue(int command);
    /* Places the model at the character info (name inferred). */
    void RebuildTransform();
    /* Copies the owner's parameters (name inferred). */
    void LoadParams(UnkPlacement_800C9CD0 *owner);
    /* The attack callback of the collider (name inferred). */
    void OnAttack(AttackCallbackParam &param);

    static const char *s_taskName;

    /* 0x30 */ boost::shared_ptr<MonsterCharInfo> m_chara;
    /* 0x38 */ boost::shared_ptr<GadgetModelInfo> m_model;
    /* 0x40 */ MonsterParams m_params;
    /* 0x50 */ boost::shared_ptr<CharaColliBase> m_collider;
    /* 0x58 */ boost::shared_ptr<PathCtrl> m_path;
    /* 0x60 */ s32 m_state;    /* 0 waiting, 1 starting, 2 running (inferred) */
    /* 0x64 */ float m_lead;   /* the distance below which it runs at the slow speed (inferred) */
    /* 0x68 */ bool m_started; /* the start motion was played (inferred) */
    /* 0x6C */ boost::shared_ptr<GadgetParticle::GadgetParticleCtrl> m_particle;
    /* 0x74 */ u32 pad_0x74; /* the class size is a multiple of 8 */
};
SIZE_ASSERT(Monster, 0x78);

const char *Monster::s_taskName = "Monster";

/* The collider's size and offset (in metres, scaled to game units). */
static Vec3f s_colliderSize(fn_8005AE20(6.0f), fn_8005AE20(6.0f), fn_8005AE20(0.5f));
static Vec3f s_colliderOffset(0.0f, fn_8005AE20(-5.0f), fn_8005AE20(-6.0f));
/* How far behind the collider's front the player is measured from (name inferred). */
static float s_frontDepth = std::fabs(s_colliderOffset.z);
/* The initial lead, how much command 1 reduces it, and the speed at which the run motion plays
 * at normal rate (names inferred). */
static const float s_initialLead = Span(150.0f); /* const: retail loads it early */
static float s_leadStep = Span(75.0f);
static float s_motionSpeed = Span(500.0f);

MonsterCharInfo::~MonsterCharInfo() {}

void MonsterCharInfo::Issue(int command, MonsterControl *control) {
    m_monster->Issue(command);
    control->Stop();
}

Monster::Monster(Task *parent, UnkPlacement_800C9CD0 *owner)
    : Gadget(parent, owner), m_state(0), m_lead(s_initialLead), m_started(false) {
    m_name = s_taskName;
    m_path = boost::shared_ptr<PathCtrl>(new PathCtrl);
    fn_8001C7B4(fn_8001B494(), m_path.get(), 0x22);
    fn_80018304(m_path.get());
    m_chara = boost::shared_ptr<MonsterCharInfo>(new MonsterCharInfo(this));
    m_chara->SetField5C(4);
    m_chara->field_0x30 = 1;
    {
        PathCtrl *path = m_path.get();
        fn_800091D0(&m_chara->m_position, &(const Vec3f &)fn_80018634(path));
    }
    /* Retail calls both shared_ptrs' operator-> out of line here, as for an inline member. */
    m_chara->m_rotation = fn_80018434(m_path.operator->(), 1, 0);
    fn_8040CE60(&m_chara->field_0x18);
    fn_801DCD8C(fn_800DDF20(), boost::shared_ptr<CharInfo>(m_chara));
    m_model = fn_8015F800(1, "MONSTER.BIN");
    m_model->fn_80162A68(0, 0);
    m_collider = fn_800C7C34(fn_80021B08(), m_chara, &s_colliderSize, 0x10120, 6, 0,
                             &s_colliderOffset, &lbl_805E07B8, 0);
    {
        boost::shared_ptr<CharaColliAttack> attack =
            fn_800C4374(fn_80021B08(), m_collider).lock();
        if (attack.get() != 0) {
            fn_80096B64(attack.get(), 1, boost::bind(&Monster::OnAttack, this, _1));
            attack->field_0x55 = 0;
        }
        m_collider->TurnOff(-1);
    }
    LoadParams(owner);
    m_particle = fn_80166AF4();

    UnkParticleAttach_80169D00 attach;
    attach.ctrl = m_particle;
    fn_8015FA78(m_model.get(), fn_80169D00, &attach);
}

Monster::~Monster() {
    fn_801DCCF4(fn_800DDF20(), boost::shared_ptr<CharInfo>(m_chara));
}

extern "C" void fn_801392A0(void) {
    fn_8006B474("MONSTER.BIN", 1);
}

extern "C" void fn_8013929C(void) {}

void Monster::Instantiate(UnkPlacement_800C9CD0 *owner) {
    new Monster(fn_801EB304(fn_8001C4F8(), 11), owner);
}

static GadgetParamDesc lbl_80528D24[] = {
    {4, "MIN_SPPED m/sec", 0, 0, 0, 30.0f, 0.0f, 65536.0f, fn_800CBC20, 0},
    {4, "MAX_SPPED m/sec", 0, 0, 0, 50.0f, 0.0f, 65536.0f, fn_800CBC20, 0},
    {4, "ACCEL m/sec^2", 0, 0, 0, 100.0f, 0.0f, 65536.0f, fn_800CBC20, 0},
    {4, "DECEL m/sec^2", 0, 0, 0, 150.0f, 0.0f, 65536.0f, fn_800CBC20, 0},
    {0},
};

GadgetClassDesc lbl_80528DEC = {
    "Monster", fn_801392A0, fn_8013929C, Monster::Instantiate, 0, 0, 0xBC7, 1, 0, 10, lbl_80528D24,
};

void Monster::Behave(float dt) {
    if (m_path.get() != 0 && fn_800ABDCC(m_path.get())) {

    switch (m_state) {
    case 1:
        if (m_started) {
            int done = fn_80416F28(fn_801602B4(m_model.get(), 0).lock().get(), 0, 3, 1);
            if (done != 0) {
                m_collider->TurnOn(-1);
                boost::shared_ptr<CharaColliAttack> attack =
                    fn_800C42D0(fn_80021B08(), m_collider).lock();
                if (attack.get() != 0) {
                    attack->field_0x54 = 1;
                    attack->field_0x55 = 1;
                }
                fn_80160A84(m_model.get(), 0, 2);
                m_state = 2;
            }
        } else {
            m_started = true;
        }
        break;
    case 2: {
        Mtx34f matrix;
        fn_8040C148(&matrix, &lbl_805E0720);
        fn_8040B59C(&matrix, &m_chara->m_position);
        fn_8040B80C(&matrix, m_chara->m_rotation.z);
        fn_8040B884(&matrix, m_chara->m_rotation.x);
        fn_8040B848(&matrix, m_chara->m_rotation.y);
        fn_8040BCD4(&matrix);
        boost::shared_ptr<CharInfo> player = fn_800A8B74(fn_80074194(), lbl_805EE588).lock();
        if (player.get() != 0) {
            Vec3f local;
            fn_800091EC(&local, &(const Vec3f &)fn_8040B67C(&matrix, &player->m_position));
            float distance = -local.z - s_frontDepth;
            float speed = fn_8040CF50(&m_chara->field_0x18);
            float target;
            if (distance < m_lead) {
                target = m_params.minSpeed * dt;
            } else {
                target = m_params.maxSpeed * dt;
            }
            if (speed > target) {
                float step = dt * (m_params.decel * dt);
                if (speed - target > step) {
                    distance = speed - step;
                } else {
                    distance = target;
                }
            } else {
                float step = dt * (m_params.accel * dt);
                if (target - speed > step) {
                    distance = speed + step;
                } else {
                    distance = target;
                }
            }
            Vec3f next;
            fn_8001C91C(fn_8001B494(), m_path.get(), 0, &next, &m_chara->m_rotation, 1,
                        distance);
            fn_800091D0(&m_chara->field_0x18,
                        &Subtract(&Vec3f(), &next, &m_chara.px->m_position));
            fn_800091D0(&m_chara->m_position, &next);
            boost::shared_ptr<_stMtnPlayWork> work = fn_801602B4(m_model.get(), 0).lock();
            if (work.get() != 0) {
                float rate;
                if (fn_8000A950(dt)) {
                    rate = 0.0f;
                } else {
                    rate = distance / dt;
                }
                fn_80416DC8(work.get(), 0, rate / (60.0f * (s_motionSpeed / 80.0f)));
            }
        }
        RebuildTransform();
        break;
    }
    }

    m_model->fn_80162FB0(dt);
    if (m_state == 2) {
        boost::shared_ptr<_stMtnPlayWork> work = fn_801602B4(m_model.get(), 0).lock();
        if (work.get() != 0) {
            _stMtnPlayWork *iterator = work.get();
            u16 type;
            void *data;
            while (fn_80415854(iterator, 0, &type, &data, 0) != 0) {
                if (type == 0x5000) {
                    fn_80167448(m_particle.get(), *(const u32 *)data);
                } else if (type == 0x5001) {
                    const u32 *soundId = (const u32 *)data;
                    fn_801D54F4(fn_80053164(), *soundId, m_chara.get(), 0, 1);
                }
                iterator = 0;
            }
        }
    }
    fn_8016761C(m_particle.get(), dt);
    }
}

void Monster::ReloadPlacement(UnkPlacement_800C9CD0 *owner) {
    m_model->fn_80162EF4();
    m_collider->Reset();
    LoadParams(owner);
}

bool Monster::HasEnded() {
    if (m_owner != 0) {
        if (fn_800C9A64(m_owner)) {
            m_flags |= 1;
            return true;
        }
        if (m_state == 0) {
            if (fn_800C996C(m_owner)) {
                m_flags |= 1;
                return true;
            }
        } else {
            boost::shared_ptr<CharInfo> player =
                fn_800A8B74(fn_80074194(), lbl_805EE588).lock();
            if (player.get() != 0) {
                Vec3f offset;
                Vec3f difference;
                fn_8040CD74(&difference, &player->m_position, &m_chara->m_position);
                fn_800091EC(&offset, &difference);
                float range = fn_800C9938(m_owner);
                if (fn_8040CF50(&offset) > range) {
                    m_flags |= 1;
                    return true;
                }
            }
        }
    }
    return false;
}

void Monster::LoadParams(UnkPlacement_800C9CD0 *owner) {
    fn_800C9DBC(owner, &m_params, sizeof(m_params));
    m_params.minSpeed = 10.0f * m_params.minSpeed;
    m_params.maxSpeed = 10.0f * m_params.maxSpeed;
    m_params.accel = 10.0f * m_params.accel;
    m_params.decel = 10.0f * m_params.decel;
}

void Monster::RebuildTransform() {
    Mtx34f matrix;

    fn_8040C148(&matrix, &lbl_805E0720);
    fn_8040B59C(&matrix, &m_chara->m_position);
    fn_8040B80C(&matrix, m_chara->m_rotation.z);
    fn_8040B884(&matrix, m_chara->m_rotation.x);
    fn_8040B848(&matrix, m_chara->m_rotation.y);
    m_model->fn_801633DC(&matrix);
}

void Monster::Issue(int command) {
    switch (command) {
    case 0:
        if (m_state == 0) {
            m_model->fn_80160C28(0);
            fn_80160A84(m_model.get(), 0, 1);
            fn_8016271C(m_model.get());
            fn_80167010(m_particle.get(), 1);
            RebuildTransform();
            fn_801D54F4(fn_80053164(), 0x744F, m_chara.get(), 0, 1);
            m_started = false;
            m_state = 1;
        }
        break;
    case 1:
        if (m_state == 2) {
            m_lead -= s_leadStep;
            fn_801D54F4(fn_80053164(), 0x744F, m_chara.get(), 0, 1);
        }
        break;
    case 2:
        fn_80166F88(m_particle.get());
        fn_801D54F4(fn_80053164(), 0x744F, m_chara.get(), 0, 1);
        m_flags |= 1;
        break;
    }
}

void Monster::OnAttack(AttackCallbackParam &param) {
    if (fn_8015D4E4(param)) {
        boost::shared_ptr<CharaColliAttack> attacker = param.m_attacker.lock();
        if (attacker.get() != 0 && (attacker->m_flags & 0x10) != 0) {
            param.m_result = 3;
            {
                boost::shared_ptr<CharInfo> owner = attacker->m_owner.lock();
                fn_8020642C(owner, 0x30);
            }
            fn_801D54F4(fn_80053164(), 0x744F, m_chara.get(), 0, 1);
            m_flags |= 1;
            return;
        }
    }
    param.m_result = 5;
}
