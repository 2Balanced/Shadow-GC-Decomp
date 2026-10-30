/* FootingBreak (class name from RTTI; its s_taskName is "FootingBreak"): the gadget type of the
 * same name, a floating platform of FOOTINGBREAK.BIN in two models: the platform (variants 0 and 1)
 * and a float under it (variant 2), each with a character info of its own. Both bob up and down
 * ("HoverLength" over "HoverSec"); once the owner's switch is on the platform shakes for "WaitSec"
 * seconds, breaks off with particles, and moves by the "MoveLength" offset over "MoveSec"
 * seconds. A "Burst" footing then shatters (A10603FOOTINGBREAKFLOAT_BRK.DFF debris), a "Hover"
 * footing keeps bobbing at its new place. A SetTransparentCommand fades either model. File name
 * from the class. Names marked "(name inferred)" are not original. Definitions are in reverse
 * address order (deferred inlining emits functions last-defined first). The unit's code reaches its
 * .data through a pooled base address, so the linker keeps that section whole: Game/TNumEdit.h and
 * Game/CharaColliBaseCharInfo.h supply the RTTI base lists it starts with. */
#include "Game/MathConstants.h"
#include "Game/MathTypes.h"

#include <boost/bind.hpp>
#include <boost/function.hpp>

#include "Game/TNumEdit.h"
/* After TNumEdit.h: the order of the casts sets the order of the RTTI base lists in .data. */
#include "Game/CharaColliBaseCharInfo.h"
#include "Game/CharCommand.h"
#include "Game/CharCommandUnit.h"
#include "Game/CharInfo.h"
#include "Game/DebugDisp.h"
#include "Game/Gadget.h"
#include "Game/GadgetCharInfo.h"
#include "Game/GadgetModelInfo.h"
#include "Game/GadgetParticle.h"
#include "Game/GadgetSound.h"
#include "Game/GadgetClassDesc.h"
#include "Game/MathConstants.h"
#include "Game/UnkSwitchBank_800CB69C.h"
#include "Game/FileQueue.h"
#include "Game/MathTypes.h"
#include "boost/shared_ptr.hpp"
#include "boost/weak_ptr.hpp"
#include "size_assert.h"

/* The bobbing and the move are function templates (name and form inferred): the game emits them
 * with the unit's template instances, ahead of boost's, so they are declared before boost. */
extern "C" {
/* True when value is (nearly) zero (behaviour inferred). */
bool fn_8000A950(float value);
/* Vector helpers (behaviour inferred): vector * scale and a + b. */
Vec3f fn_8040CDA8(const Vec3f *vector, float scale);
Vec3f fn_8040CDD0(const Vec3f *a, const Vec3f *b);
}

/* MSL's float overloads, which the game calls out of line (<cmath> is not included: its sqrtf
 * constants would be emitted into this unit's .sdata2). */
namespace std {
float sin(float x);
float cos(float x);
} // namespace std

/* Eases position from start to start + delta over duration seconds at time t. */
template <class T> void Move(T *position, const T *start, const T *delta, float t, float duration) {
    if (t >= duration || 0.0f == duration) {
        fn_800091D0(position, &(const T &)fn_8040CDD0(start, delta));
    } else {
        float rate = t / duration;
        float c = std::cos(rate * lbl_805FAD38);
        fn_800091D0(position,
                    &(const T &)fn_8040CDD0(start, &(const T &)fn_8040CDA8(delta, 0.5f * (1.0f - c))));
    }
}

/* Bobs position about base: amplitude up and down over period seconds at time t. */
template <class T> void Hover(T *position, const T *base, float amplitude, float t, float period) {
    if (fn_8000A950(period)) {
        fn_800091D0(position, base);
    } else {
        float rate = t / period;
        float s = std::sin(rate * lbl_805FAD38);
        fn_800091D0(position, base);
        float offset = 0.5f * (s * amplitude);
        position->y += offset;
    }
}

