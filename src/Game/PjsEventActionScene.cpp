/* TPjsEventActionScene (class name from RTTI; vtable at 0x8052BA24, bases Task and the empty
 * TPjsEventActionData): one scene of a real-time event. It loads the scene's .ONE archive
 * (event\eventNNNN_sceneX.one, the next scene's file being checked as well), sorts the archive's
 * files by extension into the event camera (.SNB), the centre point (.CEN), a second camera file
 * (.SNN), the models of the event's characters and the models of the stage props, and then plays
 * them all on the event's clock (lbl_805EF2A8). The event manager (TPjsEventActionManager, the
 * next unit) creates the scenes and drives the clock through the event state the two units share
 * (lbl_805EF2A0 and the variables after it). File name inferred from the class. Names marked
 * "(name inferred)" and all class names other than TPjsEventActionScene and TPjsEventActionData
 * are not original; the functions other units call keep their fn_ names. Definitions are in reverse
 * address order (deferred inlining emits functions last-defined first). The unit's code reaches its
 * .rodata through a pooled base address, so the linker keeps that section whole.
 *
 * Not matching yet (82 of 90 functions match):
 * - Retail's .sdata2 keeps separate copies of equal constants for each class (0.0f six times, 30.0f,
 *   1.0f and the extension strings twice) and pools them in source order, with the character table's
 *   strings after the constants of the code before it; GC/2.7 merges equal constants and pools
 *   every file-scope initializer before all function constants (see docs/research-notes.md). The
 *   .rodata strings and .data/.sdata objects come out in a different order for the same reason, so
 *   StartLoad's offsets from the pooled .rodata base differ.
 * - IsSnbFile addresses the .SNB string as an array of unknown size (lis/addi), which the original
 *   compiler did by generating it before the string's definition; here it names the retail address
 *   (lbl_805F4C60, left undefined) and Camera::Load uses its own literal.
 * - Register or stack-slot allocation differs in UnkEventModel's Show, Update and destructor, the
 *   character list's Find and LinkParts, the camera's Update and fn_80170C58. */
#include <cstdio>
#include <cstring>

#include "Effect/System/EffectElement.h"
#include "Game/CameraModeUser.h"
#include "Game/ExternalCameraRequest.h"
#include "Game/MathTypes.h"
#include "Game/Task.h"
#include "boost/shared_ptr.hpp"
#include "boost/weak_ptr.hpp"
#include <dolphin/types.h>
#include "size_assert.h"

using SonicteamUSA::System::Camera::CameraMode;
using SonicteamUSA::System::Camera::CameraModeUser;

/* The unreferenced constant block of the original headers at the start of .rodata (as in
 * GadgetModelInfo.cpp; the bytes are reproduced). */
static const float lbl_804C3668[19] = {
    0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 1.0f, 0.0f, 0.0f,
    0.0f, 1.0f, 0.0f, 0.0f, 0.0f, 1.0f, 0.0f, 0.0f, 0.0f,
};

/* The RenderWare types used here, declared locally (layouts as in rw/rwcore.h and rw/rpworld.h,
 * reduced to the members used). */
extern "C" {
typedef struct RwV3d {
    float x;
    float y;
    float z;
} RwV3d;

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
    /* 0x08 */ u8 field_0x8[0x10 - 0x8];
    /* 0x10 */ RwMatrix modelling;
} RwFrame;

typedef struct RpClump {
    /* 0x00 */ RwObject object; /* the parent is the clump's frame */
} RpClump;

typedef struct RpAtomic RpAtomic;
typedef RpAtomic *(*RpAtomicCallBack)(RpAtomic *atomic, void *data);

typedef struct RwRGBA {
    u8 red;
    u8 green;
    u8 blue;
    u8 alpha;
} RwRGBA;

typedef struct _stMtnPlayWork _stMtnPlayWork;
}

/* The game state (struct name unknown; see Game/CityBombSmall.cpp). */
struct UnkGameState_8057D6A0 {
    /* 0x000 */ u8 field_0x0[0x50];
    /* 0x050 */ s32 m_frameRate; /* (name inferred) */
    /* 0x054 */ u8 field_0x54[0x150 - 0x54];
};
SIZE_ASSERT(UnkGameState_8057D6A0, 0x150);

extern UnkGameState_8057D6A0 lbl_8057D6A0;

/* The stage (class name unknown; fn_8003A840 returns it): the world of each layer (see
 * Game/NpcMotion.cpp), and the names of the event characters' models (name inferred). */
struct UnkCharaName_8051D208 {
    /* 0x00 */ const char *m_name;
    /* 0x04 */ u8 field_0x4[0x18 - 0x4];
};

extern UnkCharaName_8051D208 lbl_8051D208[];

class UnkStage_8003A840 {
  public:
    void *GetWorld(int layer);
    /* The model name of character index (name inferred). */
    const char *GetCharaName(int index) {
        return lbl_8051D208[index].m_name;
    }
};

/* The clumps' frame effects (see Game/ClumpFrameEffects.cpp). */
class UnkFrameEffects_803284B4 {
  public:
    UnkFrameEffects_803284B4();
    s32 Attach(RpClump *clump);
    void Tick();
};

/* A loaded clump's animation updater (see Game/GadgetModelInfo.cpp). */
class UnkClumpUpdater_80015A7C {
  public:
    UnkClumpUpdater_80015A7C(RpClump *clump, _stMtnPlayWork *motion);
    ~UnkClumpUpdater_80015A7C();

    /* 0x00 */ u8 field_0x0[0x1C];
};

/* A .ONE archive (class name unknown): its header's file count, and whether it is busy (names
 * inferred). Its constructor (0x8004CFB0) and deleting destructor (0x8004CF68) are out of line. */
struct UnkOneHeader_8004CFB0 {
    /* 0x00 */ u8 field_0x0[0x10];
    /* 0x10 */ s32 m_count;
};

struct UnkOneFile_8004CFB0 {
    /* 0x00 */ u8 field_0x0[0x34];
    /* 0x34 */ UnkOneHeader_8004CFB0 *m_header;
    /* 0x38 */ u8 field_0x38[0x54 - 0x38];
    /* 0x54 */ s32 m_busy;
    /* 0x58 */
};

/* The .CEN file (see Game/CenFile.cpp). */
class UnkCenFile_80329514 {
  public:
    UnkCenFile_80329514();
    ~UnkCenFile_80329514();

    /* 0x00 */ char m_name[0x40];
    /* 0x40 */ s32 m_state; /* 1 loading, 2 loaded */
    /* 0x44 */ u8 field_0x44[0x108 - 0x44];
};

/* The .SNN file of the unit at 0x80228104 (class and meaning unknown; constructor 0x802289B0,
 * destructor 0x80228968). Its other functions keep their fn_ names. */
class UnkSnnFile_802289B0 {
  public:
    UnkSnnFile_802289B0();
    ~UnkSnnFile_802289B0();

    /* 0x00 */ u8 field_0x0[0x54];
};

extern "C" {
void fn_8022890C(UnkSnnFile_802289B0 *file);
bool fn_80228874(UnkSnnFile_802289B0 *file);
bool fn_8022887C(UnkSnnFile_802289B0 *file, UnkOneFile_8004CFB0 *archive, int index);
void fn_80228844(UnkSnnFile_802289B0 *file, float time);
}

/* The name table (defined below, before the event models). */
class UnkNameTable_802DFC7C;

/* The motion heap (class name unknown; constructed by 0x800151EC, destroyed by 0x80015160). */
class UnkMotionHeap_800151EC;

