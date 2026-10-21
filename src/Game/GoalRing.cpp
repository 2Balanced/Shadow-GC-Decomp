/* GoalRing::GoalRing (class name from RTTI): the GOALRING gadget, the ring that ends a stage. It
 * shows GOALRING.BIN in the colour the "color" parameter selects, with a glow effect and, for the
 * first seven colours, a coloured effect, and plays a looping sound at its position. When a player
 * touches it the ring plays its touch animation, stops the sound and reports the goal; every
 * colour but 7 also sets a flag in the game state. File name inferred from the class. Method names
 * marked "(name inferred)" are not original. Definitions are in reverse address order (deferred
 * inlining emits functions last-defined first). */
#include <boost/bind.hpp>
#include <boost/function.hpp>

#include "Effect/System/EffectElement.h"
#include "Game/CharInfo.h"
#include "Game/Gadget.h"
#include "Game/GadgetCharInfo.h"
#include "Game/GadgetCollision.h"
#include "Game/GadgetModelInfo.h"
#include "Game/GadgetSound.h"
#include "Game/GadgetClassDesc.h"
#include "Game/GadgetUtil.h"
#include "Game/FileQueue.h"
#include "boost/weak_ptr.hpp"
#include "size_assert.h"

/* The game state block in .bss (layout of the byte this unit sets only). */
struct UnkGameState_8057D6A0 {
    /* 0x00 */ u8 field_0x0[0xC1];
    /* 0xC1 */ u8 field_0xC1; /* set when a goal ring other than colour 7 is touched (meaning
                                * unknown) */
    /* 0xC2 */ u8 field_0xC2[0x150 - 0xC2];
};
SIZE_ASSERT(UnkGameState_8057D6A0, 0x150);

class UnkPlayerManager_8006D6A8;

extern "C" {
/* Loads the model in fileName for a gadget (behaviour inferred). */
boost::shared_ptr<GadgetModelInfo> fn_8015F800(int arg0, const char *fileName);
/* Model methods: two set-up calls on the node named nodeName, and the touch animation (behaviour
 * inferred). */
void fn_801607A8(GadgetModelInfo *model, const char *nodeName);
void fn_801609F8(GadgetModelInfo *model, const char *nodeName);
void fn_801612E0(GadgetModelInfo *model);
/* Starts effect number for a gadget's character info, and an effect at a position and rotation
 * (behaviour inferred; the other arguments' meanings are unknown). */
boost::weak_ptr<Effect::System::EffectElement> fn_8015D794(GadgetCharInfo *charInfo, int number);
boost::weak_ptr<Effect::System::EffectElement> fn_801D2B38(int arg0, const Vec3f *position,
                                                            const Vec3f *rotation,
                                                            int number, int arg4);
/* Moves an effect to position (behaviour inferred). */
void fn_80425E4C(Effect::System::EffectElement *effect, const Vec3f *position);
/* Calls callback with every player in the collision event (behaviour inferred; fn_8015B004 is the
 * variant for all characters). */
void fn_8015AA84(ColliDetectArgs &args,
                 void (*callback)(boost::shared_ptr<CharInfo> hit, void *user), void *user);
/* The player manager, and the goal handler it is passed to (behaviour inferred). */
UnkPlayerManager_8006D6A8 *fn_8006D6A8(void);
void fn_8016DA34(UnkPlayerManager_8006D6A8 *manager);

extern UnkGameState_8057D6A0 lbl_8057D6A0;
}

/* The coloured effect for each colour; colour 7 has none. */
static const int lbl_804BB480[8] = {0x2BB, 0x2B5, 0x2BE, 0x2C1, 0x2B8, 0x2B2, 0x2AF, -1};

namespace GoalRing {

/* The parameter the owner supplies (name from the parameter table). */
struct GoalRingParams {
    GoalRingParams() : color(0) {}

    /* 0x0 */ int color;
};
SIZE_ASSERT(GoalRingParams, 0x4);

class GoalRing : public Gadget {
  public:
    GoalRing(Task *parent, UnkPlacement_800C9CD0 *owner);
    virtual ~GoalRing();
    virtual void Behave(float dt);
    virtual void ReloadPlacement(UnkPlacement_800C9CD0 *owner);

    /* Creates the ring under task layer 11 (name inferred). */
    static void Instantiate(UnkPlacement_800C9CD0 *owner);

    /* Replaces the coloured effect with the one for color (name inferred). */
    void SetColor(int color);
    /* The model's collision callback (name inferred). */
    void OnColliDetect(ColliDetectArgs &args);
    /* Places the model, the effects and the sound at the character info's position (name
     * inferred). */
    void UpdatePosition();
    /* Copies the owner's position, rotation and parameter (name inferred). */
    void LoadFromOwner(UnkPlacement_800C9CD0 *owner);

    static const char *s_taskName;

    /* 0x30 */ int m_color; /* the colour the model and the coloured effect show */
    /* 0x34 */ boost::shared_ptr<GadgetCharInfo> m_chara;
    /* 0x3C */ boost::shared_ptr<GadgetModelInfo> m_model;
    /* 0x44 */ GoalRingParams m_params;
    /* 0x48 */ boost::weak_ptr<Effect::System::EffectElement> m_glow;
    /* 0x50 */ boost::weak_ptr<Effect::System::EffectElement> m_colorEffect;
    /* 0x58 */ UnkSoundHandle_801D3DC8 m_sound;
    /* 0x74 */ u32 field_0x74; /* not used by this unit */
};

} // namespace GoalRing

SIZE_ASSERT(GoalRing::GoalRing, 0x78);

