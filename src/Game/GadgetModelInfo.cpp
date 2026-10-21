/* GadgetModelInfo (class name from RTTI): the model a gadget loads from a .BIN file (see
 * Game/GadgetModelInfo.h). The file describes the model's parts (clumps with their place, area and
 * render layer), variants (the parts shown together), motions, colliders and material
 * animations; GadgetModelInfo loads a variant's parts, plays motions on them, builds their
 * colliders and reports the colliders' contacts through three callbacks. It also blinks, fades and
 * hides the model. The unit also holds the table of every gadget type (lbl_8052B080),
 * Player::GetItemCommand's constructor and a debug cross. File name inferred from the class.
 * Names marked "(name inferred)" and the descriptor layouts are not original; functions other units
 * call keep their fn_ names. Definitions are in reverse address order (deferred inlining emits
 * functions last-defined first). The unit's code reaches its .rodata through a pooled base address,
 * so the linker keeps that section whole.
 *
 * Not matching yet: the update (fn_80162FB0) gives `this` r31 where retail gives it r30 (and the
 * collider iterator r31); the collider build over a character info (0x80162084) picks r5 for the
 * group table's address where retail picks r3; SetMaterialAlpha loads the alpha into r0 where
 * retail uses r6. Retail's .rodata also ends with "_RefCountedPtr out of memory", whose source is
 * unknown. */
/* The motions own their work through a shared_ptr with a deleter (see boost/shared_ptr.hpp). */
#define BOOST_SP_DELETER_CONSTRUCTOR
/* boost::function's operator() puts a bad_function_call on the stack (see the MSL's stdexcept). */
#define MSL_RUNTIME_ERROR_STRING
/* The shared_ptr control blocks' allocation failure throws a bad_alloc, whose vtable and RTTI this
 * unit's .rodata holds (see <new>). */
#define MSL_NEW_INLINE_BAD_ALLOC
#include <string>
#include <list>

/* The unreferenced constant block of the original headers before the CharaColliBase names (as in
 * Gong.cpp; the bytes are reproduced). */
static const float s_zeros[7] = {0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f};
#include "Game/CharaColliBaseCharInfo.h"
extern "C" {
typedef struct RwV3d {
    float x;
    float y;
    float z;
} RwV3d;
}
/* The unit axes (the part placement below turns about them). */
static const RwV3d s_xAxis = {1.0f, 0.0f, 0.0f};
static const RwV3d s_yAxis = {0.0f, 1.0f, 0.0f};
static const RwV3d s_zAxis = {0.0f, 0.0f, 1.0f};

#define GADGETMODELINFO_IMPL
#include "Game/CharCommand.h"
#include "Game/CharaColliAttack.h"
#include "Game/CharInfo.h"
#include "Game/GadgetClassDesc.h"
#include "Game/GadgetModelInfo.h"
#include "Game/GadgetUtil.h"
#include "boost/detail/sp_counted_base_impl.hpp"
#include "boost/shared_ptr.hpp"
#include "boost/weak_ptr.hpp"
#include "size_assert.h"
#include <cstring>

using SonicteamUSA::System::CharaColli::CharaColliAttack;
using SonicteamUSA::System::CharaColli::CharaColliBase;

/* The RenderWare types used here, declared locally (layouts as in rw/rwcore.h and rw/rpworld.h,
 * reduced to the members used; as in GadgetCrash3D.cpp). */
extern "C" {
typedef struct RwMatrix {
    /* 0x00 */ RwV3d right;
    /* 0x0C */ u32 flags;
    /* 0x10 */ RwV3d up;
    /* 0x1C */ u32 pad1;
    /* 0x20 */ RwV3d at;
    /* 0x2C */ u32 pad2;
    /* 0x30 */ RwV3d pos;
    /* 0x3C */ u32 pad3;
} RwMatrix;

typedef struct RwObject {
    u8 type;
    u8 subKind;
    u8 flags;
    u8 privateFlags;
    void *parent;
} RwObject;

typedef struct RwFrame {
    /* 0x00 */ RwObject object;
} RwFrame;

typedef struct RpClump {
    /* 0x00 */ RwObject object; /* the parent is the clump's frame */
} RpClump;

typedef struct RwRGBA {
    u8 red;
    u8 green;
    u8 blue;
    u8 alpha;
} RwRGBA;

typedef struct RwRGBAReal {
    float red;
    float green;
    float blue;
    float alpha;
} RwRGBAReal;

typedef struct RpMaterial {
    /* 0x0 */ void *texture;
    /* 0x4 */ RwRGBA color;
} RpMaterial;

typedef struct RpGeometry {
    /* 0x0 */ RwObject object;
    /* 0x8 */ u32 flags;
} RpGeometry;

typedef struct RpAtomic RpAtomic;
typedef RpAtomic *(*RpAtomicCallBackRender)(RpAtomic *atomic);
typedef RpAtomic *(*RpAtomicCallBack)(RpAtomic *atomic, void *data);
typedef RpMaterial *(*RpMaterialCallBack)(RpMaterial *material, void *data);

struct RpAtomic {
    /* 0x00 */ RwObject object;
    /* 0x08 */ u8 field_0x8[0x18 - 0x8];
    /* 0x18 */ RpGeometry *geometry;
    /* 0x1C */ u8 field_0x1c[0x48 - 0x1C];
    /* 0x48 */ RpAtomicCallBackRender renderCallBack;
};

/* RpClumpForAllAtomics, RpGeometryForAllMaterials, RpClumpClone and RpClumpDestroy. */
RpClump *fn_80456C54(RpClump *clump, RpAtomicCallBack callBack, void *data);
RpGeometry *fn_804590E8(RpGeometry *geometry, RpMaterialCallBack callBack, void *data);
RpClump *fn_80457090(RpClump *clump);
void fn_80457468(RpClump *clump);
/* AtomicDefaultRenderCallBack. */
RpAtomic *fn_80455EB4(RpAtomic *atomic);
/* The world a clump is in, and removing and adding it (see GadgetCrash3D.cpp). */
void *fn_8046256C(RpClump *clump);
void fn_804623EC(void *world, RpClump *clump);
void fn_804622FC(void *world, RpClump *clump);
/* RwFrameTransform, RwFrameUpdateObjects, RwFrameAddChild and RwFrameRemoveChild. */
RwFrame *fn_804876E8(RwFrame *frame, const RwMatrix *matrix, int combine);
RwFrame *fn_8048722C(RwFrame *frame);
RwFrame *fn_80487348(RwFrame *parent, RwFrame *child);
RwFrame *fn_80487448(RwFrame *frame);
/* RwMatrixUpdate, RwMatrixRotate and RwMatrixTranslate. */
RwMatrix *fn_8047EBA0(RwMatrix *matrix);
RwMatrix *fn_8047EFC8(RwMatrix *matrix, const RwV3d *axis, float angle, int combine);
RwMatrix *fn_8047F59C(RwMatrix *matrix, const RwV3d *translation, int combine);
/* RwTexDictionarySetCurrent. */
void *fn_8048D0C4(void *dictionary);
}

/* The data the game keeps with each atomic (layout from its users; names inferred): the render
 * layer, and the material state of its part. */
struct UnkAtomicData_80037D5C {
    /* 0x0 */ s32 m_layer;
    /* 0x4 */ u32 field_0x4;
    /* 0x8 */ UnkMaterialAnim_801621F0 *m_material;
};

/* What fn_80037D28 saves of an atomic: the render callback replaced. */
struct UnkSavedRender_80037D28 {
    /* 0x0 */ u32 field_0x0;
    /* 0x4 */ RpAtomicCallBackRender m_callBack;
};

/* The data passed with the render-layer callback below (names inferred). */
struct UnkLayerData_80164168 {
    UnkLayerData_80164168() : m_layer(-1), m_desc(0), field_0x8(0) {}

    /* 0x0 */ s32 m_layer;
    /* 0x4 */ const UnkModelPart_8015F7BC *m_desc;
    /* 0x8 */ u32 field_0x8;
};

/* The worlds of the stage areas (see GadgetCrash3D.cpp). */
struct UnkWorldEntry_8051D208 {
    /* 0x00 */ u8 field_0x0[0x10];
    /* 0x10 */ void *m_world;
    /* 0x14 */ u8 field_0x14[0x18 - 0x14];
};
extern UnkWorldEntry_8051D208 lbl_8051D208[];

/* An object that keeps a clump in step with a motion (class name unknown; as in
 * DevilDoomCtrlUnit.h). */
class UnkClumpUpdater_80015A7C {
  public:
    UnkClumpUpdater_80015A7C(RpClump *clump, _stMtnPlayWork *motion);
    ~UnkClumpUpdater_80015A7C();

    /* 0x00 */ u8 field_0x0[0x1C];
};

/* The frame of a motion (layout unknown; fn_804125F4 fills it). */
struct UnkMotionFrame_804125F4 {
    /* 0x0 */ u32 field_0x0;
    /* 0x4 */ float m_frame;
};

/* The manager of the clumps whose textures animate (as in HintRing.cpp). */
class UnkUVAnimManager_801EB994 {
  public:
    UnkUVAnimManager_801EB994();
    ~UnkUVAnimManager_801EB994();

    /* 0x0 */ u8 field_0x0[0x10];
};

class UnkColliderManager_80021B08;