extern "C" {
/* RenderWare: RpClumpForAllAtomics, RpClumpClone, RpClumpDestroy, the world of a clump and
 * removing and adding it (see Game/GadgetModelInfo.cpp), RwFrameUpdateObjects, RwFrameTranslate and
 * RwMatrixTransform (behaviour inferred). */
RpClump *fn_80456C54(RpClump *clump, RpAtomicCallBack callBack, void *data);
RpClump *fn_80457090(RpClump *clump);
void fn_80457468(RpClump *clump);
void *fn_8046256C(RpClump *clump);
void fn_804623EC(void *world, RpClump *clump);
void fn_804622FC(void *world, RpClump *clump);
RwFrame *fn_8048722C(RwFrame *frame);
RwFrame *fn_804875B0(RwFrame *frame, const Vec3f *offset, int combine);
RwMatrix *fn_8047F72C(RwMatrix *matrix, const RwMatrix *transform, int combine);
/* An atomic's alpha, and resetting it (behaviour inferred). */
void fn_80444630(RpAtomic *atomic, float alpha);
void fn_804443A0(RpAtomic *atomic, void *data);
/* The frame of a clump's frame tree with a name (behaviour inferred). */
RwFrame *fn_80038E84(RwFrame *frame, const char *name);
/* A clump's colour, and a clump's draw setup (behaviour inferred). */
void fn_80037B84(RpClump *clump, const RwRGBA *color);
void fn_8003815C(RpClump *clump);
void *fn_80038FEC(RpClump *clump, int arg1);
/* The stage (see above). */
UnkStage_8003A840 *fn_8003A840(void);
UnkFrameEffects_803284B4 *fn_8016F990(void);

/* Motions (see Game/NpcMotion.cpp; behaviour inferred). */
_stMtnPlayWork *fn_8041B908(void *dictionary, int arg1);
void fn_8041B884(_stMtnPlayWork *work);
void fn_80416F28(_stMtnPlayWork *work, int arg1, int arg2, int arg3);
void fn_80416E20(_stMtnPlayWork *work, int arg1, float frame);
void fn_80415EA4(_stMtnPlayWork *work);
void *fn_80415E70(_stMtnPlayWork *work, int id);
void fn_804172D8(_stMtnPlayWork *work, void *node);
void fn_804173A8(_stMtnPlayWork *work, const Vec3f *position, int arg2, int arg3);
void fn_80417568(_stMtnPlayWork *work, u16 motion);
u32 fn_80412BFC(void *data, const char *name, void (*release)(void *), int arg3);
void fn_804129D4(u16 motion);
void fn_80016390(RwMatrix *out, void *node);
void fn_800159AC(UnkClumpUpdater_80015A7C *updater);
/* A bone file (behaviour inferred). */
void fn_80442B98(void *bones);

/* The motion heap, and the bone data of a name (behaviour inferred). */
UnkMotionHeap_800151EC *fn_80170234(void);
void fn_800150FC(UnkMotionHeap_800151EC *loader, int arg1);
UnkNameTable_802DFC7C *fn_801702E4(void);
const char *fn_802DFAB4(UnkNameTable_802DFC7C *table, const char *name);
void *fn_802DFB04(UnkNameTable_802DFC7C *table, void *data);

/* Files of an archive: the name of one, and loading one with a callback (see Game/CenFile.cpp and
 * Game/CenFileNumber.cpp; behaviour inferred). */
UnkOneFile_8004CFB0 *fn_8004CFB0(void *file, int arg1, int arg2);
void fn_8004CF68(UnkOneFile_8004CFB0 *file, int flags);
bool fn_8004CAF8(UnkOneFile_8004CFB0 *file);
const char *fn_8004CA28(UnkOneFile_8004CFB0 *file, int index);
void fn_8004CC6C(UnkOneFile_8004CFB0 *file, const char *name, void (*callback)(void *),
                 void *user, void *buffer, int size);
void fn_8004C358(UnkOneFile_8004CFB0 *file, int index, void (*callback)(void *, void *),
                 void *user);
void fn_8004BF40(UnkOneFile_8004CFB0 *file, int index, void (*callback)(void *, void *),
                 void *user);
void fn_8004BB24(UnkOneFile_8004CFB0 *file, int index, void (*callback)(void *, void *),
                 void *user);
void fn_8004ADB0(UnkOneFile_8004CFB0 *file, int index, void (*callback)(void *, void *),
                 void *user);
/* Allocating and freeing (see Game/CenFileNumber.cpp), and the heap (behaviour inferred). */
void *fn_80049B90(u32 size, int heap);
void fn_80049B34(void *p);
void fn_80049B14(int heap);
void fn_80049AF4(void);
s32 fn_8004E85C(s32 size, s32 align);
/* The file system: whether a file exists, and its size (behaviour inferred). */
struct UnkFileSystem_800371D8 {
    /* 0x0 */ u32 field_0x0;
    /* 0x4 */ bool (*m_exists)(const char *name);
};
UnkFileSystem_800371D8 *fn_800371D8(void);
s32 fn_80044E34(UnkFileSystem_800371D8 *fs, const char *name);
/* OSReport (behaviour inferred). */
void fn_803A953C(const char *format, ...);
/* strstr and strnicmp (see Game/CenFileNumber.cpp and Game/CenFile.cpp). */
char *fn_803AC6C0(const char *s, const char *sub);
int fn_803AC8DC(const char *a, const char *b, u32 n);

/* The model resources (behaviour inferred). */
void *fn_80013BC4(void);
void fn_8004F9E8(void *out, void *resources, RpClump *clump, void *data, const char *name, int arg5,
                 void *user);
void *fn_8004FE88(void *resources, void *data);
void fn_8004FE00(void *resources, void *data);
void fn_8005001C(void);
void *fn_80014534(void);
void fn_80039618(void *loader, void *data, int arg2);
void fn_802E0600(void *data);
void fn_8048D0C4(void *data);

/* The .CEN file (see Game/CenFile.cpp), the stage's points and the event manager's update
 * (behaviour inferred). */
bool fn_803290D0(UnkCenFile_80329514 *file, float frame);
bool fn_80329420(UnkCenFile_80329514 *file, UnkOneFile_8004CFB0 *archive, int index);
int fn_80329524(const char *name);
struct UnkObject_80170D8C {
    /* 0x0 */ u32 field_0x0;
    /* 0x4 */ s32 m_state; /* 1 while loading (name inferred) */
    /* 0x8 */ u8 field_0x8[0x34 - 0x8];
};
UnkObject_80170D8C *fn_80170D8C(void);
UnkObject_80170D8C *fn_80174228(UnkObject_80170D8C *object);
void fn_801741E0(void *object, short flags);
void fn_80173CD4(UnkObject_80170D8C *object, s32 scene, float time, float previous);
struct UnkObject_80051734 {
    /* 0x00 */ u8 field_0x0[0x30];
    /* 0x30 */ u32 m_flags;
};
UnkObject_80051734 *fn_80051734(void);
/* The event's task holder, and a debug print to it (behaviour inferred). */
struct UnkEventHolder_801723E0;
UnkEventHolder_801723E0 *fn_801723E0(void);
void fn_80174250(UnkEventHolder_801723E0 *holder, const char *format, ...);

/* Camera paths (see Game/EggLastMechCamera.cpp; behaviour inferred). */
int fn_80010648(void *resource);
UnkCameraData_80012510 *fn_80011CE8(void *resource, int arg1);
void fn_80010010(UnkCameraData_80012510 *data, const Vec3f *position, float time);
int fn_80011B7C(UnkCameraData_80012510 *data, float *time, const float *step);
void fn_8000D968(CameraModeUser *mode, const Vec2f *angles);
float DegToRad(float degrees);
void fn_80009618(boost::shared_ptr<CameraModeUser> *mode);

/* Effects: a matrix from a RenderWare matrix, and an effect at a matrix (see
 * Game/ClumpFrameEffects.cpp and Game/GadgetCommonData.cpp). */
Mtx34f fn_800635A4(const RwMatrix *matrix);
boost::weak_ptr<Effect::System::EffectElement> fn_801D2B74(int arg0, const Mtx34f *matrix, int id,
                                                           int arg3);
}

/* Passes a returned matrix on by address (as in ClumpFrameEffects.cpp). */
inline const Mtx34f &RefM(const Mtx34f &matrix) {
    return matrix;
}

/* The .CEN files' extension (Game/CenFile.cpp) and the .SNN files' (the unit at 0x80228104). */
extern const char lbl_805F9CA8[];
extern const char lbl_805F6F80[];
/* The tag of the camera files (Game/CenFileNumber.cpp). */
extern const char *lbl_805ED3D0;
/* The event's time as other units read it (name inferred). */
extern float lbl_805E39A0;
/* The motion heap's address once created (see fn_80170234; name inferred). */
extern UnkMotionHeap_800151EC *lbl_805EDFF8;

/* The event state the scene and the event manager share (names inferred). */
s32 lbl_805EF2A0;  /* the event number */
s32 lbl_805EF2A4;  /* the event's point, or -1 (see Game/CenFile.cpp) */
float lbl_805EF2A8; /* the event's clock, in frames */
u8 lbl_805EF2AC[8];
u8 lbl_805EF2B4;   /* set while the next scene is loaded */
u8 lbl_805EF2B5;   /* the clock runs at 30 frames per second */
s32 lbl_805EF2B8;  /* the current .CEN key (see Game/CenFile.cpp) */
u8 lbl_805EF2BC;   /* the clock stops at the camera's end */
/* The cached address of the object behind the accessor at 0x801E60C4 (another unit). */
void *lbl_805EF2C0;
Vec3f lbl_80580218; /* the base position (see Game/CenFile.cpp) */
Vec3f lbl_80580224; /* the key's position */
Vec3f lbl_80580230; /* their sum */

/* Whether name ends in the extension ext (name inferred; as IsCenFile in Game/CenFile.cpp). */
inline bool IsExtension(const char *name, const char *ext) {
    const char *dot = strchr(name, '.');
    if (dot != 0 && fn_803AC8DC(dot, ext, 4) == 0) {
        return true;
    }
    return false;
}

/* A heap copy of name with its extension cut off (as in Game/CenFileNumber.cpp). */
inline char *CopyBaseName(const char *name) {
    char *copy;
    if (name != 0) {
        u32 length = strlen(name);
        copy = (char *)fn_80049B90(length + 1, 2);
        strcpy(copy, name);
        for (u32 i = length - 1; i > 0; i--) {
            if (copy[i] == '.') {
                copy[i] = 0;
            }
        }
    } else {
        copy = 0;
    }
    return copy;
}

/* An event character's model (name inferred): its name, its kind and its brightness. */
struct UnkEventChara {
    /* 0x0 */ const char *m_name;
    /* 0x4 */ s32 m_kind;
    /* 0x8 */ float m_brightness;
};

class TPjsEventActionData {};

class UnkEventModel_80170A9C;

/* A part of an event model (name inferred): a clone of the model's clump with motions of its own.
 * Part 0's owner and index serve the main model's motion callbacks. */
struct UnkEventModelPart {
    /* 0x00 */ UnkEventModel_80170A9C *m_owner;
    /* 0x04 */ s32 m_index;
    /* 0x08 */ RpClump *m_clump;
    /* 0x0C */ s32 m_clumpState;
    /* 0x10 */ char m_name[0x2C];
    /* 0x3C */ _stMtnPlayWork *m_motion;
    /* 0x40 */ UnkClumpUpdater_80015A7C *m_updater;
    /* 0x44 */ void *m_motions[12];
    /* 0x74 */ s32 m_motionStates[12];
    /* 0xA4 */ u32 m_motionId;
    /* 0xA8 */ s32 m_currentMotion;
};

/* A motion file being loaded: the part it is for and its motion number (name inferred). */
struct UnkEventMotionRequest {
    /* 0x0 */ UnkEventModelPart *m_part;
    /* 0x4 */ s32 m_motion;
};

/* A motion callback put off while the model loads (name inferred). */
struct UnkEventModelPending {
    /* 0x0 */ void *m_data;
    /* 0x4 */ void *m_user;
    /* 0x8 */ void (*m_callback)(void *data, void *user);
};

class TPjsEventActionScene;

/* An event model (class name unknown): a clump (.DFF), its bones (.BON), its textures (.TXD) and
 * motions (.MTN), and up to 19 cloned parts. */
class UnkEventModel_80170A9C {
  public:
    UnkEventModel_80170A9C(const char *name, TPjsEventActionScene *scene);
    ~UnkEventModel_80170A9C();

