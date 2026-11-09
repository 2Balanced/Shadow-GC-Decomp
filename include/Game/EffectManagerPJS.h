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
#include "Effect/System/EffectObject.h"
#include "Effect/System/EffectLensFlare.h"
#include "Effect/System/EffectParticle.h"
#include "Effect/System/EffectRay.h"
#include "Effect/System/EffectShimmer.h"
#include "Game/ClumpFade.h"
#include "Game/EffectPJSLauncher.h"
#include "size_assert.h"

#include <string>

struct RwFrame;

#ifdef EFFECT_ELEMENT_FULL
struct RwV3d;
struct UnkLocusVertex_80439B18;

/* An immediate-mode 2D vertex (RwIm2DVertex on GameCube; Game/EffectShimmerRW3.cpp). */
struct UnkIm2DVertex_804342CC {
    /* 0x00 */ float x;
    /* 0x04 */ float y;
    /* 0x08 */ float z;
    /* 0x0C */ u8 r;
    /* 0x0D */ u8 g;
    /* 0x0E */ u8 b;
    /* 0x0F */ u8 a;
    /* 0x10 */ float u;
    /* 0x14 */ float v;
};
#endif

namespace Effect {
namespace System {

#ifdef EFFECT_ELEMENT_FULL
class EffectLocusExecUnit;
class EffectCommandExec;
class UnkEffectPool_8042A344;
#endif

class EffectLocus : public EffectElement {
  public:
    virtual ~EffectLocus();
    virtual void Update(float dt);

#ifdef EFFECT_ELEMENT_FULL
    /* A trail effect (Game/EffectLocus.cpp): it samples its matrix at a fixed rate into a list of
     * points that age out, and runs its description's command stream; names inferred. */
    /* 0x8042C3A0 */
    EffectLocus(const UnkEffectDesc_801D0AFC *desc, EffectLaunchInfo *info,
                EffectManager *manager);
    /* Adds the points since the last sample (0x8042B938), and starts the commands that are due
     * (0x8042BD8C). */
    void AddPoints(float dt);
    void StartCommands();

    /* 0x0FC */ EffectLocusExecUnit *m_head; /* the oldest point */
    /* 0x100 */ EffectLocusExecUnit *m_tail; /* the newest point */
    /* 0x104 */ EffectLocusExecUnit *m_last; /* the matrix at the last sample */
    /* 0x108 */ u32 field_0x108;             /* status: bit 0 stops the effect, bits 1 and 2
                                              * see Game/EffectLocusUSA.cpp */
    /* 0x10C */ u32 m_config;
    /* 0x110 */ s32 m_maxPoints;
    /* 0x114 */ s32 m_pointCount;
    /* 0x118 */ s32 m_addedCount;   /* points added this frame */
    /* 0x11C */ s32 m_removedCount; /* points removed this frame */
    /* 0x120 */ s32 m_sectionCount;
    /* 0x124 */ float m_interval;   /* frames between points */
    /* 0x128 */ float m_elapsed;    /* frames since the last point */
    /* 0x12C */ u32 m_color[4];     /* red, green, blue, alpha: 8 bits each, from bit 22 */
    /* 0x13C */ u32 m_topColor[4];
    /* 0x14C */ u32 m_lastColor[4];
    /* 0x15C */ const u8 *m_command; /* the next command to start */
    /* 0x160 */ EffectCommandExec *m_commandHead;
    /* 0x164 */ EffectCommandExec *m_commandTail;
    /* 0x168 */ UnkEffectPool_8042A344 *m_commandPool;
    /* 0x16C */ UnkEffectPool_8042A344 *m_pointPool;
#endif
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

#ifdef EFFECT_ELEMENT_FULL
    /* The members below are Game/EffectShimmerRW3.cpp's (names inferred). */
    /* 0x158 */ UnkIm2DVertex_804342CC m_quad[4]; /* the screen area the effect covers */
    /* 0x1B8 */ UnkIm2DVertex_804342CC *m_vertices; /* the grid as a triangle strip */
    /* 0x1BC */ RwV3d *m_points;   /* the grid's points */
    /* 0x1C0 */ float *m_waveX;    /* the wave's offsets along each column and row */
    /* 0x1C4 */ float *m_waveY;
    /* 0x1C8 */ s32 m_vertexCount;
    /* 0x1CC */ s32 m_pointCount;
    /* 0x1D0 */ float m_phaseX;
    /* 0x1D4 */ float m_phaseY;
#endif
};

#ifdef EFFECT_ELEMENT_FULL
/* The lens flare's settings (Game/EffectLensFlareRW3.cpp; layout from their copies, names
 * inferred): whether it is shown, how strong the flare and the screen whiteout are, and the light's
 * position. */
struct UnkLensFlarePosition_8043232C {
    float x;
    float y;
    float z;
};

struct UnkLensFlareParam_8043232C {
    UnkLensFlareParam_8043232C(u8 enabled_ = 0, u8 alpha_ = 0, u8 whiteout_ = 0);
    void SetPosition(const UnkLensFlarePosition_8043232C &p) {
        position.x = p.x;
        position.y = p.y;
        position.z = p.z;
    }

