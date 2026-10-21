/* ElecAccessPanel (class name from RTTI; its s_taskName is "ElecAccessPanel"): the ELEC_ACCESS
 * gadget, an access panel (ACCESS.BIN) tied to a stage switch. When a character touches it while its
 * switch is off it turns the switch on, shows its lit parts and fades its "mat" material from black
 * to white over half a second; then it hits the player and plays a sound every frame. Its state
 * survives the gadget being re-created through an ElecAccessPanelData kept by the owner. File name
 * from the class. Names marked "(name inferred)" are not original. Definitions are in reverse
 * address order (deferred inlining emits functions last-defined first). */
#include <boost/bind.hpp>
#include <boost/function.hpp>

#include "Game/Mission.h"
#include "Game/CharInfo.h"
#include "Game/Gadget.h"
#include "Game/GadgetCharInfo.h"
#include "Game/GadgetClassDesc.h"
#include "Game/GadgetCollision.h"
#include "Game/GadgetModelInfo.h"
#include "Game/SetAdapterState.h"
#include "Game/UnkSwitchBank_800CB69C.h"
#include "Game/FileQueue.h"
#include "boost/shared_ptr.hpp"
#include "boost/weak_ptr.hpp"
#include "size_assert.h"

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

/* What fn_8015FA78 passes for each part of a model (see ElecFireWall.cpp). */
struct UnkModelPart_8015FA78 {
    /* 0x0 */ void *m_object;
    /* 0x4 */ void *field_0x4;
    /* 0x8 */ s32 m_index;
};

/* The game state block in .bss (only the member used here; see CityBombBig.cpp). */
struct UnkGameState_8057D6A0 {
    /* 0x000 */ u8 field_0x0[0x125];
    /* 0x125 */ u8 field_0x125; /* (meaning unknown) */
};

extern "C" {
extern UnkGameState_8057D6A0 lbl_8057D6A0;
}

/* The positioned object fn_802B4254 creates (see ElecBomb.cpp). */
class UnkSound_802B4254;

class UnkPlayerManager_8006D6A8;

extern "C" {
/* Calls callback for each part of model while it returns non-null (see ElecBomb.cpp). */
void fn_8015FA78(GadgetModelInfo *model,
                 UnkModelPart_8015FA78 *(*callback)(UnkModelPart_8015FA78 *part, u32 show), u32 show);
/* Show and hide a part's object (behaviour inferred). */
void fn_80038F94(void *object);
void fn_80038FC0(void *object);
/* GadgetModelInfo helpers (see ElecCristalWall.cpp): find the material animation named name, and
 * select the material named name. */
UnkMaterialAnim_801621F0 *fn_801621F0(GadgetModelInfo *model, const char *name);
void fn_8016239C(GadgetModelInfo *model, const char *name);
/* Blends animation's colour from from to to as t goes from t0 to t1 (see ElecSearchLight.cpp). */
void fn_80154C6C(UnkMaterialAnim_801621F0 *animation, const UnkColor_8040D2D8 *from,
                 const UnkColor_8040D2D8 *to, float t, float t0, float t1);
/* Creates the positioned object of the given kind and sets its range and a flag, and its position
 * (see ElecBomb.cpp). */
boost::shared_ptr<UnkSound_802B4254> fn_802B4254(int kind, int arg1);
void fn_802B4498(UnkSound_802B4254 *sound, float range);
void fn_802B4510(UnkSound_802B4254 *sound, u8 flag);
void fn_802B44C8(UnkSound_802B4254 *sound, const Vec3f *position);
/* shared_ptr<UnkSound_802B4254>::reset (see ElecBomb.cpp). */
void fn_800CC978(boost::shared_ptr<UnkSound_802B4254> *sound);
void fn_800C985C(UnkPlacement_800C9CD0 *owner);
/* Returns the player manager and its entry (see ElecBomb.cpp). */
UnkPlayerManager_8006D6A8 *fn_8006D6A8(void);
boost::weak_ptr<Mission::Mission> fn_800740E4(UnkPlayerManager_8006D6A8 *manager, int index);
/* destination = a + b (behaviour inferred). */
void fn_8040CDD0(Vec3f *destination, const Vec3f *a, const Vec3f *b);
/* Calls callback with every character in the collision event (see ElecSearchLight.cpp). */
void fn_8015AA84(ColliDetectArgs &args, void (*callback)(boost::shared_ptr<CharInfo> hit, void *user),
                 void *user);
/* The sound manager and a sound played for a source (see ElecSearchLight.cpp). */
void *fn_80053164(void);
void fn_801D53C4(void *manager, int id, GadgetCharInfo *charInfo, void *source, int arg4, int arg5,
                 int arg6);
}