    void ReleaseParts();
    bool LoadClump(UnkOneFile_8004CFB0 *archive, int index);
    bool LoadMorph(UnkOneFile_8004CFB0 *archive, int index);
    bool LoadMotion(UnkOneFile_8004CFB0 *archive, int index, int part, int motion);
    bool LoadBones(UnkOneFile_8004CFB0 *archive, int index);
    void SetupUpdaters();
    void SetupMotions();
    bool IsLoading();
    void Update(float previous, float time);
    void SetLayer(const char *name);
    bool Show();
    bool Hide();
    void Follow(UnkEventModel_80170A9C *target, int node);
    bool GetFollowMatrix(RwMatrix *out);
    RwFrame *FindFrame(const char *name);

    static void OnClumpLoaded(RpClump *clump, UnkEventModel_80170A9C *model);
    static void OnMorphLoaded(void *data, void *user);
    static void OnMotionLoaded(void *data, void *user);
    static void OnBonesLoaded(void *data, void *user);
    static RpAtomic *SetAtomicAlpha(RpAtomic *atomic, void *data);
    static RpAtomic *ResetAtomic(RpAtomic *atomic, void *data);

    /* The node the model follows, if any (name inferred). */
    void *GetFollowNode() {
        void *node;
        if (!m_following) {
            node = 0;
        } else if (m_followTarget->m_motion == 0) {
            node = 0;
        } else {
            void *found = fn_80415E70(m_followTarget->m_motion, m_followNode);
            if (found == 0) {
                node = 0;
                m_following = 0;
            } else {
                node = found;
            }
        }
        return node;
    }

    /* 0x000 */ s32 m_layer;
    /* 0x004 */ RpClump *m_clump;
    /* 0x008 */ s32 m_clumpState;
    /* 0x00C */ void *m_morph;
    /* 0x010 */ s32 m_morphState;
    /* 0x014 */ void *m_morphTarget;
    /* 0x018 */ u8 m_morphSetUp;
    /* 0x01C */ u32 field_0x1C;
    /* 0x020 */ _stMtnPlayWork *m_motion;
    /* 0x024 */ UnkClumpUpdater_80015A7C *m_updater;
    /* 0x028 */ s32 m_motionState;
    /* 0x02C */ RpClump *field_0x2C; /* removed from its world on hiding (meaning unknown) */
    /* 0x030 */ void *m_bonesData;
    /* 0x034 */ void *m_bones;
    /* 0x038 */ s32 m_bonesState;
    /* 0x03C */ void *m_motions[12];
    /* 0x06C */ s32 m_motionStates[12];
    /* 0x09C */ u32 m_motionId;
    /* 0x0A0 */ s32 m_currentMotion;
    /* 0x0A4 */ UnkEventModel_80170A9C *m_followTarget;
    /* 0x0A8 */ s32 m_followNode;
    /* 0x0AC */ u32 field_0xAC;
    /* 0x0B0 */ u8 m_following;
    /* 0x0B4 */ UnkEventModelPart m_parts[20];
    /* 0xE24 */ const char *m_name;
    /* 0xE28 */ u8 m_first;
    /* 0xE2C */ u32 field_0xE2C;
    /* 0xE30 */ float m_brightness;
    /* 0xE34 */ TPjsEventActionScene *m_scene;
    /* 0xE38 */ u8 m_visible;
    /* 0xE3C */ UnkEventModelPending m_pending[32];
};
SIZE_ASSERT(UnkEventModel_80170A9C, 0xFBC);

/* The props of a scene (name inferred): up to 32 models by name. */
struct UnkEventProp {
    /* 0x00 */ char m_name[0x2C];
    /* 0x2C */ UnkEventModel_80170A9C *m_model;
};

class UnkEventPropList_8017251C {
  public:
    UnkEventPropList_8017251C();
    ~UnkEventPropList_8017251C();

    void Clear();
    bool IsLoading();
    bool Load(UnkOneFile_8004CFB0 *archive, int index);
    UnkEventProp *Find(const char *name);
    void SetVisible(bool visible);
    void Update(float time);

    /* 0x000 */ UnkEventProp m_props[32];
    /* 0x600 */ float m_previousTime;
    /* 0x604 */ float m_time;
    /* 0x608 */ TPjsEventActionScene *m_scene;
};
SIZE_ASSERT(UnkEventPropList_8017251C, 0x60C);

/* The characters of a scene (name inferred): up to 64 models by name, with the character's index
 * and the part they are. */
struct UnkEventCharaModel {
    /* 0x00 */ char m_name[0x2C];
    /* 0x2C */ s32 m_chara;
    /* 0x30 */ s32 m_part;
    /* 0x34 */ UnkEventModel_80170A9C *m_model;
};

class UnkEventCharaList_80172D4C {
  public:
    UnkEventCharaList_80172D4C();
    ~UnkEventCharaList_80172D4C();

    int GetPart(const char *name);
    void Clear();
    bool IsLoading();
    bool Load(UnkOneFile_8004CFB0 *archive, int index);
    void LinkParts();
    int GetCloneNumber(const char *name);
    UnkEventCharaModel *Find(const char *name);
    void SetVisible(bool visible);
    void Update(float time);

    /* 0x000 */ UnkEventCharaModel m_models[64];
    /* 0xE00 */ float m_previousTime;
    /* 0xE04 */ float m_time;
    /* 0xE08 */ TPjsEventActionScene *m_scene;
};
SIZE_ASSERT(UnkEventCharaList_80172D4C, 0xE0C);

/* The event camera (name inferred): a camera path (.SNB) played on a user camera mode. */
class UnkEventCamera_80171F80 {
  public:
    UnkEventCamera_80171F80();
    ~UnkEventCamera_80171F80();

    void Release();
    bool Load(UnkOneFile_8004CFB0 *archive, int index);
    bool IsLoading();
    void SetTime(float time);
    void Update();
    float GetEndTime();

    static void OnLoaded(void *data, void *user);

    /* 0x00 */ UnkCameraData_80012510 *m_data;
    /* 0x04 */ void *m_resource;
    /* 0x08 */ float m_time;
    /* 0x0C */ u8 m_finished;
    /* 0x0D */ u8 m_started;
    /* 0x0E */ char m_name[0x42];
    /* 0x50 */ boost::weak_ptr<PJSCamera::ExternalCameraRequest> m_request;
    /* 0x58 */ boost::shared_ptr<CameraModeUser> m_mode;
};
SIZE_ASSERT(UnkEventCamera_80171F80, 0x60);

class TPjsEventActionScene : public Task, public TPjsEventActionData {
  public:
    TPjsEventActionScene(Task *parent, s32 scene);
    virtual ~TPjsEventActionScene();
    virtual void Tick(float dt);

    bool StartLoad();
    void Release();
    bool SortFiles();
    bool LoadFiles();
    void UpdateLoading();
    bool IsTxdFile(const char *name);
    bool IsSnbFile(const char *name);
    bool IsSnnFile(const char *name);
    bool IsCenFile(const char *name);
    bool IsAdbFile(const char *name);

    static void OnArchiveOpened(void *user);
    static void OnTexturesLoaded(void *data, void *user);
    static void OnAdbLoaded(void *data, void *user);

    /* 0x0028 */ s32 m_scene;
    /* 0x002C */ float m_startTime;
    /* 0x0030 */ float m_endTime;
    /* 0x0034 */ float m_time;
    /* 0x0038 */ float m_lastTime;
    /* 0x003C */ float m_propTime;
    /* 0x0040 */ char m_fileName[0x20];
    /* 0x0060 */ s32 m_state;
    /* 0x0064 */ UnkOneFile_8004CFB0 *m_archive;
    /* 0x0068 */ u8 m_paused;
    /* 0x0069 */ u8 m_visible;
    /* 0x006C */ s32 m_fileSize;
    /* 0x0070 */ s32 m_retries;
    /* 0x0074 */ void *m_textures;
    /* 0x0078 */ UnkCenFile_80329514 m_cen;
    /* 0x0180 */ UnkEventCamera_80171F80 m_camera;
    /* 0x01E0 */ UnkSnnFile_802289B0 m_snn;
    /* 0x0234 */ UnkEventCharaList_80172D4C m_charas;
    /* 0x1040 */ UnkEventPropList_8017251C m_props;
    /* 0x164C */ UnkEventPropList_8017251C m_asobi;
    /* 0x1C58 */ void *m_adb;

    static const char *s_taskName;
};
SIZE_ASSERT(TPjsEventActionScene, 0x1C5C);

/* The event characters' models (name inferred): the model name, its kind and its brightness. */
static const UnkEventChara lbl_804C39E0[56] = {
    {"BKBOSS!2C", 6, 1.0f},
    {"BKBOSS!4C", 6, 1.0f},
    {"BKGIANT", 6, 1.0f},
    {"BKSOLDIER", 6, 1.0f},
    {"BKWINGL", 6, 1.0f},
    {"BKWINGS", 6, 1.0f},
    {"SHADOW!ROBOT", 6, 1.0f},
    {"SOLDIERS!GRIP", 6, 1.0f},
    {"SOLDIERS", 6, 1.0f},
    {"SOLDIER!ARMED", 6, 1.0f},
    {"SOLDIER!GRIP", 6, 1.0f},
    {"SOLDIERO", 6, 1.0f},
    {"SOLDIER", 6, 1.0f},
    {"AMYL", 6, 1.0f},
    {"AMY", 6, 1.0f},
    {"BEE", 6, 1.0f},
    {"BKLARVA", 6, 1.0f},
    {"BLACKDOOMG", 13, 1.0f},
    {"BLACKDOOMN", 6, 1.0f},
    {"DOOMSEYE", 6, 1.0f},
    {"EFFSUPERSHADOW", 13, 1.0f},
    {"EGGMANGC", 6, 1.0f},
    {"EGGMANL", 6, 1.0f},
    {"EGGMAN", 6, 1.0f},
    {"ESPIOGC", 6, 1.0f},
    {"ESPIO", 6, 1.0f},
    {"EV!DAITORYO", 6, 1.0f},
    {"EV!GERALD", 6, 1.0f},
    {"EV!MARIA", 6, 1.0f},
    {"EV!OPERATOR!A", 6, 1.0f},
    {"EV!OPERATOR!B", 6, 1.0f},
    {"EV!OPERATOR!C", 6, 1.0f},
    {"EV!OPERATOR!D", 6, 1.0f},
    {"EV!OPERATOR!E", 6, 1.0f},
    {"EV!OPERATOR!F", 6, 1.0f},
    {"EV!OPERATOR!G", 6, 1.0f},
    {"EV!OPERATOR!H", 6, 1.0f},
    {"EV!OPERATOR!O", 6, 1.0f},
    {"EV!SIREIKAN1", 6, 1.0f},
    {"EV!SIREIKAN2", 6, 1.0f},
    {"KNUCKLESL", 6, 1.0f},
    {"KNUCKLES", 6, 1.0f},
    {"OMEGA", 6, 1.0f},
    {"ROUGEL2", 6, 1.0f},
    {"ROUGEL", 6, 1.0f},
    {"ROUGE", 6, 1.0f},
    {"SUPERSHADOW", 6, 1.0f},
    {"SUPERSHABE", 6, 1.0f},
    {"SHADOWL", 6, 1.0f},
    {"SHADOW", 6, 1.0f},
    {"SONICL", 6, 1.0f},
    {"SONIC", 6, 1.0f},
    {"TAILSGC", 6, 1.0f},
    {"TAILSL", 6, 1.0f},
    {"TAILS", 6, 1.0f},
    {"VECTOR", 6, 1.0f},
};

