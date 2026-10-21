/* FootingMovable::FootingMovable (class name from RTTI): the MOVE_FOOTING gadget, a platform of
 * FOOTINGMOVABLE.BIN (model picked by "model") that moves by its "type": swinging between its
 * start and the offset param0/param1/param2 ("normal", optionally clocked by the frame counter or
 * the stage time, or pausing "pause" seconds at each end), along path number param0 ("path",
 * "loop path") or once along the offset ("one way"). Types 0, 3 and 6 wait for the owner's switch.
 * An attack stops it for "pause damage" seconds; a SetTransparentCommand fades it and makes its
 * collision see-through. It plays its model's sound while it moves. File name inferred from the
 * class. Names marked "(name inferred)" are not original. Definitions are in reverse address order
 * (deferred inlining emits functions last-defined first). */
#include <boost/bind.hpp>
#include <boost/function.hpp>
#include <cmath>

#include "Game/CharCommand.h"
#include "Game/CharCommandUnit.h"
#include "Game/CharInfo.h"
#include "Game/CharaColli.h"
#include "Game/CharaColliAttack.h"
#include "Game/Gadget.h"
#include "Game/GadgetCharInfo.h"
#include "Game/GadgetModelInfo.h"
#include "Game/GadgetParticle.h"
#include "Game/GadgetSound.h"
#include "Game/GadgetClassDesc.h"
#include "Game/GadgetUtil.h"
#include "Game/TNumEdit.h"
/* After TNumEdit.h: the order of the casts sets the order of the RTTI base lists in .data. */
#include "Game/CharaColliBaseCharInfo.h"
#include "Game/PathCtrl.h"
#include "Game/UnkSwitchBank_800CB69C.h"
#include "Game/FileQueue.h"
#include "boost/weak_ptr.hpp"
#include "size_assert.h"

using SonicteamUSA::System::PathCtrl;
using SonicteamUSA::System::CharaColli::AttackCallback;
using SonicteamUSA::System::CharaColli::AttackCallbackParam;
using SonicteamUSA::System::CharaColli::CharaColliAttack;
using SonicteamUSA::System::CharaColli::CharaColliBase;

/* A float set up by a constructor, so that it is initialised at run time (the static initialiser
 * at 0x8014BD08; the original type is unknown). */
struct UnkFloat_8014BD08 {
    UnkFloat_8014BD08(float value) : m_value(value) {}

    float m_value;
};

/* The speed above which the footing plays its sound (name inferred). */
static UnkFloat_8014BD08 s_soundSpeed(0.5f);

/* The cursor handed to a parameter's label callback (layout from fn_800CBC20 and fn_80149744;
 * names inferred). */
class UnkLabelQuery_800CBC20 {
  public:
    /* 0x0 */ const GadgetParamDesc *params;
    /* 0x4 */ s32 index; /* the parameter being shown */
    /* 0x8 */ s32 value; /* its current value */
};

/* SetTransparentCommand (class name from RTTI): fades a character's model; its command type is
 * 0x17 (as in Door; only the members used here are named). */
class SetTransparentCommand : public CharCommand {
  public:
    /* 0x14 */ u8 m_transparent; /* nonzero to fade, zero to restore (inferred) */
    /* 0x18 */ float m_alpha;
    /* 0x1C */ u32 field_0x1C;
};
SIZE_ASSERT(SetTransparentCommand, 0x20);

class UnkPathManager_8001B494;

/* A sound entry of a model's table (layout as in Elevator). */
struct UnkSoundEntry_80208888 {
    /* 0x00 */ const char *name;
    /* 0x04 */ u32 field_0x4;
    /* 0x08 */ u32 soundId;
};

class UnkModelTable_80208888;

/* The game state block in .bss (only the members used here). */
struct UnkGameState_8057D6A0 {
    /* 0x00 */ u8 field_0x0[0x90];
    /* 0x90 */ u32 m_frameCount; /* (name inferred) */
    /* 0x94 */ float m_time;     /* (name inferred) */
    /* 0x98 */ u8 field_0x98[0x150 - 0x98];
};
SIZE_ASSERT(UnkGameState_8057D6A0, 0x150);