    /* 0x0 */ u8 enabled;
    /* 0x1 */ u8 alpha;
    /* 0x2 */ u8 whiteout;
    /* 0x4 */ UnkLensFlarePosition_8043232C position;
};
#endif

class EffectLensFlareRW3 : public ::Effect::System::EffectLensFlare {
  public:
    EffectLensFlareRW3(const ::Effect::System::UnkEffectDesc_801D0AFC *desc,
                       ::Effect::System::EffectLaunchInfo *info,
                       ::Effect::System::EffectManager *manager);
    virtual ~EffectLensFlareRW3();
    virtual void Update(float dt);
    /* Draws nothing (0x801D0F54). */
    virtual void Draw() {}

#ifdef EFFECT_ELEMENT_FULL
    /* 0x108 */ UnkLensFlareParam_8043232C m_startParam; /* the manager's, when it was launched */
    /* 0x118 */ UnkLensFlareParam_8043232C m_param;      /* the description's */
#else
    /* 0x054 */ u8 field_0x54[0x128 - 0x54];
#endif
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

#ifdef EFFECT_ELEMENT_FULL
    /* Fills m_vertices from the trail's points (0x80439B18, Game/EffectLocusRW3.cpp). */
    void BuildVertices();

    /* 0x170 */ ::UnkLocusVertex_80439B18 *m_vertices; /* two per point and section */
    /* 0x174 */ u8 field_0x174[0x17C - 0x174];
#else
    /* 0x054 */ u8 field_0x54[0x108 - 0x54];
    /* 0x108 */ u32 field_0x108; /* bit 1: show or hide requested, bit 2: hide (see
                                  * Game/EffectLocusUSA.cpp; meanings inferred) */
    /* 0x10C */ u8 field_0x10C[0x17C - 0x10C];
#endif
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

#ifdef EFFECT_ELEMENT_FULL
    /* 0x134 */ struct UnkRayVertex_8042C820 *m_vertices; /* four RenderWare vertices per ray */
#else
    /* 0x10C */ u8 field_0x10C[0x138 - 0x10C];
#endif
};
SIZE_ASSERT(EffectRayRW3, 0x138);

class EffectArrangerRW3 : public ::Effect::System::EffectArranger {
  public:
    /* 0x8043418C. */
    EffectArrangerRW3(const ::Effect::System::UnkEffectDesc_801D0AFC *desc,
                      ::Effect::System::EffectLaunchInfo *info,
                      ::Effect::System::EffectManager *manager);
    virtual ~EffectArrangerRW3();

#ifndef EFFECT_ELEMENT_FULL
    /* 0x054 */ u8 field_0x54[0x104 - 0x54];
    /* The arranger's own copy of the launch description (Game/EffectArrangerUSA.cpp; inferred). */
    /* 0x104 */ ::Effect::System::EffectLaunchInfo *m_launchInfo;
    /* 0x108 */ u8 field_0x108[0x11C - 0x108];
#endif
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

    /* 0x13C */ void *m_model;     /* the particles' RenderWare model data (a PTank atomic) */
    /* 0x140 */ RwFrame *m_frame;  /* its frame */
    /* 0x144 */ void *field_0x144; /* when set, Draw leaves the atomic to someone else */
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

    /* 0x13C */ void *m_model;   /* the particles' RenderWare model data: a copy of the clump or
                                * four vertices per particle */
    /* 0x140 */ void *m_clump;   /* the particles' model (a RenderWare clump) */
    /* 0x144 */ void *m_texDict; /* their texture (Draw uses its raster) */
    /* 0x148 */ void *m_world;   /* the world of the stage area the model is in */
    /* 0x14C */ Vec3f m_corners[4]; /* a particle quad's corners, when there is no model */
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

#ifndef EFFECT_ELEMENT_FULL
    /* 0x054 */ u8 field_0x54[0x1D8 - 0x54];
#endif
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
