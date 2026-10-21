/* ShootingGame::BalloonGenerator (class name from RTTI): the PARK_BALLOON gadget of the Circus Park
 * shooting game. While it runs it releases "Num" balloons every "Sec" seconds in a ring of
 * "Radius" around itself: each ShootingGame::Balloon (class name from RTTI) rises at a random speed
 * between "MinSpeed" and "MaxSpeed" until it is "Height" above its start. "Rate" packs four counts
 * (B:N:E:G) that fill a ShootingGame::RandomSampling of balloon kinds; the angles come from a second
 * sampling, shuffled. A balloon that is shot scores for the shooting game entry "Id" (the
 * ShootingGame manager unit keeps the table) and spawns its kind's effect. A generator with an id
 * waits for its entry to start and stops when it ends; one without counts the DeadCommands its
 * character info receives and stops at "MaxNum". File name inferred from the class. Method names
 * marked "(name inferred)" are not original. Definitions are in reverse address order (deferred
 * inlining emits functions last-defined first).
 *
 * Not yet matching: in Spawn, retail computes 360 / total before the shared pi / 180 and the
 * spawn counter's increment early; GC/2.7 hoists the common subexpression pi / 180 ahead of it,
 * which swaps two divisions, two float registers and a few loads (11 instructions; the constant
 * pool order and everything else match). About 20 source shapes were tried. Every other function
 * and all data match. */
#include <boost/bind.hpp>
#include <boost/function.hpp>
#include <cstdio>
#include <cstring>

#include "Effect/System/EffectElement.h"
#include "Game/CharCommand.h"
#include "Game/CharCommandUnit.h"
#include "Game/CharInfo.h"
#include "Game/CharaColliAttack.h"
#include "Game/Gadget.h"
#include "Game/GadgetCharInfo.h"
#include "Game/GadgetModelInfo.h"
#include "Game/GadgetClassDesc.h"
#include "Game/GadgetUtil.h"
#include "Game/ShootingGameTable.h"
#include "Game/WeaponBullet.h"
#include "Game/FileQueue.h"
#include "boost/shared_ptr.hpp"
#include "boost/weak_ptr.hpp"
#include "size_assert.h"

using SonicteamUSA::System::CharaColli::AttackCallback;
using SonicteamUSA::System::CharaColli::AttackCallbackParam;
using SonicteamUSA::System::CharaColli::CharaColliAttack;

/* Target (class name from RTTI): a point other objects can aim at (see Spring.cpp). */
class Target {
  public:
    /* Moves the target to position (name inferred). */
    void fn_800CCE38(const Vec3f *position);
};

/* DeadCommand (class name from the RTTI of the vtable its constructor, 0x8009EE8C, stores): a
 * command telling a character that something it owns has died (meaning inferred). */
class DeadCommand : public CharCommand {
  public:
    DeadCommand(int arg0);
};

using ShootingGame::BalloonTable;

