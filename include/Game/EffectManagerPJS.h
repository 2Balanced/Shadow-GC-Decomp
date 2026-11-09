#ifndef GAME_EFFECTMANAGERPJS_H
#define GAME_EFFECTMANAGERPJS_H

/* The effect element classes and the game's PJS effect manager (class names and bases from RTTI).
 * Each kind of element has a portable class in Effect::System, a RenderWare class in Effect::RW3,
 * usually a game class in SonicteamUSA::System::Effect, and the PJS class in Effect::PJS that
 * Effect::PJS::EffectManagerPJS::CreateElement (Game/EffectManagerPJS.cpp) creates. Only what the
 * PJS units use is declared. All elements are built from the same three arguments; the classes'
 * own members are unknown and stand as padding up to their sizes. Member names other than the
 * classes' are inferred. */
#include "Effect/System/EffectDesc.h"
#include "Effect/System/EffectGlobalParam.h"
#include "Effect/System/EffectArranger.h"
#include "Effect/System/EffectLensFlare.h"
#include "Effect/System/EffectShimmer.h"
#include "Game/ClumpFade.h"
#include "Game/EffectPJSLauncher.h"
#include "size_assert.h"

#include <string>

namespace Effect {
namespace System {

class EffectLocus : public EffectElement {
  public:
    virtual ~EffectLocus();
    virtual void Update(float dt);
};

class EffectRay : public EffectElement {
  public:
    virtual ~EffectRay();
    virtual void Update(float dt);

    /* 0x054 */ u8 field_0x54[0x104 - 0x54];
    /* 0x104 */ u32 m_request; /* as EffectParticle's */
    /* 0x108 */ u32 m_config;  /* bits 2 and 3 pick the draw pass (as EffectParticle's) */
};

/* One particle of a particle effect (layout from Game/EffectParticleUSA.cpp). */
struct UnkParticle_80027DC4 {
    /* 0x00 */ u8 field_0x0[0x54];
    /* 0x54 */ u32 m_color[4]; /* red, green, blue, alpha: 8 bits each, from bit 22 */
    /* 0x64 */ u8 field_0x64[0x68 - 0x64];
    /* 0x68 */ UnkParticle_80027DC4 *m_next;
};

class EffectParticle : public EffectElement {
  public:
    virtual ~EffectParticle();
    virtual void Update(float dt);

    /* 0x054 */ u8 field_0x54[0xFC - 0x54];
    /* 0x0FC */ UnkParticle_80027DC4 *m_particles; /* the first particle */
    /* 0x100 */ u8 field_0x100[0x104 - 0x100];
    /* 0x104 */ u32 m_request; /* bit 1: show or hide requested, bit 2: hide (as EffectLocusRW3's
                                * field_0x108) */
    /* 0x108 */ u32 m_config;  /* bits 2 and 3 pick the draw pass, bit 4: has textures, bit 15:
                                * has a model */
    /* 0x10C */ s32 m_count;   /* the number of particles */
    /* 0x110 */ u8 field_0x110[0x13C - 0x110];
    /* 0x13C */ void *m_model; /* the particles' RenderWare model data */
};

class EffectObject : public EffectElement {
  public:
    virtual ~EffectObject();
    virtual void Update(float dt);
};

} // namespace System

namespace RW3 {

class EffectShimmerRW3 : public ::Effect::System::EffectShimmer {
  public:
    EffectShimmerRW3(const ::Effect::System::UnkEffectDesc_801D0AFC *desc,
                     ::Effect::System::EffectLaunchInfo *info,
                     ::Effect::System::EffectManager *manager);
    virtual ~EffectShimmerRW3();
    virtual void Update(float dt);
    virtual void Draw();
};

class EffectLensFlareRW3 : public ::Effect::System::EffectLensFlare {
  public:
    EffectLensFlareRW3(const ::Effect::System::UnkEffectDesc_801D0AFC *desc,
                       ::Effect::System::EffectLaunchInfo *info,
                       ::Effect::System::EffectManager *manager);
    virtual ~EffectLensFlareRW3();
    virtual void Update(float dt);
    /* Draws nothing (0x801D0F54). */
    virtual void Draw() {}

