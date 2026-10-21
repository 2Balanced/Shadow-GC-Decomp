/* EggLastMech::SlotSwitch (class name from the RTTI of its attack callback's bind_t; declared in
 * Game/EggLastMechSlotSwitch.h): a reel of EggLastMech's slot machine (Game/EggLastMechSlot.cpp).
 * Each reel is a model placed on one of the boss's motion nodes, with a sphere collider and a
 * target; it turns while it spins, and hitting it stops it (the slot machine picks the symbol). A
 * stopping reel turns on until its symbol comes round, then settles with a small swing. The file
 * name and the method names are inferred. Definitions are in reverse address order (deferred
 * inlining emits functions last-defined first). */
#include <boost/bind.hpp>
#include <boost/function.hpp>
#include <cmath>

#include "Effect/System/EffectElement.h"
#include "Game/CharInfo.h"
#include "Game/CharaColli.h"
#include "Game/CharaColliAttack.h"
#include "Game/EggLastMech.h"
#include "Game/EggLastMechSlotSwitch.h"
#include "Game/EnemyCharInfo.h"
#include "Game/EnemyHint.h"
#include "Game/EnemyRender.h"
#include "Game/MathConstants.h"
#include "Game/Random.h"
#include "Game/MathTypes.h"
#include "boost/shared_ptr.hpp"
#include "boost/weak_ptr.hpp"
#include "size_assert.h"

using SonicteamUSA::System::CharaColli::AttackCallback;
using SonicteamUSA::System::CharaColli::AttackCallbackParam;
using SonicteamUSA::System::CharaColli::CharaColliAttack;
using SonicteamUSA::System::CharaColli::CharaColliBase;

/* Target (class name from RTTI): a point other objects can aim at (see DevilDoomCtrlUnit.cpp). */
class Target {
  public:
    void TurnOn() {
        m_flags |= 1;
    }
    void TurnOff() {
        m_flags &= ~1;
    }
    bool IsEnabled() {
        return m_flags & 1;
    }
    void SetField24(float value) {
        field_0x24 = value;
    }
    /* Moves the target to position (name inferred). */
    void fn_800CCE38(const Vec3f *position);

    /* 0x00 */ u8 field_0x0[0x24];
    /* 0x24 */ float field_0x24;
    /* 0x28 */ u8 field_0x28[0x30 - 0x28];
    /* 0x30 */ u32 m_flags;
};

float DegToRad(float degrees);

/* The empty character info (an .sbss shared_ptr; see BlockChange.cpp). */
extern boost::shared_ptr<CharInfo> lbl_805EE698;

extern "C" {
/* The collider manager, a sphere collider, and the attack collision of a collider (see
 * BlockChange.cpp and EggLastMech.cpp). */
void *fn_80021B08(void);
boost::shared_ptr<CharaColliBase> fn_800C7EA8(void *manager, const Vec3f *position, u32 flags,
                                              int arg3, int arg4, const boost::weak_ptr<CharInfo> &owner,
                                              const Vec3f *offset, float radius);
boost::weak_ptr<CharaColliAttack> fn_800C4374(void *manager, boost::shared_ptr<CharaColliBase> collider);
void fn_80096B64(CharaColliAttack *attack, int arg1, const AttackCallback &callback);
/* Creates a target with the given flags (see CircusBall.cpp), and points it along direction
 * (behaviour inferred). */
boost::shared_ptr<Target> fn_801E72DC(int flags);
void fn_801E73B0(Target *target, const Vec3f *direction);

/* The resource loader, and the model, motion and animation files of a resource slot (behaviour
 * inferred, see EggLastMech.cpp). */
void *fn_800122D4(void);
void *fn_80012684(void *loader, int slot, int index);
void *fn_80012608(void *loader, int slot, int index);
void *fn_8001277C(void *loader, int slot, int index);
/* Sets an animation's frame (behaviour inferred). */
void fn_80039B94(void *animation, float arg1, float frame);

/* EnemyRender's methods (behaviour inferred, see EggLastMech.cpp). */
boost::shared_ptr<EnemyRender> fn_802CDB08(int slot, const EnemyMotionTable &table, void *model,
                                           void *motion);
void fn_802CD638(EnemyRender *render, float scale);
void fn_802CD68C(EnemyRender *render, int arg1, float arg2);
void fn_802CD6F0(EnemyRender *render, void *node);
void fn_802CD824(EnemyRender *render, float dt);
void fn_802CD7E8(EnemyRender *render);
Mtx34f *fn_802CD560(EnemyRender *render, int bone);

/* A node of a motion (behaviour inferred). */
void *fn_801A46E4(void *motion, int node);
/* Launches effect id on node (behaviour inferred). */
boost::weak_ptr<Effect::System::EffectElement> fn_801D2C28(int arg0, void *node, int id, void *arg3);

/* The boss's sounds (behaviour inferred). */
void fn_80336B1C(EggLastMech::UnkSound_80336F48 *sound, int id);
void fn_80336B58(EggLastMech::UnkSound_80336F48 *sound, int id);
/* Stops the slot machine's reel on the next symbol (Game/EggLastMechSlot.cpp). */
void fn_80313C98(EggLastMech::UnkSlot_803142C0 *slot, int reel);

/* Vector helpers (behaviour inferred): the translation of a matrix, matrix * point, and a x b. */
Vec3f fn_8040FB08(const Mtx34f *matrix);
Vec3f fn_8040B618(const Mtx34f *matrix, const Vec3f *point);
Vec3f fn_8040F364(const Vec3f *a, const Vec3f *b);
}