extern "C" {
/* The shooting game manager's table of 0x100 entries (allocated by fn_8013D434). */
extern BalloonTable *lbl_805EEFC0;
/* Creates and destroys the manager and its table (the load and unload hooks call them). */
void fn_8013D434(void);
void fn_8013D3DC(void);
/* Adds delta to entry id's score (behaviour inferred). */
void fn_8013D7BC(BalloonTable *table, int id, int delta);

/* Starts the activation and attack bookkeeping of a gadget character info (behaviour unknown). */
void fn_8015CF7C(GadgetCharInfo *charInfo);
/* Builds the character info's matrix (behaviour inferred). */
void fn_8015CEA0(GadgetCharInfo *charInfo, Mtx34f *matrix);
/* Loads model fileName for a gadget (behaviour inferred; arg0's meaning is unknown). */
boost::shared_ptr<GadgetModelInfo> fn_8015F800(int arg0, const char *fileName);
/* GadgetModelInfo methods that are not declared in its header yet (see ArkCannon.cpp). */
bool fn_80161FC8(GadgetModelInfo *model, int variant, const boost::weak_ptr<CharInfo> &info);
boost::weak_ptr<CharaColliAttack> fn_80161428(GadgetModelInfo *model, int variant);
/* Creates a target with the given flags, and removes a gadget's target (behaviour inferred). */
boost::shared_ptr<Target> fn_801E72DC(int flags);
void fn_801E7108(boost::shared_ptr<Target> *target);
/* Launches effect id at position plus offset (behaviour inferred). */
boost::weak_ptr<Effect::System::EffectElement> fn_80168C48(const Vec3f *position,
                                                          const Vec3f *offset, u32 id);
/* Plays sound id at the character info's position through the sound manager (behaviour
 * inferred). */
void fn_801D54F4(void *manager, u16 id, GadgetCharInfo *charInfo, int arg3, int arg4);
/* Marks the gadget finished (behaviour inferred). */
void fn_8015A794(Gadget *gadget);
/* Draws a debug cylinder (behaviour inferred; arg2 unknown). */
void fn_802E0C60(const Vec3f *position, const Vec3f *rotation, int arg2,
                 float halfHeight, float radius);
/* Random numbers in [0, 1) (behaviour inferred). */
float fn_8040ECF8(void);
float fn_8040EE90(void);
}

/* True when the manager's entry id is in state (inlined in the original; name inferred). */
inline bool IsEntryState(BalloonTable *table, int id, int state) {
    if (id >= 0 && id < 0x100) {
        return table->m_entries[id].m_state == state;
    }
    return false;
}

/* Degrees to radians (inlined in the original; name inferred). */
inline float DegToRad(float degrees) {
    return degrees * (lbl_805FAD38 / 180.0f);
}

/* The effect of each balloon kind. */
static u32 lbl_80529000[] = {0x19, 0x1F, 0x13, 0x0E};

namespace ShootingGame {

namespace Kind {
/* The balloon kinds, in the order of their effects and sounds; "Rate" packs their counts as
 * B:N:E:G (names inferred from that label). */
enum __Kind {
    B = 0,
    E = 1,
    N = 2,
    G = 3,
};
} // namespace Kind

/* RandomSampling (class name from the RTTI of its shared_ptr control blocks): a reservoir sample of
 * at most m_size values (names inferred). */
template <class T> class RandomSampling {
  public:
    RandomSampling(int size);
    ~RandomSampling();
    /* Adds value, replacing a random kept one once the sample is full (name inferred). */
    void Add(T value);
    /* Removes and returns the last value (name inferred). */
    T Pop();
    /* Shuffles the kept values (name inferred). */
    void Shuffle();

    /* 0x0 */ T *m_data;
    /* 0x4 */ int m_size;
    /* 0x8 */ int m_count;
};

template <class T> RandomSampling<T>::RandomSampling(int size) {
    m_data = new T[size];
    memset(m_data, 0, sizeof(T));
    m_size = size;
    m_count = 0;
}

template <class T> RandomSampling<T>::~RandomSampling() {
    delete[] m_data;
    m_data = 0;
    m_size = 0;
    m_count = 0;
}

template <class T> void RandomSampling<T>::Add(T value) {
    if (++m_count <= m_size) {
        m_data[m_count - 1] = value;
    } else {
        int index = m_count * fn_8040EE90();
        if (index < m_size) {
            m_data[index] = value;
        }
    }
}

template <class T> T RandomSampling<T>::Pop() {
    return m_data[--m_size];
}

template <class T> void RandomSampling<T>::Shuffle() {
    for (int i = 0; i < m_size; i++) {
        int j = m_size * fn_8040EE90();
        T value = m_data[i];
        m_data[i] = m_data[j];
        m_data[j] = value;
    }
}

class Balloon : public Task {
  public:
    /* What the generator passes to a new balloon (name inferred). */
    struct Param {
        Param() : m_speed(0.0f), m_height(0.0f), m_kind(0), m_id(-1) {}