extern "C" {
/* The file manager, and loading a file, a model file by number and a motion file through it
 * (behaviour inferred). */
void *fn_800122D4(void);
void *fn_80012700(void *manager, int group, const char *fileName);
GadgetModelData *fn_8001277C(void *manager, int group, int modelId);
s32 fn_8001258C(void *manager, int group, const char *fileName);
/* The texture manager, and giving a clump a texture (behaviour inferred). */
void *fn_80013BC4(void);
GadgetModelTexture fn_8004F9E8(void *manager, RpClump *clump, void *dictionary, const char *name,
                               int arg4, int arg5);
/* Adds and removes a named clump (see HintRing.cpp). */
void fn_801EB660(UnkUVAnimManager_801EB994 *manager, const std::string &name, RpClump *clump);
void fn_801EB550(UnkUVAnimManager_801EB994 *manager, const std::string &name);
/* Motion work: the animation of an id, a work over it, setting its motion, the motion's frame,
 * the frame, playing, a step, its matrix and its deleter (behaviour inferred). */
void *fn_804106C0(int id);
_stMtnPlayWork *fn_8041B908(void *anim, int arg1);
void fn_8041B884(_stMtnPlayWork *work);
s32 fn_80412738(int id);
void fn_80417568(_stMtnPlayWork *work, u16 motion);
void fn_804125F4(UnkMotionFrame_804125F4 *frame, u16 motion);
void fn_80416E20(_stMtnPlayWork *work, int arg1, float frame);
int fn_80416F28(_stMtnPlayWork *work, int arg1, int arg2, int arg3);
void fn_80415EA4(_stMtnPlayWork *work);
void fn_804172D8(_stMtnPlayWork *work, const Mtx34f *matrix);
void fn_800159AC(UnkClumpUpdater_80015A7C *updater);
void fn_80016414(Mtx34f *matrix, const RwMatrix *from);
/* Clump helpers: tint, show, hide and find a node's frame (behaviour inferred). */
void fn_800390E8(RpClump *clump);
void fn_80039114(RpClump *clump, const RwRGBAReal *color);
void fn_80038F94(RpClump *clump);
void fn_80038FC0(RpClump *clump);
RwFrame *fn_80038E84(RwFrame *frame, s32 node);
void fn_80169448(RpClump *clump);
/* The area a clump is in, and its loading (see GadgetCrash3D.cpp). */
void *fn_80169364(RpClump *clump);
int fn_80169350(void *frame);
/* Called before an area's world is used (it takes no argument; behaviour unknown). */
void fn_8003A840(void);
/* The game's data of an atomic, saving and getting back its render callback (behaviour
 * inferred). */
UnkAtomicData_80037D5C *fn_80037D5C(RpAtomic *atomic);
void fn_80037D80(RpAtomic *atomic, const void *data, int size);
void fn_80037D28(RpAtomic *atomic, const UnkSavedRender_80037D28 *saved);
UnkSavedRender_80037D28 *fn_80037D04(RpAtomic *atomic);
/* The render state: setting the layer and the alpha reference (behaviour inferred). */
void *fn_800289B0(void);
void fn_80048390(void *state, int layer);
void fn_800482E0(void *state, int arg1);
/* The collider manager: building colliders over a character info and dropping one (see
 * BKTank.cpp and CityGadget.cpp). */
UnkColliderManager_80021B08 *fn_80021B08(void);
boost::shared_ptr<CharaColliBase> fn_800C7F7C(UnkColliderManager_80021B08 *manager,
                                              const boost::weak_ptr<CharInfo> &charInfo, void *file,
                                              _stMtnPlayWork *work, int group, int arg5);
boost::shared_ptr<CharaColliBase> fn_800C81F4(UnkColliderManager_80021B08 *manager,
                                              const boost::weak_ptr<CharInfo> &charInfo, void *file,
                                              int group, int arg4);
void fn_800C78B0(UnkColliderManager_80021B08 *manager, boost::shared_ptr<CharaColliBase> *collider);
boost::weak_ptr<CharaColliAttack> fn_800C4374(UnkColliderManager_80021B08 *manager,
                                              boost::shared_ptr<CharaColliBase> collider);
/* Compares n characters (behaviour inferred). */
int fn_803AC8DC(const char *a, const char *b, int n);
}

/* The common texture dictionary of the stage's gadgets. */
const char *lbl_805E6648 = "STG_GDT.TXD";

/* The character kinds and collision groups of the collider descriptors, the render layers of the
 * part descriptors and the areas of the parts placed by table. */
static const s32 s_colliderKinds[3] = {0, 1, 2};
static const s32 s_colliderGroups[11] = {0, 1, 2, 3, 4, 5, 6, 7, 8, 10, 11};
static const s32 s_renderLayers[16] = {4, 5, 6, 7, 0, 1, 2, 3, 8, 9, 10, 11, 12, 13, 14, 15};
static const s32 s_partAreas[14] = {6, 4, 2, 5, 1, 9, 8, 4, 11, 11, 13, 14, 12, 0};

/* The table of every gadget type (see GadgetClassDesc.h), ended by a null entry. */
extern GadgetClassDesc lbl_8052AFC8, lbl_805591D0, lbl_80525470, lbl_80525658, lbl_805257A4,
    lbl_805258A4, lbl_8054D744, lbl_80524708, lbl_80527898, lbl_80525A14, lbl_80526784,
    lbl_80526BE4, lbl_8052686C, lbl_80526EA0, lbl_80528344, lbl_805261AC, lbl_80527B40,
    lbl_80525AFC, lbl_80527D0C, lbl_80524C78, lbl_80525E24, lbl_805271CC, lbl_80527158,
    lbl_805249F4, lbl_80524E90, lbl_80524FCC, lbl_80525118, lbl_80526490, lbl_80527FC0,
    lbl_805265B0, lbl_80524840, lbl_80524920, lbl_80525FA4, lbl_80525BBC, lbl_805269C4,
    lbl_805280DC, lbl_80526D4C, lbl_80527670, lbl_80527734, lbl_80527018, lbl_80524BD4,
    lbl_80525278, lbl_805275B8, lbl_80526234, lbl_80527440, lbl_805260A4, lbl_8053E060,
    lbl_805440AC, lbl_80557C54, lbl_80549ED8, lbl_8054DDB8, lbl_805470EC, lbl_80559FC0,
    lbl_80527E30, lbl_8055D858, lbl_8055DA88, lbl_8052AFEC, lbl_8051E968, lbl_8051EB68,
    lbl_8051F810, lbl_8051E578, lbl_8051E410, lbl_8051F978, lbl_80541E58, lbl_8051EC38,
    lbl_8051E9B0, lbl_8051EDA8, lbl_80559E60, lbl_8051EA68, lbl_8051E660, lbl_8051F8C8,
    lbl_8051E7C0, lbl_8051E710, lbl_80541DA8, lbl_8051E4C8, lbl_8051EE84, lbl_8051EED8,
    lbl_8054D094, lbl_8054D0E8, lbl_8054D13C, lbl_80546558, lbl_80546140, lbl_80546480,
    lbl_805463A8, lbl_8051EFC4, lbl_8051E870, lbl_80559DB0, lbl_8051F018, lbl_8051E98C,
    lbl_8052B678, lbl_80527A10, lbl_80527E0C, lbl_80524490, lbl_805244B4, lbl_80524340,
    lbl_80524364, lbl_80529D68, lbl_8052A0B0, lbl_80529CB0, lbl_805299D4, lbl_805297F8,
    lbl_80525E48, lbl_8051F06C, lbl_8051F0C0, lbl_8051F114, lbl_8051F168, lbl_80547D18,
    lbl_8051F1BC, lbl_8051F210, lbl_8051F264, lbl_8051F2B8, lbl_8051F30C, lbl_8051F360,
    lbl_8051F3B4, lbl_8051F408, lbl_8051F45C, lbl_8051F4B0, lbl_8051F504, lbl_8051F558,
    lbl_8051F5AC, lbl_8051F600, lbl_8051F654, lbl_8051F6A8, lbl_8051F6FC, lbl_8052A494,
    lbl_80529534, lbl_8052A2F0, lbl_80529744, lbl_80529AF4, lbl_8054CE54, lbl_8055A5BC,
    lbl_80523E08, lbl_80523F50, lbl_805234EC, lbl_8052383C, lbl_80523BE0, lbl_80523090,
    lbl_80522E90, lbl_805231C8, lbl_805232D8, lbl_80559210, lbl_80528744, lbl_80528B74,
    lbl_80528AB0, lbl_805285D8, lbl_80528DEC, lbl_80528F9C, lbl_80547948, lbl_805289AC,
    lbl_80529404, lbl_805291B8, lbl_805292FC, lbl_80528C94, lbl_805284B0, lbl_80528804,
    lbl_80529A04, lbl_80523F9C, lbl_805241D8, lbl_805240A0, lbl_8055A314, lbl_8055A508,
    lbl_8055A070, lbl_8055A180, lbl_8055C4F8, lbl_8055C7A0, lbl_8055D0CC, lbl_8055D058,
    lbl_8055CF88, lbl_805422C0, lbl_80542370, lbl_80557B00, lbl_8052A8BC, lbl_8054A130,
    lbl_8052A6E8, lbl_8052AAF0, lbl_8052ACC0, lbl_805424C4, lbl_80542578, lbl_8054BF48,
    lbl_80541F94, lbl_8054BFBC, lbl_80548B70, lbl_8054C1C0, lbl_8052ADE8, lbl_80542218,
    lbl_80545940, lbl_8054588C, lbl_805459A8, lbl_80546F98, lbl_80547000, lbl_80546F34,
    lbl_8054C2AC, lbl_8054C084, lbl_80558E60, lbl_8055D638, lbl_8055BEF4, lbl_8055C1F4,
    lbl_8055BFB4, lbl_8055C06C, lbl_80546334, lbl_8054629C, lbl_80545BCC, lbl_80545E40,
    lbl_80546700, lbl_8054669C, lbl_8054C72C, lbl_80548904, lbl_80548AF4, lbl_80548A30,
    lbl_80549F40, lbl_8054CBA8, lbl_8054C790, lbl_8054BE38, lbl_8054BC24, lbl_80546DEC,
    lbl_80546EA4, lbl_80546A9C, lbl_805468C8, lbl_80546A28, lbl_80546BA0, lbl_80549E24,
    lbl_80549D44, lbl_80549B28, lbl_8054B810, lbl_80554718, lbl_8054C5A8, lbl_8054DC60,
    lbl_8055ACF8, lbl_8055AE58, lbl_8055AD78, lbl_8055ABFC, lbl_8055AF34, lbl_8055AA18,
    lbl_8055AEC0, lbl_8052B69C, lbl_8053A9A8, lbl_80538760, lbl_80536050, lbl_8054E85C,
    lbl_805373A4, lbl_8054F7E4, lbl_80530D68, lbl_805326C0, lbl_80533A98, lbl_80534AE4,
    lbl_8053081C, lbl_80555FA8, lbl_80554B40, lbl_8052C3BC, lbl_80550810, lbl_80552BB0,
    lbl_80547374, lbl_80548404, lbl_80549164, lbl_80549A34, lbl_8054E2F8, lbl_80559A74,
    lbl_8055994C, lbl_8055980C, lbl_80558BE0, lbl_8055A7F8, lbl_8055B7A4, lbl_8053A9CC,
    lbl_8053BF10, lbl_8053B948, lbl_8053B974, lbl_8053B9A0, lbl_8053B9CC, lbl_8053B2D8,
    lbl_8053B2FC, lbl_8053B4B0, lbl_8053B644, lbl_80557FB4, lbl_8053BF34, lbl_8054D978,
    lbl_8054D99C, lbl_8054D9C0, lbl_8054D9E4, lbl_8054DA08, lbl_8054DA2C, lbl_8054DA50,
    lbl_8054DA74, lbl_8054DA98, lbl_8054DABC, lbl_8054DAE0, lbl_8054DB04;