/* ElecAccessPanelData (class name from RTTI): the panel's state kept by the owner. */
class ElecAccessPanelData : public SetAdapterState {
  public:
    /* 0x4 */ s32 m_state; /* the panel's m_state (inferred) */
};
SIZE_ASSERT(ElecAccessPanelData, 0x8);

class ElecAccessPanel : public Gadget {
  public:
    ElecAccessPanel(Task *parent, UnkPlacement_800C9CD0 *owner);
    virtual ~ElecAccessPanel();
    virtual void Behave(float dt);
    virtual void fn_800CBC1C();
    virtual void ReloadPlacement(UnkPlacement_800C9CD0 *owner);

    /* Creates the panel under task layer 11 (name inferred). */
    static void Instantiate(UnkPlacement_800C9CD0 *owner);

    /* Method names are inferred. */
    /* Copies the owner's position, rotation and switch number into the gadget. */
    void ReadPlacement(UnkPlacement_800C9CD0 *owner);
    /* Places the model and the positioned object at the character info's position. */
    void RebuildTransform();
    /* Shows the lit parts and starts the fade. */
    void Activate();
    /* The collider's callback. */
    void OnColliDetect(ColliDetectArgs &args);

    /* Shows (show != 0) or hides the parts of the model. */
    static UnkModelPart_8015FA78 *ShowParts(UnkModelPart_8015FA78 *part, u32 show);

    /* The panel's state kept by the owner (name inferred). */
    ElecAccessPanelData *GetData();

    static const char *s_taskName;

    /* Member names are inferred. */
    /* 0x30 */ GadgetModelInfo *m_model;
    /* 0x34 */ boost::shared_ptr<GadgetCharInfo> m_chara;
    /* 0x3C */ u8 m_switchId;
    /* 0x40 */ u32 field_0x40;
    /* 0x44 */ s32 m_state; /* 0 waiting, 1 activated, 2 fading, 3 done */
    /* 0x48 */ float m_timer;
    /* 0x4C */ boost::shared_ptr<UnkSound_802B4254> m_sound;
    /* 0x54 */ u32 field_0x54;
};
SIZE_ASSERT(ElecAccessPanel, 0x58);

/* Turns stage switch id on (name inferred). */
inline void TurnSwitchOn(u8 id) {
    fn_801690D0(fn_800CB69C(), id, 1);
}

/* The material's colours while the panel lights up (names inferred). */
static UnkColor_8040D2D8 s_black(0, 0, 0, 0);
static UnkColor_8040D2D8 s_white(0xFF, 0xFF, 0xFF, 0xFF);

const char *ElecAccessPanel::s_taskName = "ElecAccessPanel";

inline ElecAccessPanelData *ElecAccessPanel::GetData() {
    return dynamic_cast<ElecAccessPanelData *>(m_owner->m_adapterState);
}

/* Turns the panel's switch on and lights the panel for the first character the collision reports
 * (name inferred). */
static void HitCharacter(boost::shared_ptr<CharInfo> hit, void *user) {
    ElecAccessPanel **panel = (ElecAccessPanel **)user;

    if (panel != 0) {
        if (fn_8016904C(fn_800CB69C(), (*panel)->m_switchId) != 1) {
            fn_801690D0(fn_800CB69C(), (*panel)->m_switchId, 1);
            (*panel)->Activate();
        }
    }
}

ElecAccessPanel::ElecAccessPanel(Task *parent, UnkPlacement_800C9CD0 *owner) : Gadget(parent, owner) {
    m_name = s_taskName;
    m_chara = fn_8015CB88(0x59, 4, "ElecAccessPanel");
    ReadPlacement(owner);
    if (owner->m_adapterState == 0) {
        ElecAccessPanelData *data = new ElecAccessPanelData;
        fn_800C9930(owner, data);
        data->m_state = 0;
        m_state = 0;
    } else if (GetData()->m_state == 3) {
        /* The int cast gives retail's clrlwi of the switch number. */
        TurnSwitchOn((int)m_switchId);
    }

    m_model = new GadgetModelInfo(1, "ACCESS.BIN");
    m_model->fn_80162A68(0, 0);
    m_sound = fn_802B4254(2, 0);
    fn_802B4498(m_sound.px, 1200.0f);
    fn_802B4510(m_sound.px, 1);
    fn_8016239C(m_model, "mat");

    ElecAccessPanelData *data = GetData();
    if (fn_8016904C(fn_800CB69C(), m_switchId) != 1) {
        m_state = 0;
        data->m_state = 0;
        fn_8015FA78(m_model, ShowParts, 0);
        m_model->fn_80161F08("col", m_chara);
        fn_800D0B64(m_model->field_0x4, boost::bind(&ElecAccessPanel::OnColliDetect, this, _1));
    } else {
        m_state = 3;
        data->m_state = 3;
        UnkMaterialAnim_801621F0 *animation = fn_801621F0(m_model, "mat");
        animation->color.r = 0xFF;
        animation->color.g = 0xFF;
        animation->color.b = 0xFF;
        animation->color.a = 0xFF;
        fn_8015FA78(m_model, ShowParts, 1);
    }
}