        /* 0x00 */ Fn8040CopyVector3 m_position;
        /* 0x0C */ float m_speed;
        /* 0x10 */ float m_height; /* how far it rises */
        /* 0x14 */ s32 m_kind;     /* Kind::__Kind */
        /* 0x18 */ s32 m_id;       /* the manager entry, or -1 */
        /* 0x1C */ boost::weak_ptr<GadgetCharInfo> m_owner; /* the generator's character info */
    };

    Balloon(Task *parent, const Param &param);
    virtual ~Balloon();
    virtual void Tick(float dt);

    /* Creates a balloon under task layer 11 (name inferred). */
    static void Instantiate(const Param &param);

    /* Places the model and the target (name inferred). */
    void RebuildTransform();
    /* The attack callback of the model's collision (name inferred). */
    void OnAttack(AttackCallbackParam &param);

    static const char *s_taskName;

    /* 0x28 */ boost::shared_ptr<GadgetCharInfo> m_chara;
    /* 0x30 */ boost::shared_ptr<GadgetModelInfo> m_model;
    /* 0x38 */ boost::shared_ptr<Target> m_target;
    /* 0x40 */ s32 m_state; /* 0 new, 1 rising, 2 shot, 3 done (inferred) */
    /* 0x44 */ Param m_param;
};

/* The parameters the owner supplies (names from the parameter table). */
struct BalloonGeneratorParams {
    BalloonGeneratorParams()
        : height(0.0f), radius(0.0f), minSpeed(0.0f), maxSpeed(0.0f), num(0), maxNum(0), rate(0),
          sec(0.0f), id(0) {}

    /* 0x00 */ float height;
    /* 0x04 */ float radius;
    /* 0x08 */ float minSpeed;
    /* 0x0C */ float maxSpeed;
    /* 0x10 */ int num;
    /* 0x14 */ int maxNum;
    /* 0x18 */ u32 rate; /* the B, N, E and G counts, one per byte from the top */
    /* 0x1C */ float sec;
    /* 0x20 */ int id;
};

class BalloonGenerator : public Gadget {
  public:
    BalloonGenerator(Task *parent, UnkPlacement_800C9CD0 *owner);
    virtual ~BalloonGenerator();
    virtual void Behave(float dt);
    virtual void ReloadPlacement(UnkPlacement_800C9CD0 *owner);

    /* Creates the generator under task layer 11 (name inferred). */
    static void Instantiate(UnkPlacement_800C9CD0 *owner);

    /* Copies the owner's position, rotation and parameters and restarts (name inferred). */
    void LoadFromOwner(UnkPlacement_800C9CD0 *owner);
    /* Releases count balloons (name inferred). */
    void Spawn(int count);
    /* The command callback of the character info (name inferred). */
    void OnCommand(CharCommand &command);

    static const char *s_taskName;

    /* 0x30 */ s32 m_state; /* 0 new, 1 waiting for the entry, 2 releasing, 3 done (inferred) */
    /* 0x34 */ BalloonGeneratorParams m_params;
    /* 0x58 */ boost::shared_ptr<GadgetCharInfo> m_chara;
    /* 0x60 */ float m_timer;
    /* 0x64 */ int m_deadCount;
    /* 0x68 */ int m_spawnCount;
    /* 0x6C */ u32 pad_0x6C;
};

} // namespace ShootingGame

SIZE_ASSERT(ShootingGame::Balloon::Param, 0x24);
SIZE_ASSERT(ShootingGame::Balloon, 0x68);
SIZE_ASSERT(ShootingGame::BalloonGeneratorParams, 0x24);
SIZE_ASSERT(ShootingGame::BalloonGenerator, 0x70);