    /* 0x054 */ u8 field_0x54[0x128 - 0x54];
};
SIZE_ASSERT(EffectLensFlareRW3, 0x128);

class EffectLocusRW3 : public ::Effect::System::EffectLocus {
  public:
    EffectLocusRW3(const ::Effect::System::UnkEffectDesc_801D0AFC *desc,
                   ::Effect::System::EffectLaunchInfo *info,
                   ::Effect::System::EffectManager *manager);
    virtual ~EffectLocusRW3();
    virtual void Update(float dt);
    virtual void Draw();

    /* 0x054 */ u8 field_0x54[0x108 - 0x54];
    /* 0x108 */ u32 field_0x108; /* bit 1: show or hide requested, bit 2: hide (see
                                  * Game/EffectLocusUSA.cpp; meanings inferred) */
    /* 0x10C */ u8 field_0x10C[0x17C - 0x10C];
};
SIZE_ASSERT(EffectLocusRW3, 0x17C);

class EffectRayRW3 : public ::Effect::System::EffectRay {
  public:
    /* 0x8042D150. */
    EffectRayRW3(const ::Effect::System::UnkEffectDesc_801D0AFC *desc,
                 ::Effect::System::EffectLaunchInfo *info,
                 ::Effect::System::EffectManager *manager);
    virtual ~EffectRayRW3();
    virtual void Update(float dt);
    virtual void Draw();

    /* 0x10C */ u8 field_0x10C[0x138 - 0x10C];
};
SIZE_ASSERT(EffectRayRW3, 0x138);

class EffectArrangerRW3 : public ::Effect::System::EffectArranger {
  public:
    /* 0x8043418C. */
    EffectArrangerRW3(const ::Effect::System::UnkEffectDesc_801D0AFC *desc,
                      ::Effect::System::EffectLaunchInfo *info,
                      ::Effect::System::EffectManager *manager);
    virtual ~EffectArrangerRW3();

    /* 0x054 */ u8 field_0x54[0x104 - 0x54];
    /* The arranger's own copy of the launch description (Game/EffectArrangerUSA.cpp; inferred). */
    /* 0x104 */ ::Effect::System::EffectLaunchInfo *m_launchInfo;
    /* 0x108 */ u8 field_0x108[0x11C - 0x108];
};
SIZE_ASSERT(EffectArrangerRW3, 0x11C);

class EffectParticleRW3 : public ::Effect::System::EffectParticle {
  public:
    /* 0x80433CE0. */
    EffectParticleRW3(const ::Effect::System::UnkEffectDesc_801D0AFC *desc,
                      ::Effect::System::EffectLaunchInfo *info,
                      ::Effect::System::EffectManager *manager);
    virtual ~EffectParticleRW3();
    virtual void Update(float dt);
    virtual void Draw();

    /* 0x140 */ u8 field_0x140[0x148 - 0x140];
};
SIZE_ASSERT(EffectParticleRW3, 0x148);

class EffectObjectRW3 : public ::Effect::System::EffectObject {
  public:
    /* 0x80432C80. */
    EffectObjectRW3(const ::Effect::System::UnkEffectDesc_801D0AFC *desc,
                    ::Effect::System::EffectLaunchInfo *info,
                    ::Effect::System::EffectManager *manager);
    virtual ~EffectObjectRW3();
    virtual void Update(float dt);
    virtual void Draw();

