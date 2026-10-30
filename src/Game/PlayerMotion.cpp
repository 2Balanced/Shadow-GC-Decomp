/* Player::PlayerMotion (class name from RTTI; declared in Game/PlayerWeaponBase.h): the animation
 * of a player character. It plays the character's motions on its motion work, passes the motion's
 * events to the player (its event callback, voices, the weapon), keeps the nodes of the skeleton
 * that other code follows, fades the model's colour and hides the effects that follow the model
 * with the weapon. File name inferred from the class; other names marked "(inferred)" are not
 * original. Functions other units call by address keep their fn_ names (C functions taking the
 * motion). Definitions are in reverse address order (deferred inlining emits functions
 * last-defined first). */
/* boost::function's operator() puts a bad_function_call on the stack (see the MSL's stdexcept). */
#define MSL_RUNTIME_ERROR_STRING
#define PLAYER_MOTION_EVENT_CALL
#define PLAYER_MOTION_MEMBERS
#include "Effect/System/EffectElement.h"
#include "Game/MathTypes.h"
#include "Game/Player.h"
#include "Game/PlayerMotionEvent.h"
#include "Game/PlayerSoundBase.h"
#include "Game/PlayerWeaponBase.h"
#include "Game/Task.h"
#include "boost/shared_ptr.hpp"
#include "boost/weak_ptr.hpp"
#include <dolphin/types.h>
#include "size_assert.h"

/* The player's members used here (struct name unknown; Player::PlayerBase leaves them unmapped). */
struct UnkPlayerFields_800AA0D4 {
    /* 0x000 */ u8 field_0x0[0x60];
    /* 0x060 */ float *m_frameTime; /* [1]: the frame's time scale (inferred) */
    /* 0x064 */ u8 field_0x64[0xA8 - 0x64];
    /* 0x0A8 */ u32 m_stateBits[2];
    /* 0x0B0 */ u8 field_0xB0[0xB4 - 0xB0];
    /* 0x0B4 */ u32 m_motionFlags; /* bits set while the motion's flags 0..15 are on (inferred) */
    /* 0x0B8 */ u8 field_0xB8[0x234 - 0xB8];
    /* 0x234 */ void *m_voice;
    /* 0x238 */ Player::WeaponBase *m_weapon;
    /* 0x23C */ void *m_effect;
    /* 0x240 */ Player::SoundBase *m_sound;
};

#define Fields(player) reinterpret_cast<UnkPlayerFields_800AA0D4 *>(player)

/* The frames of a motion (struct name unknown). */
struct UnkMotionRange_804125F4 {
    /* 0x0 */ float m_start;
    /* 0x4 */ float m_end;
    /* 0x8 */ u8 field_0x8[0x10 - 0x8];
};

struct UnkMotionFrames_80415D40 {
    /* 0x0 */ float m_start;
    /* 0x4 */ float m_end;
    /* 0x8 */ float field_0x8;
    /* 0xC */ float m_frame;
};

/* What a motion's events carry (struct name unknown): event 0xD000 holds the voice to play (its
 * arguments and volume; names inferred). */
struct UnkMotionVoice_800AA0D4 {
    /* 0x0 */ s32 m_id;
    /* 0x4 */ s32 m_arg;
    /* 0x8 */ u8 field_0x8[0xC - 0x8];
    /* 0xC */ float m_volume;
};