namespace ShootingGame {

const char *Balloon::s_taskName = "ShootingGame::Balloon";
const char *BalloonGenerator::s_taskName = "ShootingGame::BalloonGenerator";

Balloon::Balloon(Task *parent, const Param &param) : Task(parent) {
    m_name = s_taskName;
    m_param = param;
    m_chara = fn_8015CB88(0x47, 4, "ParkBalloon");
    fn_800091D0(&m_chara->m_position, (const Vec3f *)&m_param.m_position);
    m_model = fn_8015F800(1, "SHOOTINGGAMEBALLOON.BIN");
    m_model->fn_80162A68(m_param.m_kind, 0);
    fn_80161FC8(m_model.get(), m_param.m_kind, m_chara);
    boost::weak_ptr<CharaColliAttack> attack = fn_80161428(m_model.get(), m_param.m_kind);
    if (boost::shared_ptr<CharaColliAttack> collision = attack.lock()) {
        collision->field_0x54 = 0;
        collision->field_0x55 = 1;
        collision->m_attackCallback = AttackCallback(boost::bind(&Balloon::OnAttack, this, _1));
    }
    m_target = fn_801E72DC(0xC00);
    m_state = 0;
    RebuildTransform();
}

Balloon::~Balloon() {
    fn_801E7108(&m_target);
}

void Balloon::Tick(float dt) {
    switch (m_state) {
    case 0:
        m_state = 1;
        break;
    case 1:
        m_chara->m_position.y += 30.0f * m_param.m_speed * dt;
        if (m_chara->m_position.y - m_param.m_position.y > m_param.m_height) {
            m_state = 3;
        }
        break;
    case 2: {
        int id = m_param.m_id;
        switch (m_param.m_kind) {
        case Kind::B:
            fn_8013D7BC(lbl_805EEFC0, id, 1);
            break;
        case Kind::E:
            fn_8013D7BC(lbl_805EEFC0, id, -2);
            break;
        case Kind::N:
            fn_8013D7BC(lbl_805EEFC0, id, 2);
            break;
        case Kind::G:
            break;
        }
        if (!m_param.m_owner.expired()) {
            boost::shared_ptr<GadgetCharInfo> owner = m_param.m_owner.lock();
            owner->PushCommand(new DeadCommand(4));
        }
        {
            Vec3f position;
            fn_800091EC(&position, &m_chara->m_position);
            Vec3f offset(0.0f, 1.0f, 0.0f);
            fn_80168C48(&position, &offset, lbl_80529000[m_param.m_kind]);
        }
        if (m_param.m_kind == Kind::B || m_param.m_kind == Kind::N) {
            fn_801D54F4(fn_80053164(), 0x7433, m_chara.get(), 0, 1);
        } else if (m_param.m_kind == Kind::E) {
            fn_801D54F4(fn_80053164(), 0x7433, m_chara.get(), 0, 1);
            fn_801D54F4(fn_80053164(), 0xE005, m_chara.get(), 0, 1);
        } else {
            fn_801D54F4(fn_80053164(), 0x7434, m_chara.get(), 0, 1);
            fn_801D54F4(fn_80053164(), 0x7435, m_chara.get(), 0, 1);
        }
        m_state = 3;
        break;
    }
    case 3:
        m_flags |= 1;
        break;
    }
    RebuildTransform();
}

void Balloon::RebuildTransform() {
    Mtx34f matrix;

    fn_8015CEA0(m_chara.get(), &matrix);
    m_model->fn_801633DC(&matrix);
    if (m_target.get() != 0) {
        Vec3f position;
        fn_800091EC(&position, &m_chara->m_position);
        position.y += 10.0f;
        m_target->fn_800CCE38(&position);
    }
}

void Balloon::OnAttack(AttackCallbackParam &param) {
    if (m_state == 1) {
        if (boost::shared_ptr<CharaColliAttack> attacker = param.m_attacker.lock()) {
            if (attacker->m_flags & 2) {
                param.m_result = 5;
                return;
            }
        }
        param.m_result = 3;
        m_state = 2;
    } else {
        param.m_result = 5;
    }
}

void Balloon::Instantiate(const Param &param) {
    new Balloon(fn_801EB304(fn_8001C4F8(), 11), param);
}

BalloonGenerator::BalloonGenerator(Task *parent, UnkPlacement_800C9CD0 *owner)
    : Gadget(parent, owner) {
    m_name = s_taskName;
    m_chara = fn_8015CB88(0x48, 4, "ParkBalloonGen");
    fn_8015CF7C(m_chara.get());
    fn_800729B0(&m_chara->m_commandUnit->m_callback,
                &(const CharCommandCallback &)CharCommandCallback(
                    boost::bind(&BalloonGenerator::OnCommand, this, _1)));
    LoadFromOwner(owner);
}

BalloonGenerator::~BalloonGenerator() {}

void BalloonGenerator::Behave(float dt) {
    switch (m_state) {
    case 0:
        if (m_params.id > 0) {
            m_state = 1;
        } else {
            m_state = 2;
        }
        break;
    case 1:
        if (IsEntryState(lbl_805EEFC0, m_params.id, 1)) {
            m_state = 2;
        }
        break;
    case 2:
        m_timer -= dt;
        if (m_timer < 0.0f) {
            m_timer = m_params.sec;
            Spawn(m_params.num);
        }
        if (m_params.id > 0) {
            if (IsEntryState(lbl_805EEFC0, m_params.id, 3)) {
                m_state = 3;
            }
        } else if (m_deadCount >= m_params.maxNum) {
            m_state = 3;
        }
        break;
    case 3:
        fn_8015A794(this);
        m_flags |= 1;
        break;
    }
}

void BalloonGenerator::ReloadPlacement(UnkPlacement_800C9CD0 *owner) {
    LoadFromOwner(owner);

    Vec3f rotation(0.0f, 0.0f, 0.0f);
    rotation.x = DegToRad(90.0f);
    float halfHeight = 0.5f * m_params.height;
    Vec3f position;
    fn_800091EC(&position, &m_chara->m_position);
    position.y += halfHeight;
    fn_802E0C60(&position, &rotation, 0, halfHeight, m_params.radius);
}

void BalloonGenerator::LoadFromOwner(UnkPlacement_800C9CD0 *owner) {
    fn_800C9F14(owner, &m_chara->m_position);
    fn_800C9ED4(owner, &m_chara->m_rotation);
    fn_800C9DBC(owner, &m_params, sizeof(m_params));
    m_state = 0;
    m_timer = m_params.sec;
    m_deadCount = 0;
    m_spawnCount = 0;
}

void BalloonGenerator::Spawn(int count) {
    boost::shared_ptr<RandomSampling<Kind::__Kind> > kinds =
        boost::shared_ptr<RandomSampling<Kind::__Kind> >(new RandomSampling<Kind::__Kind>(count));
    boost::shared_ptr<RandomSampling<float> > angles =
        boost::shared_ptr<RandomSampling<float> >(new RandomSampling<float>(count));

    u32 rate = m_params.rate;
    int numB = rate >> 24;
    int numN = (rate >> 16) & 0xFF;
    int numE = (rate >> 8) & 0xFF;
    int numG = rate & 0xFF;
    for (int i = 0; i < numB; i++) {
        kinds->Add(Kind::B);
    }
    for (int i = 0; i < numN; i++) {
        kinds->Add(Kind::N);
    }
    for (int i = 0; i < numE; i++) {
        kinds->Add(Kind::E);
    }
    for (int i = 0; i < numG; i++) {
        kinds->Add(Kind::G);
    }

    int total = count + 6;
    float step = 360.0f / total * (lbl_805FAD38 / 180.0f);
    float offset = 13.0f * m_spawnCount++ * (lbl_805FAD38 / 180.0f);
    for (int i = 0; i < total; i++) {
        angles->Add(step * i + offset);
    }
    angles->Shuffle();

    for (int i = 0; i < count; i++) {
        Mtx34f matrix;
        Vec3f position(0.0f, 0.0f, m_params.radius);

        fn_8040B6F8(&matrix);
        fn_8040B59C(&matrix, &m_chara->m_position);
        fn_8040B848(&matrix, angles->Pop());
        AssignVector(position, fn_8040B67C(&matrix, &position));

        Balloon::Param param;
        fn_800091D0((Vec3f *)&param.m_position, &position);
        param.m_height = m_params.height;
        float range = m_params.maxSpeed - m_params.minSpeed;
        param.m_speed = range * fn_8040ECF8() + m_params.minSpeed;
        if (m_params.id > 0) {
            param.m_id = m_params.id;
        }
        param.m_kind = kinds->Pop();
        param.m_owner = m_chara;
        Balloon::Instantiate(param);
    }
}

void BalloonGenerator::OnCommand(CharCommand &command) {
    if (command.TypeId() == 8) {
        m_deadCount++;
    }
}

} // namespace ShootingGame

