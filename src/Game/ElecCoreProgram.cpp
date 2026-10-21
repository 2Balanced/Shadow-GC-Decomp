/* ElecCoreProgram (class name from RTTI): the ELEC_CORE gadget of the electric circuit area, the
 * core shown by CORE.BIN. Five of its frames turn continuously, each at its own tilt and speed,
 * while its "mat1" material pulses. Attacks flash its "mat2" material and wear it down; after
 * sixteen hits it overloads for a second, scaling two of its frames and fading its materials, then
 * hands the first player on, rumbles the pads and reports itself finished. It registers a command
 * callback on its character info and keeps a Target and a positioned sound object at its position.
 * File name inferred from the class. Method names marked "(name inferred)" are not original.
 * Definitions are in reverse address order (deferred inlining emits functions last-defined first).
 *
 * The constructor addresses its two member pointer constants and the vtable through the start of
 * .data, so the linker keeps this unit's .data whole: the orphan RTTI base lists of the two first
 * headers lead it (in this order), and zeroed copies of the weak classes' vtables close it. */

/* Keeps the dead copy of bad_alloc's vtable in .data, as retail has it (see <new>). */
#define MSL_NEW_INLINE_BAD_ALLOC

#include <boost/bind.hpp>
#include <boost/function.hpp>

#include "Game/CharCommand.h"
#include "Game/CharCommandUnit.h"
#include "Game/CharInfo.h"
#include "Game/CharaColliAttack.h"
#include "Game/Gadget.h"
#include "Game/GadgetCharInfo.h"
#include "Game/GadgetModelInfo.h"
#include "Game/GadgetClassDesc.h"
#include "Game/MathConstants.h"
#include "Game/TNumEdit.h"
/* After TNumEdit.h: the order of the casts sets the order of the RTTI base lists in .data. */
#include "Game/CharaColliBaseCharInfo.h"
#include "Game/Vibration.h"
#include "Game/FileQueue.h"
#include "boost/detail/sp_counted_base_impl.hpp"
#include "boost/shared_ptr.hpp"
#include "boost/weak_ptr.hpp"
#include "size_assert.h"

using SonicteamUSA::System::CharaColli::AttackCallback;
using SonicteamUSA::System::CharaColli::AttackCallbackParam;
using SonicteamUSA::System::CharaColli::CharaColliAttack;

/* The RenderWare types used here, as in rw/rwcore.h (which needs the C library headers). */
struct RwV3d {
    float x;
    float y;
    float z;
};

struct RwMatrix {
    /* 0x00 */ RwV3d right;
    /* 0x0C */ u32 flags;
    /* 0x10 */ RwV3d up;
    /* 0x1C */ u32 pad1;
    /* 0x20 */ RwV3d at;
    /* 0x2C */ u32 pad2;
    /* 0x30 */ RwV3d pos;
    /* 0x3C */ u32 pad3;
};
SIZE_ASSERT(RwMatrix, 0x40);

struct RwObject {
    /* 0x0 */ u8 type;
    /* 0x1 */ u8 subKind;
    /* 0x2 */ u8 flags;
    /* 0x3 */ u8 privateFlags;
    /* 0x4 */ void *parent;
};

/* Only the members used here are declared (the frame is 0xA4 bytes). */
struct RwFrame {
    /* 0x00 */ RwObject object;
    /* 0x08 */ u8 inDirtyListLink[0x8];
    /* 0x10 */ RwMatrix modelling;
};

/* What fn_8015FA78 passes for each part of a model (see ElecFireWall.cpp). */
struct UnkModelPart_8015FA78 {
    /* 0x0 */ RwObject *m_object;
    /* 0x4 */ void *field_0x4;
    /* 0x8 */ s32 m_index;
};
SIZE_ASSERT(UnkModelPart_8015FA78, 0xC);

typedef UnkModelPart_8015FA78 *(*UnkModelPartCallback_8015FA78)(UnkModelPart_8015FA78 *part,
                                                                void *data);