extern "C" {
/* The compiler's registration of a static object's destructor (see
 * Runtime.PPCEABI.H/global_destructor_chain.c), used where the object's class is another unit's. */
void *__register_global_object(void *object, void (*destructor)(void *, short), void *link);
UnkMotionHeap_800151EC *fn_800151EC(void *heap);
void fn_80015160(void *heap, short flags);
}

/* The parts of the characters' models (name inferred). */
static const char *lbl_8052BA58[4] = {"", "CHEEK", "EYEL", "HEAD"};

/* The tag of the cloned parts (name inferred). */
const char *lbl_805E6830 = "CLONE";

/* The event camera's angles of view, in degrees (name inferred). */
float lbl_805E6838[2] = {58.0f, 44.0f};

/* Resets the event state (name inferred; the event manager calls it). */
extern "C" void fn_80172EE8(void) {
    lbl_805EF2A0 = -1;
    lbl_805EF2A4 = -1;
    lbl_805EF2A8 = 0.0f;
    memset(lbl_805EF2AC, 0, sizeof(lbl_805EF2AC));
    memset(&lbl_80580218, 0, sizeof(lbl_80580218));
    lbl_805EF2B4 = 0;
}

int GetEventChara(const char *name);

/* Whether name is an event character's model (name inferred). */
bool IsEventChara(const char *name) {
    if (GetEventChara(name) != -1) {
        return true;
    }
    return false;
}

/* The index of the event character whose model name is in name, or -1 (name inferred). */
int GetEventChara(const char *name) {
    int i;
    const char *base = fn_802DFAB4(fn_801702E4(), name);
    for (i = 0; i < 56; i++) {
        if (fn_803AC6C0(name, lbl_804C39E0[i].m_name) != 0) {
            return i;
        }
        if (fn_803AC6C0(base, lbl_804C39E0[i].m_name) != 0) {
            return i;
        }
    }
    return -1;
}

int UnkEventCharaList_80172D4C::GetPart(const char *name) {
    int i;
    const char *base = fn_802DFAB4(fn_801702E4(), name);
    for (i = 1; i < 4; i++) {
        if (fn_803AC6C0(name, lbl_8052BA58[i]) != 0) {
            return i;
        }
        if (fn_803AC6C0(base, lbl_8052BA58[i]) != 0) {
            return i;
        }
    }
    return 0;
}

UnkEventCharaList_80172D4C::UnkEventCharaList_80172D4C() {
    memset(m_models, 0, sizeof(m_models));
    m_previousTime = 0.0f;
    m_time = 0.0f;
}

UnkEventCharaList_80172D4C::~UnkEventCharaList_80172D4C() {
    Clear();
}

void UnkEventCharaList_80172D4C::Clear() {
    UnkEventCharaModel *model;
    for (int i = 0; i < 64; i++) {
        model = &m_models[i];
        if (model->m_name[0] != 0) {
            delete model->m_model;
            model->m_model = 0;
            model->m_name[0] = 0;
        }
    }
}

bool UnkEventCharaList_80172D4C::IsLoading() {
    for (int i = 0; i < 64; i++) {
        if (m_models[i].m_model != 0 && m_models[i].m_model->IsLoading()) {
            return true;
        }
    }
    return false;
}

bool UnkEventCharaList_80172D4C::Load(UnkOneFile_8004CFB0 *archive, int index) {
    const char *name = fn_8004CA28(archive, index);
    if (IsExtension(name, ".DFF")) {
        UnkEventCharaModel *model = Find(name);
        model->m_model->LoadClump(archive, index);
        model->m_model->m_layer = lbl_804C39E0[GetEventChara(name)].m_kind;
        model->m_model->m_brightness = lbl_804C39E0[GetEventChara(name)].m_brightness;
    } else if (IsExtension(fn_8004CA28(archive, index), ".MTN")) {
        UnkEventCharaModel *model = Find(name);
        model->m_model->LoadMotion(archive, index, GetCloneNumber(name), fn_80329524(name));
    } else if (IsExtension(fn_8004CA28(archive, index), ".BON")) {
        Find(name)->m_model->LoadBones(archive, index);
    } else if (IsExtension(fn_8004CA28(archive, index), ".DMA")) {
        Find(name)->m_model->LoadMorph(archive, index);
    } else {
        return false;
    }
    return true;
}

void UnkEventCharaList_80172D4C::LinkParts() {
    for (int i = 0; i < 64; i++) {
        UnkEventCharaModel *model = &m_models[i];
        if (model->m_name[0] != 0 && model != 0) {
            UnkEventModel_80170A9C *target;
            for (int j = 0; j < 64; j++) {
                UnkEventCharaModel *other = &m_models[j];
                if (other->m_name[0] != 0 && other != model && model->m_chara == other->m_chara &&
                    other->m_part == 0) {
                    target = other->m_model;
                    goto found;
                }
            }
            target = 0;
        found:
            if (target != 0) {
                model->m_model->Follow(target, 30);
            }
        }
    }
}

int UnkEventCharaList_80172D4C::GetCloneNumber(const char *name) {
    int number = 0;
    char *copy = CopyBaseName(name);
    char *tag = fn_803AC6C0(copy, lbl_805E6830);
    if (tag != 0) {
        number = (tag[5] - '0') * 10 + tag[6] - '0';
    }
    fn_80049B34(copy);
    return number;
}

UnkEventCharaModel *UnkEventCharaList_80172D4C::Find(const char *name) {
    int i;
    int free;
    const char *base;
    char *copy;
    char *tag;
    UnkEventCharaModel *model;
    free = -1;
    base = fn_802DFAB4(fn_801702E4(), name);
    copy = CopyBaseName(base);
    tag = fn_803AC6C0(copy, "!LOCATOR");
    if (tag != 0) {
        *tag = 0;
    }
    tag = fn_803AC6C0(copy, lbl_805E6830);
    if (tag != 0) {
        *tag = 0;
    }
    tag = fn_803AC6C0(copy, lbl_805ED3D0);
    if (tag != 0) {
        *tag = 0;
    }
    for (i = 0; i < 64; i++) {
        model = &m_models[i];
        if (model->m_name[0] == 0) {
            if (free < 0) {
                free = i;
            }
        } else if (strcmp(model->m_name, copy) == 0) {
            fn_80049B34(copy);
            return model;
        }
    }
    if (free < 0) {
        fn_80049B34(copy);
        return 0;
    }
    model = &m_models[free];
    strcpy(model->m_name, copy);
    model->m_chara = GetEventChara(copy);
    model->m_part = GetPart(copy);
    fn_80049B34(copy);
    m_models[free].m_model = new UnkEventModel_80170A9C(model->m_name, m_scene);
    return model;
}

void UnkEventCharaList_80172D4C::SetVisible(bool visible) {
    for (int i = 0; i < 64; i++) {
        UnkEventCharaModel *model = &m_models[i];
        if (model->m_name[0] != 0 && model->m_model != 0) {
            if (visible) {
                model->m_model->Show();
            } else {
                model->m_model->Hide();
            }
        }
    }
}

void UnkEventCharaList_80172D4C::Update(float time) {
    m_previousTime = m_time;
    m_time = time;
    for (int i = 0; i < 64; i++) {
        UnkEventCharaModel *model = &m_models[i];
        if (model->m_name[0] != 0 && model->m_model != 0 && model->m_part == 0) {
            model->m_model->Update(m_previousTime, m_time);
        }
    }
    for (int i = 0; i < 64; i++) {
        UnkEventCharaModel *model = &m_models[i];
        if (model->m_name[0] != 0 && model->m_model != 0 && model->m_part != 0) {
            model->m_model->Update(m_previousTime, m_time);
        }
    }
}

UnkEventPropList_8017251C::UnkEventPropList_8017251C() {
    memset(m_props, 0, sizeof(m_props));
    m_previousTime = 0.0f;
    m_time = 0.0f;
}

UnkEventPropList_8017251C::~UnkEventPropList_8017251C() {
    Clear();
}

void UnkEventPropList_8017251C::Clear() {
    UnkEventProp *prop;
    for (int i = 0; i < 32; i++) {
        prop = &m_props[i];
        if (prop->m_name[0] != 0) {
            delete prop->m_model;
            prop->m_model = 0;
            prop->m_name[0] = 0;
        }
    }
}

bool UnkEventPropList_8017251C::IsLoading() {
    for (int i = 0; i < 32; i++) {
        if (m_props[i].m_model != 0 && m_props[i].m_model->IsLoading()) {
            return true;
        }
    }
    return false;
}

/* The event's task holder (name inferred; other units call it). */
extern "C" UnkEventHolder_801723E0 *fn_801723E0(void) {
    static UnkEventHolder_801723E0 *s_cached;
    if (s_cached == 0) {
        static u32 s_object[4];
        s_cached = (UnkEventHolder_801723E0 *)s_object;
    }
    return s_cached;
}