extern "C" void fn_8013AEE4(void) {
    fn_8006B474("SHOOTINGGAMEBALLOON.BIN", 1);
    fn_8013D434();
}

extern "C" void fn_8013AEC4(void) {
    fn_8013D3DC();
}

void ShootingGame::BalloonGenerator::Instantiate(UnkPlacement_800C9CD0 *owner) {
    Task *parent = fn_801EB304(fn_8001C4F8(), 11);
    new BalloonGenerator(parent, owner);
}

/* The fields of the cursor that the label callback reads (layout from its accesses). */
struct BalloonGeneratorParamCursor {
    /* 0x0 */ u32 field_0x0;
    /* 0x4 */ int index; /* the parameter */
    /* 0x8 */ int value;
};

/* The label callback of the Rate and Id parameters. */
extern "C" const char *fn_8013ADC4(UnkLabelQuery_800CBC20 *cursor) {
    static const char *s_format = "B:N:E:G = %2d:%2d:%2d:%2d";
    static char s_buffer[0x40];
    static const char *s_idLabels[] = {"Id Off", "Id On"};
    const BalloonGeneratorParamCursor *fields = (const BalloonGeneratorParamCursor *)cursor;

    if (fields->index == 6) {
        u32 rate = fields->value;
        sprintf(s_buffer, s_format, rate >> 24, (rate >> 16) & 0xFF, (rate >> 8) & 0xFF, rate & 0xFF);
        return s_buffer;
    }
    if (fields->index == 8) {
        if (fields->value == 0) {
            return s_idLabels[0];
        }
        return s_idLabels[1];
    }
    return 0;
}