extern "C" {
extern UnkGameState_8057D6A0 lbl_8057D6A0;

/* A 32-bit flag word: fn_80014E04 constructs it, fn_80014E10 clears it, fn_80014E1C sets bit,
 * fn_8006127C clears bit and fn_80014E34 tests bit (behaviour inferred from their bodies). */
void fn_80014E04(u32 *flags);
void fn_80014E10(u32 *flags);
void fn_80014E1C(u32 *flags, int bit);
bool fn_80014E34(u32 *flags, int bit);
void fn_8006127C(u32 *flags, int bit);

/* Copies the owner's switch id into value (behaviour inferred). */
void fn_800C9E50(UnkPlacement_800C9CD0 *owner, s32 *value);
/* Copies params back into the owner's parameter block when its size equals size (behaviour
 * inferred). */
void fn_800C9D68(UnkPlacement_800C9CD0 *owner, const void *params, int size);
/* Gives the character info a command handler unless it has one (behaviour inferred). */
void fn_8015CF7C(GadgetCharInfo *charInfo);

/* Model helpers (behaviour inferred, as in Door). */
boost::shared_ptr<GadgetModelInfo> fn_8015F800(int arg0, const char *fileName);
int fn_8015FF44(GadgetModelInfo *model);
void fn_80161FC8(GadgetModelInfo *model, int index, const boost::weak_ptr<CharInfo> &info);
void fn_8015FDD8(GadgetModelInfo *model, float alpha);
void fn_8015FD10(GadgetModelInfo *model);
boost::weak_ptr<CharaColliBase> fn_80161614(GadgetModelInfo *model, int index);
boost::weak_ptr<CharaColliAttack> fn_80161428(GadgetModelInfo *model, int index);
void fn_8015FA78(GadgetModelInfo *model,
                 void *(*callback)(void *part, UnkParticleAttach_80169D00 *param),
                 UnkParticleAttach_80169D00 *param);
void *fn_80169D00(void *part, UnkParticleAttach_80169D00 *param);
/* Returns the sound entry of the table for model (behaviour inferred). */
UnkSoundEntry_80208888 *fn_80208848(UnkModelTable_80208888 *table, int model);

/* Particle controller helpers (behaviour inferred). */
boost::shared_ptr<GadgetParticle::GadgetParticleCtrl> fn_80166AF4(void);
void fn_8016761C(GadgetParticle::GadgetParticleCtrl *ctrl, float dt);
void fn_80167010(GadgetParticle::GadgetParticleCtrl *ctrl, int id);

/* Starts sound soundId at charInfo's position for owner (behaviour inferred). */
UnkSoundHandle_801D3DC8 fn_801D3C38(int soundId, GadgetCharInfo *charInfo, void *owner);

/* Paths (behaviour inferred): the path manager, finding path id, restarting,
 * the length, the start position and rotation, and moving a character info distance along it
 * (reporting whether it went past the end). */
UnkPathManager_8001B494 *fn_8001B494(void);
bool fn_8001C7B4(UnkPathManager_8001B494 *manager, PathCtrl *path, u32 id);
void fn_80018304(PathCtrl *path);
float fn_800183D8(PathCtrl *path);
Vec3f fn_80018634(PathCtrl *path);
Vec3f fn_80018434(PathCtrl *path, int arg1, int arg2);
bool fn_8001C91C(UnkPathManager_8001B494 *manager, PathCtrl *path, int arg2,
                 GadgetCharInfo *charInfo, Vec3f *rotation, int loop, float distance);

/* True when value is (nearly) zero (behaviour inferred). */
bool fn_8000A950(float value);
/* Vector helpers (behaviour inferred): result = vector * scale, a + b, vector normalised, and
 * vector += value. */
Vec3f fn_8040CDA8(const Vec3f *vector, float scale);
Vec3f fn_8040CDD0(const Vec3f *a, const Vec3f *b);
Vec3f fn_8040CEE8(const Vec3f &vector);
void fn_8040CFF8(Vec3f *vector, const Vec3f *value);
}

/* Pi, and degrees to radians (as in TargetSwitch; retail calls the shared out-of-line copy of the
 * conversion at 0x8000AB90). */
inline float Pi() {
    return lbl_805FAD38;
}

inline float DegToRad(float degrees) {
    return degrees * (Pi() / 180.0f);
}

/* The collider attribute added while the footing is faded (as in Door). */
const u32 kTransparentAttribute = 0x100;

/* The 0x18 bytes fn_80020A20 builds a debug line in; retail passes a temporary (name inferred). */
struct UnkSegment {
    UnkSegment() {}

    u8 m_data[0x18];
};

/* fn_8040CD74 called the way the original declares it, returning a - b by value (the shared header
 * declares it with an explicit result pointer, which is the same call). */
typedef Vec3f (*UnkVectorSub_8040CD74)(const Vec3f *a, const Vec3f *b);

