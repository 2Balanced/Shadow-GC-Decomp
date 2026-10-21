#ifndef SHADOW_GAME_GADGETMODELINFO_H
#define SHADOW_GAME_GADGETMODELINFO_H

#include "Game/MathTypes.h"
#include "boost/weak_ptr.hpp"
#include <dolphin/types.h>
#include "size_assert.h"

class CharInfo;

#ifdef GADGETMODELINFO_IMPL
/* Only Game/GadgetModelInfo.cpp defines GADGETMODELINFO_IMPL: the model's members below need
 * <list> and boost::function, whose templates would renumber the compiler's @N local symbols in
 * the units that include this header. Names in this block are inferred unless noted. */
#include <list>
#include <boost/function.hpp>

#include "Game/CharaColli.h"
#include "Game/GadgetCollision.h"
#include "boost/shared_ptr.hpp"

struct _stMtnPlayWork;
struct RpClump;
struct RwFrame;
struct RwMatrix;
class UnkClumpUpdater_80015A7C;

/* A part of the model file (0x40 bytes): one clump, where it hangs and how it is placed. */
struct UnkModelPart_8015F7BC {
    /* 0x00 */ const char *m_name;
    /* 0x04 */ const char *m_fileName;   /* the clump's file */
    /* 0x08 */ const char *m_uvAnimName; /* its animated textures, or null */
    /* 0x0C */ const char *m_texName;    /* its texture dictionary, or null */
    /* 0x10 */ const char *m_parentName; /* the part it hangs from (flag 1) */
    /* 0x14 */ s32 m_parentNode;         /* the node of that part */
    /* 0x18 */ Vec3f m_position;
    /* 0x24 */ float m_rotationX;
    /* 0x28 */ float m_rotationY;
    /* 0x2C */ float m_rotationZ;
    /* 0x30 */ s32 m_area;  /* index into the area table when flag 2 is clear */
    /* 0x34 */ s32 m_layer; /* index into the render layer table */
    /* 0x38 */ u32 m_flags; /* bit 0: hangs from another part, bit 1: area from its place */
    /* 0x3C */ s32 m_id;
};

/* A variant of the model: the parts shown together. */
struct GadgetModelVariantPart {
    /* 0x0 */ u32 field_0x0;
    /* 0x4 */ s32 m_part;
};

struct GadgetModelVariant {
    /* 0x0 */ const char *m_name;
    /* 0x4 */ const GadgetModelVariantPart *m_parts;
    /* 0x8 */ s32 m_id;
    /* 0xC */ s32 m_partCount;
};

struct GadgetModelMotionDesc {
    /* 0x00 */ const char *m_name;
    /* 0x04 */ const char *m_partName; /* the part it moves */
    /* 0x08 */ u32 field_0x8;
    /* 0x0C */ s32 m_animId;
    /* 0x10 */ const char *m_fileName; /* a .MTP file, or another motion source */
    /* 0x14 */ s32 m_motionId;
    /* 0x18 */ s32 m_id;
};

struct GadgetModelColliderDesc {
    /* 0x00 */ const char *m_name;
    /* 0x04 */ const char *m_fileName;
    /* 0x08 */ s32 m_kind;  /* index into the character kind table */
    /* 0x0C */ s32 m_group; /* index into the collision group table */
    /* 0x10 */ u32 field_0x10;
    /* 0x14 */ s32 m_id;
};

struct GadgetModelMaterialDesc {
    /* 0x00 */ const char *m_name;
    /* 0x04 */ const char *m_partName;
    /* 0x08 */ s32 m_type; /* 0: tint the whole clump, 1: per material */
    /* 0x0C */ u8 m_alpha;
    /* 0x0D */ u8 m_red;
    /* 0x0E */ u8 m_green;
    /* 0x0F */ u8 m_blue;
    /* 0x10 */ s32 m_id;
};

/* The model file: tables of parts, variants, motions, colliders and material animations. */
struct GadgetModelData {
    /* 0x00 */ UnkModelPart_8015F7BC *m_parts;
    /* 0x04 */ s32 m_partCount;
    /* 0x08 */ GadgetModelVariant *m_variants;
    /* 0x0C */ s32 m_variantCount;
    /* 0x10 */ GadgetModelMotionDesc *m_motions;
    /* 0x14 */ s32 m_motionCount;
    /* 0x18 */ GadgetModelColliderDesc *m_colliders;
    /* 0x1C */ s32 m_colliderCount;
    /* 0x20 */ GadgetModelMaterialDesc *m_materials;
    /* 0x24 */ s32 m_materialCount;
};