extern "C" {
bool fn_80014E34(u32 *flags, int bit);
void fn_80014E1C(u32 *flags, int bit);
void fn_8006127C(u32 *flags, int bit);
void fn_80014E10(u32 *flags);
bool fn_8006CD78(u32 *bits, int bit);
bool fn_8000A950(float value);
const float &fn_80070460(const float &value, const float &low, const float &high);
void *memset(void *dst, int value, u32 size);

/* Vectors and matrices. */
Vec3f fn_8040FB08(const Mtx34f *matrix);
Vec3f fn_8040B618(const Mtx34f *matrix, const Vec3f *vector);
void fn_8040CE60(Vec3f *vector);

/* The motion work (behaviour inferred). */
int fn_804125F4(UnkMotionRange_804125F4 *range, u16 motion);
void fn_80415D40(_stMtnPlayWork *work, int track, UnkMotionFrames_80415D40 *frames);
int fn_80415854(_stMtnPlayWork *work, int track, u16 *event, void **data, int arg4);
int fn_80416F28(_stMtnPlayWork *work, int track, int flag, int arg3);
void fn_804174F0(_stMtnPlayWork *work, int track, u16 motion);
void fn_80416DC8(_stMtnPlayWork *work, int track, float speed);
int fn_804165D8(_stMtnPlayWork *work, int track, int arg2, int arg3);
void fn_80416148(_stMtnPlayWork *work, int track, float time);
void fn_80416198(_stMtnPlayWork *work, int track);
void fn_80417568(_stMtnPlayWork *work, u16 motion);
void fn_80416014(_stMtnPlayWork *work, int track, float frame);
void fn_80416E20(_stMtnPlayWork *work, int track, float frame);

/* The player's effects and voices (behaviour inferred). */
void fn_800A29C0(void *effect);
void fn_800A2978(void *effect, void *data);
void fn_80077974(void *voice, s32 id, s32 arg, float volume);
void fn_8007792C(void *voice);
}

/* fn_8040CD74 (a - b) called as returning its result by value. */
typedef Vec3f (*SubtractFn)(const Vec3f *a, const Vec3f *b);
extern "C" void fn_8040CD74(Vec3f *out, const Vec3f *lhs, const Vec3f *rhs);
#define Subtract ((SubtractFn)fn_8040CD74)

/* The limits of the weapon arm's blend and of the colour fade's alpha. */
#pragma explicit_zero_data on
static float sBlendMin = 0.0f;
static float sBlendMax = 1.0f;
static float sAlphaMin = 0.0f;
static float sAlphaMax = 1.0f;
#pragma explicit_zero_data reset

namespace Player {

/* The slots of PlayerMotion's vtable called here with the arguments their bodies take (a view:
 * PlayerMotion declares them without arguments; see Game/PlayerShadowSuper.cpp). Slot 0x2C sets
 * and slot 0x30 reads a value of a track (the weapon arm's blend for track 1; names inferred). */
class UnkMotionSlots_800AA7C8 : public Task {
  public:
    virtual ~UnkMotionSlots_800AA7C8();
    virtual void vf_0x10();
    virtual void vf_0x14();
    virtual void vf_0x18();
    virtual void vf_0x1C();
    virtual void vf_0x20();
    virtual void vf_0x24();
    virtual void vf_0x28();
    virtual void SetBlend(int track, float blend);
    virtual float GetBlend(int track);
};

#define Slots(motion) reinterpret_cast< ::Player::UnkMotionSlots_800AA7C8 *>(motion)

/* The effects the weapon hides (indices into m_effects). */
static const u32 sWeaponEffects[3] = {0, 3, 4};

void UnkMotionNode_800AA760::Update() {
    if (m_used) {
        if (m_matrix != 0) {
            Vec3f position;
            fn_800091EC(&position, &(const Vec3f &)fn_8040FB08(m_matrix));
            fn_800091D0(&m_front, &(const Vec3f &)fn_8040B618(m_matrix, &Vec3f(0.0f, 0.0f, -1.0f)));
            fn_800091D0(&m_up, &(const Vec3f &)fn_8040B618(m_matrix, &Vec3f(0.0f, 1.0f, 0.0f)));
            fn_800091D0(&m_move, &(const Vec3f &)Subtract(&position, &m_position));
            fn_800091D0(&m_position, &position);
        } else {
            fn_800091D0(&m_front, &Vec3f(0.0f, 0.0f, -1.0f));
            fn_800091D0(&m_up, &Vec3f(0.0f, 1.0f, 0.0f));
            fn_8040CE60(&m_move);
            fn_8040CE60(&m_position);
        }
    }
}

PlayerMotion::PlayerMotion(Task *parent, const boost::shared_ptr<PlayerBase> &player)
    : Task(parent), m_player(player) {
    m_player->m_motion = this;
    m_name = "Motion";
    fn_80014E10(&m_flags.m_bits);
    m_motionBase = 0;
    m_work = 0;
    m_fadeMode = 0;
    m_step = 0;
    memset(m_nodes, 0, sizeof(m_nodes));
    for (int i = 0; i < 2; i++) {
        (&m_nextMotion)[i] = -1;
    }
    m_prevMotionId = 0x30;
    m_motionId = 0x30;
    m_blendTime = 0.0f;
    m_startFrame = -1.0f;
    *reinterpret_cast<float *>(&field_0x438) = 0.0f;
    m_speed = 1.0f;
    for (int i = 0; i < 2; i++) {
        (&m_playingMotion)[i] = -1;
    }
    *reinterpret_cast<float *>(&field_0x444[4]) = 0.0f;
}

PlayerMotion::~PlayerMotion() {
    m_player->m_motion = 0;
    for (int i = 0; i < 6; i++) {
        if (!m_effects[i].expired()) {
            m_effects[i].lock()->Retire();
        }
    }
}

void PlayerMotion::Tick(float dt) {
    float frameTime = *Fields(m_player.get())->m_frameTime;
    float blend = Slots(this)->GetBlend(1);
    if (fn_80014E34(&m_flags.m_bits, 0)) {
        blend += 60.0f * (0.333f * frameTime);
    } else {
        blend -= 60.0f * (0.0625f * frameTime);
    }
    blend = fn_80070460(blend, sBlendMin, sBlendMax);
    Slots(this)->SetBlend(1, blend);
    bool hidden = m_player->IsWeaponHidden();
    for (u32 i = 0; i < 3; i++) {
        boost::shared_ptr<Effect::System::EffectElement> effect = m_effects[sWeaponEffects[i]].lock();
        if (effect) {
            if (hidden) {
                effect->Hide();
            } else {
                effect->Show();
            }
        }
    }
}

} // namespace Player