namespace FootingMovable {

/* The parameters the owner supplies (names from the parameter table). */
struct FootingMovableParams {
    FootingMovableParams()
        : model(0), type(0), speed(0.0f), acc(0.0f), pause(0.0f), pos(0.0f), param0(0.0f),
          param1(0.0f), param2(0.0f), pauseDamage(0.0f) {}

    /* 0x00 */ s32 model;
    /* 0x04 */ s32 type;
    /* 0x08 */ float speed; /* "spd" */
    /* 0x0C */ float acc;
    /* 0x10 */ float pause;
    /* 0x14 */ float pos; /* "Init Pos(0.0-1.0)" */
    /* 0x18 */ float param0; /* X, or the path id */
    /* 0x1C */ float param1; /* Y */
    /* 0x20 */ float param2; /* Z */
    /* 0x24 */ float pauseDamage;
};
SIZE_ASSERT(FootingMovableParams, 0x28);

class FootingMovable : public Gadget {
  public:
    FootingMovable(Task *parent, UnkPlacement_800C9CD0 *owner);
    virtual ~FootingMovable();
    virtual void Behave(float dt);
    virtual void ReloadPlacement(UnkPlacement_800C9CD0 *owner);

    /* Creates the footing under task layer 11 (name inferred). */
    static void Instantiate(UnkPlacement_800C9CD0 *owner);

    /* The states (names inferred): 0 waits for the switch; 1, 2, 3, 10, 12 and 14 start the
     * movements 4, 5, 6, 11, 13 and 15; 7 is the pause after an attack; 9 is the one-way move. */
    void UpdateWaitSwitch(float dt);
    void StartNormal(float dt);
    void StartPath(float dt);
    void StartLoopPath(float dt);
    void UpdateNormal(float dt);
    void UpdatePath(float dt);
    void UpdateLoopPath(float dt);
    void UpdateOneWay(float dt);
    void StartNormalSync(float dt);
    void UpdateNormalSync(float dt);
    void StartPathSync(float dt);
    void UpdatePathSync(float dt);
    void StartNormalPause(float dt);
    void UpdateNormalPause(float dt);
    /* Places the footing for the stage time t (name inferred). */
    void SetNormalSyncPosition(float t);
    /* Copies the owner's placement, parameters and switch id and picks the first state (name
     * inferred). */
    void LoadFromOwner(UnkPlacement_800C9CD0 *owner);
    /* Places the model at the character info (name inferred). */
    void RebuildTransform();
    /* Swings between the ends, pausing at each (name inferred). */
    void Swing(float dt);
    /* Starts or stops the moving sound (name inferred). */
    void UpdateSound(float dt);
    /* The attack collision's callback (name inferred). */
    void OnAttack(AttackCallbackParam &param);
    /* The command unit's callback (name inferred). */
    void OnCommand(CharCommand &command);
    /* Creates the particle controller and starts it (name inferred). */
    void StartParticle();

    static const char *s_taskName;

    /* Member names are inferred. */
    /* 0x030 */ s32 m_type;
    /* 0x034 */ s32 m_shownModel;
    /* 0x038 */ boost::shared_ptr<GadgetCharInfo> m_chara;
    /* 0x040 */ boost::shared_ptr<GadgetModelInfo> m_model;
    /* 0x048 */ FootingMovableParams m_params;
    /* 0x070 */ s32 m_state;
    /* 0x074 */ s32 m_resumeState; /* the state to go back to after an attack */
    /* 0x078 */ float m_startPhase;
    /* 0x07C */ float m_phase; /* degrees, a distance or a fraction, by state */
    /* 0x080 */ float m_pauseTime;
    /* 0x084 */ PathCtrl m_path;
    /* 0x0DC */ boost::shared_ptr<GadgetParticle::GadgetParticleCtrl> m_particle;
    /* 0x0E4 */ Vec3f m_origin;
    /* 0x0F0 */ s32 m_switchId;
    /* 0x0F4 */ u32 m_attributes[16]; /* the collider's attributes before fading */
    /* 0x134 */ u16 m_soundId;
    /* 0x138 */ UnkSoundHandle_801D3DC8 m_sound;
    /* 0x154 */ Vec3f m_lastPosition;
    /* 0x160 */ float m_time;
    /* 0x164 */ u32 m_flags; /* bit 0: attacks pause it; 1: pausing; 2: swinging out; 3: swinging
                              * back; 4: has a sound; 5: sound not started yet */
};

} // namespace FootingMovable

SIZE_ASSERT(FootingMovable::FootingMovable, 0x168);

