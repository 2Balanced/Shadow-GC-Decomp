/* FootingRoll (class name from RTTI; its s_taskName is "FootingRoll"): the gadget type of the same
 * name, a platform of FOOTINGROLL.BIN (model 0 small, 1 large) that, once the owner's switch is on,
 * swings between its start and the "MoveLength" offset (eased over "MoveSec" seconds, pausing
 * "MovePauseSec" seconds at each end) while it turns about its "RotAxis": one way at "RotSpd"
 * degrees a second, or back and forth up to "RotMax" degrees. A SetTransparentCommand fades it.
 * File name from the class. Names marked "(name inferred)" are not original. Definitions are in
 * reverse address order (deferred inlining emits functions last-defined first). */
#include <boost/bind.hpp>
#include <boost/function.hpp>

#include "Game/CharCommand.h"
#include "Game/CharCommandUnit.h"
#include "Game/CharInfo.h"
#include "Game/DebugDisp.h"
#include "Game/Gadget.h"
#include "Game/GadgetCharInfo.h"
#include "Game/GadgetModelInfo.h"
#include "Game/GadgetClassDesc.h"
#include "Game/MathConstants.h"
#include "Game/UnkSwitchBank_800CB69C.h"
#include "Game/FileQueue.h"
#include "Game/MathTypes.h"
#include "boost/shared_ptr.hpp"
#include "boost/weak_ptr.hpp"
#include "size_assert.h"

/* MSL's float overloads, which the game calls out of line (<cmath> is not included: its sqrtf
 * constants would be emitted into this unit's .sdata2). */
namespace std {
float cos(float x);
float fabs(float x);
} // namespace std

/* SetTransparentCommand (command type 0x17) is declared in Game/CharCommand.h. */

extern "C" {
/* Builds the character info of a gadget type (see Game/GadgetUtil.h). */
boost::shared_ptr<GadgetCharInfo> fn_8015CD90(int arg0, int arg1);
/* vector *= scale, and a debug line from start to end (see Game/GadgetUtil.h). */
void fn_8040CE38(Vec3f *vector, float scale);
void *fn_80020A20(void *segment, const Vec3f *start, const Vec3f *end);
/* The colour nearly every gadget's debug view uses (see Game/DebugDraw.h). */
extern u32 lbl_805F06D8;
/* Gives the character info a command handler unless it has one (behaviour inferred). */
void fn_8015CF7C(GadgetCharInfo *charInfo);
/* Model helpers (behaviour inferred, as in Door): load a model file, attach a character info to
 * the collision of variant index, fade the model to alpha and make it opaque again. */
boost::shared_ptr<GadgetModelInfo> fn_8015F800(int arg0, const char *fileName);
void fn_80161FC8(GadgetModelInfo *model, int index, const boost::weak_ptr<CharInfo> &info);
void fn_8015FDD8(GadgetModelInfo *model, float alpha);
void fn_8015FD10(GadgetModelInfo *model);

/* True when value is (nearly) zero (behaviour inferred). */
bool fn_8000A950(float value);
/* An angle wrapped into -pi..pi (behaviour inferred). */
float fn_8040E99C(float angle);
/* Vector helpers (behaviour inferred): vector += value, vector * scale (two copies), a + b, and
 * vector turned about z, x and y. */
void fn_8040CFF8(Vec3f *vector, const Vec3f *value);
Vec3f fn_800091A4(const Vec3f *vector, float scale);
Vec3f fn_8040CDA8(const Vec3f *vector, float scale);
Vec3f fn_8040CDD0(const Vec3f *a, const Vec3f *b);
Vec3f fn_8040D044(const Vec3f *vector, float angle);
Vec3f fn_8040D118(const Vec3f *vector, float angle);
Vec3f fn_8040D0AC(const Vec3f *vector, float angle);
/* Matrix product and the rotation angles of a matrix (see GravitySignBoard.cpp). */
Mtx34f fn_8001E044(const Mtx34f *a, const Mtx34f *b);
void fn_800494F8(Vec3f *rotation, const Mtx34f *matrix);
/* The debug line draw call (see Meteor.cpp). */
void fn_802E0AA0(UnkDebugLine_80020A20 *segment, const void *color, float width);
}