/* Four colour bytes built by an out-of-line constructor (see ElecBarrier.cpp). */
struct UnkColor_8040D2D8 {
    UnkColor_8040D2D8(u8 r, u8 g, u8 b, u8 a);

    /* 0x0 */ u8 r;
    /* 0x1 */ u8 g;
    /* 0x2 */ u8 b;
    /* 0x3 */ u8 a;
};
SIZE_ASSERT(UnkColor_8040D2D8, 0x4);

/* The material animation of a model fn_801621F0 finds by name (see ElecCristalWall.cpp). */
struct UnkMaterialAnim_801621F0 {
    /* 0x0 */ u8 field_0x0[0x8];
    /* 0x8 */ UnkColor_8040D2D8 color;
};

/* Target (class name from RTTI): a point other objects can aim at (see Spring.cpp). */
class Target {
  public:
    /* Moves the target to position (name inferred). */
    void fn_800CCE38(const Vec3f *position);
};

/* The positioned object fn_802B4254 creates (see ElecBomb.cpp). */
class UnkSound_802B4254;

/* An object kept by the manager returned by fn_8006D6A8 (see ElecBomb.cpp; the first entry is
 * probably the player). Only the members this unit uses are declared (names inferred). */
class UnkPlayer_800740E4 {
  public:
    bool IsState1() const {
        return m_state == 1;
    }

    void SetFlag14() {
        field_0x14 = 1;
    }

    /* 0x00 */ u8 field_0x0[0xC];
    /* 0x0C */ s32 m_state;
    /* 0x10 */ u32 field_0x10;
    /* 0x14 */ u8 field_0x14;
};

class UnkPlayerManager_8006D6A8;
class UnkSoundManager_80053164;

extern "C" {
/* RenderWare's RwFrameScale and RwFrameTransform (rw/rwcore.h); the combine operations are
 * replace (0), pre-concatenate (1) and post-concatenate (2). */
RwFrame *fn_8048764C(RwFrame *frame, const RwV3d *scale, int combine);
RwFrame *fn_804876E8(RwFrame *frame, const RwMatrix *transform, int combine);
/* Turn a frame about its z, x and y axes (see Door.cpp). */
void fn_80109AF8(RwFrame *frame, float angle, int combine);
void fn_800E9C64(RwFrame *frame, float angle, int combine);
void fn_800E9CEC(RwFrame *frame, float angle, int combine);
/* Calls callback for each part of model while it returns non-null (behaviour inferred). */
void fn_8015FA78(GadgetModelInfo *model, UnkModelPartCallback_8015FA78 callback, void *data);
/* GadgetModelInfo helpers (see ElecCristalWall.cpp): find the material animation named name, and
 * select the material named name. */
UnkMaterialAnim_801621F0 *fn_801621F0(GadgetModelInfo *model, const char *name);
void fn_8016239C(GadgetModelInfo *model, const char *name);
/* Blends animation's colour from from to to as t goes from t0 to t1 (see ElecBarrier.cpp). */
void fn_80154C6C(UnkMaterialAnim_801621F0 *animation, const UnkColor_8040D2D8 *from,
                 const UnkColor_8040D2D8 *to, float t, float t0, float t1);
/* Returns the attack collider of model number index (behaviour inferred). */
boost::weak_ptr<CharaColliAttack> fn_80161428(GadgetModelInfo *model, int index);
/* Adds a callback to an attack collision (see ElecCristalWall.cpp). */
void fn_80096B64(CharaColliAttack *attack, int arg1, const AttackCallback &callback);
/* Creates a target with the given flags, and removes a gadget's target (see Spring.cpp). */
boost::shared_ptr<Target> fn_801E72DC(int flags);
void fn_801E7108(boost::shared_ptr<Target> *target);
/* Creates the positioned object of the given kind and sets its range, a flag and its position
 * (see ElecBomb.cpp). */
boost::shared_ptr<UnkSound_802B4254> fn_802B4254(int kind, int arg1);
void fn_802B4498(UnkSound_802B4254 *sound, float range);
void fn_802B4510(UnkSound_802B4254 *sound, u8 flag);
void fn_802B44C8(UnkSound_802B4254 *sound, const Vec3f *position);
/* destination = a + b (behaviour inferred). */
void fn_8040CDD0(Vec3f *destination, const Vec3f *a, const Vec3f *b);
/* Returns the player manager and its entry of an index (behaviour inferred). */
UnkPlayerManager_8006D6A8 *fn_8006D6A8(void);
boost::weak_ptr<UnkPlayer_800740E4> fn_800740E4(UnkPlayerManager_8006D6A8 *manager, int index);
/* Sets the word at 0xC of player to 1 and updates it (behaviour inferred). */
void fn_8016B2A4(UnkPlayer_800740E4 *player);
/* Starts a pad rumble at a position (behaviour and arguments inferred). */
boost::weak_ptr<Vibration::RequestHandle> fn_8034DE08(int pattern, const Vec3f *position,
                                                      int pad);
/* Tells the gadget's owner that the gadget is done (behaviour inferred). */
void fn_8015A794(Gadget *gadget);
/* Returns the sound manager and plays sound id through it at the character info's position
 * (behaviour inferred). */
UnkSoundManager_80053164 *fn_80053164(void);
void fn_801D54F4(UnkSoundManager_80053164 *manager, int id, GadgetCharInfo *charInfo, int arg3,
                 int arg4);
}