GadgetClassDesc *lbl_8052B080[] = {
    &lbl_8052AFC8, &lbl_805591D0, &lbl_80525470, &lbl_80525658, &lbl_805257A4, &lbl_805258A4,
    &lbl_8054D744, &lbl_80524708, &lbl_80527898, &lbl_80525A14, &lbl_80526784, &lbl_80526BE4,
    &lbl_8052686C, &lbl_80526EA0, &lbl_80528344, &lbl_805261AC, &lbl_80527B40, &lbl_80525AFC,
    &lbl_80527D0C, &lbl_80524C78, &lbl_80525E24, &lbl_805271CC, &lbl_80527158, &lbl_805249F4,
    &lbl_80524E90, &lbl_80524FCC, &lbl_80525118, &lbl_80526490, &lbl_80527FC0, &lbl_805265B0,
    &lbl_80524840, &lbl_80524920, &lbl_80525FA4, &lbl_80525BBC, &lbl_805269C4, &lbl_805280DC,
    &lbl_80526D4C, &lbl_80527670, &lbl_80527734, &lbl_80527018, &lbl_80524BD4, &lbl_80525278,
    &lbl_805275B8, &lbl_80526234, &lbl_80527440, &lbl_805260A4, &lbl_8053E060, &lbl_805440AC,
    &lbl_80557C54, &lbl_80549ED8, &lbl_8054DDB8, &lbl_805470EC, &lbl_80559FC0, &lbl_80527E30,
    &lbl_8055D858, &lbl_8055DA88, &lbl_8052AFEC, &lbl_8051E968, &lbl_8051EB68, &lbl_8051F810,
    &lbl_8051E578, &lbl_8051E410, &lbl_8051F978, &lbl_80541E58, &lbl_8051EC38, &lbl_8051E9B0,
    &lbl_8051EDA8, &lbl_80559E60, &lbl_8051EA68, &lbl_8051E660, &lbl_8051F8C8, &lbl_8051E7C0,
    &lbl_8051E710, &lbl_80541DA8, &lbl_8051E4C8, &lbl_8051EE84, &lbl_8051EED8, &lbl_8054D094,
    &lbl_8054D0E8, &lbl_8054D13C, &lbl_80546558, &lbl_80546140, &lbl_80546480, &lbl_805463A8,
    &lbl_8051EFC4, &lbl_8051E870, &lbl_80559DB0, &lbl_8051F018, &lbl_8051E98C, &lbl_8052B678,
    &lbl_80527A10, &lbl_80527E0C, &lbl_80524490, &lbl_805244B4, &lbl_80524340, &lbl_80524364,
    &lbl_80529D68, &lbl_8052A0B0, &lbl_80529CB0, &lbl_805299D4, &lbl_805297F8, &lbl_80525E48,
    &lbl_8051F06C, &lbl_8051F0C0, &lbl_8051F114, &lbl_8051F168, &lbl_80547D18, &lbl_8051F1BC,
    &lbl_8051F210, &lbl_8051F264, &lbl_8051F2B8, &lbl_8051F30C, &lbl_8051F360, &lbl_8051F3B4,
    &lbl_8051F408, &lbl_8051F45C, &lbl_8051F4B0, &lbl_8051F504, &lbl_8051F558, &lbl_8051F5AC,
    &lbl_8051F600, &lbl_8051F654, &lbl_8051F6A8, &lbl_8051F6FC, &lbl_8052A494, &lbl_80529534,
    &lbl_8052A2F0, &lbl_80529744, &lbl_80529AF4, &lbl_8054CE54, &lbl_8055A5BC, &lbl_80523E08,
    &lbl_80523F50, &lbl_805234EC, &lbl_8052383C, &lbl_80523BE0, &lbl_80523090, &lbl_80522E90,
    &lbl_805231C8, &lbl_805232D8, &lbl_80559210, &lbl_80528744, &lbl_80528B74, &lbl_80528AB0,
    &lbl_805285D8, &lbl_80528DEC, &lbl_80528F9C, &lbl_80547948, &lbl_805289AC, &lbl_80529404,
    &lbl_805291B8, &lbl_805292FC, &lbl_80528C94, &lbl_805284B0, &lbl_80528804, &lbl_80529A04,
    &lbl_80523F9C, &lbl_805241D8, &lbl_805240A0, &lbl_8055A314, &lbl_8055A508, &lbl_8055A070,
    &lbl_8055A180, &lbl_8055C4F8, &lbl_8055C7A0, &lbl_8055D0CC, &lbl_8055D058, &lbl_8055CF88,
    &lbl_805422C0, &lbl_80542370, &lbl_80557B00, &lbl_8052A8BC, &lbl_8054A130, &lbl_8052A6E8,
    &lbl_8052AAF0, &lbl_8052ACC0, &lbl_805424C4, &lbl_80542578, &lbl_8054BF48, &lbl_80541F94,
    &lbl_8054BFBC, &lbl_80548B70, &lbl_8054C1C0, &lbl_8052ADE8, &lbl_80542218, &lbl_80545940,
    &lbl_8054588C, &lbl_805459A8, &lbl_80546F98, &lbl_80547000, &lbl_80546F34, &lbl_8054C2AC,
    &lbl_8054C084, &lbl_80558E60, &lbl_8055D638, &lbl_8055BEF4, &lbl_8055C1F4, &lbl_8055BFB4,
    &lbl_8055C06C, &lbl_80546334, &lbl_8054629C, &lbl_80545BCC, &lbl_80545E40, &lbl_80546700,
    &lbl_8054669C, &lbl_8054C72C, &lbl_80548904, &lbl_80548AF4, &lbl_80548A30, &lbl_80549F40,
    &lbl_8054CBA8, &lbl_8054C790, &lbl_8054BE38, &lbl_8054BC24, &lbl_80546DEC, &lbl_80546EA4,
    &lbl_80546A9C, &lbl_805468C8, &lbl_80546A28, &lbl_80546BA0, &lbl_80549E24, &lbl_80549D44,
    &lbl_80549B28, &lbl_8054B810, &lbl_80554718, &lbl_8054C5A8, &lbl_8054DC60, &lbl_8055ACF8,
    &lbl_8055AE58, &lbl_8055AD78, &lbl_8055ABFC, &lbl_8055AF34, &lbl_8055AA18, &lbl_8055AEC0,
    &lbl_8052B69C, &lbl_8053A9A8, &lbl_80538760, &lbl_80536050, &lbl_8054E85C, &lbl_805373A4,
    &lbl_8054F7E4, &lbl_80530D68, &lbl_805326C0, &lbl_80533A98, &lbl_80534AE4, &lbl_8053081C,
    &lbl_80555FA8, &lbl_80554B40, &lbl_8052C3BC, &lbl_80550810, &lbl_80552BB0, &lbl_80547374,
    &lbl_80548404, &lbl_80549164, &lbl_80549A34, &lbl_8054E2F8, &lbl_80559A74, &lbl_8055994C,
    &lbl_8055980C, &lbl_80558BE0, &lbl_8055A7F8, &lbl_8055B7A4, &lbl_8053A9CC, &lbl_8053BF10,
    &lbl_8053B948, &lbl_8053B974, &lbl_8053B9A0, &lbl_8053B9CC, &lbl_8053B2D8, &lbl_8053B2FC,
    &lbl_8053B4B0, &lbl_8053B644, &lbl_80557FB4, &lbl_8053BF34, &lbl_8054D978, &lbl_8054D99C,
    &lbl_8054D9C0, &lbl_8054D9E4, &lbl_8054DA08, &lbl_8054DA2C, &lbl_8054DA50, &lbl_8054DA74,
    &lbl_8054DA98, &lbl_8054DABC, &lbl_8054DAE0, &lbl_8054DB04,
    0,
};

/* Defined below (in reverse address order). */
extern "C" {
void fn_8016232C(GadgetModelInfo *model);
void fn_8016271C(GadgetModelInfo *model);
void fn_80162790(GadgetModelInfo *model);
void fn_8015FDD8(GadgetModelInfo *model, float alpha);
void fn_8015FD10(GadgetModelInfo *model);
boost::weak_ptr<_stMtnPlayWork> fn_801602B4(GadgetModelInfo *model, int id);
}

/* The clump manager (an inline accessor of HintRing.cpp's headers, whose one out-of-line copy is
 * in Game/HintRing.cpp). */
UnkUVAnimManager_801EB994 *GetUVAnimManager();

/* Where another unit's accessor (fn_801E6500) caches its object (meaning of the rest unknown). */
struct UnkCache_801E6500 {
    /* 0x0 */ void *m_object;
    /* 0x4 */ u32 field_0x4;
};
UnkCache_801E6500 lbl_805EF208;

/* Flags shared by the gadget models (meaning unknown; fn_8015FA48 resets them). */
static GadgetModelFlags s_sharedFlags(0);

/* Whether name ends in the extension ext (name inferred). */
inline bool HasExtension(const char *name, const char *ext) {
    const char *dot = strchr(name, '.');
    if (dot != 0 && fn_803AC8DC(dot, ext, 4) == 0) {
        return true;
    }
    return false;
}

