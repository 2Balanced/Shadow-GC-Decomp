/* The PJS effect files of the four effect kinds: the common effects, Diablon's, Shadow's and the
 * boss effects of the current stage. fn_801D2DE8 loads a kind's file (counting its users),
 * fn_801D2D14 releases it, and the fn_801D2AFC..fn_801D2C28 functions launch an effect of a kind
 * through the launch functions of Game/EffectPJS.cpp. The unit has no class of its own; its file
 * name and all names are inferred, and the functions keep their address names. Definitions are in
 * reverse address order (deferred inlining emits functions last-defined first). */
#include "Effect/System/EffectElement.h"
#include "Game/EffectPJSLauncher.h"
#include "Game/MathTypes.h"
#include "boost/shared_ptr.hpp"
#include "boost/weak_ptr.hpp"
#include "size_assert.h"

using Effect::PJS::Launcher;
using Effect::System::EffectElement;

/* The game state (as in Game/AdvBattleMode.cpp): the stage being played (name inferred). */
struct UnkGameState_8057D6A0 {
    /* 0x000 */ u8 field_0x0[0xAC];
    /* 0x0AC */ s32 m_stage;
    /* 0x0B0 */ u8 field_0xB0[0x150 - 0xB0];
};
SIZE_ASSERT(UnkGameState_8057D6A0, 0x150);

extern UnkGameState_8057D6A0 lbl_8057D6A0;

/* An effect kind's file: one file, or one per stage from m_firstStage to m_lastStage. */
struct UnkEffectFile_804D0294 {
    /* 0x0 */ const char *m_file;
    /* 0x4 */ const char **m_stageFiles;
    /* 0x8 */ s32 m_firstStage;
    /* 0xC */ s32 m_lastStage;
};

/* The boss effects of stages 0x1C to 0x2C. */
static const char *s_bossEffectFiles[17] = {
    "enemy/boss/Boss0210.one",
    "enemy/boss/Boss0310.one",
    "enemy/boss/Boss0510.one",
    "enemy/boss/Boss0310.one",
    "enemy/boss/Boss0210.one",
    "enemy/boss/Boss0510.one",
    "enemy/boss/Boss0310.one",
    "enemy/boss/BossBKlast.one",
    "enemy/boss/diabloneffect.one",
    "enemy/boss/EggLastMechEffect.one",
    "enemy/boss/diabloneffect.one",
    "enemy/boss/EggLastMechEffect.one",
    "enemy/boss/EggLastMechEffect.one",
    "enemy/boss/BossBKlast.one",
    "enemy/boss/BossBKlast.one",
    "enemy/boss/diabloneffect.one",
    "enemy/boss/BossDevilDoom.one",
};

static const UnkEffectFile_804D0294 s_effectFiles[4] = {
    {"common/CommonEffect.one", 0, -1, -1},
    {"enemy/boss/diabloneffect.one", 0, -1, -1},
    {"common/SShadowEffect.one", 0, -1, -1},
    {0, s_bossEffectFiles, 0x1C, 0x2C},
};

/* The launcher of each kind's file while it is loaded. */
static boost::shared_ptr<Launcher> s_launchers[4];

/* boost/shared_ptr.hpp only declares shared_ptr::reset, which the game has out of line; this unit
 * emits the copy for Launcher (0x801D2D6C; boost 1.32's definition). */
namespace boost {
template <> inline void shared_ptr<Launcher>::reset() {
    this_type().swap(*this);
}
} // namespace boost