/* What the frame search callback looks for and finds (names inferred). */
struct ElecCoreProgramFrameSearch {
    ElecCoreProgramFrameSearch() : index(0), frame(0) {}

    /* 0x0 */ s32 index;
    /* 0x4 */ RwFrame *frame;
};

/* The value at t of the line through (t0, v0) and (t1, v1) (name inferred). */
inline float Interpolate(float t, float t0, float t1, float v0, float v1) {
    float rate = (t - t0) / (t1 - t0);
    return v0 + rate * (v1 - v0);
}

/* x reduced to [0, period) (name inferred). */
inline float Wrap(float x, float period) {
    return x - (int)(x / period) * period;
}

class ElecCoreProgram : public Gadget {
  public:
    ElecCoreProgram(Task *parent, UnkPlacement_800C9CD0 *owner);
    virtual ~ElecCoreProgram();
    virtual void Behave(float dt);
    virtual void ReloadPlacement(UnkPlacement_800C9CD0 *owner);

    /* Creates the core under task layer 11 (name inferred). */
    static void Instantiate(UnkPlacement_800C9CD0 *owner);

    /* The character info's command callback; it ignores the commands (name inferred). */
    void OnCommand(CharCommand &command);
    /* The attack callback (name inferred). */
    void OnAttack(AttackCallbackParam &param);
    /* Scales frames 0 and 8 by the time since the core broke (name inferred). */
    void UpdateFrameScales();
    /* Turns frames 3 to 7 (name inferred). */
    void UpdateRings(float dt);
    /* Places the model, the target and the sound object at the character info's position (name
     * inferred). */
    void RebuildTransform();
    /* Copies the owner's position and rotation into the gadget (name inferred). */
    void ReadPlacement(UnkPlacement_800C9CD0 *owner);
    /* State 0: pulses "mat1" and fades the hit flash of "mat2" (name inferred). */
    void UpdateIdle(float dt);
    /* State 1: fades the materials while the core breaks (name inferred). */
    void UpdateBreak(float dt);

    static const char *s_taskName;

    /* 0x030 */ boost::shared_ptr<GadgetCharInfo> m_chara;
    /* 0x038 */ GadgetModelInfo *m_model;
    /* 0x03C */ boost::shared_ptr<Target> m_target;
    /* 0x044 */ int m_state; /* 0 working, 1 breaking (inferred; names inferred below) */
    /* 0x048 */ int m_hits; /* hits left */
    /* 0x04C */ u32 field_0x4C;
    /* 0x050 */ RwFrame *m_frames[9];
    /* 0x074 */ RwMatrix m_frameMatrices[9];
    /* 0x2B4 */ float m_angles[9];
    /* 0x2D8 */ float m_time;
    /* 0x2DC */ float m_flashTime;
    /* 0x2E0 */ boost::shared_ptr<UnkSound_802B4254> m_sound;
};
SIZE_ASSERT(ElecCoreProgram, 0x2E8);