/* The parameters of a debris spawn (layout as in ArkCannon.cpp; see Game/GadgetDebris.h). */
struct UnkDebrisParam_8015F0B8 {
    UnkDebrisParam_8015F0B8(const boost::shared_ptr<CharInfo> &info, int index);

    /* 0x00 */ u8 field_0x0[0x34];
    /* 0x34 */ float scale;
    /* 0x38 */ u8 field_0x38[0x40 - 0x38];
    /* 0x40 */ float field_0x40;
    /* 0x44 */ s32 model;
    /* 0x48 */ u8 field_0x48[0x4A - 0x48];
    /* 0x4A */ u8 field_0x4A;
    /* 0x4B */ u8 field_0x4B;
    /* 0x4C */ s32 field_0x4C;
    /* 0x50 */ u8 field_0x50[0x54 - 0x50];
    /* 0x54 */ u8 field_0x54;
    /* 0x55 */ u8 field_0x55[0x5C - 0x55];
};
SIZE_ASSERT(UnkDebrisParam_8015F0B8, 0x5C);

extern "C" {
/* Launches the debris described by param (behaviour inferred). */
void fn_8015D93C(UnkDebrisParam_8015F0B8 *param);
}

/* SetTransparentCommand (command type 0x17) is declared in Game/CharCommand.h. */

extern "C" {
/* Builds the character info of a gadget type (see Game/GadgetUtil.h). */
boost::shared_ptr<GadgetCharInfo> fn_8015CD90(int arg0, int arg1);
/* Gives the character info a command handler unless it has one (behaviour inferred). */
void fn_8015CF7C(GadgetCharInfo *charInfo);
/* Model helpers (behaviour inferred, as in Door): load a model file, attach a character info to
 * the collision of variant index (fn_80161BD8 with a fourth argument), fade the model to alpha and
 * make it opaque again, and add particles to its parts. fn_8016082C is called with a variant
 * (behaviour unknown). */
boost::shared_ptr<GadgetModelInfo> fn_8015F800(int arg0, const char *fileName);
void fn_80161FC8(GadgetModelInfo *model, int index, const boost::weak_ptr<CharInfo> &info);
bool fn_80161BD8(GadgetModelInfo *model, int index, const boost::weak_ptr<CharInfo> &info,
                 int arg3);
void fn_8016082C(GadgetModelInfo *model, int index);
void fn_8015FDD8(GadgetModelInfo *model, float alpha);
void fn_8015FD10(GadgetModelInfo *model);
void fn_8015FA78(GadgetModelInfo *model,
                 void *(*callback)(void *part, UnkParticleAttach_80169D00 *param),
                 UnkParticleAttach_80169D00 *param);
void *fn_80169D00(void *part, UnkParticleAttach_80169D00 *param);

/* Particle controller helpers (behaviour inferred, as in Door): create one, advance it by dt,
 * start its emitters and stop them. */
boost::shared_ptr<GadgetParticle::GadgetParticleCtrl> fn_80166AF4(void);
void fn_8016761C(GadgetParticle::GadgetParticleCtrl *ctrl, float dt);
void fn_80167448(GadgetParticle::GadgetParticleCtrl *ctrl, int id);
void fn_80166F88(GadgetParticle::GadgetParticleCtrl *ctrl);

/* The sound manager, a one-shot sound at a character info, and a looping sound at a character
 * info for owner (behaviour inferred; see BombingWall.cpp and GravitySignBoard.cpp). */
void *fn_80053164(void);
void fn_801D54F4(void *manager, int id, GadgetCharInfo *source, int arg3, int arg4);
UnkSoundHandle_801D3DC8 fn_801D3E58(int soundId, GadgetCharInfo *charInfo, void *owner);

/* The file loader and a file of it (see Door.cpp). */
void *fn_800122D4(void);
s32 fn_80012700(void *manager, int arg1, const char *fileName);

/* memcmp (see AdvOption.cpp). */
int fn_803A8DA8(const void *a, const void *b, u32 size);
/* Whether two vectors are equal (see CameraAdjust.cpp). */
bool fn_8000AD08(const Vec3f *a, const Vec3f *b);
/* Vector helpers (behaviour inferred): vector += value and out = a - b. */
void fn_8040CFF8(Vec3f *vector, const Vec3f *value);
void fn_8040CD74(Vec3f *out, const Vec3f *a, const Vec3f *b);
/* A debug line from start to end, and its draw call (see Game/GadgetUtil.h and Meteor.cpp). */
void *fn_80020A20(void *segment, const Vec3f *start, const Vec3f *end);
void fn_802E0AA0(void *segment, const UnkColor_8040D2D8 &color, float width);
/* The colour nearly every gadget's debug view uses (see Game/DebugDraw.h). */
extern UnkColor_8040D2D8 lbl_805F06D8;
}