ElecAccessPanel::~ElecAccessPanel() {
    if (m_model != 0) {
        delete m_model;
        m_model = 0;
    }
}

void ElecAccessPanel::Behave(float dt) {
    if (m_model != 0) {
        m_model->fn_80162FB0(dt);
    }

    switch (m_state) {
    case 0:
        break;
    case 1:
        m_model->fn_80161880();
        m_state = 2;
        break;
    case 2:
        m_timer += dt;
        if (m_timer >= 0.5f) {
            m_timer = 0.5f;
            if (lbl_8057D6A0.field_0x125 != 0) {
                boost::shared_ptr<Mission::Mission> player = fn_800740E4(fn_8006D6A8(), 1).lock();
                if (player.get() != 0) {
                    player->AddCount(1, 0);
                }
            } else {
                boost::shared_ptr<Mission::Mission> player = fn_800740E4(fn_8006D6A8(), 2).lock();
                if (player.get() != 0) {
                    player->AddCount(1, 0);
                }
            }
            fn_800CC978(&m_sound);
            m_state = 3;
        }
        fn_80154C6C(fn_801621F0(m_model, "mat"), &s_black, &s_white, m_timer, 0.0f, 0.5f);
        break;
    case 3:
        fn_801D53C4(fn_80053164(), 0x7249, m_chara.px, this, 3, 0, 5);
        break;
    }

    RebuildTransform();
}

void ElecAccessPanel::ReloadPlacement(UnkPlacement_800C9CD0 *owner) {
    ReadPlacement(owner);
}

void ElecAccessPanel::fn_800CBC1C() {
    RebuildTransform();
    if (m_model != 0) {
        m_model->fn_80162EF4();
        m_model->fn_8016108C();
    }
}

void ElecAccessPanel::RebuildTransform() {
    if (m_model != 0) {
        Mtx34f matrix;
        Mtx34f translation;

        SetTranslationMtx(&translation, m_chara->m_position);
        fn_8040C148(&matrix, &translation);
        fn_8040B848(&matrix, m_chara->m_rotation.y);
        m_model->fn_801633DC(&matrix);
    }

    if (m_sound.get() != 0) {
        Vec3f offset(0.0f, 34.0f, 0.0f);
        Vec3f position;

        fn_8040CDD0(&position, &offset, &m_chara->m_position);
        fn_802B44C8(m_sound.px, &position);
    }
}

void ElecAccessPanel::ReadPlacement(UnkPlacement_800C9CD0 *owner) {
    fn_800C9F14(owner, &m_chara.px->m_position);
    fn_800C9ED4(owner, &m_chara.px->m_rotation);
    fn_800C9E60(owner, &m_switchId);
}

UnkModelPart_8015FA78 *ElecAccessPanel::ShowParts(UnkModelPart_8015FA78 *part, u32 show) {
    if (show != 0) {
        fn_80038F94(part->m_object);
    } else {
        fn_80038FC0(part->m_object);
    }
    return part;
}

void ElecAccessPanel::Activate() {
    m_state = 1;
    m_timer = 0.0f;
    fn_8015FA78(m_model, ShowParts, 1);
    UnkMaterialAnim_801621F0 *animation = fn_801621F0(m_model, "mat");
    animation->color.r = 0;
    animation->color.g = 0;
    animation->color.b = 0;
    animation->color.a = 0;
    GetData()->m_state = 3;
    fn_800C985C(m_owner);
}

void ElecAccessPanel::OnColliDetect(ColliDetectArgs &args) {
    ElecAccessPanel *self = this;

    fn_8015AA84(args, HitCharacter, &self);
}

extern "C" void fn_802489E4(void) {
    fn_8006B474("ACCESS.BIN", 1);
}

extern "C" void fn_802489E0(void) {}

void ElecAccessPanel::Instantiate(UnkPlacement_800C9CD0 *owner) {
    new ElecAccessPanel(fn_801EB304(fn_8001C4F8(), 11), owner);
}

GadgetClassDesc lbl_80548B70 = {
    "ELEC_ACCESS", fn_802489E4, fn_802489E0, ElecAccessPanel::Instantiate, 0, 0, 0x7DD, 1, 0, 10, 0,
};