const char *ElecCoreProgram::s_taskName = "ElecCoreProgram";

/* Defined before the constructor: the original emits it after the constructor's template
 * instances. */

/* Part callback: stores the frame of the part whose index is the one searched for, and stops the
 * walk once it has one (name inferred). */
static UnkModelPart_8015FA78 *FindFrame(UnkModelPart_8015FA78 *part, void *data) {
    ElecCoreProgramFrameSearch *search = static_cast<ElecCoreProgramFrameSearch *>(data);

    if (search != 0 && part->m_object != 0 && part->m_index == search->index) {
        search->frame = static_cast<RwFrame *>(part->m_object->parent);
        if (search->frame != 0) {
            return 0;
        }
    }
    return part;
}

ElecCoreProgram::ElecCoreProgram(Task *parent, UnkPlacement_800C9CD0 *owner)
    : Gadget(parent, owner) {
    m_name = s_taskName;
    m_chara = fn_8015CB88(5, 4, "ElecCoreProgram");
    m_sound = fn_802B4254(0, 0);
    fn_802B4498(m_sound.px, 1200.0f);
    fn_802B4510(m_sound.px, 1);
    m_chara->m_commandUnit = boost::shared_ptr<CharCommandUnit>(new CharCommandUnit);
    fn_800729B0(&m_chara->m_commandUnit->m_callback,
                &(const CharCommandCallback &)CharCommandCallback(
                    boost::bind(&ElecCoreProgram::OnCommand, this, _1)));
    m_model = new GadgetModelInfo(1, "CORE.BIN");
    m_model->fn_80162A68(0, 0);
    m_model->fn_80161F08("col", m_chara);
    m_target = fn_801E72DC(0xC00);

    boost::weak_ptr<CharaColliAttack> attack = fn_80161428(m_model, 0);
    if (boost::shared_ptr<CharaColliAttack> collision = attack.lock()) {
        fn_80096B64(collision.get(), 0, boost::bind(&ElecCoreProgram::OnAttack, this, _1));
    }

    fn_8016239C(m_model, "mat1");
    fn_8016239C(m_model, "mat2");
    fn_8016239C(m_model, "mat3");
    fn_8016239C(m_model, "mat4_1");
    fn_8016239C(m_model, "mat4_2");
    fn_8016239C(m_model, "mat4_3");
    fn_8016239C(m_model, "mat4_4");
    fn_8016239C(m_model, "mat4_5");
    fn_8016239C(m_model, "mat5");
    ReadPlacement(m_owner);
    m_state = 0;
    m_hits = 16;
    m_time = 0.0f;
    m_flashTime = 0.0f;
    RebuildTransform();
}

ElecCoreProgram::~ElecCoreProgram() {
    if (m_model != 0) {
        delete m_model;
        m_model = 0;
    }
    fn_801E7108(&m_target);
}

void ElecCoreProgram::Behave(float dt) {
    if (m_model != 0) {
        m_model->fn_80162FB0(dt);
    }
    RebuildTransform();
    UpdateRings(dt);

    switch (m_state) {
    case 0:
        UpdateIdle(dt);
        break;
    case 1:
        if (m_time >= 1.0f) {
            boost::shared_ptr<UnkPlayer_800740E4> player = fn_800740E4(fn_8006D6A8(), 0).lock();
            if (player.get() != 0 && !player->IsState1()) {
                player->SetFlag14();
                fn_8016B2A4(player.get());
            }
            fn_8034DE08(9, &m_chara->m_position, -2);
            fn_8015A794(this);
            m_flags |= 1;
        } else {
            UpdateBreak(dt);
            UpdateFrameScales();
        }
        break;
    }
}