bool UnkEventPropList_8017251C::Load(UnkOneFile_8004CFB0 *archive, int index) {
    const char *name = fn_8004CA28(archive, index);
    if (IsExtension(name, ".DFF")) {
        Find(name)->m_model->LoadClump(archive, index);
    } else if (IsExtension(fn_8004CA28(archive, index), ".MTN")) {
        UnkEventProp *prop = Find(name);
        prop->m_model->LoadMotion(archive, index, 0, fn_80329524(name));
    } else if (IsExtension(fn_8004CA28(archive, index), ".BON")) {
        Find(name)->m_model->LoadBones(archive, index);
    } else {
        fn_80174250(fn_801723E0(), "[%s]...\x83\x77\x83\x93\x82\xc8\x83\x66\x81\x5b\x83\x5e\x82\xaa\x83\x49\x83\x75\x83\x57\x83\x46\x83\x4e\x83\x67\x82\xc6\x82\xb5\x82\xc4\x93\xc7\x82\xdc\x82\xea\x82\xc4\x82\xe9\x89\xc2\x94\x5c\x90\xab\x82\xa0\x82\xe8", name);
        return false;
    }
    return true;
}

UnkEventProp *UnkEventPropList_8017251C::Find(const char *name) {
    UnkEventProp *prop;
    int i;
    int free = -1;
    const char *base = fn_802DFAB4(fn_801702E4(), name);
    char *copy = CopyBaseName(base);
    char *tag = fn_803AC6C0(copy, lbl_805ED3D0);
    if (tag != 0) {
        *tag = 0;
    }
    for (i = 0; i < 32; i++) {
        prop = &m_props[i];
        if (prop->m_name[0] == 0) {
            if (free < 0) {
                free = i;
            }
        } else if (strcmp(prop->m_name, copy) == 0) {
            fn_80049B34(copy);
            return prop;
        }
    }
    if (free < 0) {
        fn_80049B34(copy);
        return 0;
    }
    prop = &m_props[free];
    strcpy(prop->m_name, copy);
    fn_80049B34(copy);
    m_props[free].m_model = new UnkEventModel_80170A9C(prop->m_name, m_scene);
    return prop;
}

void UnkEventPropList_8017251C::SetVisible(bool visible) {
    for (int i = 0; i < 32; i++) {
        UnkEventProp *prop = &m_props[i];
        if (prop->m_name[0] != 0 && prop->m_model != 0) {
            if (visible) {
                prop->m_model->Show();
            } else {
                prop->m_model->Hide();
            }
        }
    }
}

void UnkEventPropList_8017251C::Update(float time) {
    if (time < 0.0f) {
        time = 0.0f;
    }
    m_previousTime = m_time;
    m_time = time;
    for (int i = 0; i < 32; i++) {
        UnkEventProp *prop = &m_props[i];
        if (prop->m_name[0] != 0 && prop->m_model != 0) {
            prop->m_model->Update(m_previousTime, m_time);
        }
    }
}

UnkEventCamera_80171F80::UnkEventCamera_80171F80()
    : m_data(0), m_resource(0), m_time(0.0f), m_finished(0), m_started(0) {
    m_name[0] = 0;
}

UnkEventCamera_80171F80::~UnkEventCamera_80171F80() {
    Release();
}

void UnkEventCamera_80171F80::Release() {
    if (m_resource != 0) {
        fn_801CEAD4(&m_request);
        fn_80009618(&m_mode);
        fn_80049B34(m_resource);
        m_resource = 0;
    }
}

/* Whether name ends in the extension ext (name inferred; the unit at 0x80228104 calls it). */
extern "C" bool fn_80171E74(const char *name, const char *ext) {
    const char *dot = strchr(name, '.');
    if (dot != 0 && fn_803AC8DC(dot, ext, 4) == 0) {
        return true;
    }
    return false;
}

/* The camera files' extension (.SNB). Retail has one copy, which Camera::Load addresses small-data
 * relative and IsSnbFile as an array of unknown size (lis/addi): the original compiler generated
 * IsSnbFile before the definition, which GC/2.7 (deferred code generation) cannot reproduce. Here
 * IsSnbFile refers to it by its address's name, left undefined, and Camera::Load uses a literal. */
extern const char lbl_805F4C60[];

bool UnkEventCamera_80171F80::Load(UnkOneFile_8004CFB0 *archive, int index) {
    if (fn_80171E74(fn_8004CA28(archive, index), ".SNB")) {
        strcpy(m_name, fn_8004CA28(archive, index));
        fn_8004C358(archive, index, OnLoaded, this);
        return true;
    }
    return false;
}

bool UnkEventCamera_80171F80::IsLoading() {
    return m_data == 0;
}

void UnkEventCamera_80171F80::SetTime(float time) {
    m_time = time;
    Update();
}

/* Sets a user camera mode's angles of view (name inferred). */
inline void SetAngles(CameraModeUser *mode, const Vec2f &angles) {
    fn_8000D968(mode, &angles);
}

void UnkEventCamera_80171F80::Update() {
    Vec3f position;
    float step;
    if (m_time < 0.0f) {
        return;
    }
    if (!m_started) {
        m_mode = fn_80009704(0, 0);
        m_request = fn_801CED24(boost::shared_ptr<CameraMode>(m_mode), 0, 0, 0, 0, 0, 1);
        m_started = 1;
    }
    fn_80008E1C(&position, lbl_80580218.x, lbl_80580218.y, lbl_80580218.z);
    m_time = (int)m_time;
    fn_80010010(m_data, &position, m_time);
    SetAngles(m_mode.operator->(), Vec2f(DegToRad(lbl_805E6838[0]), DegToRad(lbl_805E6838[1])));
    step = 1.0f / lbl_8057D6A0.m_frameRate;
    m_time -= step;
    if (fn_80011B7C(m_data, &m_time, &step) != 0) {
        m_finished = 1;
    }
}

void UnkEventCamera_80171F80::OnLoaded(void *data, void *user) {
    UnkEventCamera_80171F80 *camera = (UnkEventCamera_80171F80 *)user;
    camera->m_resource = data;
    if (fn_80010648(camera->m_resource) != 0) {
        camera->m_data = fn_80011CE8(camera->m_resource, 2);
    }
}

float UnkEventCamera_80171F80::GetEndTime() {
    if (m_data != 0) {
        return m_data->m_endTime;
    }
    return 0.0f;
}

const char *TPjsEventActionScene::s_taskName = "TPjsEventActionScene";

TPjsEventActionScene::TPjsEventActionScene(Task *parent, s32 scene)
    : Task(parent), m_scene(scene), m_startTime(0.0f), m_endTime(0.0f), m_time(0.0f),
      m_lastTime(0.0f), m_propTime(-1.0f), m_state(1), m_archive(0), m_visible(0), m_fileSize(0),
      m_retries(0), m_textures(0) {
    m_adb = 0;
    m_name = s_taskName;
    m_paused = 1;
    m_charas.m_scene = this;
    m_props.m_scene = this;
}

TPjsEventActionScene::~TPjsEventActionScene() {
    m_camera.Release();
    fn_8022890C(&m_snn);
    m_charas.Clear();
    m_props.Clear();
    m_asobi.Clear();
    Release();
    fn_8005001C();
}

bool TPjsEventActionScene::StartLoad() {
    char nextName[0x48];
    void *buffer;
    if (m_state != 1) {
        return false;
    }
    buffer = 0;
    char letter = 'A' + m_scene;
    sprintf(nextName, "event\\event%04d_scene%c.one", lbl_805EF2A0, letter + 1);
    sprintf(m_fileName, "event\\event%04d_scene%c.one", lbl_805EF2A0, letter);
    UnkFileSystem_800371D8 *fs = fn_800371D8();
    if (fs->m_exists(nextName)) {
        if (m_fileSize == 0) {
            s32 size = fn_80044E34(fn_800371D8(), m_fileName);
            s32 nextSize = fn_80044E34(fn_800371D8(), nextName);
            if (size > nextSize) {
                m_fileSize = size;
            } else {
                fn_803A953C("[%s]\x8e\x9f\x83\x56\x81\x5b\x83\x93\x82\xcc\x83\x54\x83\x43\x83\x59\x82\xc5\x8a\x6d\x95\xdb\n", m_fileName);
                m_fileSize = nextSize;
            }
            m_fileSize = fn_8004E85C(m_fileSize, 0x20);
        }
        fn_80049B14(0x40);
        buffer = fn_80049B90(m_fileSize, 2);
        fn_80049AF4();
        if (buffer == 0) {
            m_retries++;
            if (m_retries < 30) {
                fn_803A953C("[%s]\x82\xed\x82\xa8\x81\x41\x83\x7d\x83\x8d\x83\x62\x83\x4e\x82\xc5\x82\xab\x82\xc8\x82\xa2\n", m_fileName);
                return false;
            }
            buffer = fn_80049B90(m_fileSize, 1);
            if (buffer == 0) {
                return false;
            }
            fn_803A953C("[%s]\x82\xc5\x82\xe0\x81\x41\x82\xaa\x82\xf1\x82\xce\x82\xc1\x82\xbd\n", m_fileName);
        }
    }
    UnkOneFile_8004CFB0 *archive = (UnkOneFile_8004CFB0 *)operator new(0x58);
    if (archive != 0) {
        archive = fn_8004CFB0(archive, 0, 0);
    }
    m_archive = archive;
    fn_8004CC6C(m_archive, m_fileName, OnArchiveOpened, this, buffer, m_fileSize);
    m_state = 2;
    return true;
}

void TPjsEventActionScene::Release() {
    if (m_visible) {
        m_props.SetVisible(false);
        m_charas.SetVisible(false);
        if (m_propTime >= 0.0f) {
            m_asobi.SetVisible(false);
        } else {
            m_asobi.SetVisible(false);
        }
        m_visible = 0;
    }
    void *adb = m_adb;
    if (adb != 0) {
        fn_8004FE00(fn_80013BC4(), adb);
        m_adb = 0;
    }
    if (m_textures != 0) {
        fn_802E0600(m_textures);
    }
    if (m_archive != 0) {
        fn_8004CF68(m_archive, 1);
        m_archive = 0;
    }
}