    /* 0x054 */ u8 field_0x54[0x104 - 0x54];
    /* 0x104 */ u32 m_color[4]; /* red, green, blue, alpha: 8 bits each, from bit 22 (inferred;
                              * see Game/EffectObjectUSA.cpp) */
    /* 0x114 */ u8 field_0x114[0x124 - 0x114];
    /* 0x124 */ void *m_clump;   /* the model (a RenderWare clump) */
    /* 0x128 */ void *m_uvAnim;  /* its UV animation, if any */
    /* 0x12C */ void *m_world;   /* the world of the stage area the model is in */
    /* 0x130 */ bool m_inWorld;  /* set once the model is added to m_world */
    /* 0x131 */ u8 field_0x131[0x134 - 0x131];
};
SIZE_ASSERT(EffectObjectRW3, 0x134);

class EffectParticle3DRW3 : public ::Effect::System::EffectParticle {
  public:
    /* 0x804335E8. */
    EffectParticle3DRW3(const ::Effect::System::UnkEffectDesc_801D0AFC *desc,
                        ::Effect::System::EffectLaunchInfo *info,
                        ::Effect::System::EffectManager *manager);
    virtual ~EffectParticle3DRW3();
    virtual void Update(float dt);
    virtual void Draw();

    /* 0x140 */ void *m_clump;   /* the particles' model (a RenderWare clump) */
    /* 0x144 */ void *m_texDict; /* their texture dictionary */
    /* 0x148 */ void *m_world;   /* the world of the stage area the model is in */
    /* 0x14C */ u8 field_0x14C[0x17C - 0x14C];
};
SIZE_ASSERT(EffectParticle3DRW3, 0x17C);

} // namespace RW3
} // namespace Effect

namespace SonicteamUSA {
namespace System {
namespace Effect {

/* The game's element classes. Most of their destructors are inline (the PJS units emit
 * ~EffectLocusUSA, 0x801D0E1C); those of EffectArrangerUSA and EffectParticle3DUSA do more and are
 * not. The constructors other than EffectShimmerUSA's are not inline either. */
class EffectShimmerUSA : public ::Effect::RW3::EffectShimmerRW3 {
  public:
    EffectShimmerUSA(const ::Effect::System::UnkEffectDesc_801D0AFC *desc,
                     ::Effect::System::EffectLaunchInfo *info,
                     ::Effect::System::EffectManager *manager)
        : EffectShimmerRW3(desc, info, manager) {
        m_drawPass = 6;
    }
    virtual ~EffectShimmerUSA() {}

    /* 0x054 */ u8 field_0x54[0x1D8 - 0x54];
};
SIZE_ASSERT(EffectShimmerUSA, 0x1D8);

class EffectLocusUSA : public ::Effect::RW3::EffectLocusRW3 {
  public:
    EffectLocusUSA(const ::Effect::System::UnkEffectDesc_801D0AFC *desc,
                   ::Effect::System::EffectLaunchInfo *info,
                   ::Effect::System::EffectManager *manager);
    virtual ~EffectLocusUSA() {}
    virtual void Update(float dt);

    /* 0x17C */ s32 m_shownDrawPass; /* the draw pass to go back to when shown (inferred) */
};
SIZE_ASSERT(EffectLocusUSA, 0x180);

class EffectRayUSA : public ::Effect::RW3::EffectRayRW3 {
  public:
    EffectRayUSA(const ::Effect::System::UnkEffectDesc_801D0AFC *desc,
                 ::Effect::System::EffectLaunchInfo *info,
                 ::Effect::System::EffectManager *manager);
    virtual ~EffectRayUSA() {}
    virtual void Update(float dt);

    /* 0x138 */ s32 m_shownDrawPass; /* as EffectParticleUSA's */
};
SIZE_ASSERT(EffectRayUSA, 0x13C);

class EffectArrangerUSA : public ::Effect::RW3::EffectArrangerRW3 {
  public:
    EffectArrangerUSA(const ::Effect::System::UnkEffectDesc_801D0AFC *desc,
                      ::Effect::System::EffectLaunchInfo *info,
                      ::Effect::System::EffectManager *manager);
    virtual ~EffectArrangerUSA();
};
SIZE_ASSERT(EffectArrangerUSA, 0x11C);

class EffectParticleUSA : public ::Effect::RW3::EffectParticleRW3 {
  public:
    EffectParticleUSA(const ::Effect::System::UnkEffectDesc_801D0AFC *desc,
                      ::Effect::System::EffectLaunchInfo *info,
                      ::Effect::System::EffectManager *manager);
    virtual ~EffectParticleUSA() {}
    virtual void Update(float dt);