void ElecCoreProgram::UpdateBreak(float dt) {
    static UnkColor_8040D2D8 color0(0xFE, 0xB2, 0xB2, 0xB2);
    static UnkColor_8040D2D8 color1(0xFE, 0x19, 0x19, 0x19);
    static UnkColor_8040D2D8 color2(0xFE, 0xFF, 0xFF, 0xFF);
    static UnkColor_8040D2D8 color3(0xFE, 0x00, 0x00, 0x00);
    static UnkColor_8040D2D8 color4(0xFE, 0xFF, 0xFF, 0xFF);
    static UnkColor_8040D2D8 color5(0xFE, 0x00, 0x00, 0x00);
    static UnkColor_8040D2D8 color6(0xFE, 0x00, 0x00, 0x00);
    static UnkColor_8040D2D8 color7(0xFE, 0x7F, 0x7F, 0x7F);

    UnkMaterialAnim_801621F0 *mat1 = fn_801621F0(m_model, "mat1");
    UnkMaterialAnim_801621F0 *mat2 = fn_801621F0(m_model, "mat2");
    UnkMaterialAnim_801621F0 *mat3 = fn_801621F0(m_model, "mat3");
    UnkMaterialAnim_801621F0 *mat4_1 = fn_801621F0(m_model, "mat4_1");
    UnkMaterialAnim_801621F0 *mat4_2 = fn_801621F0(m_model, "mat4_2");
    UnkMaterialAnim_801621F0 *mat4_3 = fn_801621F0(m_model, "mat4_3");
    UnkMaterialAnim_801621F0 *mat4_4 = fn_801621F0(m_model, "mat4_4");
    UnkMaterialAnim_801621F0 *mat4_5 = fn_801621F0(m_model, "mat4_5");
    UnkMaterialAnim_801621F0 *mat5 = fn_801621F0(m_model, "mat5");

    m_time += dt;
    float t = m_time;
    if (mat1 != 0) {
        if (0.0f <= t && t < 0.2f) {
            fn_80154C6C(mat1, &color0, &color1, t, 0.0f, 0.2f);
        } else if (0.2f <= t && t < 0.8f) {
            fn_80154C6C(mat1, &color1, &color2, t, 0.2f, 0.8f);
        } else {
            fn_80154C6C(mat1, &color2, &color3, t, 0.8f, 1.0f);
        }
    }
    if (mat2 != 0 && mat3 != 0 && mat4_1 != 0 && mat4_2 != 0 && mat4_3 != 0 && mat4_4 != 0 &&
        mat4_5 != 0 && 0.8f <= t) {
        fn_80154C6C(mat2, &color4, &color5, t, 0.8f, 1.0f);
        fn_80154C6C(mat3, &color4, &color5, t, 0.8f, 1.0f);
        fn_80154C6C(mat4_1, &color4, &color5, t, 0.8f, 1.0f);
        fn_80154C6C(mat4_2, &color4, &color5, t, 0.8f, 1.0f);
        fn_80154C6C(mat4_3, &color4, &color5, t, 0.8f, 1.0f);
        fn_80154C6C(mat4_4, &color4, &color5, t, 0.8f, 1.0f);
        fn_80154C6C(mat4_5, &color4, &color5, t, 0.8f, 1.0f);
    }
    if (mat5 != 0) {
        if (0.0f <= t && t < 0.1f) {
            fn_80154C6C(mat5, &color6, &color7, t, 0.0f, 0.1f);
        } else if (0.8f <= t) {
            fn_80154C6C(mat5, &color7, &color6, t, 0.8f, 1.0f);
        }
    }
}

