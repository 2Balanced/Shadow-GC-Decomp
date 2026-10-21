/* SecretKey (class SecretKey::SecretKey from RTTI; its s_taskName is "SecretKey"): the SECRETKEY
 * gadget, one of the five keys that open the secret door. It shows model 0xDE with an effect and a
 * target at the owner's position. When a character touches it, its key id is added to the stage's
 * key slots, a sound plays, the switch of that id is set and the key finishes; a key whose id is
 * already collected finishes at once. The key slots are kept here and copied to and from the save
 * data. File name inferred from the class. Names marked "(name inferred)" are not original.
 * Definitions are in reverse address order (deferred inlining emits functions last-defined
 * first). */
#include <boost/bind.hpp>
#include <boost/function.hpp>

#include "Effect/System/EffectElement.h"
#include "Game/CharInfo.h"
#include "Game/Gadget.h"
#include "Game/GadgetCharInfo.h"
#include "Game/GadgetCollision.h"
#include "Game/GadgetModelInfo.h"
#include "Game/GadgetClassDesc.h"
#include "Game/GadgetUtil.h"
#include "Game/UnkSwitchBank_800CB69C.h"
#include "boost/shared_ptr.hpp"
#include "boost/weak_ptr.hpp"
#include "size_assert.h"

/* Target (class name from RTTI): a point other objects can aim at (meaning inferred). */
class Target {
  public:
    /* Moves the target to position (name inferred). */
    void fn_800CCE38(const Vec3f *position);
};

/* One stage's record in the save data (0x60 bytes). Only the key slots are known; how their
 * offset 0xF4 from the start of the save data splits between the record array and the record is
 * not (layout inferred from fn_8011BDA4 and fn_8011BE14). */
struct UnkStageSave_800BB74C {
    /* 0x00 */ u8 field_0x0[0x34];
    /* 0x34 */ s32 keys[5];
    /* 0x48 */ u8 field_0x48[0x60 - 0x48];
};
SIZE_ASSERT(UnkStageSave_800BB74C, 0x60);

/* The save data fn_800BB74C returns (class unknown). */
struct UnkSaveData_800BB74C {
    /* 0x00 */ u8 field_0x0[0xC0];
    /* 0xC0 */ UnkStageSave_800BB74C stages[1]; /* indexed by the stage number */
};

/* The game state block in .bss (layout of the word this unit reads only). */
struct UnkGameState_8057D6A0 {
    /* 0x00 */ u8 field_0x0[0xAC];
    /* 0xAC */ s32 m_stage; /* the current stage's number (name inferred) */
    /* 0xB0 */ u8 field_0xB0[0x150 - 0xB0];
};
SIZE_ASSERT(UnkGameState_8057D6A0, 0x150);

extern "C" {
/* Builds the character info of a gadget (behaviour inferred; arguments unknown). */
boost::shared_ptr<GadgetCharInfo> fn_8015CD90(int arg0, int arg1);
/* Model method: set-up call on the node named nodeName (behaviour inferred). */
void fn_801607A8(GadgetModelInfo *model, const char *nodeName);
/* Starts effect number for a gadget's character info (behaviour inferred). */
boost::weak_ptr<Effect::System::EffectElement> fn_8015D794(GadgetCharInfo *charInfo, int number);
/* Moves an effect to position (behaviour inferred). */
void fn_80425E4C(Effect::System::EffectElement *effect, const Vec3f *position);
/* Creates a target for the character with the given flags (behaviour inferred). */
boost::shared_ptr<Target> fn_801E7128(const boost::weak_ptr<CharInfo> &character, int flags);
/* Reads the owner's integer parameter (behaviour inferred). */
void fn_800C9E50(UnkPlacement_800C9CD0 *owner, s32 *value);
/* True when a character stands in the collision event's contacts (behaviour inferred). */
bool fn_8015AC84(ColliDetectArgs &args);
/* Tells the owner that the gadget is done (behaviour inferred). */
void fn_8015A794(Gadget *gadget);

/* Returns the save data, or null (behaviour inferred). */
UnkSaveData_800BB74C *fn_800BB74C(void);
/* An object, and the call that records a collected key in it at +0x724 (behaviour inferred). */
u8 *fn_80079808(void);
void fn_803357F4(void *object);

/* The key slots' functions defined at the end of this unit (names inferred): fn_8011BDA4 stores
 * the slots in the save data, fn_8011BE14 clears them and reloads them from it, fn_8011BE9C
 * counts the collected keys and fn_8011BED0 adds a key id to a free slot. */
bool fn_8011BDA4(void);
void fn_8011BE14(void);
u32 fn_8011BE9C(void);
bool fn_8011BED0(int id);

extern UnkGameState_8057D6A0 lbl_8057D6A0;
}