/* a + b (name inferred). */
inline Vec3f AddVectors(const Vec3f &a, const Vec3f &b) {
    return fn_8040CDD0(&a, &b);
}

/* The parameters the owner supplies (names from the parameter table). */
struct FootingBreakParams {
    /* 0x00 */ u32 type; /* 0 burst, 1 hover */
    /* 0x04 */ float waitSec;
    /* 0x08 */ float hoverLength;
    /* 0x0C */ float hoverSec;
    /* 0x10 */ float moveX;
    /* 0x14 */ float moveY;
    /* 0x18 */ float moveZ;
    /* 0x1C */ float moveSec;
};
SIZE_ASSERT(FootingBreakParams, 0x20);

class FootingBreak : public Gadget {
  public:
    FootingBreak(Task *parent, UnkPlacement_800C9CD0 *owner);
    virtual ~FootingBreak();
    virtual void Behave(float dt);
    virtual void ReloadPlacement(UnkPlacement_800C9CD0 *owner);

    /* Creates the footing under task layer 11 (name inferred). */
    static void Instantiate(UnkPlacement_800C9CD0 *owner);

    /* Method names are inferred. */
    /* Copies the owner's position, rotation, switch id and parameters. */
    void ReadPlacement(UnkPlacement_800C9CD0 *owner);
    /* Place the platform's and the float's models at their character infos. */
    void RebuildTransform();
    void RebuildFloatTransform();
    /* Enters state (0 none, 1 waiting for the switch, 2 shaking, 3 breaking off, 4 moving,
     * 5 bursting, 6 burst, 7 hovering at the new place). */
    void SetState(int state);
    /* Swaps the models' variants, collisions and particles from one state to the next. */
    void ChangeModels(int from, int to);
    /* The command units' callbacks: fade the platform or the float. */
    void OnCommand(CharCommand &command);
    void OnFloatCommand(CharCommand &command);

    static const char *s_taskName;
    /* The debris of a bursting footing. */
    static const char *s_debrisFile;

    /* Member names are inferred. */
    /* 0x30 */ FootingBreakParams m_params;
    /* 0x50 */ boost::shared_ptr<GadgetCharInfo> m_chara;
    /* 0x58 */ boost::shared_ptr<GadgetCharInfo> m_floatChara;
    /* 0x60 */ boost::shared_ptr<GadgetModelInfo> m_model;
    /* 0x68 */ boost::shared_ptr<GadgetModelInfo> m_floatModel;
    /* 0x70 */ float m_alpha;
    /* 0x74 */ float m_floatAlpha;
    /* 0x78 */ boost::shared_ptr<GadgetParticle::GadgetParticleCtrl> m_particle;
    /* 0x80 */ s32 m_state;
    /* 0x84 */ float m_timer;
    /* 0x88 */ Vec3f m_start;
    /* 0x94 */ Vec3f m_delta;
    /* 0xA0 */ float m_hoverTimer;
    /* 0xA4 */ Vec3f m_hoverBase;
    /* 0xB0 */ float m_moveHoverTimer;
    /* 0xB4 */ Vec3f m_moveHoverBase;
    /* 0xC0 */ u8 m_switchId;
    /* 0xC4 */ UnkSoundHandle_801D3DC8 m_sound;
};
SIZE_ASSERT(FootingBreak, 0xE0);

