/* RailSwitch::SetRailSwitch (class name from RTTI; its s_taskName is "SetRailSwitch"): the
 * RAIL SWITCH gadget, a switch (RAILSWITCH.BIN) that a strong enough attack flips. Each flip plays
 * its motion, recolours its "mat" material, rumbles the pad, plays a sound and sends a
 * SwitchCommand for path "route" to character 0x80 (the object riding the rails). It starts flipped
 * when that path already runs on its alternative. File name inferred from the namespace. Names
 * marked "(name inferred)" are not original. Definitions are in reverse address order (deferred
 * inlining emits functions last-defined first). */
#include <boost/bind.hpp>
#include <boost/function.hpp>

#include "Game/CharCommand.h"
#include "Game/CharInfo.h"
#include "Game/CharaColliAttack.h"
#include "Game/Gadget.h"
#include "Game/GadgetCharInfo.h"
#include "Game/GadgetModelInfo.h"
#include "Game/GadgetClassDesc.h"
#include "Game/GadgetUtil.h"
#include "Game/Vibration.h"
#include "Game/FileQueue.h"
#include "boost/shared_ptr.hpp"
#include "boost/weak_ptr.hpp"
#include "size_assert.h"

using SonicteamUSA::System::CharaColli::AttackCallback;
using SonicteamUSA::System::CharaColli::AttackCallbackParam;
using SonicteamUSA::System::CharaColli::CharaColliAttack;

/* Four colour bytes built by an out-of-line constructor (see ElecBarrier.cpp). */
struct UnkColor_8040D2D8 {
    UnkColor_8040D2D8(u8 r, u8 g, u8 b, u8 a);

    /* 0x0 */ u8 r;
    /* 0x1 */ u8 g;
    /* 0x2 */ u8 b;
    /* 0x3 */ u8 a;
};
SIZE_ASSERT(UnkColor_8040D2D8, 0x4);

/* The material animation of a model fn_801621F0 finds by name (as in ElecCristalWall.cpp). */
struct UnkMaterialAnim_801621F0 {
    /* 0x0 */ u8 field_0x0[0x8];
    /* 0x8 */ UnkColor_8040D2D8 color;
};

/* SwitchCommand (class name from RTTI; vtable at 0x8053D404): command 9, which switches path
 * m_index of a path-following object to its alternative (see BKTankController.cpp). Its virtual
 * functions are defined with the other commands, so this unit only refers to its vtable. */
class SwitchCommand : public CharCommand {
  public:
    SwitchCommand(int index) : CharCommand(CommandType(9), 4), m_index(index) {}
    virtual ~SwitchCommand();

    /* The command type as CharCommand stores it. Calling this inline function keeps the
     * constructor out of line, as the game has it (as GadgetCommand.h does). */
    static u16 CommandType(int type) {
        return type;
    }

    /* 0x14 */ s32 m_index;
};
SIZE_ASSERT(SwitchCommand, 0x18);

/* A path of the rails (struct name unknown); only the member read here is laid out. */
struct UnkRailPath_8054B8E0 {
    /* 0x0 */ u8 field_0x0[0x8];
    /* 0x8 */ s32 m_alternative; /* 1 while the path runs on its alternative (inferred) */
    /* 0xC */ u32 field_0xC;
};
SIZE_ASSERT(UnkRailPath_8054B8E0, 0x10);

class UnkCharManager_800DDF20;

extern "C" {
/* The paths of the rails (see above). */
extern UnkRailPath_8054B8E0 lbl_8054B8E0[];

/* A 32-bit flag word (see BAGunShip.cpp; fn_80014E04 constructs it cleared). */
void fn_80014E04(u32 *flags);
void fn_80014E1C(u32 *flags, int bit);
void fn_8006127C(u32 *flags, int bit);
bool fn_80014E34(u32 *flags, int bit);

/* The switch id the owner was placed with (see BAGunShip.cpp). */
void fn_800C9E50(UnkPlacement_800C9CD0 *owner, s32 *id);

/* GadgetModelInfo helpers (behaviour inferred; see BAGunShip.cpp and ElecCristalWall.cpp). */
boost::shared_ptr<GadgetModelInfo> fn_8015F800(int arg0, const char *fileName);
bool fn_80161BD8(GadgetModelInfo *model, int index, const boost::weak_ptr<CharInfo> &info,
                 int arg3);
boost::weak_ptr<CharaColliAttack> fn_80161428(GadgetModelInfo *model, int variant);
void fn_80160A84(GadgetModelInfo *model, int index, int motion);
UnkMaterialAnim_801621F0 *fn_801621F0(GadgetModelInfo *model, const char *name);
void fn_8016239C(GadgetModelInfo *model, const char *name);
/* The character info's world matrix (see BalloonGenerator.cpp). */
void fn_8015CEA0(GadgetCharInfo *charInfo, Mtx34f *matrix);

/* Controller vibrations near charInfo (see BAGunShip.cpp). */
boost::weak_ptr<Vibration::RequestHandle> fn_80169B80(int type, GadgetCharInfo *charInfo);
/* Plays sound id at a character info, once and as a loop for source (see BAGunShip.cpp). */
void fn_801D54F4(void *manager, int id, GadgetCharInfo *source, int arg3, int arg4);
void fn_801D53C4(void *manager, int id, GadgetCharInfo *charInfo, void *source, int arg4, int arg5,
                 int arg6);
/* The character manager and its character with id, or 0 (see BKTankColi.cpp). */
UnkCharManager_800DDF20 *fn_800DDF20(void);
boost::weak_ptr<CharInfo> *fn_801DCB14(UnkCharManager_800DDF20 *manager, int id);
}