/* The ids of the stage's collected keys; -1 marks a free slot (name inferred). */
static s32 s_keys[5];

/* True when the key id is collected (name inferred). */
inline bool IsKeyCollected(int id) {
    for (int i = 0; i < 5; i++) {
        if (id == s_keys[i]) {
            return true;
        }
    }
    return false;
}

namespace SecretKey {

class SecretKey : public Gadget {
  public:
    SecretKey(Task *parent, UnkPlacement_800C9CD0 *owner);
    virtual ~SecretKey();
    virtual void Behave(float dt);
    virtual void ReloadPlacement(UnkPlacement_800C9CD0 *owner);

    /* Creates the key under task layer 11 (name inferred). */
    static void Instantiate(UnkPlacement_800C9CD0 *owner);

    /* Copies the owner's position, rotation and key id and restarts the key (name inferred). */
    void LoadFromOwner(UnkPlacement_800C9CD0 *owner);
    /* Places the model, the effect and the target at the character info's position (name
     * inferred). */
    void UpdatePosition();
    /* The model's collision callback (name inferred). */
    void OnColliDetect(ColliDetectArgs &args);
    /* Sets the key's switch, reports the gadget done and finishes (name inferred). */
    void Retire();

    static const char *s_taskName;

    /* Member names from 0x40 on are inferred. */
    /* 0x30 */ boost::shared_ptr<GadgetCharInfo> m_chara;
    /* 0x38 */ boost::shared_ptr<GadgetModelInfo> m_model;
    /* 0x40 */ s32 m_state; /* 0 new, 1 waiting, 2 touched, 3 finishing */
    /* 0x44 */ boost::weak_ptr<Effect::System::EffectElement> m_effect;
    /* 0x4C */ s32 m_keyId; /* also the switch it sets */
    /* 0x50 */ boost::shared_ptr<Target> m_target;
};

} // namespace SecretKey

SIZE_ASSERT(SecretKey::SecretKey, 0x58);