    /* 0x148 */ s32 m_shownDrawPass; /* the draw pass to go back to when shown (inferred) */
};
SIZE_ASSERT(EffectParticleUSA, 0x14C);

class EffectObjectUSA : public ::Effect::RW3::EffectObjectRW3 {
  public:
    EffectObjectUSA(const ::Effect::System::UnkEffectDesc_801D0AFC *desc,
                    ::Effect::System::EffectLaunchInfo *info,
                    ::Effect::System::EffectManager *manager);
    virtual ~EffectObjectUSA() {}
    virtual void Update(float dt);

    /* 0x134 */ UnkClumpFade_80027D5C m_fade; /* the model's colour (Game/EffectObjectUSA.cpp) */
};
SIZE_ASSERT(EffectObjectUSA, 0x160);

class EffectParticle3DUSA : public ::Effect::RW3::EffectParticle3DRW3 {
  public:
    EffectParticle3DUSA(const ::Effect::System::UnkEffectDesc_801D0AFC *desc,
                        ::Effect::System::EffectLaunchInfo *info,
                        ::Effect::System::EffectManager *manager);
    virtual ~EffectParticle3DUSA();
    virtual void Update(float dt);

    /* 0x17C */ s32 m_shownDrawPass;        /* as EffectParticleUSA's */
    /* 0x180 */ UnkClumpFade_80027D5C *m_fades; /* one colour fade per particle */
    /* 0x184 */ bool *m_fadeSet;            /* whether each fade is applied to its clump */
};
SIZE_ASSERT(EffectParticle3DUSA, 0x188);

} // namespace Effect
} // namespace System
} // namespace SonicteamUSA

namespace Sonicteam {
namespace System {

/* The singleton holder (class names from RTTI); its accessor is fn_800A31F0 for the PJS effect
 * manager. */
template <class T> class CreateStatic {};
template <class T, class Creator> class Singleton {};

} // namespace System
} // namespace Sonicteam

