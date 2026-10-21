/* BreakObj (class name from RTTI): the BRK_OBJ gadget, a breakable object shown with the model of
 * its "kind" from BREAKOBJ.BIN. It registers OnAttack as the attack callback of the model's
 * collision; once hit by an attack that may break it, it gives the attacker its reward, spawns the
 * kind's debris with a sound and an effect, and finishes. File name inferred from the class. Names
 * marked "(name inferred)" are not original. Definitions are in reverse address order (deferred
 * inlining emits functions last-defined first). */
#include "Effect/System/EffectElement.h"
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
#include <cstring>
#include "size_assert.h"

class UnkSound_80053164;

using SonicteamUSA::System::CharaColli::AttackCallback;
using SonicteamUSA::System::CharaColli::AttackCallbackParam;
using SonicteamUSA::System::CharaColli::CharaColliAttack;

/* A part of a model looked up by id (only the name is known). */
struct UnkModelPart_8015F7BC {
    /* 0x0 */ u32 field_0x0;
    /* 0x4 */ const char *name; /* the part's model file */
};

/* The parameters of a debris spawn (layout from its constructor at 0x8015F0B8 and
 * BreakObj::Break; names inferred). */
struct UnkDebrisParam_8015F0B8 {
    /* Sets up the parameters at a character info's place from template index (behaviour
     * inferred). */
    UnkDebrisParam_8015F0B8(const boost::shared_ptr<CharInfo> &info, int index);

    /* 0x00 */ u8 field_0x0[0x34];
    /* 0x34 */ float scale;
    /* 0x38 */ u8 field_0x38[0x44 - 0x38];
    /* 0x44 */ s32 model;
    /* 0x48 */ u8 field_0x48[0x5C - 0x48];
};
SIZE_ASSERT(UnkDebrisParam_8015F0B8, 0x5C);

extern "C" {
/* GadgetModelInfo methods that are not declared in its header yet (behaviour inferred):
 * fn_80162EA8 reports whether the model can change, fn_80161FC8 attaches a character info to the
 * collision of a model variant, fn_80161428 returns that collision's attack and fn_8015F7BC
 * returns the part with the given id. */
bool fn_80162EA8(GadgetModelInfo *model);
bool fn_80161FC8(GadgetModelInfo *model, int variant, const boost::weak_ptr<CharInfo> &info);
boost::weak_ptr<CharaColliAttack> fn_80161428(GadgetModelInfo *model, int variant);
UnkModelPart_8015F7BC *fn_8015F7BC(GadgetModelInfo *model, int id);

/* Builds the character info of a gadget (behaviour inferred; arguments unknown). */
boost::shared_ptr<GadgetCharInfo> fn_8015CD90(int arg0, int arg1);
/* Spawns debris (behaviour inferred). */
void fn_8015D93C(UnkDebrisParam_8015F0B8 *param);
/* Returns the model manager, and the id of a model file in it (behaviour inferred). */
void *fn_800122D4(void);
s32 fn_80012700(void *manager, int arg1, const char *fileName);

/* Launches effect id at position (behaviour inferred). */
boost::weak_ptr<Effect::System::EffectElement> fn_8015D794(const Vec3f *position, int id);
/* Returns the sound manager and plays sound id at a character info (behaviour inferred). */
UnkSound_80053164 *fn_80053164(void);
void fn_801D54F4(UnkSound_80053164 *sound, int id, GadgetCharInfo *source, int arg3, int arg4);

/* Gives the attacker the reward for breaking the object (behaviour inferred). */
void fn_8020642C(const boost::shared_ptr<CharInfo> &attacker, int reward);
/* Tells the owner that the gadget is done (behaviour inferred). */
void fn_800C9B34(UnkPlacement_800C9CD0 *owner);
}

class BreakObj : public Gadget {
  public:
    BreakObj(Task *parent, UnkPlacement_800C9CD0 *owner);
    virtual ~BreakObj();
    virtual void Behave(float dt);
    virtual void ReloadPlacement(UnkPlacement_800C9CD0 *owner);

    /* Creates the gadget under task layer 11 (name inferred). */
    static void Instantiate(UnkPlacement_800C9CD0 *owner);

    /* Copies the owner's position, rotation and kind into the gadget (name inferred). */
    void ReadPlacement(UnkPlacement_800C9CD0 *owner);
    /* Places the model at the character info's translation and Y rotation (name inferred). */
    void RebuildTransform();
    /* The attack callback of the model's collision (name inferred). */
    void OnAttack(AttackCallbackParam &param);
    /* Spawns the debris of the kind with its sound and effect (name inferred). */
    void Break();

    static const char *s_taskName;

    /* Member names are inferred. */
    /* 0x30 */ s32 m_variant; /* the kind the model shows */
    /* 0x34 */ boost::shared_ptr<GadgetCharInfo> m_chara;
    /* 0x3C */ GadgetModelInfo *m_model;
    /* 0x40 */ s32 m_kind;  /* the "kind" parameter */
    /* 0x44 */ s32 m_state; /* 0 new, 1 waiting for an attack, 2 broken */
};
SIZE_ASSERT(BreakObj, 0x48);

const char *BreakObj::s_taskName = "BreakObj";