/* The switch's colours, off and on (names inferred). */
static UnkColor_8040D2D8 s_colors[2] = {
    UnkColor_8040D2D8(0xFF, 0, 0x88, 0x62),
    UnkColor_8040D2D8(0xFF, 0x88, 1, 0),
};

namespace RailSwitch {

/* The parameters the owner supplies (name from the parameter table). */
struct SetRailSwitchParams {
    SetRailSwitchParams() : route(0) {}

    /* 0x0 */ s32 route;
};
SIZE_ASSERT(SetRailSwitchParams, 0x4);

/* The flag word at 0x58 (struct name unknown). */
struct SetRailSwitchFlags {
    SetRailSwitchFlags() {
        fn_80014E04(&m_bits);
    }
    void Set(int bit) {
        fn_80014E1C(&m_bits, bit);
    }
    void Clear(int bit) {
        fn_8006127C(&m_bits, bit);
    }
    bool Test(int bit) {
        return fn_80014E34(&m_bits, bit);
    }

    u32 m_bits; /* bit 0: the flip sound is due; 1: on (loop sound) */
};

class SetRailSwitch : public Gadget {
  public:
    SetRailSwitch(Task *parent, UnkPlacement_800C9CD0 *owner);
    virtual ~SetRailSwitch();
    virtual void Behave(float dt);
    virtual void ReloadPlacement(UnkPlacement_800C9CD0 *owner);

    /* Creates the switch under task layer 11 (name inferred). */
    static void Instantiate(UnkPlacement_800C9CD0 *owner);

    /* Method names are inferred. */
    /* Copies the owner's position, rotation, parameters and switch id, and resets the state. */
    void ReadPlacement(UnkPlacement_800C9CD0 *owner);
    /* Places the model at the character info. */
    void UpdateMatrix();
    /* The attack collision's callback: flips the switch. */
    void OnAttack(AttackCallbackParam &param);

    static const char *s_fileName;
    static const char *s_taskName;