/* Moves clump into world (as in GadgetCrash3D.cpp). */
inline void SetClumpWorld(RpClump *clump, void *world) {
    void *current = fn_8046256C(clump);
    if (current != world) {
        if (current != 0) {
            fn_804623EC(current, clump);
        }
        fn_804622FC(world, clump);
    }
}

inline void GadgetModelMotion::Release() {
    m_name = 0;
    if (m_updater != 0) {
        delete m_updater;
        m_updater = 0;
    }
    m_anim = 0;
    m_base = 0;
    m_desc = 0;
    m_flags = 0;
}

inline bool UnkMaterialAnim_801621F0::IsNamed(const char *name) {
    if (m_desc != 0) {
        return strcmp(m_desc->m_name, name) == 0;
    }
    return false;
}

inline bool UnkMaterialAnim_801621F0::HasId(int id) {
    if (m_desc != 0) {
        return id == m_desc->m_id;
    }
    return false;
}

/* Material callbacks: give a material the state's colour, and the faded alpha. */
static RpMaterial *SetMaterialColor(RpMaterial *material, void *data) {
    UnkMaterialAnim_801621F0 *state = (UnkMaterialAnim_801621F0 *)data;
    if (state != 0) {
        RwRGBA color;
        color.alpha = state->m_alpha;
        color.red = state->m_red;
        color.green = state->m_green;
        color.blue = state->m_blue;
        material->color = color;
    }
    return material;
}

static RpMaterial *SetMaterialAlpha(RpMaterial *material, void *data) {
    if (data != 0) {
        material->color.alpha = *(u8 *)data;
    }
    return material;
}

/* The render callback of the model's atomics: sets the render layer and the material state, then
 * renders through the callback it replaced. */
static RpAtomic *RenderAtomic(RpAtomic *atomic) {
    UnkAtomicData_80037D5C *data = fn_80037D5C(atomic);
    if (data != 0) {
        if (data->m_layer != -1) {
            fn_80048390(fn_800289B0(), (s8)data->m_layer);
            u8 *state = (u8 *)fn_800289B0();
            fn_800482E0(state, state[0x456]);
        }
        if (data->m_material != 0) {
            if (data->m_material->m_flags.Test(2)) {
                if (data->m_material->m_flags.Test(0)) {
                    if (atomic->geometry != 0) {
                        fn_804590E8(atomic->geometry, SetMaterialColor, data->m_material);
                    }
                } else if (data->m_material->m_flags.Test(1)) {
                    u8 alpha = data->m_material->m_fadeAlpha;
                    RpGeometry *geometry = atomic->geometry;
                    if (geometry != 0) {
                        geometry->flags |= 0x40;
                        fn_804590E8(geometry, SetMaterialAlpha, &alpha);
                    }
                } else if (atomic->geometry != 0) {
                    atomic->geometry->flags &= ~0x40;
                }
            } else if (data->m_material->m_flags.Test(0)) {
                if (atomic->geometry != 0) {
                    fn_804590E8(atomic->geometry, SetMaterialColor, data->m_material);
                }
            }
        }
    }
    fn_80037D04(atomic)->m_callBack(atomic);
    if (data != 0 && data->m_material != 0 && data->m_material->m_flags.Test(2) &&
        !data->m_material->m_flags.Test(0) && atomic->geometry != 0) {
        atomic->geometry->flags &= ~0x40;
    }
    return atomic;
}

/* Atomic callbacks: set the render layer and install RenderAtomic, set the material state, apply
 * a material animation, and drop and stop the material state. */
static RpAtomic *SetAtomicLayer(RpAtomic *atomic, void *data) {
    fn_80037D80(atomic, data, 0xC);
    UnkSavedRender_80037D28 saved;
    saved.m_callBack = atomic->renderCallBack;
    if (saved.m_callBack == RenderAtomic) {
        return atomic;
    }
    fn_80037D28(atomic, &saved);
    atomic->renderCallBack = RenderAtomic;
    if (atomic->renderCallBack == 0) {
        atomic->renderCallBack = fn_80455EB4;
    }
    return atomic;
}

static RpAtomic *SetAtomicMaterial(RpAtomic *atomic, void *data) {
    UnkAtomicData_80037D5C *atomicData = fn_80037D5C(atomic);
    if (atomicData != 0) {
        atomicData->m_material = (UnkMaterialAnim_801621F0 *)data;
    }
    return atomic;
}

static RpAtomic *StartMaterialAnim(RpAtomic *atomic, void *data) {
    const GadgetModelMaterialDesc *desc = (const GadgetModelMaterialDesc *)data;
    UnkAtomicData_80037D5C *atomicData = fn_80037D5C(atomic);
    if (atomicData != 0 && atomicData->m_material != 0) {
        if (desc != 0) {
            UnkMaterialAnim_801621F0 *material = atomicData->m_material;
            material->m_desc = desc;
            material->m_alpha = desc->m_alpha;
            material->m_red = desc->m_red;
            material->m_green = desc->m_green;
            material->m_blue = desc->m_blue;
        }
        atomicData->m_material->m_flags.Set(0);
    }
    return atomic;
}

static RpAtomic *DropAtomicMaterial(RpAtomic *atomic, void *data) {
    UnkAtomicData_80037D5C *atomicData = fn_80037D5C(atomic);
    if (atomicData != 0) {
        atomicData->m_material = 0;
    }
    return atomic;
}

static RpAtomic *StopAtomicMaterial(RpAtomic *atomic, void *data) {
    UnkAtomicData_80037D5C *atomicData = fn_80037D5C(atomic);
    if (atomicData != 0 && atomicData->m_material != 0) {
        atomicData->m_material->m_flags.Reset(0);
    }
    return atomic;
}

GadgetModelInfo::GadgetModelInfo(int arg0, int modelId) {
    m_fileGroup = arg0;
    m_data = fn_8001277C(fn_800122D4(), m_fileGroup, modelId);
    Init();
}

GadgetModelInfo::GadgetModelInfo(int arg0, const char *path) {
    m_fileGroup = arg0;
    m_data = (GadgetModelData *)fn_80012700(fn_800122D4(), m_fileGroup, path);
    Init();
}

/* Resets the state and empties the lists (name inferred). */
void GadgetModelInfo::Init() {
    m_variantIndex = 0;
    m_partCount = 0;
    m_variant = 0;
    m_collisionOffTime = 0.0f;
    m_flags.Clear();
    m_blinkTime = 0.0f;
    m_alpha = 1.0f;
    m_fadeSpeed = 0.15f;
    m_partCount = m_data->m_partCount;
    m_parts.clear();
    m_motions.clear();
    m_colliders.clear();
}

GadgetModelInfo::~GadgetModelInfo() {
    fn_8016232C(this);
    fn_80161880();
    fn_801604FC();
    fn_80162804();
}

/* Puts each part into the world of its area (name inferred). */
void GadgetModelInfo::MoveToWorld() {
    for (PartIter it = m_parts.begin(); it != m_parts.end(); it++) {
        int area;
        if (it->m_desc->m_flags & 2) {
            area = fn_80169350(fn_80169364(it->m_clump));
            fn_8003A840();
            void *world = lbl_8051D208[area].m_world;
            SetClumpWorld((*it).m_clump, world);
        } else {
            area = s_partAreas[it->m_desc->m_area];
            fn_8003A840();
            void *world = lbl_8051D208[area].m_world;
            SetClumpWorld((*it).m_clump, world);
        }
    }
}

/* Puts every part into the world of area (name inferred). */
void GadgetModelInfo::MoveToArea(int area) {
    fn_8003A840();
    void *world = lbl_8051D208[area].m_world;
    for (PartIter it = m_parts.begin(); it != m_parts.end(); it++) {
        SetClumpWorld((*it).m_clump, world);
    }
}

/* Hangs the parts with flag 1 from the node of their parent part (name inferred). */
void GadgetModelInfo::AttachParts() {
    for (PartIter it = m_parts.begin(); it != m_parts.end(); it++) {
        if (it->m_desc->m_flags & 1) {
            for (PartIter parent = m_parts.begin(); parent != m_parts.end();
                 parent++) {
                if (strcmp(parent->m_desc->m_name, it->m_desc->m_parentName) == 0) {
                    RwFrame *node = fn_80038E84(parent->m_frame, it->m_desc->m_parentNode);
                    if (node != 0) {
                        fn_80487348(node, it->m_frame);
                        break;
                    }
                }
            }
        }
    }
}

/* Takes every part out of its world (name inferred). */
void GadgetModelInfo::RemoveFromWorld() {
    for (PartIter it = m_parts.begin(); it != m_parts.end(); it++) {
        void *world = fn_8046256C(it->m_clump);
        if (world != 0) {
            fn_804623EC(world, it->m_clump);
        }
    }
}

/* Unhangs the parts with flag 1 from their parent (name inferred). */
void GadgetModelInfo::DetachParts() {
    for (PartIter it = m_parts.begin(); it != m_parts.end(); it++) {
        if ((it->m_desc->m_flags & 1) && it->m_frame->object.parent != 0) {
            fn_80487448(it->m_frame);
        }
    }
}

void GadgetModelInfo::fn_801633DC(const Mtx34f *world) {
    RwMatrix matrix;
    matrix.right.x = world->values[0][0];
    matrix.right.y = world->values[1][0];
    matrix.right.z = world->values[2][0];
    matrix.up.x = world->values[0][1];
    matrix.up.y = world->values[1][1];
    matrix.up.z = world->values[2][1];
    matrix.at.x = world->values[0][2];
    matrix.at.y = world->values[1][2];
    matrix.at.z = world->values[2][2];
    matrix.pos.x = world->values[0][3];
    matrix.pos.y = world->values[1][3];
    matrix.pos.z = world->values[2][3];
    matrix.pad3 = 0;
    matrix.pad2 = 0;
    matrix.pad1 = 0;
    matrix.flags = 0;
    fn_8047EBA0(&matrix);
    for (PartIter it = m_parts.begin(); it != m_parts.end(); ++it) {
        if (!(it->m_desc->m_flags & 1) && !(it->m_flags & 1)) {
            fn_804876E8(it->m_frame, it->m_matrix, 0);
            fn_804876E8(it->m_frame, &matrix, 2);
        }
    }
    for (MotionIter motion = m_motions.begin(); motion != m_motions.end();
         ++motion) {
        Mtx34f motionMatrix;
        fn_80016414(&motionMatrix, &matrix);
        fn_804172D8(motion->m_work.get(), &motionMatrix);
    }
}