namespace SecretKey {

const char *SecretKey::s_taskName = "SecretKey";

SecretKey::SecretKey(Task *parent, UnkPlacement_800C9CD0 *owner) : Gadget(parent, owner) {
    m_name = s_taskName;
    m_chara = fn_8015CD90(0x13, 4);
    m_model = fn_8015F9D8(0, 0xDE);
    m_model->fn_80162A68(0, 0);
    m_model->fn_80161F08("col", m_chara);
    fn_800D0B64(m_model->field_0x4, boost::bind(&SecretKey::OnColliDetect, this, _1));
    m_model->fn_80160BA0("mtn");
    m_model->fn_8015FF60();
    fn_801607A8(m_model.get(), "mtn");
    m_model->fn_80160B18(0);
    m_effect = fn_8015D794(m_chara.px, 0x8D);
    m_target = fn_801E7128(m_chara, 0x80);
    LoadFromOwner(m_owner);
    UpdatePosition();
}

SecretKey::~SecretKey() {
    if (!m_effect.expired()) {
        boost::shared_ptr<Effect::System::EffectElement> effect = m_effect.lock();
        if (effect.px != 0) {
            effect->Retire();
        }
        m_effect.reset();
    }
}

void SecretKey::Behave(float dt) {
    if (m_model.px != 0) {
        m_model->fn_80162FB0(dt);
    }

    switch (m_state) {
    case 0:
        if (IsKeyCollected(m_keyId)) {
            Retire();
        } else {
            m_state = 1;
        }
        break;
    case 1:
        break;
    case 2:
        fn_8011BED0(m_keyId);
        fn_801D560C(fn_80053164(), 0xE017, 0, 0);
        Retire();
        break;
    case 3:
        Retire();
        break;
    }

    UpdatePosition();
}

void SecretKey::ReloadPlacement(UnkPlacement_800C9CD0 *owner) {
    LoadFromOwner(owner);
    UpdatePosition();
    if (m_model.px != 0) {
        m_model->fn_80162EF4();
        m_model->fn_8016108C();
    }
}

void SecretKey::LoadFromOwner(UnkPlacement_800C9CD0 *owner) {
    fn_800C9F14(owner, &m_chara->m_position);
    fn_800C9ED4(owner, &m_chara->m_rotation);
    fn_800C9E50(owner, &m_keyId);
    m_state = 0;
}

void SecretKey::UpdatePosition() {
    if (m_model.px != 0) {
        Mtx34f matrix;

        fn_8040C148(&matrix, &lbl_805E0720);
        fn_8040B59C(&matrix, &m_chara->m_position);
        fn_8040B848(&matrix, m_chara->m_rotation.y);
        fn_8040B884(&matrix, m_chara->m_rotation.x);
        fn_8040B80C(&matrix, m_chara->m_rotation.z);
        m_model->fn_801633DC(&matrix);
    }
    if (!m_effect.expired()) {
        boost::shared_ptr<Effect::System::EffectElement> effect = m_effect.lock();
        if (effect.px != 0) {
            fn_80425E4C(effect.px, &m_chara->m_position);
        }
    }
    if (m_target.px != 0) {
        m_target->fn_800CCE38(&m_chara->m_position);
    }
}

void SecretKey::OnColliDetect(ColliDetectArgs &args) {
    if (fn_8015AC84(args)) {
        m_state = 2;
    }
}

void SecretKey::Retire() {
    fn_801690D0(fn_800CB69C(), m_keyId, 1);
    fn_8015A794(this);
    m_flags |= 1;
}

} // namespace SecretKey

extern "C" void fn_8011BF8C(void) {
    fn_8006B50C(0xDE, 1);
    fn_8011BE14();
}

extern "C" void fn_8011BF88(void) {}

namespace SecretKey {

void SecretKey::Instantiate(UnkPlacement_800C9CD0 *owner) {
    Task *parent = fn_801EB304(fn_8001C4F8(), 11);
    new SecretKey(parent, owner);
}

} // namespace SecretKey

bool fn_8011BED0(int id) {
    for (int i = 0; i < 5; i++) {
        if (s_keys[i] == -1) {
            s_keys[i] = id;
            fn_803357F4(fn_80079808() + 0x724);
            return true;
        }
    }
    return false;
}

u32 fn_8011BE9C(void) {
    u32 count = 0;

    for (int i = 0; i < 5; i++) {
        if (s_keys[i] != -1) {
            count++;
        }
    }
    return count;
}

void fn_8011BE14(void) {
    for (int i = 0; i < 5; i++) {
        s_keys[i] = -1;
    }

    UnkSaveData_800BB74C *save = fn_800BB74C();
    if (save != 0) {
        for (int i = 0; i < 5; i++) {
            s_keys[i] = save->stages[lbl_8057D6A0.m_stage].keys[i];
        }
    }
}

bool fn_8011BDA4(void) {
    UnkSaveData_800BB74C *save = fn_800BB74C();
    if (save != 0) {
        int stage = lbl_8057D6A0.m_stage;
        for (int i = 0; i < 5; i++) {
            save->stages[stage].keys[i] = s_keys[i];
        }
        return true;
    }
    return false;
}

GadgetClassDesc lbl_805271CC = {
    "SECRETKEY", fn_8011BF8C, fn_8011BF88, SecretKey::SecretKey::Instantiate, 0, 0, 0x1D, 1, 0, 10, 0,
};

/* Not original: an unused table that creates three string literals before the class's RTTI is
 * generated at the end of the file. Retail pools "col" and "mtn" ahead of "Task"/"Gadget" in
 * .sdata2 and the allocator's message ahead of "SecretKey::SecretKey" in .rodata, as if the vtable
 * were emitted after the deferred functions; GC/2.7 emits it first (another case of the
 * compiler-version ordering difference in docs/research-notes.md). The literals pool with the
 * constructor's and the allocator's, and the linker drops the table. */
static const char *const s_pool[] = {"col", "mtn", "Memory allocation failure"};