const char *FootingBreak::s_debrisFile = "A10603FOOTINGBREAKFLOAT_BRK.DFF";
const char *FootingBreak::s_taskName = "FootingBreak";

FootingBreak::FootingBreak(Task *parent, UnkPlacement_800C9CD0 *owner)
    : Gadget(parent, owner), m_alpha(1.0f), m_floatAlpha(1.0f), m_state(0) {
    m_name = s_taskName;
    m_chara = fn_8015CD90(0x6B, 4);
    fn_8015CF7C(m_chara.get());
    fn_800729B0(&m_chara->m_commandUnit->m_callback,
                &(const CharCommandCallback &)CharCommandCallback(
                    boost::bind(&FootingBreak::OnCommand, this, _1)));
    m_floatChara = fn_8015CD90(0x6C, 4);
    fn_8015CF7C(m_floatChara.get());
    fn_800729B0(&m_floatChara->m_commandUnit->m_callback,
                &(const CharCommandCallback &)CharCommandCallback(
                    boost::bind(&FootingBreak::OnFloatCommand, this, _1)));
    ReadPlacement(owner);
    m_model = fn_8015F800(1, "FOOTINGBREAK.BIN");
    m_floatModel = fn_8015F800(1, "FOOTINGBREAK.BIN");
    SetState(1);
}

FootingBreak::~FootingBreak() {
    m_sound.fn_801D455C();
}

extern "C" void fn_80262DF0(void) {
    fn_8006B474("FOOTINGBREAK.BIN", 1);
}

extern "C" void fn_80262DEC(void) {}

void FootingBreak::Instantiate(UnkPlacement_800C9CD0 *owner) {
    new FootingBreak(fn_801EB304(fn_8001C4F8(), 11), owner);
}

void FootingBreak::Behave(float dt) {
    switch (m_state) {
    case 1:
        m_hoverTimer += dt;
        Hover(&m_chara->m_position, &m_hoverBase, m_params.hoverLength, m_hoverTimer,
              m_params.hoverSec);
        if (fn_8016904C(fn_800CB69C(), m_switchId) == 1) {
            SetState(2);
        }
        break;
    case 2:
        m_timer += dt;
        m_hoverTimer += dt;
        Hover(&m_chara->m_position, &m_hoverBase, m_params.hoverLength, m_hoverTimer,
              m_params.hoverSec);
        if (m_timer >= m_params.waitSec) {
            fn_801D54F4(fn_80053164(), 0x773E, m_chara.get(), 0, 1);
            SetState(3);
        }
        break;
    case 3:
        if (m_particle.get() == 0) {
            m_particle = fn_80166AF4();
            UnkParticleAttach_80169D00 attach;
            attach.ctrl = m_particle;
            fn_8015FA78(m_model.get(), fn_80169D00, &attach);
            fn_80167448(m_particle.get(), -1);
        }
        m_timer += dt;
        if (m_timer >= 2.0f) {
            fn_801D54F4(fn_80053164(), 0x773E, m_chara.get(), 0, 1);
            SetState(4);
            RebuildFloatTransform();
            m_floatModel->fn_80162FB0(dt);
        }
        break;
    case 4:
        if (m_particle.get() == 0) {
            m_particle = fn_80166AF4();
            UnkParticleAttach_80169D00 attach;
            attach.ctrl = m_particle;
            fn_8015FA78(m_floatModel.get(), fn_80169D00, &attach);
            fn_80167448(m_particle.get(), -1);
        }
        m_timer += dt;
        Move(&m_chara->m_position, &m_start, &m_delta, m_timer, m_params.moveSec);
        m_hoverTimer += dt;
        Hover(&m_floatChara->m_position, &m_hoverBase, m_params.hoverLength, m_hoverTimer,
              m_params.hoverSec);
        if (m_timer >= m_params.moveSec) {
            if (m_params.type == 0) {
                SetState(5);
            } else {
                SetState(7);
            }
        }
        RebuildFloatTransform();
        m_floatModel->fn_80162FB0(dt);
        break;
    case 5:
        m_timer += dt;
        m_hoverTimer += dt;
        Hover(&m_floatChara->m_position, &m_hoverBase, m_params.hoverLength, m_hoverTimer,
              m_params.hoverSec);
        if (m_timer >= 1.0f) {
            fn_801D54F4(fn_80053164(), 0x773E, m_chara.get(), 0, 1);
            SetState(6);
        }
        RebuildFloatTransform();
        m_floatModel->fn_80162FB0(dt);
        break;
    case 6:
        m_hoverTimer += dt;
        Hover(&m_floatChara->m_position, &m_hoverBase, m_params.hoverLength, m_hoverTimer,
              m_params.hoverSec);
        RebuildFloatTransform();
        m_floatModel->fn_80162FB0(dt);
        break;
    case 7:
        m_moveHoverTimer += dt;
        Hover(&m_chara->m_position, &m_moveHoverBase, m_params.hoverLength, m_moveHoverTimer,
              m_params.hoverSec);
        m_hoverTimer += dt;
        Hover(&m_floatChara->m_position, &m_hoverBase, m_params.hoverLength, m_hoverTimer,
              m_params.hoverSec);
        RebuildFloatTransform();
        m_floatModel->fn_80162FB0(dt);
        break;
    }
    RebuildTransform();
    m_model->fn_80162FB0(dt);
    if (m_particle.get() != 0) {
        fn_8016761C(m_particle.get(), dt);
    }
}