extern "C" {
void fn_80014E04(u32 *flags);
void fn_80014E10(u32 *flags);
void fn_80014E1C(u32 *flags, int bit);
bool fn_80014E34(u32 *flags, int bit);
void fn_8006127C(u32 *flags, int bit);
}

/* A 32-bit flag word (as HintRing's): fn_80014E04 clears it, fn_80014E10 clears all bits,
 * fn_80014E1C sets bit, fn_8006127C clears bit and fn_80014E34 tests bit. The game's class keeps
 * these out of line; GadgetModelFlagBits has no constructor so that the material state's inline
 * constructor can call fn_80014E04 itself. */
struct GadgetModelFlagBits {
    void Clear() {
        fn_80014E10(&m_bits);
    }
    void Set(int bit) {
        fn_80014E1C(&m_bits, bit);
    }
    void Reset(int bit) {
        fn_8006127C(&m_bits, bit);
    }
    bool Test(int bit) {
        return fn_80014E34(&m_bits, bit);
    }

    u32 m_bits;
};

struct GadgetModelFlags : public GadgetModelFlagBits {
    GadgetModelFlags() {
        fn_80014E04(&m_bits);
    }
    explicit GadgetModelFlags(u32 bits) {
        m_bits = bits;
    }
};

/* The material state of a part (what fn_801621F0 returns): its animation and colour. */
struct UnkMaterialAnim_801621F0 {
    UnkMaterialAnim_801621F0() {
        fn_80014E04(&m_flags.m_bits);
        m_desc = 0;
        m_alpha = 0;
        m_red = 0;
        m_green = 0;
        m_blue = 0;
        fn_80014E10(&m_flags.m_bits);
        m_fadeAlpha = 0;
        m_fadeRed = 0;
        m_fadeGreen = 0;
        m_fadeBlue = 0;
    }

    bool IsNamed(const char *name);
    bool HasId(int id);

    /* 0x0 */ const GadgetModelMaterialDesc *m_desc;
    /* 0x4 */ GadgetModelFlagBits m_flags; /* 0: colour set, 1: faded, 2: animated */
    /* 0x8 */ u8 m_alpha;
    /* 0x9 */ u8 m_red;
    /* 0xA */ u8 m_green;
    /* 0xB */ u8 m_blue;
    /* 0xC */ u8 m_fadeAlpha;
    /* 0xD */ u8 m_fadeRed;
    /* 0xE */ u8 m_fadeGreen;
    /* 0xF */ u8 m_fadeBlue;
};

/* The texture a part's clump was given (what fn_8004F9E8 returns). */
struct GadgetModelTexture {
    GadgetModelTexture() : field_0x0(0), field_0x2(-1) {}

    /* 0x0 */ u16 field_0x0;
    /* 0x2 */ s16 field_0x2;
};

/* A loaded part (the values of the list at 0x4C). */
struct GadgetModelPart {
    GadgetModelPart();
    ~GadgetModelPart();
    void Release();

    /* 0x00 */ RpClump *m_clump;
    /* 0x04 */ RwFrame *m_frame;
    /* 0x08 */ const char *m_uvAnimName;
    /* 0x0C */ GadgetModelTexture m_texture;
    /* 0x10 */ const UnkModelPart_8015F7BC *m_desc;
    /* 0x14 */ RwMatrix *m_matrix; /* the part's place in the model */
    /* 0x18 */ u32 m_flags;        /* bit 0: a motion moves it */
    /* 0x1C */ UnkMaterialAnim_801621F0 *m_material;
    /* 0x20 */ u8 *field_0x20;
};

/* A playing motion (the values of the list at 0x58). */
struct GadgetModelMotion {
    GadgetModelMotion() : m_name(0) {
        m_updater = 0;
        m_anim = 0;
        m_base = 0;
        m_desc = 0;
        m_flags = 0;
    }

    /* Inline (defined in Game/GadgetModelInfo.cpp, where the updater's class is known). */
    inline void Release();

    /* 0x00 */ const char *m_name;
    /* 0x04 */ boost::shared_ptr<_stMtnPlayWork> m_work;
    /* 0x0C */ UnkClumpUpdater_80015A7C *m_updater;
    /* 0x10 */ void *m_anim;
    /* 0x14 */ s32 m_base; /* the first motion number */
    /* 0x18 */ const GadgetModelMotionDesc *m_desc;
    /* 0x1C */ u32 m_flags; /* bit 0: paused, bit 1: frozen */
};