/* vector += value (name inferred). */
inline void AddVector(Vec3f &vector, const Vec3f &value) {
    fn_8040CFF8(&vector, &value);
}

/* a + b (name inferred). */
inline Vec3f AddVectors(const Vec3f &a, const Vec3f &b) {
    return fn_8040CDD0(&a, &b);
}

/* The parameters the owner supplies (names from the parameter table). */
struct FootingRollParams {
    /* 0x00 */ u32 model; /* 0 small, 1 large */
    /* 0x04 */ float moveX;
    /* 0x08 */ float moveY;
    /* 0x0C */ float moveZ;
    /* 0x10 */ float moveSec;
    /* 0x14 */ float movePauseSec;
    /* 0x18 */ s32 rotType; /* 0 one way, 1 back and forth */
    /* 0x1C */ s32 rotAxis; /* 0 X, 1 Y, 2 Z */
    /* 0x20 */ float rotSpd; /* degrees a second; radians once read */
    /* 0x24 */ float rotMax; /* degrees; radians once read */
};
SIZE_ASSERT(FootingRollParams, 0x28);

class FootingRoll : public Gadget {
  public:
    FootingRoll(Task *parent, UnkPlacement_800C9CD0 *owner);
    virtual ~FootingRoll();
    virtual void Behave(float dt);
    virtual void ReloadPlacement(UnkPlacement_800C9CD0 *owner);

    /* Creates the footing under task layer 11 (name inferred). */
    static void Instantiate(UnkPlacement_800C9CD0 *owner);

    /* Method names are inferred. */
    /* Copies the owner's position, rotation, parameters and switch id, and resets the motion. */
    void ReadPlacement(UnkPlacement_800C9CD0 *owner);
    /* Places the model at the character info, turned by the roll angle about the axis. */
    void RebuildTransform();
    /* The command unit's callback: fades the model. */
    void OnCommand(CharCommand &command);

    static const char *s_taskName;

    /* Member names are inferred. */
    /* 0x30 */ FootingRollParams m_params;
    /* 0x58 */ boost::shared_ptr<GadgetCharInfo> m_chara;
    /* 0x60 */ boost::shared_ptr<GadgetModelInfo> m_model;
    /* 0x68 */ s32 m_state;     /* 0 waiting for the switch, 1 moving */
    /* 0x6C */ s32 m_movePhase; /* 0 moving, 1 pausing */
    /* 0x70 */ float m_timer;
    /* 0x74 */ s32 m_moveBack;  /* 1 on the way back */
    /* 0x78 */ s32 m_rollBack;  /* 1 turning backwards */
    /* 0x7C */ float m_angle;
    /* 0x80 */ u8 m_switchId;
    /* 0x84 */ u8 field_0x84[4]; /* unused */
};
SIZE_ASSERT(FootingRoll, 0x88);

const char *FootingRoll::s_taskName = "FootingRoll";

FootingRoll::FootingRoll(Task *parent, UnkPlacement_800C9CD0 *owner)
    : Gadget(parent, owner), m_state(0) {
    m_name = s_taskName;
    m_chara = fn_8015CD90(0x6D, 4);
    fn_8015CF7C(m_chara.get());
    fn_800729B0(&m_chara->m_commandUnit->m_callback,
                &(const CharCommandCallback &)CharCommandCallback(
                    boost::bind(&FootingRoll::OnCommand, this, _1)));
    ReadPlacement(owner);
    m_model = fn_8015F800(1, "FOOTINGROLL.BIN");
    m_model->fn_80162A68(m_params.model, 1);
    fn_80161FC8(m_model.get(), m_params.model * 2, m_chara);
    fn_80161FC8(m_model.get(), m_params.model * 2 + 1, m_chara);
}

FootingRoll::~FootingRoll() {}

extern "C" void fn_80261658(void) {
    fn_8006B474("FOOTINGROLL.BIN", 1);
}

extern "C" void fn_80261654(void) {}

void FootingRoll::Instantiate(UnkPlacement_800C9CD0 *owner) {
    new FootingRoll(fn_801EB304(fn_8001C4F8(), 11), owner);
}