namespace FootingMovable {

const char *FootingMovable::s_taskName = "FootingMovable";

FootingMovable::FootingMovable(Task *parent, UnkPlacement_800C9CD0 *owner)
    : Gadget(parent, owner) {
    fn_80014E04(&m_flags);
    m_name = s_taskName;
    m_chara = fn_8015CD90(0x20, 4);
    fn_8015CF7C(m_chara.get());
    fn_800729B0(&m_chara->m_commandUnit->m_callback,
                &(const CharCommandCallback &)CharCommandCallback(
                    boost::bind(&FootingMovable::OnCommand, this, _1)));
    LoadFromOwner(owner);
    m_model = fn_8015F800(1, "FOOTINGMOVABLE.BIN");
    m_shownModel = m_params.model;
    m_model->fn_80162A68(m_shownModel, 1);
    fn_80161FC8(m_model.get(), m_shownModel, m_chara);
    fn_8006127C(&m_flags, 4);
    fn_8006127C(&m_flags, 5);
    UnkModelTable_80208888 *table =
        *reinterpret_cast<UnkModelTable_80208888 **>(m_model->field_0x4 + (0x30 - 0x4));
    if (table != 0) {
        UnkSoundEntry_80208888 *entry = fn_80208848(table, m_shownModel);
        if (entry != 0) {
            m_soundId = entry->soundId;
            fn_80014E1C(&m_flags, 4);
            fn_80014E1C(&m_flags, 5);
        }
    }
    RebuildTransform();
}

FootingMovable::~FootingMovable() {
    m_sound.fn_801D455C();
}

void FootingMovable::UpdateWaitSwitch(float dt) {
    if (fn_8016904C(fn_800CB69C(), m_switchId) == 1) {
        if (m_type == 0) {
            m_state = 1;
        } else if (m_type == 1) {
            m_state = 2;
        } else if (m_type == 2) {
        } else if (m_type == 3) {
            m_state = 9;
        } else if (m_type == 6) {
            m_state = 14;
        }
    }
}

void FootingMovable::StartNormal(float dt) {
    u32 frame = lbl_8057D6A0.m_frameCount;

    m_phase = m_params.speed >= 0.5f ? frame % (u32)(0.5f + m_params.speed) : 0;
    m_state = 4;
    StartParticle();
}

void FootingMovable::StartPath(float dt) {
    if (fn_8001C7B4(fn_8001B494(), &m_path, m_params.param0)) {
        fn_80018304(&m_path);
        fn_800091D0(&m_chara->m_position, &(const Vec3f &)fn_80018634(&m_path));
        m_chara->m_rotation = fn_80018434(&m_path, 0, 0);
        u32 frame = lbl_8057D6A0.m_frameCount;
        m_phase = m_params.speed >= 0.5f ? frame % (u32)(0.5f + m_params.speed) : 0;
        m_state = 5;
    }
    StartParticle();
}

void FootingMovable::StartLoopPath(float dt) {
    if (fn_8001C7B4(fn_8001B494(), &m_path, m_params.param0)) {
        fn_80018304(&m_path);
        fn_800091D0(&m_chara->m_position, &(const Vec3f &)fn_80018634(&m_path));
        m_chara->m_rotation = fn_80018434(&m_path, 0, 0);
        m_phase = 0.0f;
        m_state = 6;
        m_chara->m_rotation.x = 0.0f;
        m_chara->m_rotation.z = 0.0f;
        RebuildTransform();
    }
    StartParticle();
}

void FootingMovable::UpdateNormal(float dt) {
    Vec3f end(m_params.param0, m_params.param1, m_params.param2);
    Vec3f direction;
    Vec3f difference;

    fn_8040CFF8(&end, &m_origin);
    fn_8040CD74(&difference, &end, &m_origin);
    fn_800091EC(&direction, &difference);
    float halfLength = 0.5f * fn_8040CF50(&direction);
    AssignVector(direction, fn_8040CEE8(direction));
    m_phase += m_params.acc;
    fn_8040CE38(&direction, halfLength - halfLength * std::cos(DegToRad(m_phase)));
    const Vec3f &position = fn_8040CDD0(&direction, &m_origin);
    fn_800091D0(&m_chara->m_position, &position);
    RebuildTransform();
}

void FootingMovable::UpdatePath(float dt) {
    m_phase += m_params.acc;
    float halfLength = 0.5f * fn_800183D8(&m_path);
    float distance = halfLength - halfLength * std::cos(m_phase * (Pi() / 180.0f));
    fn_80018304(&m_path);
    fn_8001C91C(fn_8001B494(), &m_path, 0, m_chara.get(), &m_chara->m_rotation, 0,
                distance);
    m_chara->m_rotation.x = 0.0f;
    m_chara->m_rotation.z = 0.0f;
    RebuildTransform();
}

void FootingMovable::UpdateLoopPath(float dt) {
    m_phase += m_params.acc * dt;
    fn_80018304(&m_path);
    if (fn_8001C91C(fn_8001B494(), &m_path, 0, m_chara.get(), &m_chara->m_rotation, 1,
                    m_phase)) {
        m_phase -= fn_800183D8(&m_path);
    }
    m_chara->m_rotation.x = 0.0f;
    m_chara->m_rotation.z = 0.0f;
    RebuildTransform();
}

void FootingMovable::UpdateOneWay(float dt) {
    m_phase += 0.001f * m_params.speed * dt;
    if (m_phase > 1.0f) {
        m_phase = 1.0f;
    }
    Vec3f offset(m_params.param0, m_params.param1, m_params.param2);
    fn_8040CE38(&offset, m_phase);
    const Vec3f &position = fn_8040CDD0(&m_origin, &offset);
    fn_800091D0(&m_chara->m_position, &position);
    RebuildTransform();
}

void FootingMovable::StartNormalSync(float dt) {
    m_time = lbl_8057D6A0.m_time;
    SetNormalSyncPosition(m_time);
    RebuildTransform();
    m_state = 11;
    StartParticle();
}

void FootingMovable::UpdateNormalSync(float dt) {
    m_time += **reinterpret_cast<float **>(m_chara->field_0x3C + (0x60 - 0x3C));
    SetNormalSyncPosition(m_time);
    RebuildTransform();
}

void FootingMovable::StartPathSync(float dt) {
    if (fn_8001C7B4(fn_8001B494(), &m_path, m_params.param0)) {
        fn_80018304(&m_path);
        fn_800091D0(&m_chara->m_position, &(const Vec3f &)fn_80018634(&m_path));
        m_chara->m_rotation = fn_80018434(&m_path, 0, 0);
        m_state = 13;
        RebuildTransform();
    }
    m_time = lbl_8057D6A0.m_time;
    StartParticle();
}

void FootingMovable::UpdatePathSync(float dt) {
    m_time += **reinterpret_cast<float **>(m_chara->field_0x3C + (0x60 - 0x3C));
    m_phase = 0.05f * (m_params.speed * (20.0f * m_time)) + m_startPhase;
    float halfLength = 0.5f * fn_800183D8(&m_path);
    float distance = halfLength - halfLength * std::cos(m_phase * (Pi() / 180.0f));
    fn_80018304(&m_path);
    fn_8001C91C(fn_8001B494(), &m_path, 0, m_chara.get(), &m_chara->m_rotation, 0,
                distance);
    m_chara->m_rotation.x = 0.0f;
    m_chara->m_rotation.z = 0.0f;
    RebuildTransform();
}

void FootingMovable::StartNormalPause(float dt) {
    Swing(dt);
    RebuildTransform();
    fn_80014E1C(&m_flags, 2);
    m_state = 15;
    StartParticle();
}

void FootingMovable::UpdateNormalPause(float dt) {
    if (fn_80014E34(&m_flags, 1)) {
        m_pauseTime -= dt;
        if (m_pauseTime <= 0.0f) {
            fn_8006127C(&m_flags, 1);
        }
    } else {
        Swing(dt);
    }
    RebuildTransform();
}

void FootingMovable::SetNormalSyncPosition(float t) {
    m_phase = 0.05f * (m_params.speed * (20.0f * t)) + m_startPhase;
    Vec3f end(m_params.param0, m_params.param1, m_params.param2);
    Vec3f direction;
    Vec3f difference;

    fn_8040CFF8(&end, &m_origin);
    fn_8040CD74(&difference, &end, &m_origin);
    fn_800091EC(&direction, &difference);
    float halfLength = 0.5f * fn_8040CF50(&direction);
    AssignVector(direction, fn_8040CEE8(direction));
    fn_8040CE38(&direction, halfLength - halfLength * std::cos(DegToRad(m_phase)));
    const Vec3f &position = fn_8040CDD0(&direction, &m_origin);
    fn_800091D0(&m_chara->m_position, &position);
}

void FootingMovable::Behave(float dt) {
    if (m_model.get() != 0) {
        m_model->fn_80162FB0(dt);
    }
    if (m_particle.get() != 0) {
        fn_8016761C(m_particle.get(), dt);
    }

    switch (m_state) {
    case 0:
        UpdateWaitSwitch(dt);
        break;
    case 1:
        StartNormal(dt);
        break;
    case 2:
        StartPath(dt);
        break;
    case 3:
        StartLoopPath(dt);
        break;
    case 4:
        UpdateNormal(dt);
        break;
    case 5:
        UpdatePath(dt);
        break;
    case 6:
        UpdateLoopPath(dt);
        break;
    case 7:
        m_pauseTime -= dt;
        if (m_pauseTime <= 0.0f) {
            m_pauseTime = 0.0f;
            m_state = m_resumeState;
            fn_80014E1C(&m_flags, 0);
        }
        break;
    case 9:
        UpdateOneWay(dt);
        break;
    case 10:
        StartNormalSync(dt);
        break;
    case 11:
        UpdateNormalSync(dt);
        break;
    case 12:
        StartPathSync(dt);
        break;
    case 13:
        UpdatePathSync(dt);
        break;
    case 14:
        StartNormalPause(dt);
        break;
    case 15:
        UpdateNormalPause(dt);
        break;
    }
    UpdateSound(dt);
}

void FootingMovable::ReloadPlacement(UnkPlacement_800C9CD0 *owner) {
    LoadFromOwner(owner);
    if (m_model.get() != 0) {
        if (m_shownModel != m_params.model) {
            int count = fn_8015FF44(m_model.get());

            m_shownModel = m_params.model;
            if (m_shownModel < 0) {
                m_shownModel = 0;
            }
            if (m_shownModel > count - 1) {
                m_shownModel = count - 1;
            }
            m_particle.reset();
            m_model->fn_80161880();
            m_model->fn_80162804();
            m_model->fn_80162A68(m_shownModel, 1);
            fn_80161FC8(m_model.get(), m_shownModel, m_chara);

            boost::weak_ptr<CharaColliAttack> collider = fn_80161428(m_model.get(), m_shownModel);
            if (boost::shared_ptr<CharaColliAttack> locked = collider.lock()) {
                locked->field_0x54 = 0;
                locked->field_0x55 = 1;
                locked->m_attackCallback =
                    AttackCallback(boost::bind(&FootingMovable::OnAttack, this, _1));
            }
            m_params.model = m_shownModel;
            fn_800C9D68(owner, &m_params, sizeof(m_params));
        }
        RebuildTransform();

        Vec3f start;
        fn_800091EC(&start, &m_chara->m_position);
        Vec3f end(m_params.param0, m_params.param1, m_params.param2);
        fn_8040CFF8(&end, &start);
        fn_802E0B58(fn_80020A20(&UnkSegment(), &start, &end), &lbl_805F06D8);
        m_model->fn_8016108C();
    }
}

void FootingMovable::LoadFromOwner(UnkPlacement_800C9CD0 *owner) {
    fn_800C9F14(owner, &m_chara->m_position);
    fn_800C9ED4(owner, &m_chara->m_rotation);
    fn_800C9DBC(owner, &m_params, sizeof(m_params));
    fn_800C9E50(owner, &m_switchId);
    m_phase = 0.0f;
    m_type = m_params.type;
    fn_80014E10(&m_flags);
    m_particle.reset();
    if (m_params.pauseDamage > 0.0f) {
        fn_80014E1C(&m_flags, 0);
    } else {
        fn_8006127C(&m_flags, 0);
    }

    switch (m_type) {
    case 0:
        m_state = 1;
        break;
    case 1:
        m_state = 2;
        break;
    case 2:
        m_state = 3;
        break;
    case 4:
        m_state = 10;
        break;
    case 5:
        m_state = 12;
        break;
    case 6:
        m_state = 14;
        break;
    default:
        m_state = 0;
        break;
    }
    switch (m_type) {
    case 0:
    case 3:
    case 6:
        if (m_switchId > 0) {
            m_state = 0;
        }
        break;
    }

    fn_800091D0(&m_origin, &m_chara->m_position);
    m_startPhase = 180.0f - 180.0f * m_params.pos;
    m_time = 0.0f;
}

void FootingMovable::RebuildTransform() {
    if (m_model.get() != 0) {
        Mtx34f matrix;

        fn_8040C148(&matrix, &lbl_805E0720);
        fn_8040B59C(&matrix, &m_chara->m_position);
        fn_8040B848(&matrix, m_chara->m_rotation.y);
        fn_8040B884(&matrix, m_chara->m_rotation.x);
        fn_8040B80C(&matrix, m_chara->m_rotation.z);
        m_model->fn_801633DC(&matrix);
    }
}

void FootingMovable::Swing(float dt) {
    m_phase += dt * (20.0f * m_params.acc);
    Vec3f direction(m_params.param0, m_params.param1, m_params.param2);
    Vec3f end;

    fn_800091EC(&end, &(const Vec3f &)fn_8040CDD0(&m_origin, &direction));
    float halfLength = 0.5f * fn_8040CF50(&direction);
    AssignVector(direction, fn_8040CEE8(direction));
    if (fn_80014E34(&m_flags, 2)) {
        if (m_phase > 180.0f) {
            m_phase = 180.0f;
            fn_8006127C(&m_flags, 2);
            fn_80014E1C(&m_flags, 3);
            fn_80014E1C(&m_flags, 1);
            m_pauseTime = m_params.pause;
        }
    } else if (fn_80014E34(&m_flags, 3)) {
        if (m_phase > 360.0f) {
            m_phase = 0.0f;
            fn_8006127C(&m_flags, 3);
            fn_80014E1C(&m_flags, 2);
            fn_80014E1C(&m_flags, 1);
            m_pauseTime = m_params.pause;
        }
    }
    fn_8040CE38(&direction, halfLength - halfLength * std::cos(DegToRad(m_phase)));
    const Vec3f &position = fn_8040CDD0(&direction, &m_origin);
    fn_800091D0(&m_chara->m_position, &position);
}

void FootingMovable::UpdateSound(float dt) {
    if (fn_80014E34(&m_flags, 4)) {
        if (fn_80014E34(&m_flags, 5)) {
            m_sound.fn_801D455C();
            fn_800091D0(&m_lastPosition, &m_chara->m_position);
            fn_8006127C(&m_flags, 5);
            return;
        }
        if (fn_8000A950(dt)) {
            fn_800091D0(&m_chara->field_0x18, &lbl_805E07A0);
        } else {
            const Vec3f &velocity =
                fn_8040CDA8(
                    &((UnkVectorSub_8040CD74)fn_8040CD74)(&m_chara->m_position, &m_lastPosition),
                    1.0f / dt);
            fn_800091D0(&m_chara->field_0x18, &velocity);
        }
        if (fn_8040CF50(&m_chara->field_0x18) > s_soundSpeed.m_value) {
            if (m_sound.m_id == 0) {
                m_sound = fn_801D3C38(m_soundId, m_chara.get(), this);
            }
        } else if (m_sound.m_id != 0) {
            m_sound.fn_801D455C();
        }
        fn_800091D0(&m_lastPosition, &m_chara->m_position);
    }
}

void FootingMovable::OnAttack(AttackCallbackParam &param) {
    if (fn_80014E34(&m_flags, 0)) {
        if (boost::shared_ptr<CharaColliAttack> attacker = param.m_attacker.lock()) {
            if ((attacker->m_flags & 0x140) == 0) {
                param.m_result = 5;
                return;
            }
            m_resumeState = m_state;
            m_state = 7;
            m_pauseTime = m_params.pauseDamage;
            fn_8006127C(&m_flags, 0);
            param.m_result = 1;
            return;
        }
    }
    param.m_result = 5;
}

void FootingMovable::OnCommand(CharCommand &command) {
    switch (command.TypeId()) {
    case 0x17: /* SetTransparentCommand */
        if (static_cast<SetTransparentCommand &>(command).m_transparent != 0) {
            if (m_model.get() != 0) {
                fn_8015FDD8(m_model.get(), static_cast<SetTransparentCommand &>(command).m_alpha);
                boost::shared_ptr<CharaColliBase> collider =
                    fn_80161614(m_model.get(), m_shownModel).lock();
                if (collider.get() != 0) {
                    int count = collider->GetShapeCount();
                    if (count > 16) {
                        count = 16;
                    }
                    for (int i = 0; i < count; i++) {
                        u32 attributes = collider->GetAttributes(i);
                        m_attributes[i] = attributes;
                        attributes |= kTransparentAttribute;
                        collider->SetAttributes(attributes, i);
                    }
                }
            }
        } else {
            if (m_model.get() != 0) {
                fn_8015FD10(m_model.get());
                boost::shared_ptr<CharaColliBase> collider =
                    fn_80161614(m_model.get(), m_shownModel).lock();
                if (collider.get() != 0) {
                    int count = collider->GetShapeCount();
                    if (count > 16) {
                        count = 16;
                    }
                    for (int i = 0; i < count; i++) {
                        collider->SetAttributes(m_attributes[i], i);
                    }
                }
            }
        }
        break;
    }
    command.field_0x4[3] |= 2;
}

void FootingMovable::StartParticle() {
    if (m_particle.get() == 0 && m_model.get() != 0) {
        m_particle = fn_80166AF4();

        UnkParticleAttach_80169D00 attach;
        attach.ctrl = m_particle;
        fn_8015FA78(m_model.get(), fn_80169D00, &attach);
    }
    if (m_particle.get() != 0) {
        fn_80167010(m_particle.get(), -1);
    }
}

} // namespace FootingMovable

