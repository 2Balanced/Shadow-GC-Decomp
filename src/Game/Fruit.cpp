/* Fruit::FruitTask (class name from RTTI): the Fruit gadget, a piece of fruit shown with model
 * 0xD3 and particles on its parts. It registers OnHit and OnAttack with the attack collision of
 * model variant 0; once an attack that may break it hits it, the attacker gets message 0x16, the
 * fruit bursts into debris with a sound, and the model changes to variant 2000. If that variant
 * cannot be shown the gadget finishes instead. File name inferred from the namespace. Method names
 * marked "(name inferred)" are not original. Definitions are in reverse address order (deferred
 * inlining emits functions last-defined first). */
#include <boost/bind.hpp>
#include <boost/function.hpp>

#include "Game/TNumEdit.h"
/* After TNumEdit.h: the order of the casts sets the order of the RTTI base lists in .data. */
#include "Game/CharaColliBaseCharInfo.h"
#include "Game/CharInfo.h"
#include "Game/CharaColliAttack.h"
#include "Game/Gadget.h"
#include "Game/GadgetCharInfo.h"
#include "Game/GadgetDebris.h"
#include "Game/GadgetModelInfo.h"
#include "Game/GadgetParticle.h"
#include "Game/GadgetClassDesc.h"
#include "Game/GadgetUtil.h"
#include "boost/shared_ptr.hpp"
#include "boost/weak_ptr.hpp"
#include "size_assert.h"

using SonicteamUSA::System::CharaColli::AttackCallback;
using SonicteamUSA::System::CharaColli::AttackCallbackParam;
using SonicteamUSA::System::CharaColli::CharaColliAttack;

/* A part of a model looked up by id (only the name is known). */
struct UnkModelPart_8015F7BC {
    /* 0x0 */ u32 field_0x0;
    /* 0x4 */ const char *name; /* the part's model file */
};

extern "C" {
/* GadgetModelInfo methods that are not declared in its header yet (behaviour inferred):
 * fn_80162EA8 reports whether the model can change to a variant, fn_80161FC8 attaches a character
 * info to the collision of a model variant, fn_80161428 returns that collision's attack and
 * fn_8015F7BC returns the part with the given id. */
bool fn_80162EA8(GadgetModelInfo *model, int variant);
bool fn_80161FC8(GadgetModelInfo *model, int variant, const boost::weak_ptr<CharInfo> &info);
boost::weak_ptr<CharaColliAttack> fn_80161428(GadgetModelInfo *model, int variant);
UnkModelPart_8015F7BC *fn_8015F7BC(GadgetModelInfo *model, int id);

/* Returns the model manager, and the id of a model file in it (behaviour inferred). */
void *fn_800122D4(void);
s32 fn_80012700(void *manager, int arg1, const char *fileName);

/* Particle controller helpers: a new controller, the callback that attaches it to a model part,
 * the walk over a model's parts, and the start of its particles (behaviour inferred). */
boost::shared_ptr<GadgetParticle::GadgetParticleCtrl> fn_80166AF4(void);
void *fn_80169D00(void *part, UnkParticleAttach_80169D00 *param);
void fn_8015FA78(GadgetModelInfo *model,
                 void *(*callback)(void *part, UnkParticleAttach_80169D00 *param),
                 UnkParticleAttach_80169D00 *param);
void fn_80167180(GadgetParticle::GadgetParticleCtrl *ctrl, int id);

/* Plays sound id at a character info (behaviour inferred). */
void fn_801D54F4(void *sound, int id, GadgetCharInfo *source, int arg3, int arg4);

/* Sends character a message (behaviour inferred). */
int fn_8020642C(const boost::shared_ptr<CharInfo> &character, int message);
/* Tells the owner that the gadget is done (behaviour inferred). */
void fn_800C9B34(UnkPlacement_800C9CD0 *owner);
}

namespace Fruit {

class FruitTask : public Gadget {
  public:
    FruitTask(Task *parent, UnkPlacement_800C9CD0 *owner);
    virtual ~FruitTask();
    virtual void Behave(float dt);
    virtual void ReloadPlacement(UnkPlacement_800C9CD0 *owner);

    /* Creates the fruit under task layer 11 (name inferred). */
    static void Instantiate(UnkPlacement_800C9CD0 *owner);

    /* Copies the owner's position and rotation, and resets the state (name inferred). */
    void LoadFromOwner(UnkPlacement_800C9CD0 *owner);
    /* Places the model at position with rotation (name inferred). */
    void RebuildTransform(const Vec3f *position, const Vec3f *rotation);
    /* The attack collision's two callbacks (names inferred). */
    void OnAttack(AttackCallbackParam &param);
    void OnHit(AttackCallbackParam &param);
    /* Spawns the debris of the fruit with its sound (name inferred). */
    void Break();
    /* Changes the model to variant 2000; false when it cannot (name inferred). */
    bool ChangeModel();

    /* Member names are inferred. */
    /* 0x30 */ boost::shared_ptr<GadgetCharInfo> m_chara;
    /* 0x38 */ boost::shared_ptr<GadgetModelInfo> m_model;
    /* 0x40 */ boost::shared_ptr<GadgetParticle::GadgetParticleCtrl> m_particles;
    /* 0x48 */ s32 m_state; /* 0 new, 1 whole, 2 hit, 3 eaten (meanings inferred) */
    /* 0x4C */ u32 pad_0x4C; /* the class size is a multiple of 8 */
};

} // namespace Fruit

SIZE_ASSERT(Fruit::FruitTask, 0x50);