    /* Member names are inferred. */
    /* 0x30 */ boost::shared_ptr<GadgetCharInfo> m_chara;
    /* 0x38 */ boost::shared_ptr<GadgetModelInfo> m_model;
    /* 0x40 */ float m_soundTimer;
    /* 0x44 */ float m_hitCooldown;
    /* 0x48 */ s32 m_switchId;
    /* 0x4C */ s32 m_on;
    /* 0x50 */ SetRailSwitchParams m_params;
    /* 0x54 */ UnkMaterialAnim_801621F0 *m_material;
    /* 0x58 */ SetRailSwitchFlags m_flags;
    /* 0x5C */ u32 pad_0x5C; /* the class size is a multiple of 8 */
};
SIZE_ASSERT(SetRailSwitch, 0x60);

const char *SetRailSwitch::s_fileName = "RAILSWITCH.BIN";
const char *SetRailSwitch::s_taskName = "SetRailSwitch";

SetRailSwitch::SetRailSwitch(Task *parent, UnkPlacement_800C9CD0 *owner) : Gadget(parent, owner) {
    m_name = s_taskName;
    m_chara = fn_8015CD90(0x82, 4);
    ReadPlacement(owner);
    m_model = fn_8015F800(1, s_fileName);
    m_model->fn_80162A68(0, 0);
    m_model->fn_80160C28(0);
    m_model->fn_8015FF60();
    fn_80161BD8(m_model.get(), 0, m_chara, 0);
    boost::weak_ptr<CharaColliAttack> attack = fn_80161428(m_model.get(), 0);
    if (boost::shared_ptr<CharaColliAttack> collision = attack.lock()) {
        collision->field_0x54 = 0;
        collision->field_0x55 = 1;
        collision->m_attackCallback =
            AttackCallback(boost::bind(&SetRailSwitch::OnAttack, this, _1));
    }
    UpdateMatrix();
    fn_8016239C(m_model.get(), "mat");
    m_material = fn_801621F0(m_model.get(), "mat");
    if (lbl_8054B8E0[m_params.route].m_alternative == 1) {
        fn_80160A84(m_model.get(), 0, 2);
        m_material->color = s_colors[1];
        m_flags.Set(1);
        m_on = 1;
    } else {
        m_material->color = s_colors[0];
        m_flags.Clear(1);
        m_on = 0;
    }
}

SetRailSwitch::~SetRailSwitch() {}

void SetRailSwitch::Behave(float dt) {
    if (m_flags.Test(0)) {
        m_soundTimer -= dt;
        if (m_soundTimer <= 0.0f) {
            fn_801D54F4(fn_80053164(), 0x7836, m_chara.get(), 0, 1);
            m_flags.Clear(0);
        }
    }
    if (m_flags.Test(1)) {
        fn_801D53C4(fn_80053164(), 0x7837, m_chara.get(), this, 3, 0, 2);
    }
    m_hitCooldown -= dt;
    if (m_hitCooldown < 0.0f) {
        m_hitCooldown = 0.0f;
    }
    if (m_model.get() != 0) {
        m_model->fn_80162FB0(dt);
    }
    UpdateMatrix();
}

void SetRailSwitch::ReloadPlacement(UnkPlacement_800C9CD0 *owner) {
    ReadPlacement(owner);
    UpdateMatrix();
    if (m_model.get() != 0) {
        m_model->fn_80162EF4();
    }
}

void SetRailSwitch::ReadPlacement(UnkPlacement_800C9CD0 *owner) {
    fn_800C9F14(owner, &m_chara->m_position);
    fn_800C9ED4(owner, &m_chara->m_rotation);
    fn_800C9DBC(owner, &m_params, sizeof(m_params));
    fn_800C9E50(owner, &m_switchId);
    m_on = 0;
    m_hitCooldown = 0.0f;
}

void SetRailSwitch::UpdateMatrix() {
    if (m_model.get() != 0) {
        Mtx34f matrix;
        fn_8015CEA0(m_chara.get(), &matrix);
        m_model->fn_801633DC(&matrix);
    }
}

void SetRailSwitch::OnAttack(AttackCallbackParam &param) {
    param.m_result = 5;
    if (m_hitCooldown > 0.0f) {
        return;
    }
    if (boost::shared_ptr<CharaColliAttack> attack = param.m_attacker.lock()) {
        if ((attack->m_flags & 0x3D1) == 0) {
            return;
        }
        if (param.m_damage < 0.9f) {
            param.m_result = 5;
            m_hitCooldown = 1.0f;
            return;
        }
        m_on = (m_on == 0) ? 1 : 0;
        if (m_on == 1) {
            fn_80160A84(m_model.get(), 0, 2);
            m_material->color = s_colors[1];
            m_flags.Set(1);
        } else {
            fn_80160A84(m_model.get(), 0, 1);
            m_material->color = s_colors[0];
            m_flags.Clear(1);
        }
        fn_80169B80(9, m_chara.get());
        fn_801D54F4(fn_80053164(), 0x7835, m_chara.get(), 0, 1);
        boost::weak_ptr<CharInfo> *rider = fn_801DCB14(fn_800DDF20(), 0x80);
        if (rider != 0) {
            if (boost::shared_ptr<CharInfo> info = rider->lock()) {
                info->PushCommand(new SwitchCommand(m_params.route));
            }
        }
        param.m_result = 1;
        m_soundTimer = 0.2f;
        m_flags.Set(0);
        m_hitCooldown = 1.0f;
    }
}

} // namespace RailSwitch

extern "C" void fn_80257A0C(void) {
    fn_8006B474("RAILSWITCH.BIN", 1);
}

extern "C" void fn_80257A08(void) {}

void RailSwitch::SetRailSwitch::Instantiate(UnkPlacement_800C9CD0 *owner) {
    Task *parent = fn_801EB304(fn_8001C4F8(), 11);
    new SetRailSwitch(parent, owner);
}

static GadgetParamDesc lbl_80549DD4[] = {
    {1, "route", 0, 0, 0x10000, 0.0f, 0.0f, 0.0f, fn_800CBC20, 0},
    {0},
};

GadgetClassDesc lbl_80549E24 = {
    "RAIL SWITCH", fn_80257A0C, fn_80257A08, RailSwitch::SetRailSwitch::Instantiate, 0, 0, 0x13ED,
    1, 0, 10, lbl_80549DD4,
};