extern "C" void fn_801498B4(void) {
    fn_8006B474("FOOTINGMOVABLE.BIN", 1);
}

extern "C" void fn_801498B0(void) {}

void FootingMovable::FootingMovable::Instantiate(UnkPlacement_800C9CD0 *owner) {
    new FootingMovable(fn_801EB304(fn_8001C4F8(), 11), owner);
}

/* The labels of the parameters by type: the type's name, and what each type uses "spd", "acc",
 * "pause", "pos", param0, param1 and param2 for (name inferred). */
extern "C" const char *fn_80149744(UnkLabelQuery_800CBC20 *cursor) {
    static s32 type = 0;
    static const char *typeNames[7] = {
        "type : normal",       "type : path",       "type : loop path",     "type : one way",
        "type : normal(sync)", "type : path(sync)", "type : normal(pause)",
    };
    static const char *speedLabels[7] = {"spd", "spd", "N/A", "spd", "spd", "spd", "N/A"};
    static const char *accLabels[7] = {"acc", "acc", "acc", "N/A", "N/A", "N/A", "acc"};
    static const char *pauseLabels[7] = {"N/A", "N/A", "N/A", "N/A", "N/A", "N/A", "pause(sec)"};
    static const char *posLabels[7] = {
        "N/A", "N/A", "N/A", "N/A", "Init Pos(0.0-1.0)", "Init Pos(0.0-1.0)", "N/A",
    };
    static const char *param0Labels[7] = {"X", "path id", "path id", "X", "X", "path id", "X"};
    static const char *param1Labels[7] = {"Y", "N/A", "N/A", "Y", "Y", "N/A", "Y"};
    static const char *param2Labels[7] = {"Z", "N/A", "N/A", "Z", "Z", "N/A", "Z"};

    if (cursor->index == 1) {
        type = cursor->value;
        return typeNames[cursor->value];
    } else if (cursor->index == 2) {
        return speedLabels[type];
    } else if (cursor->index == 3) {
        return accLabels[type];
    } else if (cursor->index == 4) {
        return pauseLabels[type];
    } else if (cursor->index == 5) {
        return posLabels[type];
    } else if (cursor->index == 6) {
        return param0Labels[type];
    } else if (cursor->index == 7) {
        return param1Labels[type];
    } else if (cursor->index == 8) {
        return param2Labels[type];
    }
    return 0;
}