namespace Effect {
namespace PJS {

class EffectManagerPJS;

/* The PJS element classes add nothing to the game's: their constructors only set the vtable. */
class EffectShimmerPJS : public SonicteamUSA::System::Effect::EffectShimmerUSA {
  public:
    EffectShimmerPJS(const ::Effect::System::UnkEffectDesc_801D0AFC *desc,
                     ::Effect::System::EffectLaunchInfo *info,
                     ::Effect::System::EffectManager *manager)
        : EffectShimmerUSA(desc, info, manager) {}
    virtual ~EffectShimmerPJS() {}
};
SIZE_ASSERT(EffectShimmerPJS, 0x1D8);

class EffectLensFlarePJS : public ::Effect::RW3::EffectLensFlareRW3 {
  public:
    EffectLensFlarePJS(const ::Effect::System::UnkEffectDesc_801D0AFC *desc,
                       ::Effect::System::EffectLaunchInfo *info,
                       ::Effect::System::EffectManager *manager)
        : EffectLensFlareRW3(desc, info, manager) {}
    virtual ~EffectLensFlarePJS() {}
};
SIZE_ASSERT(EffectLensFlarePJS, 0x128);

class EffectGlobalParamPJS : public SonicteamUSA::System::Effect::EffectGlobalParamUSA {
  public:
    EffectGlobalParamPJS(const ::Effect::System::UnkEffectDesc_801D0AFC *desc,
                         ::Effect::System::EffectLaunchInfo *info,
                         ::Effect::System::EffectManager *manager)
        : EffectGlobalParamUSA(desc, info, manager) {}
    virtual ~EffectGlobalParamPJS() {}
};
SIZE_ASSERT(EffectGlobalParamPJS, 0x20C);

class EffectLocusPJS : public SonicteamUSA::System::Effect::EffectLocusUSA {
  public:
    EffectLocusPJS(const ::Effect::System::UnkEffectDesc_801D0AFC *desc,
                   ::Effect::System::EffectLaunchInfo *info,
                   ::Effect::System::EffectManager *manager)
        : EffectLocusUSA(desc, info, manager) {}
    virtual ~EffectLocusPJS() {}
};
SIZE_ASSERT(EffectLocusPJS, 0x180);

class EffectRayPJS : public SonicteamUSA::System::Effect::EffectRayUSA {
  public:
    EffectRayPJS(const ::Effect::System::UnkEffectDesc_801D0AFC *desc,
                 ::Effect::System::EffectLaunchInfo *info,
                 ::Effect::System::EffectManager *manager)
        : EffectRayUSA(desc, info, manager) {}
    virtual ~EffectRayPJS() {}
};
SIZE_ASSERT(EffectRayPJS, 0x13C);

class EffectArrangerPJS : public SonicteamUSA::System::Effect::EffectArrangerUSA {
  public:
    EffectArrangerPJS(const ::Effect::System::UnkEffectDesc_801D0AFC *desc,
                      ::Effect::System::EffectLaunchInfo *info,
                      ::Effect::System::EffectManager *manager)
        : EffectArrangerUSA(desc, info, manager) {}
    virtual ~EffectArrangerPJS() {}
};
SIZE_ASSERT(EffectArrangerPJS, 0x11C);

class EffectParticlePJS : public SonicteamUSA::System::Effect::EffectParticleUSA {
  public:
    EffectParticlePJS(const ::Effect::System::UnkEffectDesc_801D0AFC *desc,
                      ::Effect::System::EffectLaunchInfo *info,
                      ::Effect::System::EffectManager *manager)
        : EffectParticleUSA(desc, info, manager) {}
    virtual ~EffectParticlePJS() {}
};
SIZE_ASSERT(EffectParticlePJS, 0x14C);

/* EffectObjectPJS (Game/EffectObjectPJS.cpp) also loads the model's UV animation. */
class EffectObjectPJS : public SonicteamUSA::System::Effect::EffectObjectUSA {
  public:
    EffectObjectPJS(const ::Effect::System::UnkEffectDesc_801D0AFC *desc,
                    ::Effect::System::EffectLaunchInfo *info,
                    ::Effect::System::EffectManager *manager);
    virtual ~EffectObjectPJS();

    /* 0x160 */ std::string m_uvAnimName; /* the name the UV animation is registered under */
};
SIZE_ASSERT(EffectObjectPJS, 0x16C);

/* EffectParticle3DPJS (Game/EffectParticle3DPJS.cpp). */
class EffectParticle3DPJS : public SonicteamUSA::System::Effect::EffectParticle3DUSA {
  public:
    EffectParticle3DPJS(const ::Effect::System::UnkEffectDesc_801D0AFC *desc,
                        ::Effect::System::EffectLaunchInfo *info,
                        ::Effect::System::EffectManager *manager);
    virtual ~EffectParticle3DPJS();
};
SIZE_ASSERT(EffectParticle3DPJS, 0x188);

/* The game's effect manager (a singleton; fn_800A31F0 returns it). */
class EffectManagerPJS
    : public SonicteamUSA::System::Effect::EffectManagerUSA,
      public Sonicteam::System::Singleton<EffectManagerPJS,
                                          Sonicteam::System::CreateStatic<EffectManagerPJS> > {
  public:
    /* Out of line where Game/PlayerEffect.cpp makes the singleton (0x800A32E4). */
    EffectManagerPJS() {}
    virtual ~EffectManagerPJS() {}
    virtual ::Effect::System::EffectElement *CreateElement(
        const ::Effect::System::UnkEffectDesc_801D0AFC *desc,
        ::Effect::System::EffectLaunchInfo *info);
};

} // namespace PJS
} // namespace Effect

#endif