extern "C" {

/* Sets the time the next motion starts at (inferred). */
void fn_800AA7C0(Player::PlayerMotion *motion, float frame) {
    motion->m_startFrame = frame;
}

void fn_800AA7B8(Player::PlayerMotion *motion, float value) {
    *reinterpret_cast<float *>(&motion->field_0x438) = value;
}

/* The motion playing. */
s32 fn_800AA7B0(Player::PlayerMotion *motion) {
    return motion->m_motionId;
}

/* A node of the skeleton, brought up to date the first time it is asked for. */
Player::UnkMotionNode_800AA760 *fn_800AA760(Player::PlayerMotion *motion, int index) {
    Player::UnkMotionNode_800AA760 *node = &motion->m_nodes[index];
    if (!node->m_used) {
        node->m_used = 1;
        node->Update();
    }
    return node;
}

/* The motion work. */
_stMtnPlayWork *fn_800AA758(Player::PlayerMotion *motion) {
    return motion->m_work;
}

/* Slot 0x34: the works of the extra parts (none here). */
void vf_0x34__Q26Player12PlayerMotionFv(Player::PlayerMotion *motion, _stMtnPlayWork **first,
                                        _stMtnPlayWork **second) {
    *first = 0;
    *second = 0;
}

/* Slot 0x38: which extra model to show (none here). */
int vf_0x38__Q26Player12PlayerMotionFv(Player::PlayerMotion *motion) {
    return 0;
}

/* Sets or clears motion flag 1. */
void fn_800AA700(Player::PlayerMotion *motion, bool on) {
    if (on) {
        fn_80014E1C(&motion->m_flags.m_bits, 1);
    } else {
        fn_8006127C(&motion->m_flags.m_bits, 1);
    }
}

/* Raises or lowers the weapon arm (motion flag 0). */
void fn_800AA6C0(Player::PlayerMotion *motion, bool on) {
    if (on) {
        fn_80014E1C(&motion->m_flags.m_bits, 0);
    } else {
        fn_8006127C(&motion->m_flags.m_bits, 0);
    }
}

/* Whether the weapon arm is fully raised. */
bool fn_800AA640(Player::PlayerMotion *motion) {
    float blend = Slots(motion)->GetBlend(1);
    bool raised = false;
    if (fn_80014E34(&motion->m_flags.m_bits, 0) && blend >= 1.0f) {
        raised = true;
    }
    return raised;
}

/* Sets the colour fade's mode. */
void fn_800AA638(Player::PlayerMotion *motion, s32 mode) {
    motion->m_fadeMode = mode;
}

/* The colour fade's mode: 3 while the weapon fires, else the mode set. */
void fn_800AA5D4(Player::PlayerMotion *motion) {
    Player::WeaponBase *weapon = Fields(motion->m_player.get())->m_weapon;
    if (weapon != 0 && weapon->IsFiring()) {
        motion->m_fade.m_mode = 3;
    } else {
        motion->m_fade.m_mode = motion->m_fadeMode;
    }
}

/* Sets the colour fade's alpha (0 to 1). */
void fn_800AA58C(Player::PlayerMotion *motion, float alpha) {
    alpha = fn_80070460(alpha, sAlphaMin, sAlphaMax);
    motion->m_fade.m_color[3] = alpha;
}

/* The colour fade. */
UnkClumpFade_80027D5C *fn_800AA584(Player::PlayerMotion *motion) {
    return &motion->m_fade;
}

/* Starts the motions asked for. */
void fn_800AA404(Player::PlayerMotion *motion) {
    if (motion->m_work != 0) {
        bool changed = false;
        for (int i = 0; i < 2; i++) {
            if ((&motion->m_playingMotion)[i] != (&motion->m_nextMotion)[i]) {
                changed = true;
            }
        }
        if (changed) {
            for (int i = 0; i < 2; i++) {
                (&motion->m_playingMotion)[i] = -1;
            }
            motion->m_playingMotion = motion->m_nextMotion;
            fn_80416DC8(motion->m_work, 0, 0.0f);
            if (motion->m_blendTime > 1.0f) {
                fn_804165D8(motion->m_work, 0, -1, -1);
                fn_80416148(motion->m_work, 0, motion->m_blendTime);
            } else {
                fn_80416198(motion->m_work, 0);
            }
            if (motion->m_playingMotion >= 0) {
                fn_80417568(motion->m_work, motion->m_playingMotion);
            }
            fn_80416F28(motion->m_work, 0, 0xB, 0);
            fn_80416014(motion->m_work, 0, 1.0f);
        }
        s32 *playing = reinterpret_cast<s32 *>(motion->field_0x444);
        if (*playing != motion->m_nextMotionArg) {
            if (fn_804165D8(motion->m_work, 1, -1, -1) != 0) {
                *playing = motion->m_nextMotionArg;
                if (*playing >= 0) {
                    fn_80417568(motion->m_work, *playing);
                }
            }
        }
        fn_80416014(motion->m_work, 0, *reinterpret_cast<float *>(&motion->field_0x444[4]));
    }
}

/* Plays the motions asked for at once. */
void fn_800AA3A4(Player::PlayerMotion *motion) {
    s32 *playing = reinterpret_cast<s32 *>(motion->field_0x444);
    motion->m_playingMotion = motion->m_nextMotion;
    *playing = motion->m_nextMotionArg;
    fn_804174F0(motion->m_work, 0, *playing);
    fn_804174F0(motion->m_work, 1, motion->m_playingMotion);
}

/* Passes the motion's events of the frame on. */
void fn_800AA0D4(Player::PlayerMotion *motion) {
    _stMtnPlayWork *work = motion->m_work;
    UnkMotionVoice_800AA0D4 *voice = 0;
    bool use = false;
    u16 event;
    void *data;
    while (fn_80415854(work, 0, &event, &data, 0) != 0) {
        work = 0;
        if (PlayerMotionEventCallback(motion->m_player.get()).IsSet()) {
            Player::MotionEventArgs args;
            args.m_event = event;
            args.m_value = static_cast<s32 *>(data);
            PlayerMotionEventCallback(motion->m_player.get()).Call(args);
        }
        switch (event) {
        case 0x5000:
            switch (static_cast<s32 *>(data)[0]) {
            case 4:
            case 5:
            case 6:
                fn_800A29C0(Fields(motion->m_player.get())->m_effect);
                break;
            }
            break;
        case 0x5001:
            Fields(motion->m_player.get())
                ->m_sound->PlaySound(static_cast<s32 *>(data)[0], static_cast<s32 *>(data)[1]);
            break;
        case 0xD000:
            voice = static_cast<UnkMotionVoice_800AA0D4 *>(data);
            break;
        case 0xD001:
            use = true;
            break;
        case 0xD004:
            fn_800A2978(Fields(motion->m_player.get())->m_effect, data);
            break;
        }
    }
    if (Fields(motion->m_player.get())->m_voice != 0 &&
        !fn_8006CD78(Fields(motion->m_player.get())->m_stateBits, 0x35)) {
        if (voice != 0) {
            fn_80077974(Fields(motion->m_player.get())->m_voice, voice->m_id, voice->m_arg,
                        voice->m_volume);
        } else {
            fn_8007792C(Fields(motion->m_player.get())->m_voice);
        }
    }
    if (fn_8006CD78(Fields(motion->m_player.get())->m_stateBits, 0x1F) &&
        Fields(motion->m_player.get())->m_weapon->GetWeaponKind() == 5) {
        if (use) {
            Fields(motion->m_player.get())->m_weapon->BeginUse();
        } else {
            Fields(motion->m_player.get())->m_weapon->EndUse();
        }
    }
    for (int i = 0; i < 16; i++) {
        if (fn_80416F28(motion->m_work, 0, i, 1)) {
            fn_80014E1C(&Fields(motion->m_player.get())->m_motionFlags, i);
        } else {
            fn_8006127C(&Fields(motion->m_player.get())->m_motionFlags, i);
        }
    }
}

/* How far the motion has played (0 to 1). */
float fn_800AA064(Player::PlayerMotion *motion) {
    UnkMotionFrames_80415D40 frames;
    fn_80415D40(motion->m_work, 0, &frames);
    float length = frames.m_end - frames.m_start;
    if (fn_8000A950(length)) {
        return 0.0f;
    }
    return (frames.m_frame - frames.m_start) / length;
}

/* Sets the frame of the motions playing to t of their length. */
void fn_800A9F9C(Player::PlayerMotion *motion, float t) {
    UnkMotionRange_804125F4 range;
    if (*reinterpret_cast<s32 *>(motion->field_0x444) >= 0) {
        for (int i = 0; i < 2; i++) {
            fn_804125F4(&range, (&motion->m_playingMotion)[1 - i]);
            fn_80416E20(motion->m_work, i, range.m_start + t * (range.m_end - range.m_start));
        }
    } else if (fn_804125F4(&range, motion->m_playingMotion)) {
        fn_80416E20(motion->m_work, 0, range.m_start + t * (range.m_end - range.m_start));
    }
}

/* The motion's speed for the frame. */
float fn_800A9EF8(Player::PlayerMotion *motion) {
    float speed;
    if (*reinterpret_cast<s32 *>(motion->field_0x444) < 0) {
        speed = motion->m_speed;
    } else {
        UnkMotionRange_804125F4 first;
        UnkMotionRange_804125F4 second;
        fn_804125F4(&second, *reinterpret_cast<s32 *>(motion->field_0x444));
        fn_804125F4(&first, motion->m_playingMotion);
        float ratio = (first.m_end - first.m_start) / (second.m_end - second.m_start);
        speed = motion->m_speed /
                ((ratio - 1.0f) * (1.0f - *reinterpret_cast<float *>(&motion->field_0x444[4])) + 1.0f);
    }
    return speed * Fields(motion->m_player.get())->m_frameTime[1] + 1e-6f;
}

/* Brings the nodes that are in use up to date. */
void fn_800A9EA0(Player::PlayerMotion *motion) {
    for (int i = 0; i < 14; i++) {
        Player::UnkMotionNode_800AA760 *node = &motion->m_nodes[i];
        if (node->m_valid) {
            node->Update();
        }
    }
}

void fn_800A9E48(Player::PlayerMotion *motion) {
    for (int i = 0; i < 14; i++) {
        Player::UnkMotionNode_800AA760 *node = &motion->m_nodes[i];
        if (!node->m_valid) {
            node->Update();
        }
    }
}

/* Whether the player's motion flag 3 is on. */
bool fn_800A9E1C(boost::shared_ptr<Player::PlayerBase> *player) {
    return fn_80014E34(&Fields(player->get())->m_motionFlags, 3);
}

} // extern "C"

/* Not called anywhere: the linker strips it from the game. GC/2.7 pools a unit's float constants
 * in the order it emits the functions, and the original pooled the motion's 0.0 first (see
 * docs/research-notes.md); defined last, this is emitted first and gives that order (not
 * original). */
float PlayerMotionPoolZero() {
    return 0.0f;
}