static GadgetParamDesc lbl_80529EF8[] = {
    {1, "model", 0, 0, 0x10000, 0.0f, 0.0f, 0.0f, fn_800CBC20, 0},
    {1, "type", 0, 0, 6, 0.0f, 0.0f, 0.0f, fn_80149744, 0},
    {4, "spd", 0, 0, 0, 0.0f, 0.0f, 65536.0f, fn_80149744, 0},
    {4, "acc", 0, 0, 0, 0.0f, 0.0f, 65536.0f, fn_80149744, 0},
    {4, "pause", 0, 0, 0, 0.0f, 0.0f, 65536.0f, fn_80149744, 0},
    {4, "pos", 0, 0, 0, 0.0f, 0.0f, 65536.0f, fn_80149744, 0},
    {4, "param0", 0, 0, 0, 0.0f, -65536.0f, 65536.0f, fn_80149744, 0},
    {4, "param1", 0, 0, 0, 0.0f, -65536.0f, 65536.0f, fn_80149744, 0},
    {4, "param2", 0, 0, 0, 0.0f, -65536.0f, 65536.0f, fn_80149744, 0},
    {4, "pause damage", 0, 0, 0, 0.0f, -65536.0f, 65536.0f, fn_800CBC20, 0},
    {0},
};

GadgetClassDesc lbl_8052A0B0 = {
    "MOVE_FOOTING", fn_801498B4, fn_801498B0, FootingMovable::FootingMovable::Instantiate, 0, 0, 0xF,
    1, 0, 10, lbl_80529EF8,
};