void FootingRoll::Behave(float dt) {
    if (m_state == 0) {
        u8 switchId = m_switchId;
        if (switchId == 0 || fn_8016904C(fn_800CB69C(), switchId) == 1) {
            m_state = 1;
        }
    } else if (m_state == 1) {
        m_timer += dt;
        switch (m_movePhase) {
        case 0:
            if (m_timer < m_params.moveSec && !fn_8000A950(m_params.moveSec)) {
                float rate = m_timer / m_params.moveSec;
                float c = std::cos(rate * lbl_805FAD38);
                float t = 0.5f * (1.0f - c);
                Vec3f move(m_params.moveX, m_params.moveY, m_params.moveZ);
                fn_800C9F14(m_owner, &m_chara->m_position);
                if (m_moveBack == 0) {
                    AddVector(m_chara->m_position, fn_800091A4(&move, t));
                } else {
                    fn_800091D0(&m_chara->m_position,
                                &(const Vec3f &)AddVectors(m_chara->m_position,
                                                           fn_8040CDA8(&move, 1.0f - t)));
                }
            } else {
                m_timer = 0.0f;
                m_movePhase = 1;
            }
            break;
        case 1:
            if (m_timer > m_params.movePauseSec) {
                m_timer = 0.0f;
                if (m_moveBack == 0) {
                    m_moveBack = 1;
                } else {
                    m_moveBack = 0;
                }
                m_movePhase = 0;
            }
            break;
        }
        switch (m_params.rotType) {
        case 0:
            m_angle = m_params.rotSpd * dt + m_angle;
            m_angle = fn_8040E99C(m_angle);
            break;
        case 1:
            float sign = m_rollBack == 0 ? 1.0f : -1.0f;
            m_angle = sign * (m_params.rotSpd * dt) + m_angle;
            if (std::fabs(m_angle) > m_params.rotMax) {
                if (m_angle > 0.0f) {
                    m_angle = m_params.rotMax;
                } else {
                    m_angle = -m_params.rotMax;
                }
                if (m_rollBack == 0) {
                    m_rollBack = 1;
                } else {
                    m_rollBack = 0;
                }
            }
            break;
        }
        RebuildTransform();
    }
    m_model->fn_80162FB0(dt);
}

void FootingRoll::ReloadPlacement(UnkPlacement_800C9CD0 *owner) {
    FootingRollParams old = m_params;
    ReadPlacement(owner);
    if (old.model != m_params.model) {
        m_model->fn_80162804();
        m_model->fn_80161880();
        m_model->fn_80162A68(m_params.model, 0);
        fn_80161FC8(m_model.get(), m_params.model * 2, m_chara);
        fn_80161FC8(m_model.get(), m_params.model * 2 + 1, m_chara);
    }
    RebuildTransform();
    m_model->fn_80162EF4();
    m_model->fn_8016108C();

    u8 moveSegment[0x18];
    fn_80020A20(moveSegment, &m_chara->m_position,
                &(const Vec3f &)AddVectors(m_chara->m_position,
                                           Vec3f(m_params.moveX, m_params.moveY, m_params.moveZ)));
    fn_802E0AA0((UnkDebugLine_80020A20 *)moveSegment, &lbl_805F06D8, 5.0f);

    UnkColor_8040D2D8 color(0xFF, 0x00, 0x00, 0x00);
    Vec3f axis(0.0f, 0.0f, 0.0f);
    switch (m_params.rotAxis) {
    case 0:
        color.g = 0xFF;
        axis.x = 1.0f;
        break;
    case 1:
        color.b = 0xFF;
        axis.y = 1.0f;
        break;
    case 2:
        color.a = 0xFF;
        axis.z = 1.0f;
        break;
    }
    fn_800091D0(&axis, &(const Vec3f &)fn_8040D044(&axis, m_chara->m_rotation.z));
    fn_800091D0(&axis, &(const Vec3f &)fn_8040D118(&axis, m_chara->m_rotation.x));
    fn_800091D0(&axis, &(const Vec3f &)fn_8040D0AC(&axis, m_chara->m_rotation.y));
    fn_8040CE38(&axis, 1000.0f);
    u8 axisSegment[0x18];
    fn_80020A20(axisSegment, &m_chara->m_position,
                &(const Vec3f &)AddVectors(m_chara->m_position, axis));
    fn_802E0AA0((UnkDebugLine_80020A20 *)axisSegment, &color, 2.0f);
}