void GadgetModelInfo::fn_80162FB0(float dt) {
    if (!m_flags.Test(0) && !m_flags.Test(1)) {
        ColliDetectArgs args;
        for (ColliderIter it = m_colliders.begin(); it != m_colliders.end();
             ++it) {
            /* The contacts the collider keeps 0xC bytes in (see GadgetCollision.h). */
            UnkColliContacts_8001EE04 *contacts =
                (UnkColliContacts_8001EE04 *)((u8 *)it->m_colli.get() + 0xC);
            if (contacts != 0 && contacts->m_count > 0) {
                if (!(it->m_flags & 1)) {
                    if (!!m_onEnter.m_function) {
                        args.m_contacts = contacts;
                        args.m_index = reinterpret_cast<s32>(this);
                        m_onEnter.CallIfSet(args);
                    }
                    it->m_flags |= 1;
                }
                if (!!m_onContact.m_function) {
                    args.m_contacts = contacts;
                    args.m_index = reinterpret_cast<s32>(this);
                    m_onContact.CallIfSet(args);
                }
            } else if (it->m_flags & 1) {
                if (!!m_onLeave.m_function) {
                    args.m_index = reinterpret_cast<s32>(this);
                    args.m_contacts = 0;
                    m_onLeave.CallIfSet(args);
                }
                it->m_flags &= ~1;
            }
        }
    }
    for (MotionIter motion = m_motions.begin(); motion != m_motions.end();
         ++motion) {
        u32 flags = motion->m_flags;
        if (!(flags & 1)) {
            if (!(flags & 2)) {
                fn_80415EA4(motion->m_work.get());
            }
            fn_800159AC(motion->m_updater);
        }
    }
    for (PartIter part = m_parts.begin(); part != m_parts.end(); ++part) {
        if (part->m_desc->m_flags & 1) {
            fn_8048722C(part->m_frame);
        }
    }
    if (m_flags.Test(0)) {
        if (m_collisionOffTime > 0.0f) {
            m_collisionOffTime -= dt;
        } else {
            m_flags.Reset(0);
        }
    }
    if (m_flags.Test(5)) {
        if (m_flags.Test(2)) {
            fn_8016271C(this);
        } else {
            fn_80162790(this);
        }
    } else if (m_flags.Test(4)) {
        m_blinkTime -= dt;
        if (m_blinkTime <= 0.0f) {
            m_blinkTime = 0.0f;
            StopBlinking();
        } else if (m_flags.Test(2)) {
            fn_8016271C(this);
        } else {
            fn_80162790(this);
        }
    }
    if (m_flags.Test(7)) {
        if (m_flags.Test(9)) {
            m_alpha += 0.98f * m_fadeSpeed;
            if (m_alpha < 1.0f) {
                fn_8015FDD8(this, m_alpha);
            } else {
                m_alpha = 1.0f;
                fn_8015FD10(this);
                m_flags.Reset(7);
                m_flags.Reset(9);
            }
        } else if (m_flags.Test(8)) {
            m_alpha -= 0.98f * m_fadeSpeed;
            if (m_alpha < 0.0f) {
                m_alpha = 0.0f;
                fn_8015FDD8(this, m_alpha);
                m_flags.Reset(7);
                m_flags.Reset(8);
                m_flags.Set(10);
            } else {
                fn_8015FDD8(this, m_alpha);
            }
        }
    }
}

void GadgetModelInfo::fn_80162EF4() {
    for (MotionIter motion = m_motions.begin(); motion != m_motions.end();
         ++motion) {
        if (!(motion->m_flags & 2)) {
            fn_80415EA4(motion->m_work.get());
        }
        fn_800159AC(motion->m_updater);
    }
    for (PartIter part = m_parts.begin(); part != m_parts.end(); ++part) {
        if (part->m_desc->m_flags & 1) {
            fn_8048722C(part->m_frame);
        }
    }
}

/* Whether the model has a variant with id. */
extern "C" bool fn_80162EA8(GadgetModelInfo *model, int id) {
    if (model->m_data != 0) {
        for (int i = 0; i < model->m_data->m_variantCount; i++) {
            if (model->m_data->m_variants[i].m_id == id) {
                return true;
            }
        }
    }
    return false;
}

GadgetModelPart::GadgetModelPart()
    : m_clump(0), m_frame(0), m_uvAnimName(0), m_texture(), m_desc(0), m_matrix(0), m_flags(0),
      m_material(0), field_0x20(0) {}

GadgetModelPart::~GadgetModelPart() {}

/* Loads the parts of variant index: each part's clump, animated textures, texture, render layer
 * and material state, turned and placed as its descriptor says (name inferred). */
bool GadgetModelInfo::LoadVariant(int index, int animated) {
    if (index > m_data->m_variantCount - 1) {
        return false;
    }
    m_variantIndex = index;
    m_variant = &m_data->m_variants[m_variantIndex];
    GadgetModelPart part;
    void *manager = fn_800122D4();
    for (int i = 0; i < m_variant->m_partCount; i++) {
        part.m_desc = &m_data->m_parts[m_variant->m_parts[i].m_part];
        part.m_clump = (RpClump *)fn_80012700(manager, m_fileGroup, part.m_desc->m_fileName);
        if (part.m_desc->m_uvAnimName != 0) {
            part.m_uvAnimName = part.m_desc->m_uvAnimName;
            fn_801EB660(GetUVAnimManager(), part.m_uvAnimName, part.m_clump);
        } else {
            part.m_uvAnimName = 0;
        }
        part.m_texture.field_0x0 = 0;
        part.m_texture.field_0x2 = -1;
        if (part.m_desc->m_texName != 0) {
            void *common = fn_80012700(manager, m_fileGroup, lbl_805E6648);
            if (common != 0) {
                fn_8048D0C4(common);
                void *dictionary = fn_80012700(manager, m_fileGroup, part.m_desc->m_texName);
                part.m_texture = fn_8004F9E8(fn_80013BC4(), part.m_clump, dictionary,
                                             part.m_desc->m_texName, 1, 0);
            }
        }
        part.m_clump = fn_80457090(part.m_clump);
        part.m_frame = (RwFrame *)part.m_clump->object.parent;
        part.m_flags = 0;
        part.m_matrix = (RwMatrix *)operator new(sizeof(RwMatrix));
        part.m_material = new UnkMaterialAnim_801621F0;
        if ((u8)animated) {
            part.m_material->m_flags.Set(2);
        }
        fn_80169448(part.m_clump);
        UnkLayerData_80164168 layer;
        layer.m_desc = part.m_desc;
        layer.m_layer = s_renderLayers[part.m_desc->m_layer];
        fn_80456C54(part.m_clump, SetAtomicLayer, &layer);
        fn_80456C54(part.m_clump, SetAtomicMaterial, part.m_material);
        const UnkModelPart_8015F7BC *desc = part.m_desc;
        fn_8047EFC8(part.m_matrix, &s_zAxis, desc->m_rotationZ, 0);
        fn_8047EFC8(part.m_matrix, &s_xAxis, desc->m_rotationX, 2);
        fn_8047EFC8(part.m_matrix, &s_yAxis, desc->m_rotationY, 2);
        fn_8047F59C(part.m_matrix, (const RwV3d *)&desc->m_position, 2);
        m_parts.push_back(part);
    }
    MoveToWorld();
    AttachParts();
    return true;
}

bool GadgetModelInfo::fn_80162A68(int id, int animated) {
    for (int i = 0; i < m_data->m_variantCount; i++) {
        if (m_data->m_variants[i].m_id == id) {
            return LoadVariant(i, animated);
        }
    }
    return false;
}

/* Loads the variant called name. */
extern "C" bool fn_801629D8(GadgetModelInfo *model, const char *name, int animated) {
    if (name == 0) {
        return false;
    }
    for (int i = 0; i < model->m_data->m_variantCount; i++) {
        if (strcmp(model->m_data->m_variants[i].m_name, name) == 0) {
            return model->LoadVariant(i, animated);
        }
    }
    return false;
}

/* Frees what the part loaded (name inferred). */
void GadgetModelPart::Release() {
    if (field_0x20 != 0) {
        delete field_0x20;
        field_0x20 = 0;
    }
    if (m_matrix != 0) {
        delete m_matrix;
        m_matrix = 0;
    }
    if (m_material != 0) {
        delete m_material;
        m_material = 0;
    }
    m_clump = 0;
    m_frame = 0;
    m_uvAnimName = 0;
    m_texture.field_0x0 = 0;
    m_texture.field_0x2 = -1;
    m_desc = 0;
    m_flags = 0;
}

void GadgetModelInfo::fn_80162804() {
    DetachParts();
    RemoveFromWorld();
    for (PartIter it = m_parts.begin(); it != m_parts.end(); ++it) {
        if (it->m_uvAnimName != 0) {
            fn_801EB550(GetUVAnimManager(), it->m_uvAnimName);
        }
        if (it->m_clump != 0) {
            fn_80456C54(it->m_clump, DropAtomicMaterial, 0);
            void *world = fn_8046256C(it->m_clump);
            if (world != 0) {
                fn_804623EC(world, it->m_clump);
            }
            fn_80457468(it->m_clump);
        }
        it->Release();
    }
    m_parts.clear();
    m_flags.Reset(2);
}

/* Hides and shows the model. */
extern "C" void fn_80162790(GadgetModelInfo *model) {
    for (PartIter it = model->m_parts.begin(); it != model->m_parts.end();
         ++it) {
        if (it->m_clump != 0) {
            fn_80038FC0(it->m_clump);
        }
    }
    model->m_flags.Set(2);
}

extern "C" void fn_8016271C(GadgetModelInfo *model) {
    for (PartIter it = model->m_parts.begin(); it != model->m_parts.end();
         ++it) {
        if (it->m_clump != 0) {
            fn_80038F94(it->m_clump);
        }
    }
    model->m_flags.Reset(2);
}

