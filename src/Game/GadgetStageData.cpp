/* The GadgetStageDataStart and GadgetStageDataEnd gadget types (names from their records), whose
 * load and unload hooks set up and release the data of the current stage's gadgets (the stage's
 * stgNNNN_gdt.one file in loader slot 1 and its stgNNNN_fx.one effect launcher), and the effect
 * launch helpers gadget units call, which launch effects of the stage's launcher. The counterpart
 * of Game/GadgetCommonData.cpp; file name inferred from the records. Names of the free functions
 * are placeholders; their behaviour is inferred. Definitions are in reverse address order
 * (deferred inlining emits functions last-defined first).
 *
 * Left NonMatching: every function compiles to retail's code, but the unit cannot link while
 * symbols.txt names 0x80235B00 boost::shared_ptr<Effect::System::EffectLaunchInfo>::~shared_ptr
 * (from HeavyDog0410CtrlUnit and DevilDoomCtrlUnit). Retail's copy of that destructor for this
 * unit's s_stageEffects (whose reset at 0x80168D98 DevilDoom.cpp also calls) is 0x80168E04, and a
 * weak instance has one retail address, so one of the two names is wrong. */
#define BOOST_SP_CONVERTING_ASSIGNMENT

#include "Effect/System/EffectElement.h"
#include "Game/EffectPJSLauncher.h"
#include "Game/GadgetClassDesc.h"
#include "Game/ResourceOneFile.h"
#include "Game/ResourceOneFileReq.h"
#include "Game/MathTypes.h"
#include "boost/shared_ptr.hpp"
#include "boost/weak_ptr.hpp"
#include "size_assert.h"
#include <cstdio>

using Effect::System::EffectElement;
using Effect::System::EffectLaunchInfo;

/* The effect manager fn_800A31F0 returns (class name unknown): its second virtual function
 * launches effect id of a launcher (as in Game/EffectPJS.cpp). */
class UnkEffectManager_800A31F0 {
  public:
    virtual void vf08();
    virtual boost::weak_ptr<EffectElement> Launch(EffectLaunchInfo *info, s32 id);
};

/* The file system fn_800371D8 returns (class name unknown): the function at 0x4 tells whether a
 * file exists, and is handed the file system after the path (behaviour inferred). */
struct UnkFileSystem_800371D8 {
    /* 0x0 */ u32 field_0x0;
    /* 0x4 */ bool (*exists)(const char *path, UnkFileSystem_800371D8 *fileSystem);
};

/* The game state (layout as in Game/CityBombSmall.cpp; the field at 0xA8 numbers the stage's
 * files, name inferred). */
struct UnkGameState_8057D6A0 {
    /* 0x000 */ u8 field_0x0[0xA8];
    /* 0x0A8 */ s32 m_stageFile;
    /* 0x0AC */ u8 field_0xAC[0x150 - 0xAC];
};
SIZE_ASSERT(UnkGameState_8057D6A0, 0x150);

extern "C" {
UnkEffectManager_800A31F0 *fn_800A31F0(void);

/* Sets an effect's position, rotation and matrix (see Game/EffectPJS.cpp). */
void fn_80425E4C(EffectElement *effect, const Vec3f *position);
void fn_80425D94(EffectElement *effect, const Vec3f *rotation);
void fn_80425E68(EffectElement *effect, const Mtx34f *matrix);

/* A direction's rotation (see Game/EffectPJS.cpp), and the pitch and yaw of a direction (see
 * Game/ChaoHolder.cpp). */
Vec3f fn_80016284(const Vec3f &direction);
void fn_8003B9E8(const Vec3f *direction, float *pitch, float *yaw);

/* The file system (behaviour inferred). */
UnkFileSystem_800371D8 *fn_800371D8(void);
/* Adds PJS effect file fileName to a resource slot and returns its launcher (see
 * Game/EffectPJS.cpp). */
boost::shared_ptr<Effect::PJS::Launcher> fn_80304F40(const char *fileName);
/* Sets up gadget data the stage start needs (name unknown; see Game/GadgetCommonData.cpp). */
void fn_8006B5A0(void);

extern UnkGameState_8057D6A0 lbl_8057D6A0;
}