void FootingRoll::ReadPlacement(UnkPlacement_800C9CD0 *owner) {
    fn_800C9F14(owner, &m_chara->m_position);
    fn_800C9ED4(owner, &m_chara->m_rotation);
    fn_800C9DBC(owner, &m_params, sizeof(m_params));
    fn_800C9E60(owner, &m_switchId);
    m_params.rotSpd *= lbl_805FAD38 / 180.0f;
    m_params.rotMax *= lbl_805FAD38 / 180.0f;
    m_state = 0;
    m_movePhase = 0;
    m_timer = 0.0f;
    m_moveBack = 0;
    m_rollBack = 0;
    m_angle = 0.0f;
}

void FootingRoll::RebuildTransform() {
    Mtx34f matrix;
    Mtx34f placed;

    fn_8040C148(&matrix, &lbl_805E0720);
    fn_800C9ED4(m_owner, &m_chara->m_rotation);
    fn_8040B848(&matrix, m_chara->m_rotation.y);
    fn_8040B884(&matrix, m_chara->m_rotation.x);
    fn_8040B80C(&matrix, m_chara->m_rotation.z);
    switch (m_params.rotAxis) {
    case 0:
        fn_8040B884(&matrix, m_angle);
        break;
    case 1:
        fn_8040B848(&matrix, m_angle);
        break;
    case 2:
        fn_8040B80C(&matrix, m_angle);
        break;
    }
    fn_800494F8(&m_chara->m_rotation, &matrix);
    fn_8040C148(&placed, &lbl_805E0720);
    fn_8040B59C(&placed, &m_chara->m_position);
    fn_8040C148(&placed, &(const Mtx34f &)fn_8001E044(&placed, &matrix));
    m_model->fn_801633DC(&placed);
}

void FootingRoll::OnCommand(CharCommand &command) {
    switch (command.TypeId()) {
    case 0x17: /* SetTransparentCommand */
        if (static_cast<SetTransparentCommand &>(command).m_alpha < 1.0f) {
            fn_8015FDD8(m_model.get(), static_cast<SetTransparentCommand &>(command).m_alpha);
        } else {
            fn_8015FD10(m_model.get());
        }
        break;
    }
    command.field_0x4[3] |= 2;
}

static GadgetParamDesc lbl_8054BA6C[] = {
    {2, "Model 0:Small 1:Large", 0, 0, 1, 0.0f, 0.0f, 0.0f, fn_800CBC20, 0},
    {4, "MoveLengthX point", 0, 0, 0, 0.0f, -65536.0f, 65536.0f, fn_800CBC20, 0},
    {4, "MoveLengthY point", 0, 0, 0, 0.0f, -65536.0f, 65536.0f, fn_800CBC20, 0},
    {4, "MoveLengthZ point", 0, 0, 0, 0.0f, -65536.0f, 65536.0f, fn_800CBC20, 0},
    {4, "MoveSec", 0, 0, 0, 0.0f, 0.0f, 65536.0f, fn_800CBC20, 0},
    {4, "MovePauseSec", 0, 0, 0, 0.0f, 0.0f, 65536.0f, fn_800CBC20, 0},
    {2, "RotType 0:OneWay 1:RT", 0, 0, 1, 0.0f, 0.0f, 0.0f, fn_800CBC20, 0},
    {2, "RotAxis 0:X 1:Y 2:Z", 0, 0, 2, 0.0f, 0.0f, 0.0f, fn_800CBC20, 0},
    {4, "RotSpd deg/sec", 0, 0, 0, 0.0f, -65536.0f, 65536.0f, fn_800CBC20, 0},
    {4, "RotMax(RT Only) deg", 0, 0, 0, 0.0f, 0.0f, 65536.0f, fn_800CBC20, 0},
    {0},
};

GadgetClassDesc lbl_8054BC24 = {
    "FootingRoll", fn_80261658, fn_80261654, FootingRoll::Instantiate, 0, 0, 0x189F, 1, 0, 10,
    lbl_8054BA6C,
};