void ElecCoreProgram::UpdateIdle(float dt) {
    m_time += dt;

    static UnkColor_8040D2D8 color0(0xFE, 0x7F, 0x66, 0x00);
    static UnkColor_8040D2D8 color1(0xFE, 0xB2, 0xB2, 0xB2);

    float t = Wrap(m_time, 1.0f / 3.0f);
    UnkMaterialAnim_801621F0 *mat1 = fn_801621F0(m_model, "mat1");
    if (mat1 != 0) {
        if (0.0f <= t && t < 0.25f) {
            fn_80154C6C(mat1, &color0, &color1, t, 0.0f, 0.25f);
        } else {
            fn_80154C6C(mat1, &color1, &color0, t, 0.25f, 1.0f / 3.0f);
        }
    }

    if (m_flashTime > 0.0f) {
        m_flashTime -= dt;
        if (m_flashTime < 0.0f) {
            m_flashTime = 0.0f;
        }
    } else {
        UnkMaterialAnim_801621F0 *mat2 = fn_801621F0(m_model, "mat2");
        if (mat2 != 0) {
            if (m_hits <= 1) {
                mat2->color.r = 0x40;
                mat2->color.g = 0x00;
                mat2->color.b = 0x00;
                mat2->color.a = 0x4C;
            } else if (m_hits <= 8) {
                mat2->color.r = 0x40;
                mat2->color.g = 0x7F;
                mat2->color.b = 0x00;
                mat2->color.a = 0x00;
            } else {
                mat2->color.r = 0x40;
                mat2->color.g = 0x00;
                mat2->color.b = 0x4D;
                mat2->color.a = 0x00;
            }
        }
    }
}

/* Not called anywhere: the linker strips it from the game. The original pools the int-to-float
 * conversion constant ahead of UpdateIdle's literals, which is what compiling an out-of-line copy
 * of Wrap at this point of the file does (inferred; the original function is unknown, and the
 * name is a placeholder). */
float WrapOutOfLine(float x, float period) {
    return Wrap(x, period);
}

void ElecCoreProgram::ReloadPlacement(UnkPlacement_800C9CD0 *owner) {
    ReadPlacement(owner);
    RebuildTransform();
    if (m_model != 0) {
        m_model->fn_80162EF4();
        m_model->fn_8016108C();
    }
}

void ElecCoreProgram::ReadPlacement(UnkPlacement_800C9CD0 *owner) {
    fn_800C9F14(owner, &m_chara->m_position);
    fn_800C9ED4(owner, &m_chara->m_rotation);
}

void ElecCoreProgram::RebuildTransform() {
    if (m_model != 0) {
        Mtx34f matrix;

        fn_8040C148(&matrix, &lbl_805E0720);
        fn_8040B59C(&matrix, &m_chara->m_position);
        fn_8040B848(&matrix, m_chara->m_rotation.y);
        m_model->fn_801633DC(&matrix);
    }

    if (m_target.get() != 0) {
        m_target->fn_800CCE38(&m_chara->m_position);
    }

    if (m_sound.get() != 0) {
        Vec3f offset(0.0f, 34.0f, 0.0f);
        Vec3f position;

        fn_8040CDD0(&position, &offset, &m_chara->m_position);
        fn_802B44C8(m_sound.px, &position);
    }
}

void ElecCoreProgram::OnCommand(CharCommand &command) {}

/* The tilts (about x and z) and the turning speed of the five turning frames (names inferred);
 * the speeds are set at startup. */
static float lbl_80541F58[5][3] = {
    {-3.0f, -5.0f, 2.0f * lbl_805FAD38},
    {0.0f, 30.0f, -2.0f * lbl_805FAD38 / 1.5f},
    {50.0f, 5.0f, -2.0f * lbl_805FAD38 / 1.2f},
    {30.0f, 70.0f, 2.0f * lbl_805FAD38 / 0.8f},
    {-30.0f, -50.0f, 2.0f * lbl_805FAD38 / 0.9f},
};