/* Whether the model is hidden. */
extern "C" bool fn_801626F4(GadgetModelInfo *model) {
    return model->m_flags.Test(2);
}

/* The name of the variant loaded. */
extern "C" const char *fn_801626D8(GadgetModelInfo *model) {
    if (model->m_variant != 0) {
        return model->m_variant->m_name;
    }
    return 0;
}

/* Blinks the model for time seconds, or until stopped when time is negative. */
extern "C" void fn_8016268C(GadgetModelInfo *model, float time) {
    if (time < 0.0f) {
        model->m_blinkTime = 0.0f;
        model->m_flags.Set(5);
    } else {
        model->m_blinkTime = time;
        model->m_flags.Set(4);
    }
}

/* Stops blinking and shows the model (name inferred). */
void GadgetModelInfo::StopBlinking() {
    m_flags.Reset(5);
    m_flags.Reset(4);
    fn_8016271C(this);
}

/* Loads variant index after the parts loaded, wrapping around the part count, and returns it. */
extern "C" int fn_801625D8(GadgetModelInfo *model, int index, int animated) {
    if (index < 0) {
        index = model->m_partCount - 1;
    }
    if (index > model->m_partCount - 1) {
        index = 0;
    }
    model->fn_80162804();
    model->LoadVariant(index, animated);
    return index;
}

/* Starts material animation index on its part (name inferred). */
bool GadgetModelInfo::ApplyMaterial(int index) {
    const GadgetModelMaterialDesc *desc = &m_data->m_materials[index];
    for (PartIter it = m_parts.begin(); it != m_parts.end(); ++it) {
        if (strcmp(desc->m_partName, it->m_desc->m_name) == 0) {
            if (desc->m_type == 0) {
                RwRGBAReal color;
                color.alpha = desc->m_alpha / 255.0f;
                color.red = desc->m_red / 255.0f;
                color.green = desc->m_green / 255.0f;
                color.blue = desc->m_blue / 255.0f;
                fn_80039114(it->m_clump, &color);
                return true;
            }
            if (desc->m_type == 1) {
                fn_800390E8(it->m_clump);
                fn_80456C54(it->m_clump, StartMaterialAnim, (void *)desc);
                return true;
            }
        }
    }
    return false;
}

/* Starts the material animation with id, or the one called name. */
extern "C" bool fn_80162424(GadgetModelInfo *model, int id) {
    for (int i = 0; i < model->m_data->m_materialCount; i++) {
        if (model->m_data->m_materials[i].m_id == id) {
            return model->ApplyMaterial(i);
        }
    }
    return false;
}

extern "C" bool fn_8016239C(GadgetModelInfo *model, const char *name) {
    if (name == 0) {
        return false;
    }
    for (int i = 0; i < model->m_data->m_materialCount; i++) {
        if (strcmp(model->m_data->m_materials[i].m_name, name) == 0) {
            return model->ApplyMaterial(i);
        }
    }
    return false;
}

/* Stops the material animations of every part. */
extern "C" void fn_8016232C(GadgetModelInfo *model) {
    for (PartIter it = model->m_parts.begin(); it != model->m_parts.end();
         ++it) {
        fn_80456C54(it->m_clump, StopAtomicMaterial, 0);
    }
}

/* The material state of the part whose material animation has id, or is called name. */
extern "C" UnkMaterialAnim_801621F0 *fn_80162298(GadgetModelInfo *model, int id) {
    for (PartIter it = model->m_parts.begin(); it != model->m_parts.end();
         ++it) {
        if (it->m_material->HasId(id)) {
            return it->m_material;
        }
    }
    return 0;
}

extern "C" UnkMaterialAnim_801621F0 *fn_801621F0(GadgetModelInfo *model, const char *name) {
    if (name == 0) {
        return 0;
    }
    for (PartIter it = model->m_parts.begin(); it != model->m_parts.end();
         ++it) {
        if (it->m_material->IsNamed(name)) {
            return it->m_material;
        }
    }
    return 0;
}

/* Builds collider index of the model over info (name inferred). */
bool GadgetModelInfo::AddCollider(int index, boost::weak_ptr<CharInfo> info) {
    if (m_data->m_colliderCount <= 0) {
        return false;
    }
    if (index < 0 || index >= m_data->m_colliderCount) {
        return false;
    }
    const GadgetModelColliderDesc *desc = &m_data->m_colliders[index];
    void *file = fn_80012700(fn_800122D4(), m_fileGroup, desc->m_fileName);
    if (file != 0) {
        GadgetModelCollider collider;
        collider.m_name = desc->m_name;
        collider.m_colli = fn_800C81F4(fn_80021B08(), boost::weak_ptr<CharInfo>(info), file,
                                       s_colliderGroups[desc->m_group], 0);
        collider.m_desc = desc;
        m_colliders.push_back(collider);
        boost::shared_ptr<CharInfo> charInfo = info.lock();
        if (charInfo.get() != 0) {
            charInfo->field_0x30 = s_colliderKinds[desc->m_kind];
        }
        return true;
    }
    return false;
}

/* Builds the collider with id over info. */
extern "C" bool fn_80161FC8(GadgetModelInfo *model, int id, const boost::weak_ptr<CharInfo> &info) {
    if (info.expired()) {
        return false;
    }
    for (int i = 0; i < model->m_data->m_colliderCount; i++) {
        if (model->m_data->m_colliders[i].m_id == id) {
            return model->AddCollider(i, info);
        }
    }
    return false;
}

bool GadgetModelInfo::fn_80161F08(const char *node, const boost::weak_ptr<CharInfo> &info) {
    if (node == 0 || info.expired()) {
        return false;
    }
    for (int i = 0; i < m_data->m_colliderCount; i++) {
        if (strcmp(m_data->m_colliders[i].m_name, node) == 0) {
            return AddCollider(i, info);
        }
    }
    return false;
}

/* Builds collider index of the model over info, moved by work (name inferred). */
bool GadgetModelInfo::AddCollider(int index, boost::weak_ptr<CharInfo> info, _stMtnPlayWork *work) {
    const GadgetModelColliderDesc *desc = &m_data->m_colliders[index];
    void *file = fn_80012700(fn_800122D4(), m_fileGroup, desc->m_fileName);
    if (file != 0) {
        GadgetModelCollider collider;
        collider.m_name = desc->m_name;
        collider.m_colli = fn_800C7F7C(fn_80021B08(), boost::weak_ptr<CharInfo>(info), file, work,
                                       s_colliderGroups[desc->m_group], 0);
        collider.m_desc = desc;
        m_colliders.push_back(collider);
        boost::shared_ptr<CharInfo> charInfo = info.lock();
        if (charInfo.get() != 0) {
            charInfo->field_0x30 = s_colliderKinds[desc->m_kind];
        }
        return true;
    }
    return false;
}

/* Builds the collider with id over info, moved by the motion with motionId. */
extern "C" bool fn_80161BD8(GadgetModelInfo *model, int id, const boost::weak_ptr<CharInfo> &info,
                            int motionId) {
    _stMtnPlayWork *work = 0;
    for (MotionIter it = model->m_motions.begin();
         it != model->m_motions.end(); ++it) {
        if (it->m_desc->m_id == motionId) {
            work = it->m_work.get();
        }
    }
    if (work == 0) {
        return false;
    }
    for (int i = 0; i < model->m_data->m_colliderCount; i++) {
        if (model->m_data->m_colliders[i].m_id == id) {
            return model->AddCollider(i, info, work);
        }
    }
    return false;
}

/* Builds the collider called name over info, moved by the motion called motionName. */
extern "C" bool fn_80161AB8(GadgetModelInfo *model, const char *name,
                            const boost::weak_ptr<CharInfo> &info, const char *motionName) {
    if (name == 0 || motionName == 0) {
        return false;
    }
    _stMtnPlayWork *work = 0;
    for (MotionIter it = model->m_motions.begin();
         it != model->m_motions.end(); ++it) {
        if (strcmp(it->m_desc->m_name, motionName) == 0) {
            work = it->m_work.get();
        }
    }
    if (work == 0) {
        return false;
    }
    for (int i = 0; i < model->m_data->m_colliderCount; i++) {
        if (strcmp(model->m_data->m_colliders[i].m_name, name) == 0) {
            return model->AddCollider(i, info, work);
        }
    }
    return false;
}

/* Drops the collider with id, or the one called name. */
extern "C" bool fn_80161A24(GadgetModelInfo *model, int id) {
    for (ColliderIter it = model->m_colliders.begin();
         it != model->m_colliders.end(); ++it) {
        if (it->m_desc->m_id == id) {
            it->Release();
            model->m_colliders.erase(it);
            return true;
        }
    }
    return false;
}

extern "C" bool fn_8016198C(GadgetModelInfo *model, const char *name) {
    for (ColliderIter it = model->m_colliders.begin();
         it != model->m_colliders.end(); ++it) {
        if (strcmp(it->m_name, name) == 0) {
            it->Release();
            model->m_colliders.erase(it);
            return true;
        }
    }
    return false;
}

/* Drops the collider (name inferred). */
void GadgetModelCollider::Release() {
    if (m_colli.get() != 0) {
        fn_800C78B0(fn_80021B08(), &m_colli);
    }
    m_name = 0;
    m_flags = 0;
    m_desc = 0;
}

void GadgetModelInfo::fn_80161880() {
    for (ColliderIter it = m_colliders.begin(); it != m_colliders.end();
         ++it) {
        it->Release();
    }
    m_colliders.clear();
}

void GadgetModelInfo::fn_801617D0(int id, int arg1, float x, float y, float z) {
    for (ColliderIter it = m_colliders.begin(); it != m_colliders.end();
         ++it) {
        if (it->m_desc->m_id == id) {
            it->m_colli->SetSize(x, y, z, arg1);
        }
    }
}

/* Moves the collider with id by offset. */
extern "C" void fn_80161740(GadgetModelInfo *model, int id, const Vec3f *offset, int arg3) {
    for (ColliderIter it = model->m_colliders.begin();
         it != model->m_colliders.end(); ++it) {
        if (it->m_desc->m_id == id) {
            it->m_colli->SetOffset(offset, arg3);
        }
    }
}