void TPjsEventActionScene::OnArchiveOpened(void *user) {
    ((TPjsEventActionScene *)user)->m_state = 3;
}

bool TPjsEventActionScene::SortFiles() {
    int i;
    bool loadingTextures;
    if (m_state != 3 || !fn_8004CAF8(m_archive)) {
        m_state = 8;
        return false;
    }
    loadingTextures = false;
    int count = m_archive->m_header->m_count;
    for (i = 2; i < count + 2; i++) {
        if (IsTxdFile(fn_8004CA28(m_archive, i))) {
            fn_8004BF40(m_archive, i, OnTexturesLoaded, this);
            loadingTextures = true;
        } else if (IsAdbFile(fn_8004CA28(m_archive, i))) {
            fn_8004C358(m_archive, i, OnAdbLoaded, this);
        }
    }
    if (!loadingTextures) {
        m_state = 5;
        return true;
    }
    m_state = 4;
    return true;
}

void TPjsEventActionScene::OnTexturesLoaded(void *data, void *user) {
    TPjsEventActionScene *scene = (TPjsEventActionScene *)user;
    scene->m_textures = data;
    scene->m_state = 5;
    if (data != 0) {
        fn_80039618(fn_80014534(), data, 6);
    }
}

void TPjsEventActionScene::OnAdbLoaded(void *data, void *user) {
    ((TPjsEventActionScene *)user)->m_adb = fn_8004FE88(fn_80013BC4(), data);
}

bool TPjsEventActionScene::LoadFiles() {
    int i;
    if (m_state != 5 || !fn_8004CAF8(m_archive)) {
        m_state = 8;
        return false;
    }
    int count = m_archive->m_header->m_count;
    if (m_textures != 0) {
        fn_8048D0C4(m_textures);
    }
    for (i = 2; i < count + 2; i++) {
        if (IsTxdFile(fn_8004CA28(m_archive, i))) {
        } else if (IsSnbFile(fn_8004CA28(m_archive, i))) {
            m_camera.Load(m_archive, i);
        } else if (IsCenFile(fn_8004CA28(m_archive, i))) {
            fn_80329420(&m_cen, m_archive, i);
        } else if (IsSnnFile(fn_8004CA28(m_archive, i))) {
            fn_8022887C(&m_snn, m_archive, i);
        } else if (IsEventChara(fn_8004CA28(m_archive, i))) {
            m_charas.Load(m_archive, i);
        } else {
            bool isAsobi = fn_803AC6C0(fn_8004CA28(m_archive, i), "ASOBI") != 0;
            if (isAsobi) {
                m_asobi.Load(m_archive, i);
            } else {
                m_props.Load(m_archive, i);
            }
        }
    }
    m_charas.LinkParts();
    m_state = 6;
    return true;
}

/* Shows or hides the scene's models (name inferred; the event manager calls it). */
extern "C" void fn_801712F4(TPjsEventActionScene *scene, bool visible) {
    if (visible != scene->m_visible) {
        scene->m_props.SetVisible(visible);
        scene->m_charas.SetVisible(visible);
        if (scene->m_propTime >= 0.0f) {
            scene->m_asobi.SetVisible(visible);
        } else {
            scene->m_asobi.SetVisible(false);
        }
        scene->m_visible = visible;
    }
}

void TPjsEventActionScene::Tick(float dt) {
    if (m_state == 7) {
        if (!m_paused) {
            float previous = lbl_805EF2A8;
            if (lbl_805EF2B5) {
                lbl_805EF2A8 = 30.0f * dt + previous;
            } else {
                lbl_805EF2A8 = 60.0f * dt + previous;
            }
            if (lbl_805EF2BC && lbl_805EF2A8 >= m_camera.GetEndTime()) {
                lbl_805EF2A8 = m_camera.GetEndTime() - 1.0f;
            }
            m_time = lbl_805EF2A8 - m_startTime;
            if (m_time < 0.0f) {
                m_time = 0.0f;
            } else if ((fn_80051734()->m_flags & 1) == 1) {
                m_propTime = m_time;
            }
            lbl_805E39A0 = lbl_805EF2A8;
            m_camera.SetTime(lbl_805EF2A8 + dt);
            if (fn_803290D0(&m_cen, lbl_805EF2A8)) {
                fn_80228844(&m_snn, lbl_805EF2A8);
                m_charas.Update(lbl_805EF2A8);
                m_props.Update(lbl_805EF2A8);
                m_asobi.Update(m_time - m_propTime);
                fn_80173CD4(fn_80170D8C(), m_scene, lbl_805EF2A8, previous);
                fn_8016F990()->Tick();
                m_lastTime = m_time;
            }
        }
    } else {
        UpdateLoading();
    }
}

/* Whether the scene plays and its camera has finished (name inferred; the event manager calls
 * it). */
extern "C" bool fn_80171124(TPjsEventActionScene *scene) {
    if (scene->m_state == 7) {
        if (scene->m_paused) {
            return false;
        }
        if (scene->m_camera.m_finished) {
            return true;
        }
    }
    return false;
}

void TPjsEventActionScene::UpdateLoading() {
    switch (m_state) {
    case 1:
        if (!lbl_805EF2B4 && StartLoad()) {
            lbl_805EF2B4 = 1;
        }
        break;
    case 3:
        SortFiles();
        break;
    case 5:
        LoadFiles();
        break;
    case 2:
    case 4:
    case 7:
        break;
    case 6:
        if (m_cen.m_state != 1 && !m_props.IsLoading() && !m_camera.IsLoading() &&
            !fn_80228874(&m_snn) && !m_charas.IsLoading() && fn_80170D8C()->m_state != 1 &&
            !m_asobi.IsLoading()) {
            UnkOneFile_8004CFB0 *archive = m_archive;
            if (archive != 0) {
                if (archive->m_busy != 0) {
                    return;
                }
                fn_8004CF68(archive, 1);
                m_archive = 0;
                lbl_805EF2B4 = 0;
            }
            m_state = 7;
            m_endTime = m_camera.GetEndTime();
        }
        break;
    }
}

bool TPjsEventActionScene::IsTxdFile(const char *name) {
    if (IsExtension(name, ".TXD")) {
        return true;
    }
    return false;
}

bool TPjsEventActionScene::IsSnbFile(const char *name) {
    if (IsExtension(name, lbl_805F4C60)) {
        return true;
    }
    return false;
}

bool TPjsEventActionScene::IsSnnFile(const char *name) {
    if (IsExtension(name, lbl_805F6F80)) {
        return true;
    }
    return false;
}

bool TPjsEventActionScene::IsCenFile(const char *name) {
    if (IsExtension(name, lbl_805F9CA8)) {
        return true;
    }
    return false;
}

bool TPjsEventActionScene::IsAdbFile(const char *name) {
    if (IsExtension(name, ".ADB")) {
        return true;
    }
    return false;
}

/* The stage's points (see Game/CenFile.cpp; name inferred; other units call it). */
extern "C" UnkObject_80170D8C *fn_80170D8C(void) {
    static UnkObject_80170D8C *s_cached;
    if (s_cached == 0) {
        static s8 s_initialized;
        static u32 s_object[13];
        static u32 s_destructorLink[3];
        if (!s_initialized) {
            __register_global_object(fn_80174228((UnkObject_80170D8C *)s_object), fn_801741E0,
                                     s_destructorLink);
            s_initialized = 1;
        }
        s_cached = (UnkObject_80170D8C *)s_object;
    }
    return s_cached;
}

/* Starts effect id at the frame name of every model of the scene, if start is set (name inferred;
 * the event manager calls it). */
extern "C" void fn_80170C58(TPjsEventActionScene *scene, const char *name, bool start, int id) {
    for (u32 i = 0; i < 64; i++) {
        UnkEventModel_80170A9C *model = scene->m_charas.m_models[i].m_model;
        if (model != 0) {
            RwFrame *const &frame = model->FindFrame(name);
            if (frame != 0 && start) {
                Mtx34f matrix;
                fn_8040C148(&matrix, &RefM(fn_800635A4(&frame->modelling)));
                fn_801D2B74(0, &matrix, id, 0);
            }
        }
    }
    for (u32 i = 0; i < 32; i++) {
        UnkEventModel_80170A9C *model = scene->m_props.m_props[i].m_model;
        if (model != 0) {
            RwFrame *const &frame = model->FindFrame(name);
            if (frame != 0 && start) {
                Mtx34f matrix;
                fn_8040C148(&matrix, &RefM(fn_800635A4(&frame->modelling)));
                fn_801D2B74(0, &matrix, id, 0);
            }
        }
    }
}

/* Retail emits the name table vector's storage members here, after the models' constructor: the
 * original included <vector> at this point. The vector has the class between it and its storage
 * (see <vector>). */
#define MSL_VECTOR_MIDDLE
#include <vector>

/* The name table (class name unknown; constructed by 0x802DFC7C in its own unit, which also holds
 * its lookups): a vector of 8-byte entries and a word. */
struct UnkNameTableEntry_802DFC7C {
    /* 0x0 */ u32 field_0x0;
    /* 0x4 */ u32 field_0x4;
};

class UnkNameTable_802DFC7C {
  public:
    UnkNameTable_802DFC7C();

    /* 0x0 */ std::vector<UnkNameTableEntry_802DFC7C> m_entries;
    /* 0xC */ u32 field_0xC;
};
SIZE_ASSERT(UnkNameTable_802DFC7C, 0x10);

/* The vector's storage members, as explicit specializations so that they are emitted here, after the
 * models' constructor, where retail has them (GC/2.7 emits template instances at the end of the
 * unit). Their code is that of <vector>. */