namespace GoalRing {

const char *GoalRing::s_taskName = "GoalRing";

/* Records that a player touched the ring and reports the goal (name inferred). */
static void TouchCharacter(boost::shared_ptr<CharInfo> hit, void *user) {
    bool *touched = static_cast<bool *>(user);

    if (touched != 0) {
        *touched = true;
        fn_8016DA34(fn_8006D6A8());
    }
}

GoalRing::GoalRing(Task *parent, UnkPlacement_800C9CD0 *owner) : Gadget(parent, owner) {
    m_name = s_taskName;
    m_chara = fn_8015CD90(7, 4);
    LoadFromOwner(m_owner);
    m_model = fn_8015F800(0, "GOALRING.BIN");
    m_color = m_params.color;
    m_model->fn_80162A68(m_color, 0);
    m_model->fn_80161F08("ring", m_chara);
    fn_800D0B64(m_model->field_0x4, boost::bind(&GoalRing::OnColliDetect, this, _1));
    m_model->fn_80160BA0("ring");
    fn_801607A8(m_model.get(), "ring");
    fn_801609F8(m_model.get(), "ring");
    m_model->fn_8015FF60();
    m_glow = fn_8015D794(m_chara.px, 0x8E);
    if (lbl_804BB480[m_color] != -1) {
        m_colorEffect = fn_801D2B38(0, &m_chara->m_position, &m_chara->m_rotation,
                                    lbl_804BB480[m_color], 0);
    }
    m_sound = fn_801D3DC8(0xE00D, m_chara.px, this);
    SetColor(m_color);
    UpdatePosition();
}

GoalRing::~GoalRing() {
    m_sound.fn_801D455C();
    if (!m_colorEffect.expired()) {
        boost::shared_ptr<Effect::System::EffectElement> effect = m_colorEffect.lock();
        if (effect.px != 0) {
            effect->Retire();
        }
        m_colorEffect.reset();
    }
    if (!m_glow.expired()) {
        boost::shared_ptr<Effect::System::EffectElement> effect = m_glow.lock();
        if (effect.px != 0) {
            effect->Retire();
        }
        m_glow.reset();
    }
}

void GoalRing::Behave(float dt) {
    if (m_model.get() != 0) {
        m_model->fn_80162FB0(dt);
    }
    UpdatePosition();
}

void GoalRing::ReloadPlacement(UnkPlacement_800C9CD0 *owner) {
    LoadFromOwner(owner);
    UpdatePosition();
    if (m_model.get() != 0) {
        m_model->fn_80162EF4();
        m_model->fn_8016108C();
        if (m_params.color != m_color) {
            m_color = m_params.color;
            SetColor(m_color);
        }
    }
}

void GoalRing::LoadFromOwner(UnkPlacement_800C9CD0 *owner) {
    fn_800C9F14(owner, &m_chara->m_position);
    fn_800C9ED4(owner, &m_chara->m_rotation);
    fn_800C9DBC(owner, &m_params, sizeof(m_params));
}

void GoalRing::UpdatePosition() {
    if (m_model.get() != 0) {
        Mtx34f matrix;

        fn_8040C148(&matrix, &lbl_805E0720);
        fn_8040B59C(&matrix, &m_chara->m_position);
        m_model->fn_801633DC(&matrix);
    }
    if (!m_colorEffect.expired()) {
        boost::shared_ptr<Effect::System::EffectElement> effect = m_colorEffect.lock();
        if (effect.px != 0) {
            fn_80425E4C(effect.px, &m_chara->m_position);
        }
    }
    if (!m_glow.expired()) {
        boost::shared_ptr<Effect::System::EffectElement> effect = m_glow.lock();
        if (effect.px != 0) {
            fn_80425E4C(effect.px, &m_chara->m_position);
        }
    }
    if (m_sound.m_id != 0) {
        fn_800CCE14(&m_sound, &m_chara->m_position);
    }
}

void GoalRing::OnColliDetect(ColliDetectArgs &args) {
    bool touched = false;

    fn_8015AA84(args, TouchCharacter, &touched);
    if (touched) {
        if (m_model.get() != 0) {
            fn_801612E0(m_model.get());
        }
        m_sound.fn_801D455C();
        if (m_params.color != 7) {
            lbl_8057D6A0.field_0xC1 = 1;
        }
    }
}

void GoalRing::SetColor(int color) {
    if (m_model.get() != 0) {
        if (!m_colorEffect.expired()) {
            boost::shared_ptr<Effect::System::EffectElement> effect = m_colorEffect.lock();
            if (effect.px != 0) {
                effect->Retire();
            }
        }
        if (lbl_804BB480[m_color] != -1) {
            m_colorEffect = fn_801D2B38(0, &m_chara->m_position, &m_chara->m_rotation,
                                        lbl_804BB480[color], 0);
        }
    }
}

} // namespace GoalRing

extern "C" void fn_80106324(void) {
    fn_8006B474("GOALRING.BIN", 1);
}

extern "C" void fn_80106320(void) {}

void GoalRing::GoalRing::Instantiate(UnkPlacement_800C9CD0 *owner) {
    new GoalRing(fn_801EB304(fn_8001C4F8(), 11), owner);
}

static GadgetParamDesc lbl_8052615C[] = {
    {1, "color", 0, 0, 7, 0.0f, 0.0f, 0.0f, fn_800CBC20, 0},
    {0},
};

GadgetClassDesc lbl_805261AC = {
    "GOALRING", fn_80106324, fn_80106320, GoalRing::GoalRing::Instantiate, 0, 0, 0x14, 1, 0, 10,
    lbl_8052615C,
};