/* The collision file of the collider with id. */
extern "C" void *fn_801616CC(GadgetModelInfo *model, int id) {
    for (int i = 0; i < model->m_data->m_colliderCount; i++) {
        const GadgetModelColliderDesc *desc = &model->m_data->m_colliders[i];
        if (id == desc->m_id) {
            return fn_80012700(fn_800122D4(), model->m_fileGroup, desc->m_fileName);
        }
    }
    return 0;
}

/* The collider with id, or the one called name. */
extern "C" boost::weak_ptr<CharaColliBase> fn_80161614(GadgetModelInfo *model, int id) {
    boost::weak_ptr<CharaColliBase> result;
    for (ColliderIter it = model->m_colliders.begin();
         it != model->m_colliders.end(); ++it) {
        if (it->m_desc->m_id == id) {
            result = it->m_colli;
            return result;
        }
    }
    return result;
}

extern "C" boost::weak_ptr<CharaColliBase> fn_80161518(GadgetModelInfo *model, const char *name) {
    boost::weak_ptr<CharaColliBase> result;
    for (ColliderIter it = model->m_colliders.begin();
         it != model->m_colliders.end(); ++it) {
        if (strcmp(it->m_name, name) == 0) {
            result = it->m_colli;
            return result;
        }
    }
    return result;
}

/* The attack collision of the collider with id, or the one called name. */
extern "C" boost::weak_ptr<CharaColliAttack> fn_80161428(GadgetModelInfo *model, int id) {
    boost::weak_ptr<CharaColliAttack> result;
    for (ColliderIter it = model->m_colliders.begin();
         it != model->m_colliders.end(); ++it) {
        if (it->m_desc->m_id == id) {
            result = fn_800C4374(fn_80021B08(), it->m_colli);
            return result;
        }
    }
    return result;
}

extern "C" boost::weak_ptr<CharaColliAttack> fn_80161334(GadgetModelInfo *model, const char *name) {
    boost::weak_ptr<CharaColliAttack> result;
    for (ColliderIter it = model->m_colliders.begin();
         it != model->m_colliders.end(); ++it) {
        if (strcmp(it->m_name, name) == 0) {
            result = fn_800C4374(fn_80021B08(), it->m_colli);
            return result;
        }
    }
    return result;
}

/* Turns the collisions off for time seconds, on and off. */
extern "C" void fn_80161308(GadgetModelInfo *model, float time) {
    model->m_collisionOffTime = time;
    model->m_flags.Set(0);
}

extern "C" void fn_801612E0(GadgetModelInfo *model) {
    model->m_flags.Set(1);
}

extern "C" void fn_801612B8(GadgetModelInfo *model) {
    model->m_flags.Reset(1);
}

/* Turns the collider with id, or the one called name, off and on. */
extern "C" void fn_80161230(GadgetModelInfo *model, int id, int arg2) {
    for (ColliderIter it = model->m_colliders.begin();
         it != model->m_colliders.end(); ++it) {
        if (it->m_desc->m_id == id) {
            it->m_colli->TurnOff(arg2);
        }
    }
}

extern "C" void fn_801611A4(GadgetModelInfo *model, const char *name, int arg2) {
    for (ColliderIter it = model->m_colliders.begin();
         it != model->m_colliders.end(); ++it) {
        if (strcmp(it->m_name, name) == 0) {
            it->m_colli->TurnOff(arg2);
        }
    }
}

extern "C" void fn_8016111C(GadgetModelInfo *model, int id, int arg2) {
    for (ColliderIter it = model->m_colliders.begin();
         it != model->m_colliders.end(); ++it) {
        if (it->m_desc->m_id == id) {
            it->m_colli->TurnOn(arg2);
        }
    }
}

extern "C" void fn_80161090(GadgetModelInfo *model, const char *name, int arg2) {
    for (ColliderIter it = model->m_colliders.begin();
         it != model->m_colliders.end(); ++it) {
        if (strcmp(it->m_name, name) == 0) {
            it->m_colli->TurnOn(arg2);
        }
    }
}

void GadgetModelInfo::fn_8016108C() {}

/* Starts motion index on its part (name inferred). */
bool GadgetModelInfo::StartMotion(int index) {
    const GadgetModelMotionDesc *desc = &m_data->m_motions[index];
    for (PartIter it = m_parts.begin(); it != m_parts.end(); ++it) {
        if (strcmp(desc->m_partName, it->m_desc->m_name) == 0) {
            if (it->m_flags & 1) {
                return false;
            }
            void *anim = fn_804106C0(desc->m_animId);
            s32 base;
            if (HasExtension(desc->m_fileName, ".MTP")) {
                base = fn_8001258C(fn_800122D4(), m_fileGroup, desc->m_fileName);
            } else {
                base = (u16)fn_80412738(desc->m_motionId);
            }
            _stMtnPlayWork *work = fn_8041B908(anim, 1);
            UnkClumpUpdater_80015A7C *updater = new UnkClumpUpdater_80015A7C(it->m_clump, work);
            GadgetModelMotion motion;
            motion.m_name = desc->m_name;
            motion.m_work = boost::shared_ptr<_stMtnPlayWork>(work, fn_8041B884);
            motion.m_anim = anim;
            motion.m_base = base;
            motion.m_updater = updater;
            motion.m_desc = desc;
            m_motions.push_back(motion);
            it->m_flags |= 1;
            return true;
        }
    }
    return false;
}

bool GadgetModelInfo::fn_80160C28(int id) {
    for (int i = 0; i < m_data->m_motionCount; i++) {
        if (m_data->m_motions[i].m_id == id) {
            return StartMotion(i);
        }
    }
    return false;
}

bool GadgetModelInfo::fn_80160BA0(const char *motion) {
    if (motion == 0) {
        return false;
    }
    for (int i = 0; i < m_data->m_motionCount; i++) {
        if (strcmp(m_data->m_motions[i].m_name, motion) == 0) {
            return StartMotion(i);
        }
    }
    return false;
}

bool GadgetModelInfo::fn_80160B18(int id) {
    for (MotionIter it = m_motions.begin(); it != m_motions.end(); ++it) {
        if (it->m_desc->m_id == id) {
            fn_80417568(it->m_work.get(), it->m_base);
            return true;
        }
    }
    return false;
}

/* Sets motion number motion (counted from 1) of the playing motion with id, or called name. */
extern "C" bool fn_80160A84(GadgetModelInfo *model, int id, int motion) {
    for (MotionIter it = model->m_motions.begin();
         it != model->m_motions.end(); ++it) {
        if (it->m_desc->m_id == id) {
            fn_80417568(it->m_work.get(), motion + it->m_base - 1);
            return true;
        }
    }
    return false;
}

extern "C" bool fn_801609F8(GadgetModelInfo *model, const char *name) {
    for (MotionIter it = model->m_motions.begin();
         it != model->m_motions.end(); ++it) {
        if (strcmp(it->m_name, name) == 0) {
            fn_80417568(it->m_work.get(), it->m_base);
            return true;
        }
    }
    return false;
}

extern "C" bool fn_80160960(GadgetModelInfo *model, const char *name, int motion) {
    for (MotionIter it = model->m_motions.begin();
         it != model->m_motions.end(); ++it) {
        if (strcmp(it->m_name, name) == 0) {
            fn_80417568(it->m_work.get(), motion + it->m_base - 1);
            return true;
        }
    }
    return false;
}

/* Sets motion number motion of the playing motion with id and jumps to its frame. */
extern "C" bool fn_801608AC(GadgetModelInfo *model, int id, int motion) {
    for (MotionIter it = model->m_motions.begin();
         it != model->m_motions.end(); ++it) {
        if (it->m_desc->m_id == id) {
            u16 number = motion + it->m_base - 1;
            fn_80417568(it->m_work.get(), number);
            UnkMotionFrame_804125F4 frame;
            fn_804125F4(&frame, number);
            fn_80416E20(it->m_work.get(), 0, frame.m_frame);
            return true;
        }
    }
    return false;
}

/* Restarts the playing motion with id, or every one called name. */
extern "C" void fn_8016082C(GadgetModelInfo *model, int id) {
    for (MotionIter it = model->m_motions.begin();
         it != model->m_motions.end(); ++it) {
        if (it->m_desc->m_id == id) {
            fn_80416F28(it->m_work.get(), 0, 1, 0);
        }
    }
}

extern "C" void fn_801607A8(GadgetModelInfo *model, const char *name) {
    for (MotionIter it = model->m_motions.begin();
         it != model->m_motions.end(); ++it) {
        if (strcmp(it->m_name, name) == 0) {
            fn_80416F28(it->m_work.get(), 0, 1, 0);
        }
    }
}

/* Stops the motion with id and frees its part. */
extern "C" void fn_80160670(GadgetModelInfo *model, int id) {
    for (MotionIter it = model->m_motions.begin();
         it != model->m_motions.end(); ++it) {
        if (it->m_desc->m_id == id) {
            for (PartIter part = model->m_parts.begin();
                 part != model->m_parts.end(); ++part) {
                if (strcmp(it->m_desc->m_partName, part->m_desc->m_name) == 0) {
                    part->m_flags &= ~1;
                    break;
                }
            }
            (*it).Release();
            model->m_motions.erase(it);
            return;
        }
    }
}

void GadgetModelInfo::fn_801604FC() {
    for (MotionIter it = m_motions.begin(); it != m_motions.end(); ++it) {
        for (PartIter part = m_parts.begin(); part != m_parts.end();
             ++part) {
            if (strcmp(it->m_desc->m_partName, part->m_desc->m_name) == 0) {
                part->m_flags &= ~1;
                break;
            }
        }
        (*it).Release();
    }
    m_motions.clear();
}

/* Pauses the motion with id, or called name, and resumes the one with id. */
extern "C" void fn_80160480(GadgetModelInfo *model, int id) {
    for (MotionIter it = model->m_motions.begin();
         it != model->m_motions.end(); ++it) {
        if (it->m_desc->m_id == id) {
            it->m_flags |= 1;
            break;
        }
    }
}