typedef std::allocator<UnkNameTableEntry_802DFC7C> UnkNameTableAllocator;
namespace std {
template <> __vec_base<UnkNameTableEntry_802DFC7C, UnkNameTableAllocator>::~__vec_base();
template <> UnkNameTableAllocator &__vec_cap_pair<UnkNameTableAllocator, size_t>::first();
template <> UnkNameTableEntry_802DFC7C *&__vec_base<UnkNameTableEntry_802DFC7C, UnkNameTableAllocator>::data();
template <> size_t &__vec_cap_pair<UnkNameTableAllocator, size_t>::second();
template <> void __vec_base<UnkNameTableEntry_802DFC7C, UnkNameTableAllocator>::destroy_all();

template <> void __vec_base<UnkNameTableEntry_802DFC7C, UnkNameTableAllocator>::destroy_all() {
    pointer first = data_;
    pointer last = first + size_;
    while (last > first) {
        cap_.first().destroy(--last);
    }
    size_ = 0;
}

template <> size_t &__vec_cap_pair<UnkNameTableAllocator, size_t>::second() {
    return __vec_ref(this->second_);
}

template <> UnkNameTableEntry_802DFC7C *&__vec_base<UnkNameTableEntry_802DFC7C, UnkNameTableAllocator>::data() {
    return __vec_ref(data_);
}

template <> UnkNameTableAllocator &__vec_cap_pair<UnkNameTableAllocator, size_t>::first() {
    return __vec_ref<UnkNameTableAllocator>(*this);
}

template <> __vec_base<UnkNameTableEntry_802DFC7C, UnkNameTableAllocator>::~__vec_base() {
    destroy_all();
    if (data_ != 0) {
        cap_.first().deallocate(data(), cap_.second());
    }
}
} // namespace std


UnkEventModel_80170A9C::UnkEventModel_80170A9C(const char *name, TPjsEventActionScene *scene)
    : m_layer(4), m_name(name), m_first(1), field_0xE2C(0), m_brightness(0.0f), m_scene(scene),
      m_visible(0) {
    memset(&m_clump, 0, 0x8);
    memset(&m_morph, 0, 0x14);
    memset(&m_motion, 0, 0x84);
    memset(&m_followTarget, 0, 0x10);
    memset(m_parts, 0, sizeof(m_parts));
    m_currentMotion = -1;
    for (int i = 0; i < 20; i++) {
        m_parts[i].m_currentMotion = -1;
    }
    memset(m_pending, 0, sizeof(m_pending));
}

UnkEventModel_80170A9C::~UnkEventModel_80170A9C() {
    m_visible = 0;
    if (field_0x2C != 0) {
        void *world = fn_8046256C(field_0x2C);
        if (world != 0) {
            fn_804623EC(world, field_0x2C);
        }
    }
    for (int i = 1; i < 20; i++) {
        if (m_parts[i].m_clump != 0) {
            void *world = fn_8046256C(m_parts[i].m_clump);
            if (world != 0) {
                fn_804623EC(world, m_parts[i].m_clump);
            }
        }
    }
    ReleaseParts();
    if (m_updater != 0) {
        delete m_updater;
    }
    if (m_motionState == 2) {
        for (int i = 0; i < 12; i++) {
            if (m_motion != 0) {
                fn_8041B884(m_motion);
                m_motion = 0;
            }
        }
    }
    if (m_currentMotion >= 0) {
        fn_804129D4(m_motionId);
        m_currentMotion = -1;
    }
    for (int i = 0; i < 12; i++) {
        if (m_motionStates[i] == 2 && m_motions[i] != 0) {
            fn_80049B34(m_motions[i]);
            m_motions[i] = 0;
        }
    }
    memset(&m_motion, 0, 0x84);
    if (m_morph != 0) {
        fn_80442B98(m_morph);
        m_morphState = 0;
    }
    memset(&m_morph, 0, 0x14);
    if (m_clump != 0) {
        void *world = fn_8046256C(m_clump);
        if (world != 0) {
            fn_804623EC(world, m_clump);
        }
        fn_80457468(m_clump);
        m_clumpState = 0;
    }
    memset(&m_clump, 0, 0x8);
    memset(&m_followTarget, 0, 0x10);
}

void UnkEventModel_80170A9C::ReleaseParts() {
    for (int i = 1; i < 20; i++) {
        if (m_parts[i].m_updater != 0) {
            delete m_parts[i].m_updater;
        }
        for (int j = 0; j < 12; j++) {
            if (m_parts[i].m_motion != 0) {
                fn_8041B884(m_parts[i].m_motion);
                m_parts[i].m_motion = 0;
            }
        }
        if (m_parts[i].m_currentMotion >= 0) {
            fn_804129D4(m_parts[i].m_motionId);
            m_parts[i].m_currentMotion = -1;
        }
        for (int j = 0; j < 12; j++) {
            if (m_parts[i].m_motionStates[j] == 2 && m_parts[i].m_motions[j] != 0) {
                fn_80049B34(m_parts[i].m_motions[j]);
                m_parts[i].m_motions[j] = 0;
            }
        }
        if (m_parts[i].m_clump != 0) {
            void *world = fn_8046256C(m_parts[i].m_clump);
            if (world != 0) {
                fn_804623EC(world, m_parts[i].m_clump);
            }
            fn_80457468(m_parts[i].m_clump);
            m_parts[i].m_clumpState = 0;
        }
    }
    memset(m_parts, 0, sizeof(m_parts));
}

bool UnkEventModel_80170A9C::LoadClump(UnkOneFile_8004CFB0 *archive, int index) {
    fn_8004BB24(archive, index, (void (*)(void *, void *))OnClumpLoaded, this);
    m_clumpState = 1;
    SetLayer(fn_8004CA28(archive, index));
    return true;
}

void UnkEventModel_80170A9C::OnClumpLoaded(RpClump *clump, UnkEventModel_80170A9C *model) {
    RwRGBA color;
    model->m_clump = clump;
    model->m_clumpState = 2;
    model->SetupUpdaters();
    void *adb;
    TPjsEventActionScene *scene;
    scene = model->m_scene;
    adb = scene->m_adb;
    if (adb != 0) {
        char out[4];
        fn_8004F9E8(out, fn_80013BC4(), clump, adb, "Event", 0, scene);
    }
    switch (model->m_layer) {
    case 10:
        color.alpha = 255.0f * model->m_brightness;
        color.red = 0xFF;
        color.green = 0xFF;
        color.blue = 0xFF;
        fn_80037B84(clump, &color);
        break;
    }
    fn_8003815C(clump);
}

bool UnkEventModel_80170A9C::LoadMorph(UnkOneFile_8004CFB0 *archive, int index) {
    fn_8004ADB0(archive, index, OnMorphLoaded, this);
    m_morphState = 1;
    return true;
}

void UnkEventModel_80170A9C::OnMorphLoaded(void *data, void *user) {
    UnkEventModel_80170A9C *model = (UnkEventModel_80170A9C *)user;
    model->m_morph = data;
    model->m_morphState = 2;
    model->SetupUpdaters();
}

bool UnkEventModel_80170A9C::LoadMotion(UnkOneFile_8004CFB0 *archive, int index, int part,
                                        int motion) {
    UnkEventMotionRequest *request = (UnkEventMotionRequest *)fn_80049B90(8, 2);
    request->m_part = &m_parts[part];
    request->m_motion = motion;
    m_parts[part].m_owner = this;
    m_parts[part].m_index = part;
    fn_8004C358(archive, index, OnMotionLoaded, request);
    m_parts[part].m_clumpState = 1;
    m_parts[part].m_motionStates[motion] = 1;
    strcpy(m_parts[part].m_name, fn_8004CA28(archive, index));
    return true;
}

void UnkEventModel_80170A9C::OnMotionLoaded(void *data, void *user) {
    UnkEventMotionRequest *request = (UnkEventMotionRequest *)user;
    UnkEventModelPart *part = request->m_part;
    UnkEventModel_80170A9C *model = part->m_owner;
    if (model->m_bonesState != 2) {
        for (int i = 0; i < 32; i++) {
            if (model->m_pending[i].m_data == 0) {
                model->m_pending[i].m_data = data;
                model->m_pending[i].m_user = user;
                model->m_pending[i].m_callback = OnMotionLoaded;
                break;
            }
        }
    }
    if (part->m_index == 0) {
        model->m_motions[request->m_motion] = data;
        model->m_motionStates[request->m_motion] = 2;
        model->m_parts[0].m_clumpState = 0;
        model->m_parts[0].m_motionStates[0] = 2;
    } else {
        model->m_parts[part->m_index].m_motions[request->m_motion] = data;
        model->m_parts[part->m_index].m_motionStates[request->m_motion] = 2;
    }
    fn_80049B34(request);
    model->SetupMotions();
}

bool UnkEventModel_80170A9C::LoadBones(UnkOneFile_8004CFB0 *archive, int index) {
    fn_8004C358(archive, index, OnBonesLoaded, this);
    m_motionState = 1;
    m_bonesState = 1;
    return true;
}

/* The name table (name inferred; other units call it). */
extern "C" UnkNameTable_802DFC7C *fn_801702E4(void) {
    static UnkNameTable_802DFC7C *s_cached;
    if (s_cached == 0) {
        static UnkNameTable_802DFC7C s_object;
        s_cached = &s_object;
    }
    return s_cached;
}

void UnkEventModel_80170A9C::OnBonesLoaded(void *data, void *user) {
    UnkEventModel_80170A9C *model = (UnkEventModel_80170A9C *)user;
    model->m_bonesData = data;
    model->m_bonesState = 2;
    model->m_bones = fn_802DFB04(fn_801702E4(), model->m_bonesData);
    model->SetupMotions();
}

/* The motion heap (name inferred; other units call it). */
extern "C" UnkMotionHeap_800151EC *fn_80170234(void) {
    if (lbl_805EDFF8 == 0) {
        static s8 s_initialized;
        static u8 s_object[1];
        static u32 s_destructorLink[3];
        if (!s_initialized) {
            __register_global_object(fn_800151EC(s_object), fn_80015160, s_destructorLink);
            s_initialized = 1;
        }
        lbl_805EDFF8 = (UnkMotionHeap_800151EC *)s_object;
    }
    return lbl_805EDFF8;
}