void FootingBreak::ReloadPlacement(UnkPlacement_800C9CD0 *owner) {
    FootingBreakParams oldParams = m_params;
    u8 oldSwitchId = m_switchId;
    Vec3f oldPosition;
    fn_800091EC(&oldPosition, &m_chara->m_position);
    ReadPlacement(owner);
    if (fn_803A8DA8(&oldParams, &m_params, sizeof(m_params)) != 0 || oldSwitchId != m_switchId ||
        !fn_8000AD08(&oldPosition, &m_chara->m_position)) {
        SetState(1);
    }
    RebuildTransform();
    m_model->fn_80162EF4();
    m_model->fn_8016108C();

    u8 moveSegment[0x18];
    fn_80020A20(moveSegment, &m_chara->m_position,
                &(const Vec3f &)AddVectors(m_chara->m_position,
                                           Vec3f(m_params.moveX, m_params.moveY, m_params.moveZ)));
    fn_802E0AA0(moveSegment, lbl_805F06D8, 5.0f);

    Vec3f bottom;
    fn_800091EC(&bottom, &m_chara->m_position);
    Vec3f top;
    fn_800091EC(&top, &m_chara->m_position);
    bottom.y -= 0.5f * m_params.hoverLength;
    top.y = 0.5f * m_params.hoverLength + bottom.y;
    u8 hoverSegment[0x18];
    fn_80020A20(hoverSegment, &bottom, &top);
    fn_802E0AA0(hoverSegment, UnkColor_8040D2D8(0xC0, 0xFF, 0xFF, 0x00), 5.0f);
}

void FootingBreak::ReadPlacement(UnkPlacement_800C9CD0 *owner) {
    fn_800C9F14(owner, &m_chara->m_position);
    fn_800C9ED4(owner, &m_chara->m_rotation);
    fn_800C9E60(owner, &m_switchId);
    fn_800C9DBC(owner, &m_params, sizeof(m_params));
}

void FootingBreak::RebuildTransform() {
    Mtx34f matrix;

    fn_8040C148(&matrix, &lbl_805E0720);
    fn_8040B59C(&matrix, &m_chara->m_position);
    fn_8040B848(&matrix, m_chara->m_rotation.y);
    fn_8040B884(&matrix, m_chara->m_rotation.x);
    fn_8040B80C(&matrix, m_chara->m_rotation.z);
    m_model->fn_801633DC(&matrix);
}

