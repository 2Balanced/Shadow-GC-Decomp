/* BlastBreak (class name from RTTI): the BREAK_BLAST gadget, an object shown with the
 * BLASTBREAK.BIN model that only an attack whose flags have bit 4 set (inferred: a blast) can break. It
 * registers OnAttack as the attack callback of the model's "col" collision; once broken it gives
 * the attacker reward 0x17 and finishes. File name inferred from the class. Names marked "(name
 * inferred)" are not original. Definitions are in reverse address order (deferred inlining emits
 * functions last-defined first). */
#include "Game/CharInfo.h"
#include "Game/CharaColliAttack.h"
#include "Game/Gadget.h"
#include "Game/GadgetCharInfo.h"
#include "Game/GadgetModelInfo.h"
#include "Game/GadgetClassDesc.h"
#include "Game/FileQueue.h"
#include "boost/bind.hpp"
#include "boost/function.hpp"
#include "boost/weak_ptr.hpp"
#include "size_assert.h"

using SonicteamUSA::System::CharaColli::AttackCallbackParam;
using SonicteamUSA::System::CharaColli::AttackCallback;
using SonicteamUSA::System::CharaColli::CharaColliAttack;

extern "C" {
/* Loads the model fileName (behaviour inferred; arg0 unknown). */
boost::shared_ptr<GadgetModelInfo> fn_8015F800(int arg0, const char *fileName);
/* Returns the attack of the model's collision named nodeName (behaviour inferred). */
boost::weak_ptr<CharaColliAttack> fn_80161334(GadgetModelInfo *model, const char *nodeName);
/* Builds the matrix of the character info's position and rotation (behaviour inferred). */
void fn_8015CEA0(GadgetCharInfo *charInfo, Mtx34f *matrix);
/* Gives the attacker the reward for breaking the object (behaviour inferred). */
void fn_8020642C(const boost::shared_ptr<CharInfo> &attacker, int reward);
/* Tells the owner that the gadget is done (behaviour inferred). */
void fn_8015A794(Gadget *gadget);
}

class BlastBreak : public Gadget {
  public:
    BlastBreak(Task *parent, UnkPlacement_800C9CD0 *owner);
    virtual ~BlastBreak();
    virtual void Behave(float dt);
    virtual void ReloadPlacement(UnkPlacement_800C9CD0 *owner);

    /* Creates the gadget under task layer 11 (name inferred). */
    static void Instantiate(UnkPlacement_800C9CD0 *owner);

    /* Copies the owner's position and rotation and restarts the gadget (name inferred). */
    void ReadPlacement(UnkPlacement_800C9CD0 *owner);
    /* Places the model at the character info's matrix (name inferred). */
    void RebuildTransform();
    /* The attack callback of the model's collision (name inferred). */
    void OnAttack(AttackCallbackParam &param);

    static const char *s_taskName;

    /* Member names are inferred. */
    /* 0x30 */ boost::shared_ptr<GadgetCharInfo> m_chara;
    /* 0x38 */ boost::shared_ptr<GadgetModelInfo> m_model;
    /* 0x40 */ s32 m_state; /* 0 new, 1 waiting for an attack, 2 broken */
    /* 0x44 */ u32 pad_0x44; /* the class size is a multiple of 8 */
};
SIZE_ASSERT(BlastBreak, 0x48);

const char *BlastBreak::s_taskName = "BlastBreak";

BlastBreak::BlastBreak(Task *parent, UnkPlacement_800C9CD0 *owner) : Gadget(parent, owner) {
    m_name = s_taskName;
    m_chara = fn_8015CB88(4, 4, "BlastBreak");
    m_model = fn_8015F800(0, "BLASTBREAK.BIN");
    m_model->fn_80162A68(0, 0);
    if (m_model->fn_80161F08("col", m_chara)) {
        boost::weak_ptr<CharaColliAttack> attack = fn_80161334(m_model.get(), "col");
        if (boost::shared_ptr<CharaColliAttack> collision = attack.lock()) {
            collision->field_0x54 = 0;
            collision->field_0x55 = 1;
            collision->m_attackCallback =
                AttackCallback(boost::bind(&BlastBreak::OnAttack, this, _1));
        }
    }
    ReadPlacement(owner);
    RebuildTransform();
}

BlastBreak::~BlastBreak() {}

void BlastBreak::Behave(float dt) {
    if (m_model.get() != 0) {
        m_model->fn_80162FB0(dt);
    }

    switch (m_state) {
    case 0:
        m_state = 1;
        break;
    case 1:
        break;
    case 2:
        fn_8015A794(this);
        m_flags |= 1;
        break;
    }
}

void BlastBreak::ReloadPlacement(UnkPlacement_800C9CD0 *owner) {
    ReadPlacement(owner);
    RebuildTransform();
    m_model->fn_8016108C();
}

void BlastBreak::ReadPlacement(UnkPlacement_800C9CD0 *owner) {
    fn_800C9F14(owner, &m_chara->m_position);
    fn_800C9ED4(owner, &m_chara->m_rotation);
    m_state = 0;
}

void BlastBreak::RebuildTransform() {
    if (m_model.get() != 0) {
        Mtx34f matrix;

        fn_8015CEA0(m_chara.get(), &matrix);
        m_model->fn_801633DC(&matrix);
    }
}

void BlastBreak::OnAttack(AttackCallbackParam &param) {
    if (m_state == 1) {
        if (boost::shared_ptr<CharaColliAttack> collision = param.m_attacker.lock()) {
            if (!(collision->m_flags & 0x10)) {
                param.m_result = 5;
                return;
            }
            if (boost::shared_ptr<CharInfo> attacker = collision->m_owner.lock()) {
                fn_8020642C(attacker, 0x17);
            }
            param.m_result = 3;
            m_state = 2;
            return;
        }
    }
    param.m_result = 5;
}

extern "C" void fn_800EC99C(void) {
    fn_8006B474("BLASTBREAK.BIN", 1);
}

extern "C" void fn_800EC998(void) {}

void BlastBreak::Instantiate(UnkPlacement_800C9CD0 *owner) {
    new BlastBreak(fn_801EB304(fn_8001C4F8(), 11), owner);
}

GadgetClassDesc lbl_805249F4 = {
    "BREAK_BLAST", fn_800EC99C, fn_800EC998, BlastBreak::Instantiate, 0, 0, 0x1C, 1, 0, 10, 0,
};