void UnkEventModel_80170A9C::SetupMotions() {
    if (m_bonesState == 2) {
        fn_800150FC(fn_80170234(), 1);
        if (m_motion == 0 && m_bonesState == 2) {
            m_motion = fn_8041B908(m_bones, 1);
            fn_80416F28(m_motion, 0, 15, 0);
        }
        for (int i = 1; i < 20; i++) {
                if (m_parts[i].m_motion == 0 && m_parts[i].m_motionStates[0] == 2) {
                m_parts[i].m_motion = fn_8041B908(m_bones, 1);
                fn_80416F28(m_parts[i].m_motion, 0, 15, 0);
            }
        }
        fn_800150FC(fn_80170234(), 2);
    }
    SetupUpdaters();
}

/* Resets the clump's atomics if it has a morph (name inferred). */
inline void ResetAtomics(RpClump *clump, void *morph) {
    if (clump != 0 && morph != 0) {
        fn_80456C54(clump, UnkEventModel_80170A9C::ResetAtomic, morph);
    }
}

void UnkEventModel_80170A9C::SetupUpdaters() {
    if (m_updater == 0 && m_clumpState == 2 && m_motion != 0) {
        m_updater = new UnkClumpUpdater_80015A7C(m_clump, m_motion);
    }
    if (!m_morphSetUp && m_clumpState == 2 && m_morphState == 2) {
        m_morphTarget = fn_80038FEC(m_clump, 0);
        ResetAtomics(m_clump, m_morph);
        m_morphSetUp = 1;
    }
    if (m_clumpState == 2) {
        for (int i = 1; i < 20; i++) {
                if (m_parts[i].m_motion != 0 && m_parts[i].m_updater == 0) {
                m_parts[i].m_clump = fn_80457090(m_clump);
                m_parts[i].m_updater = new UnkClumpUpdater_80015A7C(m_parts[i].m_clump, m_parts[i].m_motion);
            }
        }
    }
}

bool UnkEventModel_80170A9C::IsLoading() {
    bool loading = false;
    bool partLoading;
    int i;
    int idle;
    for (int i = 0; i < 12; i++) {
        if (m_motionStates[i] == 1 && m_motionStates[i] != 0) {
            loading = true;
        }
    }
    if (!loading) {
        m_motionState = 2;
    }
    for (i = 1; i < 20; i++) {
        idle = 0;
        partLoading = false;
        for (int j = 0; j < 12; j++) {
            if (m_parts[i].m_motionStates[j] == 0) {
                idle++;
            } else if (m_parts[i].m_motionStates[j] == 1) {
                m_motionState = 1;
                partLoading = true;
            }
        }
        if (m_parts[i].m_motion != 0 && (m_parts[i].m_updater == 0 || m_parts[i].m_clump == 0)) {
            partLoading = true;
        }
        if (idle != 12 && !partLoading) {
            m_parts[i].m_clumpState = 2;
        }
    }
    if (m_clumpState == 1 || m_morphState == 1 || m_motionState == 1) {
        return true;
    }
    for (int i = 0; i < 20; i++) {
        if (m_parts[i].m_clumpState == 1) {
            return true;
        }
    }
    return false;
}

void UnkEventModel_80170A9C::Update(float previous, float time) {
    if (IsLoading()) {
        for (int i = 0; i < 32; i++) {
            if (m_pending[i].m_data != 0) {
                m_pending[i].m_callback(m_pending[i].m_data, m_pending[i].m_user);
                m_pending[i].m_data = 0;
                return;
            }
        }
        return;
    }
    if (m_motion != 0 && m_currentMotion != lbl_805EF2B8) {
        if (m_motions[lbl_805EF2B8] != 0) {
            u16 id = fn_80412BFC(m_motions[lbl_805EF2B8], m_name, fn_80049B34, 0);
            if (m_currentMotion >= 0) {
                fn_804129D4(m_motionId);
                m_currentMotion = -1;
            }
            m_motionId = id;
        }
        m_motions[lbl_805EF2B8] = 0;
        m_currentMotion = lbl_805EF2B8;
        fn_804173A8(m_motion, &lbl_80580230, 0, 0);
        fn_80417568(m_motion, m_motionId);
    }
    for (int i = 1; i < 20; i++) {
        if (m_parts[i].m_clump != 0 && m_parts[i].m_motion != 0 && m_parts[i].m_currentMotion != lbl_805EF2B8) {
            if ((s32)m_parts[i].m_motionId >= 0) {
                fn_804129D4(m_parts[i].m_motionId);
                m_parts[i].m_motionId = -1;
            }
            m_parts[i].m_motionId =
                (u16)fn_80412BFC(m_parts[i].m_motions[lbl_805EF2B8], m_parts[i].m_name, fn_80049B34, 0);
            m_parts[i].m_motions[lbl_805EF2B8] = 0;
            m_parts[i].m_currentMotion = lbl_805EF2B8;
            fn_804173A8(m_parts[i].m_motion, &lbl_80580230, 0, 0);
            fn_80417568(m_parts[i].m_motion, m_parts[i].m_motionId);
        }
    }
    if (m_morph != 0) {
        if (!m_first) {
            float alpha = (time - previous) / 30.0f;
            if (m_clump != 0) {
                fn_80456C54(m_clump, SetAtomicAlpha, &alpha);
            }
        } else {
            ResetAtomics(m_clump, m_morph);
        }
    }
    if (IsEventChara(m_name)) {
        if (m_motion != 0) {
            void *node = GetFollowNode();
            if (node != 0) {
                fn_804172D8(m_motion, node);
            }
        } else {
            RwMatrix matrix;
            if (GetFollowMatrix(&matrix)) {
                fn_8047F72C(&((RwFrame *)m_clump->object.parent)->modelling, &matrix, 0);
                fn_8048722C((RwFrame *)m_clump->object.parent);
            }
        }
    } else {
        fn_804875B0((RwFrame *)m_clump->object.parent, &lbl_80580218, 0);
    }
    if (m_motion != 0) {
        fn_80416E20(m_motion, 0, time);
        fn_80415EA4(m_motion);
        if (m_updater != 0) {
            fn_800159AC(m_updater);
        }
    }
    if (m_motion == 0 && m_morph == 0) {
        fn_804875B0((RwFrame *)m_clump->object.parent, &lbl_80580218, 0);
    }
    for (int i = 1; i < 20; i++) {
        if (m_parts[i].m_clump != 0 && m_parts[i].m_motion != 0) {
            fn_80416E20(m_parts[i].m_motion, 0, time);
            fn_80415EA4(m_parts[i].m_motion);
            fn_800159AC(m_parts[i].m_updater);
        }
    }
    m_first = 0;
}

void UnkEventModel_80170A9C::SetLayer(const char *name) {
    if (IsEventChara(name)) {
        m_layer = 6;
        return;
    }
    for (int i = 0; i < 15; i++) {
        char chara[16];
        strcpy(chara, fn_8003A840()->GetCharaName(i));
        for (int j = 0; j < 8; j++) {
            if (chara[j] == '_') {
                chara[j] = '!';
            }
        }
        if (fn_803AC6C0(name, chara) != 0) {
            m_layer = i;
            return;
        }
    }
    m_layer = 4;
}

/* The clumps' frame effects (name inferred; other units call it). */
extern "C" UnkFrameEffects_803284B4 *fn_8016F990(void) {
    static UnkFrameEffects_803284B4 *s_cached;
    if (s_cached == 0) {
        static UnkFrameEffects_803284B4 s_object;
        s_cached = &s_object;
    }
    return s_cached;
}

/* Puts clump into world, out of the world it is in (name inferred). */
inline void MoveClump(RpClump *clump, void *world) {
    void *current = fn_8046256C(clump);
    if (current != world) {
        if (current != 0) {
            fn_804623EC(current, clump);
        }
        fn_804622FC(world, clump);
    }
}

bool UnkEventModel_80170A9C::Show() {
    bool shown = false;
    m_visible = 1;
    RpClump *clump = m_clump;
    if (clump != 0) {
        MoveClump(clump, fn_8003A840()->GetWorld(m_layer));
        shown = true;
        fn_8016F990()->Attach(m_clump);
    }
    for (int i = 1; i < 20; i++) {
        RpClump *partClump = m_parts[i].m_clump;
        if (partClump != 0) {
            MoveClump(partClump, fn_8003A840()->GetWorld(m_layer));
            shown = true;
            fn_8016F990()->Attach(m_parts[i].m_clump);
        }
    }
    return shown;
}

bool UnkEventModel_80170A9C::Hide() {
    bool hidden;
    m_visible = 0;
    if (field_0x2C != 0) {
        void *world = fn_8046256C(field_0x2C);
        if (world != 0) {
            fn_804623EC(world, field_0x2C);
        }
    }
    hidden = true;
    for (int i = 1; i < 20; i++) {
        if (m_parts[i].m_clump != 0) {
            void *world = fn_8046256C(m_parts[i].m_clump);
            if (world != 0) {
                fn_804623EC(world, m_parts[i].m_clump);
                hidden = true;
            }
        }
    }
    return hidden;
}

void UnkEventModel_80170A9C::Follow(UnkEventModel_80170A9C *target, int node) {
    if (target == 0) {
        return;
    }
    m_followTarget = target;
    m_followNode = node;
    field_0xAC = 0;
    m_following = 1;
}

bool UnkEventModel_80170A9C::GetFollowMatrix(RwMatrix *out) {
    void *node = GetFollowNode();
    if (node != 0) {
        fn_80016390(out, node);
        return true;
    }
    return false;
}

RpAtomic *UnkEventModel_80170A9C::ResetAtomic(RpAtomic *atomic, void *data) {
    fn_804443A0(atomic, data);
    return atomic;
}

RpAtomic *UnkEventModel_80170A9C::SetAtomicAlpha(RpAtomic *atomic, void *data) {
    fn_80444630(atomic, *(float *)data);
    return atomic;
}

RwFrame *UnkEventModel_80170A9C::FindFrame(const char *name) {
    RwFrame *frame;
    memset(&frame, 0, sizeof(frame));
    if (m_clump != 0) {
        frame = fn_80038E84((RwFrame *)m_clump->object.parent, name);
    }
    return frame;
}