void FootingBreak::RebuildFloatTransform() {
    Mtx34f matrix;

    fn_8040C148(&matrix, &lbl_805E0720);
    fn_8040B59C(&matrix, &m_floatChara->m_position);
    fn_8040B848(&matrix, m_floatChara->m_rotation.y);
    fn_8040B884(&matrix, m_floatChara->m_rotation.x);
    fn_8040B80C(&matrix, m_floatChara->m_rotation.z);
    m_floatModel->fn_801633DC(&matrix);
}

void FootingBreak::SetState(int state) {
    switch (m_state) {
    case 3:
    case 5:
        m_sound.fn_801D455C();
        break;
    }
    switch (state) {
    case 1:
        m_hoverTimer = 0.0f;
        fn_800C9F14(m_owner, &m_hoverBase);
        break;
    case 2:
        m_timer = 0.0f;
        break;
    case 3:
        m_timer = 0.0f;
        m_sound = fn_801D3E58(0x773D, m_chara.get(), this);
        break;
    case 4: {
        Vec3f position;
        Vec3f move(m_params.moveX, m_params.moveY, m_params.moveZ);
        fn_800091D0(&m_start, &m_chara->m_position);
        fn_800C9F14(m_owner, &position);
        fn_8040CFF8(&position, &move);
        Vec3f delta;
        fn_8040CD74(&delta, &position, &m_start);
        fn_800091D0(&m_delta, &delta);
        m_timer = 0.0f;
        fn_800091D0(&m_floatChara->m_position, &m_chara->m_position);
        m_floatChara->m_rotation = m_chara->m_rotation;
        break;
    }
    case 5:
        m_timer = 0.0f;
        m_sound = fn_801D3E58(0x773D, m_chara.get(), this);
        break;
    case 7:
        fn_800C9F14(m_owner, &m_moveHoverBase);
        m_moveHoverBase.x += m_params.moveX;
        m_moveHoverBase.y += m_params.moveY;
        m_moveHoverBase.z += m_params.moveZ;
        m_moveHoverTimer = 0.0f;
        break;
    }
    ChangeModels(m_state, state);
    m_state = state;
}

void FootingBreak::ChangeModels(int from, int to) {
    switch (from) {
    case 1:
    case 2:
        m_model->fn_80161880();
        m_model->fn_80162804();
        break;
    case 3:
        m_model->fn_801604FC();
        m_model->fn_80161880();
        m_model->fn_80162804();
        if (m_particle.get() != 0) {
            fn_80166F88(m_particle.get());
            m_particle.reset();
        }
        break;
    case 5:
        m_model->fn_801604FC();
        m_model->fn_80161880();
        m_model->fn_80162804();
        m_floatModel->fn_80161880();
        m_floatModel->fn_80162804();
        break;
    case 4:
        if (m_particle.get() != 0) {
            fn_80166F88(m_particle.get());
            m_particle.reset();
        }
        m_model->fn_80161880();
        m_model->fn_80162804();
        m_floatModel->fn_80161880();
        m_floatModel->fn_80162804();
        break;
    case 7:
        m_model->fn_80161880();
        m_model->fn_80162804();
        m_floatModel->fn_80161880();
        m_floatModel->fn_80162804();
        break;
    case 6:
        m_floatModel->fn_80161880();
        m_floatModel->fn_80162804();
        break;
    }
    switch (to) {
    case 1:
    case 2:
        m_model->fn_80162A68(0, 1);
        fn_80161FC8(m_model.get(), 0, m_chara);
        fn_80161FC8(m_model.get(), 1, m_chara);
        break;
    case 3:
        m_model->fn_80162A68(0, 1);
        m_model->fn_80160C28(0);
        fn_8016082C(m_model.get(), 0);
        fn_80161BD8(m_model.get(), 0, m_chara, 0);
        fn_80161BD8(m_model.get(), 1, m_chara, 0);
        m_model->fn_80160B18(0);
        m_particle.reset();
        break;
    case 5:
        m_model->fn_80162A68(1, 1);
        m_model->fn_80160C28(1);
        fn_8016082C(m_model.get(), 1);
        fn_80161BD8(m_model.get(), 2, m_chara, 1);
        fn_80161BD8(m_model.get(), 3, m_chara, 1);
        m_model->fn_80160B18(1);
        m_floatModel->fn_80162A68(2, 1);
        fn_80161FC8(m_floatModel.get(), 4, m_floatChara);
        break;
    case 4:
        m_model->fn_80162A68(1, 1);
        fn_80161FC8(m_model.get(), 2, m_chara);
        fn_80161FC8(m_model.get(), 3, m_chara);
        m_floatModel->fn_80162A68(2, 1);
        fn_80161FC8(m_floatModel.get(), 4, m_floatChara);
        m_particle.reset();
        break;
    case 7:
        m_model->fn_80162A68(1, 0);
        fn_80161FC8(m_model.get(), 2, m_chara);
        fn_80161FC8(m_model.get(), 3, m_chara);
        m_floatModel->fn_80162A68(2, 0);
        fn_80161FC8(m_floatModel.get(), 4, m_floatChara);
        break;
    case 6: {
        m_floatModel->fn_80162A68(2, 0);
        fn_80161FC8(m_floatModel.get(), 4, m_floatChara);
        UnkDebrisParam_8015F0B8 debris(m_chara, 0);
        debris.model = fn_80012700(fn_800122D4(), 1, s_debrisFile);
        debris.field_0x40 = m_chara->m_position.y;
        debris.scale = 1.0f;
        debris.field_0x4A = 0;
        debris.field_0x54 = 1;
        fn_8015D93C(&debris);
        break;
    }
    }
}