static GadgetParamDesc lbl_80529028[] = {
    {4, "Height", 0, 0, 0, 10.0f, 0.0f, 3600.0f, fn_800CBC20, 0},
    {4, "Radius", 0, 0, 0, 10.0f, 0.0f, 3600.0f, fn_800CBC20, 0},
    {4, "MinSpeed", 0, 0, 0, 0.1f, 0.0f, 3600.0f, fn_800CBC20, 0},
    {4, "MaxSpeed", 0, 0, 0, 1.0f, 0.0f, 3600.0f, fn_800CBC20, 0},
    {1, "Num", 1, 1, 100, 0.0f, 0.0f, 0.0f, fn_800CBC20, 0},
    {1, "MaxNum", 1, 1, 100, 0.0f, 0.0f, 0.0f, fn_800CBC20, 0},
    {3, "Rate", 0x04040101, 0, 0x7FFFFFFF, 0.0f, 0.0f, 0.0f, fn_8013ADC4, 0},
    {4, "Sec", 0, 0, 0, 5.0f, 0.0f, 3600.0f, fn_800CBC20, 0},
    {1, "Id", 0, 0, 0xFF, 0.0f, 0.0f, 0.0f, fn_8013ADC4, 0},
    {0},
};

GadgetClassDesc lbl_805291B8 = {
    "PARK_BALLOON", fn_8013AEE4, fn_8013AEC4, ShootingGame::BalloonGenerator::Instantiate, 0, 0, 0xC82, 1,
    0, 10, lbl_80529028,
};