extern "C" void fn_80160400(GadgetModelInfo *model, const char *name) {
    for (MotionIter it = model->m_motions.begin();
         it != model->m_motions.end(); ++it) {
        if (strcmp(it->m_name, name) == 0) {
            it->m_flags |= 1;
            break;
        }
    }
}

extern "C" void fn_80160384(GadgetModelInfo *model, int id) {
    for (MotionIter it = model->m_motions.begin();
         it != model->m_motions.end(); ++it) {
        if (it->m_desc->m_id == id) {
            it->m_flags &= ~1;
            break;
        }
    }
}

/* The motion work of the playing motion with id, or called name. */
extern "C" boost::weak_ptr<_stMtnPlayWork> fn_801602B4(GadgetModelInfo *model, int id) {
    boost::weak_ptr<_stMtnPlayWork> result;
    for (MotionIter it = model->m_motions.begin();
         it != model->m_motions.end(); ++it) {
        if (it->m_desc->m_id == id) {
            result = boost::weak_ptr<_stMtnPlayWork>(it->m_work);
            return result;
        }
    }
    return result;
}

extern "C" boost::weak_ptr<_stMtnPlayWork> fn_801601A0(GadgetModelInfo *model, const char *name) {
    boost::weak_ptr<_stMtnPlayWork> result;
    for (MotionIter it = model->m_motions.begin();
         it != model->m_motions.end(); ++it) {
        if (strcmp(it->m_name, name) == 0) {
            result = boost::weak_ptr<_stMtnPlayWork>(it->m_work);
            return result;
        }
    }
    return result;
}

/* The first motion number of the playing motion called name. */
extern "C" s32 fn_80160124(GadgetModelInfo *model, const char *name) {
    for (MotionIter it = model->m_motions.begin();
         it != model->m_motions.end(); ++it) {
        if (strcmp(it->m_name, name) == 0) {
            return it->m_base;
        }
    }
    return 0;
}

/* Whether the playing motion with id has ended. */
extern "C" bool fn_80160040(GadgetModelInfo *model, int id) {
    boost::weak_ptr<_stMtnPlayWork> weak = fn_801602B4(model, id);
    if (!weak.expired()) {
        boost::shared_ptr<_stMtnPlayWork> work = weak.lock();
        if (work.get() != 0) {
            return fn_80416F28(work.get(), 0, 3, 1);
        }
    }
    return false;
}

void GadgetModelInfo::fn_8015FF60() {
    for (MotionIter it = m_motions.begin(); it != m_motions.end(); ++it) {
        fn_80416F28(it->m_work.get(), 0, 0xB, 0);
    }
}

/* The number of variants of the model. */
extern "C" int fn_8015FF44(GadgetModelInfo *model) {
    if (model->m_data != 0) {
        return model->m_data->m_variantCount;
    }
    return 0;
}

/* Sets the alpha of every part (from 0 to 1) and takes the parts to the transparent area. */
extern "C" void fn_8015FDD8(GadgetModelInfo *model, float alpha) {
    if (!model->m_flags.Test(6)) {
        model->RemoveFromWorld();
        model->MoveToArea(10);
        u8 value = 255.0f * alpha;
        for (PartIter it = model->m_parts.begin();
             it != model->m_parts.end(); ++it) {
            if (it->m_material != 0) {
                (*it).m_material->m_fadeAlpha = value;
                (*it).m_material->m_flags.Set(1);
            }
        }
        model->m_flags.Set(6);
    } else {
        u8 value = 255.0f * alpha;
        for (PartIter it = model->m_parts.begin();
             it != model->m_parts.end(); ++it) {
            if (it->m_material != 0) {
                (*it).m_material->m_fadeAlpha = value;
            }
        }
    }
}

/* Makes the parts opaque again and takes them back to their areas. */
extern "C" void fn_8015FD10(GadgetModelInfo *model) {
    if (model->m_flags.Test(6)) {
        for (PartIter it = model->m_parts.begin();
             it != model->m_parts.end(); ++it) {
            if (it->m_material != 0) {
                (*it).m_material->m_flags.Reset(1);
            }
        }
        model->RemoveFromWorld();
        model->MoveToWorld();
        model->m_flags.Reset(6);
    }
}

/* Fades the model out and in, over time seconds (0.15 when not positive). */
extern "C" void fn_8015FC94(GadgetModelInfo *model, float time) {
    model->m_flags.Set(7);
    model->m_flags.Set(8);
    model->m_flags.Reset(10);
    if (time > 0.0f) {
        model->m_fadeSpeed = time;
    } else {
        model->m_fadeSpeed = 0.15f;
    }
}

extern "C" void fn_8015FC20(GadgetModelInfo *model, float time) {
    model->m_flags.Set(7);
    model->m_flags.Set(9);
    model->m_alpha = 0.0f;
    if (time > 0.0f) {
        model->m_fadeSpeed = time;
    } else {
        model->m_fadeSpeed = 0.15f;
    }
}

/* Whether the model has faded out. */
extern "C" bool fn_8015FBF8(GadgetModelInfo *model) {
    return model->m_flags.Test(10);
}

/* Not called anywhere: the linker strips it. Retail emits the collider list's begin and end
 * right after the part list's, which a function here that walks the colliders explains (inferred;
 * the original function is unknown, and the name is a placeholder). */
static int UnkCountColliders(GadgetModelInfo *model) {
    int count = 0;
    for (ColliderIter it = model->m_colliders.begin();
         it != model->m_colliders.end(); ++it) {
        count++;
    }
    return count;
}

/* Calls callback with each part of model while it returns non-null. */
struct UnkModelPart_8015FA78 {
    /* 0x0 */ RpClump *m_clump;
    /* 0x4 */ const char *m_name;
    /* 0x8 */ s32 m_id;
};

typedef UnkModelPart_8015FA78 *(*UnkModelPartCallback_8015FA78)(UnkModelPart_8015FA78 *part,
                                                                void *data);

extern "C" void fn_8015FA78(GadgetModelInfo *model, UnkModelPartCallback_8015FA78 callback,
                            void *data) {
    for (PartIter it = model->m_parts.begin(); it != model->m_parts.end();
         ++it) {
        UnkModelPart_8015FA78 part;
        part.m_clump = it->m_clump;
        part.m_name = it->m_desc->m_name;
        part.m_id = it->m_desc->m_id;
        if (callback(&part, data) == 0) {
            break;
        }
    }
}

/* Resets the shared flags (meaning unknown). */
extern "C" void fn_8015FA48(void) {
    s_sharedFlags.Clear();
    s_sharedFlags.Set(3);
}

/* Loads model modelId, or the model file called fileName. */
extern "C" boost::shared_ptr<GadgetModelInfo> fn_8015F9D8(int arg0, int modelId) {
    boost::shared_ptr<GadgetModelInfo> model(new GadgetModelInfo(arg0, modelId));
    return model;
}

extern "C" boost::shared_ptr<GadgetModelInfo> fn_8015F800(int arg0, const char *fileName) {
    boost::shared_ptr<GadgetModelInfo> model(new GadgetModelInfo(arg0, fileName));
    return model;
}

/* The descriptor of the part with id. */
extern "C" const UnkModelPart_8015F7BC *fn_8015F7BC(GadgetModelInfo *model, int id) {
    if (model->m_data != 0) {
        for (int i = 0; i < model->m_data->m_partCount; i++) {
            if (model->m_data->m_parts[i].m_id == id) {
                return &model->m_data->m_parts[i];
            }
        }
    }
    return 0;
}

/* Player::GetItemCommand (class name from RTTI; vtable at 0x805211DC): the command of type 0x11B
 * that gives a player an item. Its constructor keeps its fn_ name for its callers; it is written
 * as a function over the command's storage. */
extern "C" void __ct__Q26Player13PlayerCommandFii(void *command, int id, int arg1);
extern void *lbl_805211DC[];

struct UnkGetItemCommand_8015F760 {
    /* 0x00 */ void *m_vtable;
    /* 0x04 */ u8 field_0x4[0x14 - 0x4];
    /* 0x14 */ s32 field_0x14;
    /* 0x18 */ s32 m_item;
    /* 0x1C */ u8 field_0x1C;
};

extern "C" CharCommand *fn_8015F760(UnkGetItemCommand_8015F760 *command, int arg0, int item,
                                    int arg2, int arg3) {
    __ct__Q26Player13PlayerCommandFii(command, 0x11B, arg3);
    command->m_vtable = lbl_805211DC;
    command->field_0x14 = arg0;
    command->m_item = item;
    command->field_0x1C = arg2;
    return (CharCommand *)command;
}

/* Gives target item id (the other arguments' meaning is unknown). */
extern "C" bool fn_8015F6EC(boost::shared_ptr<CharInfo> target, int id, int arg2, int arg3) {
    CharCommand *command = (CharCommand *)CharCommand::operator new(0x20);
    if (command != 0) {
        command = fn_8015F760((UnkGetItemCommand_8015F760 *)command, id, arg2, arg3, 4);
    }
    target->PushCommand(command);
    return true;
}

/* Draws a cross of size around position with debug lines. */
extern "C" void fn_8015F5D8(const Vec3f *position, float size) {
    u8 xSegment[0x18];
    u8 ySegment[0x18];
    u8 zSegment[0x18];
    Vec3f low;
    Vec3f high;
    AssignVector(low, *fn_800091D0(&high, position));
    low.x -= size;
    high.x += size;
    fn_802E0B58(fn_80020A20(xSegment, &low, &high), &lbl_805F06D8);
    AssignVector(low, *fn_800091D0(&high, position));
    low.y -= size;
    high.y += size;
    fn_802E0B58(fn_80020A20(ySegment, &low, &high), &lbl_805F06D8);
    AssignVector(low, *fn_800091D0(&high, position));
    low.z -= size;
    high.z += size;
    fn_802E0B58(fn_80020A20(zSegment, &low, &high), &lbl_805F06D8);
}

inline void UnkModelCallback_800D0B64::CallIfSet(ColliDetectArgs &args) const {
    if (!m_function.empty()) {
        m_function(args);
    }
}