void FootingBreak::OnCommand(CharCommand &command) {
    switch (command.TypeId()) {
    case 0x17: /* SetTransparentCommand */
        m_alpha = static_cast<SetTransparentCommand &>(command).m_alpha;
        if (static_cast<SetTransparentCommand &>(command).m_transparent) {
            fn_8015FDD8(m_model.get(), m_alpha);
        } else {
            fn_8015FD10(m_model.get());
        }
        break;
    }
    command.field_0x4[3] |= 2;
}

void FootingBreak::OnFloatCommand(CharCommand &command) {
    switch (command.TypeId()) {
    case 0x17: /* SetTransparentCommand */
        m_floatAlpha = static_cast<SetTransparentCommand &>(command).m_alpha;
        if (static_cast<SetTransparentCommand &>(command).m_transparent) {
            fn_8015FDD8(m_floatModel.get(), m_floatAlpha);
        } else {
            fn_8015FD10(m_floatModel.get());
        }
        break;
    }
    command.field_0x4[3] |= 2;
}

static GadgetParamDesc lbl_8054BCD0[] = {
    {2, "Type 0:Burst 1:Hover", 0, 0, 1, 0.0f, 0.0f, 0.0f, fn_800CBC20, 0},
    {4, "WaitSec", 0, 0, 0, 2.0f, 0.0f, 65536.0f, fn_800CBC20, 0},
    {4, "HoverLength point", 0, 0, 0, 20.0f, -65536.0f, 65536.0f, fn_800CBC20, 0},
    {4, "HoverSec", 0, 0, 0, 6.0f, 0.0f, 65536.0f, fn_800CBC20, 0},
    {4, "MoveLengthX point", 0, 0, 0, 0.0f, -65536.0f, 65536.0f, fn_800CBC20, 0},
    {4, "MoveLengthY point", 0, 0, 0, 0.0f, -65536.0f, 65536.0f, fn_800CBC20, 0},
    {4, "MoveLengthZ point", 0, 0, 0, 0.0f, -65536.0f, 65536.0f, fn_800CBC20, 0},
    {4, "MoveSec", 0, 0, 0, 5.0f, 0.0f, 65536.0f, fn_800CBC20, 0},
    {0},
};

GadgetClassDesc lbl_8054BE38 = {
    "FootingBreak", fn_80262DF0, fn_80262DEC, FootingBreak::Instantiate, 0, 0, 0x189E, 1, 0, 10,
    lbl_8054BCD0,
};