namespace Fruit {

FruitTask::FruitTask(Task *parent, UnkPlacement_800C9CD0 *owner) : Gadget(parent, owner) {
    m_chara = fn_8015CB88(0x75, 4, "Fruit");
    m_model = fn_8015F9D8(0, 0xD3);
    m_model->fn_80162A68(0, 0);
    if (fn_80161FC8(m_model.get(), 0, m_chara)) {
        boost::weak_ptr<CharaColliAttack> attack = fn_80161428(m_model.get(), 0);
        if (boost::shared_ptr<CharaColliAttack> target = attack.lock()) {
            target->field_0x54 = 1;
            target->m_kind = 1;
            target->m_power = 1.0f;
            target->m_hitCallback = AttackCallback(boost::bind(&FruitTask::OnHit, this, _1));
            target->field_0x55 = 1;
            target->m_attackCallback = AttackCallback(boost::bind(&FruitTask::OnAttack, this, _1));
        }
    }
    LoadFromOwner(owner);
    RebuildTransform(&m_chara->m_position, &m_chara->m_rotation);
}

FruitTask::~FruitTask() {}

void FruitTask::Behave(float dt) {
    if (m_model.get() != 0) {
        m_model->fn_80162FB0(dt);
    }

    switch (m_state) {
    case 0:
        m_state = 1;
        if (m_model.get() != 0) {
            m_particles = fn_80166AF4();
            UnkParticleAttach_80169D00 attach;
            attach.ctrl = m_particles;
            fn_8015FA78(m_model.get(), fn_80169D00, &attach);
        }
        if (m_particles.get() != 0) {
            fn_80167180(m_particles.get(), -1);
        }
        break;
    case 1:
        break;
    case 2:
        Break();
        if (m_particles.get() != 0) {
            m_particles.reset();
        }
        if (ChangeModel()) {
            m_state = 3;
        } else {
            if (m_owner != 0) {
                fn_800C9B34(m_owner);
            }
            m_flags |= 1;
        }
        break;
    case 3: /* its empty case shapes the original's compare sequence */
        break;
    }
}

void FruitTask::ReloadPlacement(UnkPlacement_800C9CD0 *owner) {
    LoadFromOwner(owner);
    RebuildTransform(&m_chara->m_position, &m_chara->m_rotation);
    if (m_model.get() != 0) {
        m_model->fn_8016108C();
    }
}

void FruitTask::LoadFromOwner(UnkPlacement_800C9CD0 *owner) {
    if (m_chara.get() != 0) {
        fn_800C9F14(owner, &m_chara->m_position);
        fn_800C9ED4(owner, &m_chara->m_rotation);
    }
    m_state = 0;
}

void FruitTask::RebuildTransform(const Vec3f *position, const Vec3f *rotation) {
    if (m_model.get() != 0) {
        Mtx34f matrix;

        fn_8040B6F8(&matrix);
        fn_8040B59C(&matrix, position);
        fn_8040B848(&matrix, rotation->y);
        fn_8040B884(&matrix, rotation->x);
        fn_8040B80C(&matrix, rotation->z);
        m_model->fn_801633DC(&matrix);
    }
}

void FruitTask::OnAttack(AttackCallbackParam &param) {
    if (m_state == 1) {
        if (boost::shared_ptr<CharaColliAttack> attacker = param.m_attacker.lock()) {
            if (attacker->m_flags & 2) {
                param.m_result = 5;
                return;
            }
            if (boost::shared_ptr<CharInfo> owner = attacker->m_owner.lock()) {
                fn_8020642C(owner, 0x16);
            }
            param.m_result = 3;
            m_state = 2;
            return;
        }
    }
    param.m_result = 5;
}

void FruitTask::OnHit(AttackCallbackParam &param) {
    if (m_state == 1) {
        m_state = 2;
    }
}

void FruitTask::Break() {
    if (m_model.get() != 0) {
        UnkModelPart_8015F7BC *part = fn_8015F7BC(m_model.get(), 1000);
        if (part != 0) {
            void *manager = fn_800122D4();
            UnkDebrisParam_8015F0B8 debris(m_chara, 0);

            debris.scale = 1.3f;
            debris.model = fn_80012700(manager, 0, part->name);
            debris.field_0x54 = 1;
            debris.field_0x4C = 0x96;
            fn_8015D93C(&debris);
            fn_801D54F4(fn_80053164(), 0x1022, m_chara.get(), 0, 1);
        }
    }
}

bool FruitTask::ChangeModel() {
    if (fn_80162EA8(m_model.get(), 2000)) {
        m_model->fn_80161880();
        m_model->fn_80162804();
        m_model->fn_80162A68(2000, 0);
        if (fn_80161FC8(m_model.get(), 2000, m_chara)) {
            boost::weak_ptr<CharaColliAttack> attack = fn_80161428(m_model.get(), 2000);
            if (boost::shared_ptr<CharaColliAttack> target = attack.lock()) {
                target->field_0x54 = 0;
                target->field_0x55 = 1;
                target->m_attackCallback =
                    AttackCallback(boost::bind(&FruitTask::OnAttack, this, _1));
            }
        }
        RebuildTransform(&m_chara->m_position, &m_chara->m_rotation);
        return true;
    }
    return false;
}

} // namespace Fruit

extern "C" void fn_80105368(void) {
    fn_8006B50C(0xD3, 1);
}

extern "C" void fn_80105364(void) {}

void Fruit::FruitTask::Instantiate(UnkPlacement_800C9CD0 *owner) {
    Task *parent = fn_801EB304(fn_8001C4F8(), 11);
    new FruitTask(parent, owner);
}

GadgetClassDesc lbl_805260A4 = {
    "Fruit", fn_80105368, fn_80105364, Fruit::FruitTask::Instantiate, 0, 0, 0x22, 1, 0, 10, 0,
};