BreakObj::BreakObj(Task *parent, UnkPlacement_800C9CD0 *owner) : Gadget(parent, owner) {
    m_name = s_taskName;
    fn_800C9DBC(owner, &m_kind, sizeof(m_kind));
    m_chara = fn_8015CD90(0x11, 4);
    m_model = new GadgetModelInfo(0, "BREAKOBJ.BIN");
    m_variant = m_kind;
    m_model->fn_80162A68(m_variant, 0);
    if (fn_80161FC8(m_model, m_variant, m_chara)) {
        boost::weak_ptr<CharaColliAttack> attack = fn_80161428(m_model, m_variant);
        if (boost::shared_ptr<CharaColliAttack> collision = attack.lock()) {
            collision->field_0x54 = 0;
            collision->field_0x55 = 1;
            collision->m_attackCallback =
                AttackCallback(boost::bind(&BreakObj::OnAttack, this, _1));
        }
    }
    m_state = 0;
    ReadPlacement(m_owner);
    RebuildTransform();
}

BreakObj::~BreakObj() {
    if (m_model != 0) {
        delete m_model;
        m_model = 0;
    }
}

void BreakObj::Behave(float dt) {
    if (m_model != 0) {
        m_model->fn_80162FB0(dt);
    }

    switch (m_state) {
    case 0:
        m_state = 1;
        break;
    case 1:
        break;
    case 2:
        Break();
        fn_800C9B34(m_owner);
        m_flags |= 1;
        break;
    }
}

void BreakObj::ReloadPlacement(UnkPlacement_800C9CD0 *owner) {
    ReadPlacement(owner);
    RebuildTransform();
    if (m_model != 0) {
        if (m_kind != m_variant) {
            if (!fn_80162EA8(m_model)) {
                return;
            }
            m_variant = m_kind;
            m_model->fn_80161880();
            m_model->fn_80162804();
            m_model->fn_80162A68(m_variant, 0);
            if (fn_80161FC8(m_model, m_variant, m_chara)) {
                boost::weak_ptr<CharaColliAttack> attack = fn_80161428(m_model, m_variant);
                if (boost::shared_ptr<CharaColliAttack> collision = attack.lock()) {
                    collision->field_0x54 = 0;
                    collision->field_0x55 = 1;
                    collision->m_attackCallback =
                        AttackCallback(boost::bind(&BreakObj::OnAttack, this, _1));
                }
            }
        }
        m_model->fn_8016108C();
    }
}

void BreakObj::ReadPlacement(UnkPlacement_800C9CD0 *owner) {
    fn_800C9F14(owner, &m_chara->m_position);
    fn_800C9ED4(owner, &m_chara->m_rotation);
    fn_800C9DBC(owner, &m_kind, sizeof(m_kind));
}

void BreakObj::RebuildTransform() {
    if (m_model != 0) {
        Mtx34f matrix;

        fn_8040C148(&matrix, &lbl_805E0720);
        fn_8040B59C(&matrix, &m_chara->m_position);
        fn_8040B848(&matrix, m_chara->m_rotation.y);
        m_model->fn_801633DC(&matrix);
    }
}

void BreakObj::OnAttack(AttackCallbackParam &param) {
    if (m_state == 1) {
        if (boost::shared_ptr<CharaColliAttack> collision = param.m_attacker.lock()) {
            if (collision->m_flags & 2) {
                param.m_result = 5;
                return;
            }
            if (boost::shared_ptr<CharInfo> attacker = collision->m_owner.lock()) {
                switch (m_kind) {
                case 0:
                    fn_8020642C(attacker, 0x14);
                    break;
                case 1:
                    fn_8020642C(attacker, 0x15);
                    break;
                }
            }
            param.m_result = 3;
            m_state = 2;
            return;
        }
    }
    param.m_result = 5;
}

void BreakObj::Break() {
    if (m_model != 0) {
        UnkModelPart_8015F7BC *part = fn_8015F7BC(m_model, m_kind + 1000);
        if (part != 0) {
            UnkDebrisParam_8015F0B8 debris(m_chara, 0);

            debris.scale = 1.3f;
            debris.model = fn_80012700(fn_800122D4(), 0, part->name);
            fn_8015D93C(&debris);
            if (strcmp(part->name, "BRKOBJGUNCRSH.DFF") == 0) {
                fn_801D54F4(fn_80053164(), 0x101C, m_chara.px, 0, 1);
            } else {
                fn_801D54F4(fn_80053164(), 0x101D, m_chara.px, 0, 1);

                Vec3f position;
                fn_800091EC(&position, &m_chara->m_position);
                position.y += 10.0f;
                fn_8015D794(&position, 0xB9);
            }
        }
    }
}

extern "C" void fn_800F13A4(void) {
    fn_8006B474("BREAKOBJ.BIN", 1);
}

extern "C" void fn_800F13A0(void) {}

void BreakObj::Instantiate(UnkPlacement_800C9CD0 *owner) {
    new BreakObj(fn_801EB304(fn_8001C4F8(), 11), owner);
}

static GadgetParamDesc lbl_80524E40[] = {
    {1, "kind", 0, 0, 1, 0.0f, 0.0f, 0.0f, fn_800CBC20, 0},
    {0},
};

GadgetClassDesc lbl_80524E90 = {
    "BRK_OBJ", fn_800F13A4, fn_800F13A0, BreakObj::Instantiate, 0, 0, 0x1B, 1, 0, 10, lbl_80524E40,
};