/* A collider of the model (the values of the list at 0x64). */
struct GadgetModelCollider {
    GadgetModelCollider() : m_name(0) {
        m_desc = 0;
        m_flags = 0;
    }

    void Release();

    /* 0x00 */ const char *m_name;
    /* 0x04 */ boost::shared_ptr<SonicteamUSA::System::CharaColli::CharaColliBase> m_colli;
    /* 0x0C */ const GadgetModelColliderDesc *m_desc;
    /* 0x10 */ u32 m_flags; /* bit 0: touched */
};

typedef std::list<GadgetModelPart>::iterator PartIter;
typedef std::list<GadgetModelMotion>::iterator MotionIter;
typedef std::list<GadgetModelCollider>::iterator ColliderIter;

/* A collision callback of the model (class name unknown): a function1 over the collision event,
 * which fn_800D0B64 assigns. */
class UnkModelCallback_800D0B64 {
  public:
    /* Calls the function when one is set (inline, defined in Game/GadgetModelInfo.cpp). */
    inline void CallIfSet(ColliDetectArgs &args) const;

    /* 0x0 */ boost::function1<void, ColliDetectArgs &> m_function;
};
#endif

/* GadgetModelInfo, named by the game's RTTI (its vtable, at 0x8052B500, holds only the
 * destructor). It owns the model a gadget loads from a .BIN file: its parts, the motions playing
 * on them and their colliders. Its methods still carry their addresses; what callers suggest about
 * them is noted where it helps. Game/GadgetModelInfo.cpp. */
class GadgetModelInfo {
  public:
    /* Load the model named `path`. The first argument's meaning is open. */
    GadgetModelInfo(int arg0, const char *path);
    /* Load model number `modelId` (0x80163F7C). */
    GadgetModelInfo(int arg0, int modelId);
    virtual ~GadgetModelInfo();

    void fn_8015FF60();
    void fn_801604FC();
    bool fn_80160B18(int arg);
    /* Plays motion `motion`; false when the model has none of that name. */
    bool fn_80160BA0(const char *motion);
    bool fn_80160C28(int arg);
    void fn_8016108C();
    void fn_801617D0(int index, int arg1, float x, float y, float z);
    void fn_80161880();
    /* Attaches `info` to the node called `node`. */
    bool fn_80161F08(const char *node, const boost::weak_ptr<CharInfo> &info);
    void fn_80162804();
    bool fn_80162A68(int index, int arg1);
    void fn_80162EF4();
    /* Advances animation by dt. */
    void fn_80162FB0(float dt);
    /* Places the model with the given world matrix. */
    void fn_801633DC(const Mtx34f *world);

#ifdef GADGETMODELINFO_IMPL
    /* Method names are inferred. */
    void Init();
    bool StartMotion(int index);
    bool AddCollider(int index, boost::weak_ptr<CharInfo> info);
    bool AddCollider(int index, boost::weak_ptr<CharInfo> info, _stMtnPlayWork *work);
    bool LoadVariant(int index, int animated);
    bool ApplyMaterial(int index);
    void StopBlinking();
    void DetachParts();
    void RemoveFromWorld();
    void AttachParts();
    void MoveToArea(int area);
    void MoveToWorld();

    /* 0x04 */ UnkModelCallback_800D0B64 m_onContact;
    /* 0x10 */ UnkModelCallback_800D0B64 m_onEnter;
    /* 0x1C */ UnkModelCallback_800D0B64 m_onLeave;
    /* 0x28 */ s32 m_variantIndex;
    /* 0x2C */ s32 m_partCount;
    /* 0x30 */ GadgetModelData *m_data;
    /* 0x34 */ const GadgetModelVariant *m_variant;
    /* 0x38 */ float m_collisionOffTime;
    /* 0x3C */ GadgetModelFlags m_flags; /* 0: collision off for a time, 1: collision off, 2:
                                          * hidden, 4: blinking for a time, 5: blinking, 6: faded,
                                          * 7: fading, 8: fading out, 9: fading in, 10: faded out */
    /* 0x40 */ float m_blinkTime;
    /* 0x44 */ float m_alpha;
    /* 0x48 */ float m_fadeSpeed;
    /* 0x4C */ std::list<GadgetModelPart> m_parts;
    /* 0x58 */ std::list<GadgetModelMotion> m_motions;
    /* 0x64 */ std::list<GadgetModelCollider> m_colliders;
    /* 0x70 */ s32 m_fileGroup; /* passed to the file manager with every file */
#else
    /* 0x04 */ u8 field_0x4[0x74 - 0x4];
#endif
};
SIZE_ASSERT(GadgetModelInfo, 0x74);

#endif