void ElecCoreProgram::UpdateRings(float dt) {
    if (m_model != 0) {
        ElecCoreProgramFrameSearch search;

        for (int i = 0; i < 5; i++) {
            search.index = i + 3;
            search.frame = 0;
            fn_8015FA78(m_model, FindFrame, &search);
            m_frames[search.index] = search.frame;
            m_frameMatrices[search.index] = search.frame->modelling;
            m_angles[search.index] += dt * lbl_80541F58[i][2];
            fn_80109AF8(m_frames[search.index], lbl_80541F58[i][1], 0);
            fn_800E9C64(m_frames[search.index], lbl_80541F58[i][0], 1);
            fn_800E9CEC(m_frames[search.index], m_angles[search.index], 1);
            fn_804876E8(m_frames[search.index], &m_frameMatrices[search.index], 2);
        }
    }
}

void ElecCoreProgram::UpdateFrameScales() {
    if (m_model != 0) {
        ElecCoreProgramFrameSearch search;

        search.index = 0;
        search.frame = 0;
        fn_8015FA78(m_model, FindFrame, &search);
        m_frames[0] = search.frame;
        m_frameMatrices[0] = search.frame->modelling;
        search.index = 8;
        search.frame = 0;
        fn_8015FA78(m_model, FindFrame, &search);
        m_frames[8] = search.frame;
        m_frameMatrices[8] = search.frame->modelling;

        float t = m_time;
        if (m_frames[0] != 0) {
            float s;
            if (t < 0.2f) {
                s = Interpolate(t, 0.0f, 0.2f, 1.0f, 0.1f);
            } else if (t < 0.8f) {
                s = Interpolate(t, 0.2f, 0.8f, 0.1f, 20.0f);
            } else {
                s = Interpolate(t, 0.8f, 1.0f, 20.0f, 30.0f);
            }
            RwV3d scale = {0.0f, 0.0f, 0.0f};
            scale.x = s;
            scale.y = s;
            scale.z = s;
            fn_8048764C(m_frames[0], &scale, 0);
            fn_804876E8(m_frames[0], &m_frameMatrices[0], 2);
        }
        if (m_frames[8] != 0) {
            float s;
            if (t < 0.1f) {
                s = Interpolate(t, 0.0f, 0.1f, 1.0f, 20.0f);
            } else if (t < 0.8f) {
                s = Interpolate(t, 0.1f, 0.8f, 20.0f, 160.0f);
            } else {
                s = Interpolate(t, 0.8f, 1.0f, 160.0f, 200.0f);
            }
            RwV3d scale = {0.0f, 0.0f, 0.0f};
            scale.x = s;
            scale.y = s;
            scale.z = s;
            fn_8048764C(m_frames[8], &scale, 0);
            fn_804876E8(m_frames[8], &m_frameMatrices[8], 2);
        }
    }
}

void ElecCoreProgram::OnAttack(AttackCallbackParam &param) {
    if (0.0f == m_flashTime && m_state == 0) {
        UnkMaterialAnim_801621F0 *mat2 = fn_801621F0(m_model, "mat2");
        if (mat2 != 0) {
            mat2->color.r = 0x60;
            mat2->color.g = 0xC0;
            mat2->color.b = 0x40;
            mat2->color.a = 0x40;
        }
        m_flashTime = 0.05f;
        m_hits--;
        if (m_hits <= 0) {
            param.m_result = 3;
            m_state = 1;
            m_time = 0.0f;
            fn_801D54F4(fn_80053164(), 0x7241, m_chara.get(), 0, 1);
        } else {
            param.m_result = 1;
            fn_801D54F4(fn_80053164(), 0x7240, m_chara.get(), 0, 1);
        }
    } else {
        param.m_result = 8;
    }
}

extern "C" void fn_801F44F0(void) {
    fn_8006B474("CORE.BIN", 1);
}

extern "C" void fn_801F44EC(void) {}

void ElecCoreProgram::Instantiate(UnkPlacement_800C9CD0 *owner) {
    new ElecCoreProgram(fn_801EB304(fn_8001C4F8(), 11), owner);
}

GadgetClassDesc lbl_80541F94 = {
    "ELEC_CORE", fn_801F44F0, fn_801F44EC, ElecCoreProgram::Instantiate, 0, 0, 0x7DB, 1, 0, 10, 0,
};