extern "C" {
/* Game/EffectPJS.cpp: adds an effect file to a resource slot, and the launch functions (the
 * launcher comes first; the effect id and its owner follow the placement). */
boost::shared_ptr<Launcher> fn_80304F40(const char *fileName);
boost::weak_ptr<EffectElement> fn_8030536C(Launcher *launcher, const Vec3f *position,
                                           const Vec3f &direction, s32 id, u32 owner);
boost::weak_ptr<EffectElement> fn_803054D0(Launcher *launcher, const Vec3f *position,
                                           const Vec2f &angles, s32 id, u32 owner);
boost::weak_ptr<EffectElement> fn_803055F8(Launcher *launcher, const Mtx34f *matrix, s32 id,
                                           u32 owner);
boost::weak_ptr<EffectElement> fn_803056F8(Launcher *launcher, s32 id, u32 owner);
boost::weak_ptr<EffectElement> fn_803057EC(Launcher *launcher, u32 value4C, u32 value50, s32 id,
                                           u32 owner);
boost::weak_ptr<EffectElement> fn_80305900(Launcher *launcher, const Mtx34f *matrix, s32 id,
                                           u32 owner);

/* Loads the file of effect kind kind, if this stage has one (fn_801D2DE8). */
void fn_801D2DE8(int kind) {
    if (s_effectFiles[kind].m_stageFiles != 0) {
        s32 stage = lbl_8057D6A0.m_stage;
        if (s_effectFiles[kind].m_firstStage > stage || s_effectFiles[kind].m_lastStage < stage) {
            return;
        }
        boost::shared_ptr<Launcher> &launcher = s_launchers[kind];
        if (launcher.px == 0) {
            launcher = fn_80304F40(
                s_effectFiles[kind].m_stageFiles[stage - s_effectFiles[kind].m_firstStage]);
        }
    } else {
        boost::shared_ptr<Launcher> &launcher = s_launchers[kind];
        if (launcher.px == 0) {
            launcher = fn_80304F40(s_effectFiles[kind].m_file);
        }
    }
    Launcher *launcher = s_launchers[kind].px;
    if (launcher != 0) {
        launcher->field_0x10++;
    }
}

/* Releases a user of effect kind kind's file, and the file with its last user (fn_801D2D14). */
void fn_801D2D14(int kind) {
    boost::shared_ptr<Launcher> &launcher = s_launchers[kind];
    if (launcher.px != 0) {
        launcher.px->field_0x10--;
        if (launcher.px->field_0x10 == 0) {
            launcher.reset();
        }
    }
}

/* The launcher of effect kind kind (fn_801D2CA4). */
boost::shared_ptr<Launcher> fn_801D2CA4(int kind) {
    return s_launchers[kind];
}

/* Whether effect kind kind's file is loaded (fn_801D2C64). */
bool fn_801D2C64(int kind) {
    if (s_launchers[kind].px == 0) {
        return false;
    }
    return s_launchers[kind].px->IsLoaded();
}

/* Launch an effect of kind kind (fn_801D2C28..fn_801D2AFC; see the launch functions). */
boost::weak_ptr<EffectElement> fn_801D2C28(int kind, const Mtx34f *matrix, s32 id, u32 owner) {
    return fn_80305900(s_launchers[kind].px, matrix, id, owner);
}

boost::weak_ptr<EffectElement> fn_801D2BEC(int kind, u32 value4C, u32 value50, s32 id,
                                           u32 owner) {
    return fn_803057EC(s_launchers[kind].px, value4C, value50, id, owner);
}

boost::weak_ptr<EffectElement> fn_801D2BB0(int kind, s32 id, u32 owner) {
    return fn_803056F8(s_launchers[kind].px, id, owner);
}

boost::weak_ptr<EffectElement> fn_801D2B74(int kind, const Mtx34f *matrix, s32 id, u32 owner) {
    return fn_803055F8(s_launchers[kind].px, matrix, id, owner);
}

boost::weak_ptr<EffectElement> fn_801D2B38(int kind, const Vec3f *position, const Vec2f &angles,
                                           s32 id, u32 owner) {
    return fn_803054D0(s_launchers[kind].px, position, angles, id, owner);
}

boost::weak_ptr<EffectElement> fn_801D2AFC(int kind, const Vec3f *position,
                                           const Vec3f &direction, s32 id, u32 owner) {
    return fn_8030536C(s_launchers[kind].px, position, direction, id, owner);
}
}