/* Whether file path exists (name inferred). */
inline bool FileExists(const char *path) {
    UnkFileSystem_800371D8 *fileSystem = fn_800371D8();
    return fileSystem->exists(path, fileSystem);
}

/* What the stage's file request is handed (meaning unknown; see Game/GadgetCommonData.cpp). */
extern u8 lbl_80575EE4[];

/* boost 1.32's reset, which the trimmed shared_ptr.hpp declares out of line: this unit holds the
 * first copy of the instance for EffectLaunchInfo. */
template <> inline void boost::shared_ptr<EffectLaunchInfo>::reset() {
    this_type().swap(*this);
}

/* The launcher of the stage's effects (name inferred). */
boost::shared_ptr<EffectLaunchInfo> s_stageEffects;

/* GadgetStageDataStart's hooks. */
extern "C" void fn_80168FF4(void) {
    fn_8006B5A0();
}

extern "C" void fn_80168FF0(void) {}

/* GadgetStageDataEnd's load hook: puts the stage's gadget data file in loader slot 1 and loads its
 * effect file, when they exist. */
extern "C" void fn_80168E64(void) {
    char gdtPath[0x28];
    char fxPath[0x20];
    sprintf(gdtPath, "stg%04d/stg%04d_gdt.one", lbl_8057D6A0.m_stageFile,
            lbl_8057D6A0.m_stageFile);
    if (FileExists(gdtPath)) {
        fn_8001299C(fn_800122D4(), 1,
                    reinterpret_cast<ResourceOneFile *>(new ResourceOneFileReq(gdtPath, lbl_80575EE4)));
    }
    sprintf(fxPath, "stg%04d/stg%04d_fx.one", lbl_8057D6A0.m_stageFile, lbl_8057D6A0.m_stageFile);
    if (FileExists(fxPath)) {
        s_stageEffects = fn_80304F40(fxPath);
    }
}

/* GadgetStageDataEnd's unload hook. */
extern "C" void fn_80168D68(void) {
    s_stageEffects.reset();
    fn_800127F8(fn_800122D4(), 1);
}

extern "C" {

/* Launches stage effect id at position, facing direction. */
boost::weak_ptr<EffectElement> fn_80168C48(const Vec3f *position,
                                           const Vec3f *direction, int id) {
    boost::weak_ptr<EffectElement> effect;
    effect = fn_800A31F0()->Launch(s_stageEffects.get(), id);
    boost::shared_ptr<EffectElement> element(effect.lock());
    if (element.px != 0) {
        Vec3f angles;
        fn_8003B9E8(&(const Vec3f &)fn_80016284(*direction), &angles.x, &angles.y);
        angles.z = 0.0f;
        fn_80425E4C(element.px, position);
        fn_80425D94(element.px, &angles);
        element->ClearFlag4();
    }
    return effect;
}

/* Launches stage effect id at position, turned by rotation. */
boost::weak_ptr<EffectElement> fn_80168B38(const Vec3f *position,
                                           const Vec3f *rotation, int id) {
    boost::weak_ptr<EffectElement> effect;
    effect = fn_800A31F0()->Launch(s_stageEffects.get(), id);
    boost::shared_ptr<EffectElement> element(effect.lock());
    if (element.px != 0) {
        fn_80425E4C(element.px, position);
        Vec3f angles(rotation->x, rotation->y, rotation->z);
        fn_80425D94(element.px, &angles);
        element->ClearFlag4();
    }
    return effect;
}

/* Launches stage effect id placed by matrix. */
boost::weak_ptr<EffectElement> fn_80168A50(const Mtx34f *matrix, int id) {
    boost::weak_ptr<EffectElement> effect;
    effect = fn_800A31F0()->Launch(s_stageEffects.get(), id);
    boost::shared_ptr<EffectElement> element(effect.lock());
    if (element.px != 0) {
        fn_80425E68(element.px, matrix);
        element->ClearFlag4();
    }
    return effect;
}
}

GadgetClassDesc lbl_8052B678 = {
    "GadgetStageDataStart", fn_80168FF4, fn_80168FF0, 0, 0, 0x80, 0x2582, 1, 0, 0, 0,
};

GadgetClassDesc lbl_8052B69C = {
    "GadgetStageDataEnd", fn_80168E64, fn_80168D68, 0, 0, 0x80, 0x2583, 1, 0, 0, 0,
};