/* The object a character info keeps at 0x60 (struct name unknown); effects are launched with the
 * address of its member at 0x4 (meaning unknown; see EggLastMech.cpp's fn_801D2AFC call). */
struct UnkCharData_60 {
    /* 0x0 */ u32 field_0x0;
    /* 0x4 */ u8 field_0x4;
};

/* The object at 0x60 of a character info (name inferred). */
inline UnkCharData_60 *GetEffectData(CharInfo *info) {
    return *reinterpret_cast<UnkCharData_60 **>(&info->field_0x60);
}

/* A motion table built in place (not original; it gives retail's stack layout). */
struct MotionTable : public EnemyMotionTable {
    MotionTable(const EnemyMotionEntry *e, s32 c) {
        entries = e;
        count = c;
    }
};
SIZE_ASSERT(EggLastMech::SlotSwitch, 0x44);

/* The reels' model (values from .sdata2). */
static const EnemyMotionEntry s_motions[1] = {{0xD, -1}};
/* Each reel's animation (values from .rodata). */
static const s32 s_animations[3] = {6, 4, 8};

/* The speed of a settling reel's swing (name inferred). */
static const float s_swingSpeed = 200.0f * lbl_805FAD38;

namespace EggLastMech {

SlotSwitch::SlotSwitch(EggLastMech *mech, int index) : m_mech(mech), m_index(index), m_node(0x13DA - m_index) {
    m_angle = fn_8040EC80(0.0f, 0.8333333f);
    m_spinning = 1;
    m_state = 0;
    void *loader = fn_800122D4();
    void *model = fn_80012684(loader, 0x16, 0x10);
    void *motion = fn_80012608(loader, 0x16, 0x19);
    m_render = fn_802CDB08(0x16, MotionTable(s_motions, 1), model, motion);
    fn_802CD638(m_render.get(), 1.0f);
    m_motion = fn_8001277C(loader, 0x16, s_animations[m_index]);
    m_target = fn_801E72DC(0x400);
    m_target->SetField24(std::cos(DegToRad(80.0f)));
    m_target->TurnOn();
    m_collider = fn_800C7EA8(fn_80021B08(), &lbl_805E07A0, 0, 4, 0, lbl_805EE698,
                             &Vec3f(0.0f, 0.0f, 0.0f), 6.0f);
    if (m_collider) {
        boost::weak_ptr<CharaColliAttack> attack = fn_800C4374(fn_80021B08(), m_collider);
        if (boost::shared_ptr<CharaColliAttack> attackPtr = attack.lock()) {
            fn_80096B64(attackPtr.get(), 1, boost::bind(&SlotSwitch::OnAttack, this, _1));
        }
    }
    m_effect = fn_801D2C28(3, fn_801A46E4(m_mech->GetMotion(), m_node), 7, &GetEffectData(m_mech)->field_0x4);
}

SlotSwitch::~SlotSwitch() {
    if (boost::shared_ptr<Effect::System::EffectElement> effect = m_effect.lock()) {
        effect->SetMatrix(0);
        effect->Stop();
        m_effect.reset();
    }
}

void SlotSwitch::Tick() {
    int before = m_angle / 0.16666667f;
    m_angle += (1.0f + (0.03f * m_index + 0.03f)) * fn_8019FFE0(m_mech);
    if (m_angle > 0.8333333f) {
        m_angle -= 0.8333333f;
    }
    int after = m_angle / 0.16666667f;
    switch (m_state) {
    case 0:
        m_state = 1;
        break;
    case 2:
        if (after != before && after == m_symbol) {
            fn_80336B1C(m_mech->m_sound, 0x7F32);
            m_timer = 0.2f;
            m_state = 3;
        }
        break;
    case 3:
        if ((m_timer -= fn_8019FFE0(m_mech)) < 0.0f) {
            m_timer = 0.0f;
            m_state = 4;
        }
        m_angle = 0.16666667f * m_symbol;
        m_angle += m_timer / 0.2f * (0.008333334f * std::sin(s_swingSpeed * m_timer));
        if (m_angle > 0.8333333f) {
            m_angle -= 0.8333333f;
        }
        break;
    case 4:
        m_angle = 0.16666667f * m_symbol;
        break;
    }
    fn_80039B94(m_motion, 0.0f, m_angle);
    /* Retail reads the render's pointer after the node call, as a plain member read: get() would be
     * evaluated first. */
    fn_802CD6F0(m_render.px, fn_801A46E4(m_mech->GetMotion(), m_node));
    fn_802CD824(m_render.get(), fn_8019FFE0(m_mech));
    fn_802CD7E8(m_render.get());
    Mtx34f *matrix = fn_802CD560(m_render.get(), 0x4B0);
    Vec3f position;
    fn_800091EC(&position, &(const Vec3f &)fn_8040FB08(matrix));
    Vec3f up;
    fn_800091EC(&up, &(const Vec3f &)fn_8040B618(matrix, &Vec3f(0.0f, 1.0f, 0.0f)));
    Vec3f forward;
    fn_800091EC(&forward, &(const Vec3f &)fn_8040B618(matrix, &Vec3f(0.0f, 0.0f, -1.0f)));
    Vec3f side = fn_8040F364(&forward, &up);
    m_collider->SetCharInfo(reinterpret_cast<CharInfo *>(&position));
    m_collider->SetRotation(&side);
    if (m_target->IsEnabled()) {
        m_target.get()->fn_800CCE38(&position);
        fn_801E73B0(m_target.get(), &forward);
    }
}

void SlotSwitch::Stop(int symbol) {
    m_spinning = 0;
    m_symbol = symbol;
    m_target->TurnOff();
    m_collider->TurnOff(-1);
    m_state = 2;
    if (boost::shared_ptr<Effect::System::EffectElement> effect = m_effect.lock()) {
        effect->SetMatrix(0);
        effect->Stop();
        m_effect.reset();
        m_effect = fn_801D2C28(3, fn_801A46E4(m_mech->GetMotion(), m_node), 0xA, &GetEffectData(m_mech)->field_0x4);
    }
}

void SlotSwitch::Start() {
    m_spinning = 1;
    if (boost::shared_ptr<Effect::System::EffectElement> effect = m_effect.lock()) {
        effect->SetMatrix(0);
        effect->Stop();
        m_effect.reset();
        m_effect = fn_801D2C28(3, fn_801A46E4(m_mech->GetMotion(), m_node), 7, &GetEffectData(m_mech)->field_0x4);
    }
    m_target->TurnOn();
    m_collider->TurnOn(-1);
    m_state = 0;
}

void SlotSwitch::OnAttack(AttackCallbackParam &param) {
    boost::shared_ptr<CharaColliAttack> attacker = param.m_attacker.lock();
    if (!(attacker->m_flags & 8)) {
        param.m_result = 5;
        return;
    }
    param.m_result = 1;
    fn_802CD68C(m_render.get(), 0x10, 0.0f);
    fn_80336B58(m_mech->m_sound, 0x7F33);
    fn_80313C98(m_mech->m_slot, m_index);
    m_mech->OnDamage(param);
    if (m_mech->GetSlot()->GetSpinMask()) {
        static s32 s_hint = 0;
        /* Alternates between two hints (meaning inferred). */
        int hint = 0xC;
        if (s_hint ^= 1) {
            hint = 0xB;
        }
        EnemyHint::ShowBossHint(hint);
    }
}

} // namespace EggLastMech
