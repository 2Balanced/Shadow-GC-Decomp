/* ElecCristalWall (class name from RTTI; "Cristal" is the original spelling): the ELEC_WALL gadget
 * of the electric circuit area, a crystal wall (WALL.BIN) standing horizontally or vertically by its
 * type parameter. Once its circuit is switched on it rises into place from below, at the angles of
 * its "AppearAngle" parameters, spinning to rest. Attacks flash it and wear it down; after six hits
 * it breaks into pieces and reports itself finished. Its colour fades through four colours as it
 * wears. File name inferred from the class. Method names marked "(name inferred)" are not original.
 * Definitions are in reverse address order (deferred inlining emits functions last-defined
 * first).
 *
 * Not yet matching: in fn_800CBC1C the original passes the line's end point (character info
 * position + offset, through fn_8040CDD0) as a temporary in the lowest stack slot, yet reloads the
 * character info pointer after the addition. Written as one call with a temporary, GC/2.7 keeps the
 * pointer in a register across the addition instead; with a named end point, as here, the code is
 * the same but the end point sits above the other temporaries (six stack offsets differ). Everything
 * else matches. */
#include <cmath>

#include <boost/bind.hpp>
#include <boost/function.hpp>

#include "Game/CharInfo.h"
#include "Game/CharaColliAttack.h"
#include "Game/DebugDraw.h"
#include "Game/Gadget.h"
#include "Game/GadgetCharInfo.h"
#include "Game/GadgetModelInfo.h"
#include "Game/GadgetClassDesc.h"
#include "Game/MathConstants.h"
#include "Game/UnkSwitchBank_800CB69C.h"
#include "Game/FileQueue.h"
#include "Game/Random.h"
#include "boost/shared_ptr.hpp"
#include "boost/weak_ptr.hpp"
#include "size_assert.h"

using SonicteamUSA::System::CharaColli::AttackCallback;
using SonicteamUSA::System::CharaColli::AttackCallbackParam;
using SonicteamUSA::System::CharaColli::CharaColliAttack;

/* Four colour bytes (red, green, blue, alpha; inferred) built by an out-of-line constructor (see
 * ElecBarrier.cpp). */
struct UnkColor_8040D2D8 {
    UnkColor_8040D2D8(u8 r, u8 g, u8 b, u8 a);

    /* 0x0 */ u8 r;
    /* 0x1 */ u8 g;
    /* 0x2 */ u8 b;
    /* 0x3 */ u8 a;
};
SIZE_ASSERT(UnkColor_8040D2D8, 0x4);

/* The material animation of a model fn_801621F0 finds by name (layout beyond its colour
 * unknown). */
struct UnkMaterialAnim_801621F0 {
    /* 0x0 */ u8 field_0x0[0x8];
    /* 0x8 */ UnkColor_8040D2D8 color;
};

/* A model part as fn_8015F7BC returns it; only the word at 0x4 is used (layout unknown). */
struct UnkModelPart_8015F7BC {
    /* 0x0 */ u32 field_0x0;
    /* 0x4 */ void *field_0x4;
};

/* The pieces a gadget breaks into, set up by fn_8015F0B8 and launched by fn_8015D93C (see
 * ElecFireWall.cpp). */
struct UnkPieces_8015F0B8 {
    /* 0x00 */ u8 field_0x0[0x34];
    /* 0x34 */ float field_0x34;
    /* 0x38 */ u8 field_0x38[0xC];
    /* 0x44 */ u32 field_0x44;
    /* 0x48 */ u32 field_0x48;
    /* 0x4C */ s32 field_0x4C;
    /* 0x50 */ u32 field_0x50;
    /* 0x54 */ u8 field_0x54;
};

/* The fields of the cursor that the label callback reads (layout from its accesses). */
struct ElecCristalWallParamCursor {
    /* 0x0 */ u32 field_0x0;
    /* 0x4 */ int index; /* the parameter */
    /* 0x8 */ int value;
};

class UnkSoundManager_80053164;
class UnkPieceManager_800122D4;

/* A debug line on the caller's stack (layout unknown). */
struct UnkDebugLine_80020A20 {
    /* 0x00 */ u8 field_0x0[0x18];
};

extern "C" {
/* GadgetModelInfo helpers (behaviour inferred): find the material animation named name, and
 * select the material named name. */
UnkMaterialAnim_801621F0 *fn_801621F0(GadgetModelInfo *model, const char *name);
void fn_8016239C(GadgetModelInfo *model, const char *name);
/* Returns the attack collider of model number index (behaviour inferred). */
boost::weak_ptr<CharaColliAttack> fn_80161428(GadgetModelInfo *model, int index);
/* Adds a callback to an attack collision (behaviour inferred; the meaning of arg1 is unknown). */
void fn_80096B64(CharaColliAttack *attack, int arg1, const AttackCallback &callback);
/* Blends animation's colour from from to to as t goes from t0 to t1 (see ElecBarrier.cpp). */
void fn_80154C6C(UnkMaterialAnim_801621F0 *animation, const UnkColor_8040D2D8 *from,
                 const UnkColor_8040D2D8 *to, float t, float t0, float t1);
/* Returns the part of model whose id is id, and the pieces' manager, setup, lookup and launch
 * (see ElecFireWall.cpp). */
UnkModelPart_8015F7BC *fn_8015F7BC(GadgetModelInfo *model, int id);
UnkPieceManager_800122D4 *fn_800122D4(void);
void fn_8015F0B8(UnkPieces_8015F0B8 *pieces, const boost::shared_ptr<CharInfo> &charInfo, int arg2);
u32 fn_80012700(UnkPieceManager_800122D4 *manager, int arg1, void *arg2);
void fn_8015D93C(UnkPieces_8015F0B8 *pieces);
/* Builds matrix from rotation in the given axis order, and reads a rotation back from a matrix
 * (behaviour inferred). */
void fn_800193B8(Mtx34f *matrix, const Vec3f *rotation, int order);
void fn_800494F8(Vec3f *rotation, const Mtx34f *matrix);
/* vector turned about the x and the y axis by angle (behaviour inferred). */
Vec3f fn_8040D118(const Vec3f *vector, float angle);
Vec3f fn_8040D0AC(const Vec3f *vector, float angle);
/* vector * scale (behaviour inferred). */
Vec3f fn_800091A4(const Vec3f *vector, float scale);
/* destination = a + b; vector += value (behaviour inferred). */
void fn_8040CDD0(Vec3f *destination, const Vec3f *a, const Vec3f *b);
void fn_8040CFF8(Vec3f *vector, const Vec3f *value);
/* Sets up a debug line from start to end, and draws it (behaviour inferred). */
void *fn_80020A20(UnkDebugLine_80020A20 *line, const Vec3f *start, const Vec3f *end);
void fn_802E0AA0(UnkDebugLine_80020A20 *line, const UnkDrawColor_802E0E84 *color, float width);
/* Tells the gadget's owner that the gadget is done (behaviour inferred). */
void fn_8015A794(Gadget *gadget);
/* Returns the sound manager and plays sound id at position through it (behaviour inferred). */
UnkSoundManager_80053164 *fn_80053164(void);
void fn_801D54F4(UnkSoundManager_80053164 *manager, int id, const Vec3f *position,
                 int arg3, int arg4);
}

/* The vector type's out-of-line assignment (fn_800091D0), called with a temporary the way the
 * original's operator is (as in GadgetUtil.h). */
inline void AssignVector(Vec3f &vector, const Vec3f &value) {
    fn_800091D0(&vector, &value);
}

/* vector += value through fn_8040CFF8 (name inferred). */
inline Vec3f &operator+=(Vec3f &vector, const Vec3f &value) {
    fn_8040CFF8(&vector, &value);
    return vector;
}

/* The three parameters the owner supplies (names from the parameter table; the first is unnamed
 * there, its labels are "type< Horizontal >" and "type< Vertical >"). */
struct ElecCristalWallParams {
    /* 0x0 */ int type;
    /* 0x4 */ float appearAngleX; /* degrees */
    /* 0x8 */ float appearAngleY;
};
SIZE_ASSERT(ElecCristalWallParams, 0xC);

class ElecCristalWall : public Gadget {
  public:
    ElecCristalWall(Task *parent, UnkPlacement_800C9CD0 *owner);
    virtual ~ElecCristalWall();
    virtual void Behave(float dt);
    virtual void fn_800CBC1C();
    virtual void ReloadPlacement(UnkPlacement_800C9CD0 *owner);

    /* Creates the wall under task layer 11 (name inferred). */
    static void Instantiate(UnkPlacement_800C9CD0 *owner);

    /* Copies the owner's position and rotation (turned upright for a vertical wall), the
     * parameters and the circuit id into the gadget (name inferred). */
    void ReadPlacement(UnkPlacement_800C9CD0 *owner);
    /* Places the model at m_position, turned by m_rotation and the character info's rotation
     * (name inferred). */
    void RebuildTransform();
    /* The attack callback (name inferred). */
    void OnAttack(AttackCallbackParam &param);
    /* Breaks the model into pieces (name inferred). */
    void Break();

    static const char *s_taskName;

    /* 0x30 */ ElecCristalWallParams m_params;
    /* 0x3C */ boost::shared_ptr<GadgetCharInfo> m_chara;
    /* 0x44 */ GadgetModelInfo *m_model;
    /* 0x48 */ u8 field_0x48; /* copied from the owner; the circuit id it waits for (inferred) */
    /* 0x4C */ int m_state;   /* 0 new, 1 waiting, 2 appearing, 4 standing, 5 breaking (inferred) */
    /* 0x50 */ float m_appearTime; /* names inferred */
    /* 0x54 */ int m_hits;         /* hits left */
    /* 0x58 */ Vec3f m_appearOffset;
    /* 0x64 */ Vec3f m_spin;
    /* 0x70 */ int m_spinOrder;
    /* 0x74 */ Vec3f m_position;
    /* 0x80 */ Vec3f m_rotation;
    /* 0x8C */ float m_flashTime;
};
SIZE_ASSERT(ElecCristalWall, 0x90);

const char *ElecCristalWall::s_taskName = "ElecCristalWall";

/* The colours the wall fades through as it wears (meaning inferred). */
static UnkColor_8040D2D8 lbl_805EFB74(0xFF, 0x00, 0x00, 0x00);
static UnkColor_8040D2D8 lbl_805EFB78(0xFF, 0x00, 0x33, 0x00);
static UnkColor_8040D2D8 lbl_805EFB7C(0xFF, 0x26, 0x26, 0x00);
static UnkColor_8040D2D8 lbl_805EFB80(0xFF, 0x33, 0x00, 0x00);

ElecCristalWall::ElecCristalWall(Task *parent, UnkPlacement_800C9CD0 *owner)
    : Gadget(parent, owner), m_spin(0.0f, 0.0f, 0.0f), m_rotation(0.0f, 0.0f, 0.0f) {
    m_name = s_taskName;
    fn_800C9DBC(owner, &m_params, sizeof(m_params));
    m_chara = fn_8015CB88(0x58, 4, "ElecCristalWall");
    m_model = new GadgetModelInfo(1, "WALL.BIN");
    m_model->fn_80162A68(0, 0);
    fn_8016239C(m_model, "mat");
    m_model->fn_80161F08("col", m_chara);

    boost::weak_ptr<CharaColliAttack> attack = fn_80161428(m_model, 0);
    if (boost::shared_ptr<CharaColliAttack> collision = attack.lock()) {
        fn_80096B64(collision.get(), 1, boost::bind(&ElecCristalWall::OnAttack, this, _1));
    }

    ReadPlacement(m_owner);
    m_state = 0;
    m_hits = 6;
    switch (fn_8040EA90(3)) {
    case 0:
        m_spinOrder = 0;
        break;
    case 1:
        m_spinOrder = 1;
        break;
    case 2:
        m_spinOrder = 2;
        break;
    }
    m_spin.x = lbl_805FAD38 * ((u32)fn_8040EA90(2) != 0 ? 1.0f : -1.0f);
    m_spin.y = lbl_805FAD38 * ((u32)fn_8040EA90(2) != 0 ? 1.0f : -1.0f);
    m_spin.z = lbl_805FAD38 * ((u32)fn_8040EA90(2) != 0 ? 1.0f : -1.0f);
    RebuildTransform();
}

ElecCristalWall::~ElecCristalWall() {
    if (m_model != 0) {
        delete m_model;
        m_model = 0;
    }
}

void ElecCristalWall::Behave(float dt) {
    if (m_model != 0) {
        m_model->fn_80162FB0(dt);
    }

    switch (m_state) {
    case 0:
        m_state = 1;
        fn_801621F0(m_model, "mat");
        break;
    case 1:
        if (fn_8016904C(fn_800CB69C(), field_0x48) == 1) {
            m_state = 2;
            m_appearOffset.x = m_appearOffset.y = m_appearTime = 0.0f;
            m_appearOffset.z = -75.0f;
            AssignVector(m_appearOffset, fn_8040D118(&m_appearOffset, m_params.appearAngleX *
                                                                          (lbl_805FAD38 / 180.0f)));
            AssignVector(m_appearOffset, fn_8040D0AC(&m_appearOffset, m_params.appearAngleY *
                                                                          (lbl_805FAD38 / 180.0f)));
            fn_800C9F14(m_owner, &m_position);
            fn_8040CFF8(&m_position, &m_appearOffset);
            fn_801D54F4(fn_80053164(), 0x723F, &m_position, 0, 1);
        }
        break;
    case 2: {
        m_appearTime += dt;
        if (m_appearTime > 1.0f) {
            m_appearTime = 1.0f;
            m_state = 4;
            UnkMaterialAnim_801621F0 *animation = fn_801621F0(m_model, "mat");
            if (animation != 0) {
                animation->color.g = 200;
                animation->color.b = 255;
                animation->color.a = 255;
            }
            m_flashTime = 0.25f;
        }

        UnkMaterialAnim_801621F0 *animation = fn_801621F0(m_model, "mat");
        if (animation != 0) {
            fn_80154C6C(animation, &lbl_805EFB74, &lbl_805EFB78, m_appearTime, 0.0f, 1.0f);
        }

        float c = std::cos(0.5f * (m_appearTime * lbl_805FAD38));
        float scale = c * (c * c);
        fn_800C9F14(m_owner, &m_position);
        m_position += fn_800091A4(&m_appearOffset, scale);
        m_rotation.x = m_spin.x * scale;
        m_rotation.y = m_spin.y * scale;
        m_rotation.z = m_spin.z * scale;
        break;
    }
    case 5:
        if (m_hits <= 0) {
            Break();
            fn_8015A794(this);
            m_flags |= 1;
            return;
        }
        break;
    }

    RebuildTransform();
    if (m_flashTime > 0.0f) {
        m_flashTime -= dt;
        if (m_flashTime < 0.0f) {
            m_flashTime = 0.0f;
        }
    } else {
        UnkMaterialAnim_801621F0 *animation = fn_801621F0(m_model, "mat");
        if (animation != 0) {
            if (m_hits <= 3) {
                float t = (float)(3 - m_hits) / (float)m_hits;
                animation->color.g = lbl_805EFB80.g * t + lbl_805EFB7C.g * (1.0f - t);
                animation->color.b = lbl_805EFB80.b * t + lbl_805EFB7C.b * (1.0f - t);
                animation->color.a = lbl_805EFB80.a * t + lbl_805EFB7C.a * (1.0f - t);
            } else {
                float t = (float)(6 - m_hits) / 3.0f;
                animation->color.g = lbl_805EFB7C.g * t + lbl_805EFB78.g * (1.0f - t);
                animation->color.b = lbl_805EFB7C.b * t + lbl_805EFB78.b * (1.0f - t);
                animation->color.a = lbl_805EFB7C.a * t + lbl_805EFB78.a * (1.0f - t);
            }
        }
    }
}

void ElecCristalWall::ReloadPlacement(UnkPlacement_800C9CD0 *owner) {
    ReadPlacement(owner);
    m_state = 0;
    RebuildTransform();
    if (m_model != 0) {
        m_model->fn_80162EF4();
    }
}

void ElecCristalWall::fn_800CBC1C() {
    UnkDebugLine_80020A20 segment;

    fn_800C9F14(m_owner, &m_position);
    m_rotation.x = m_rotation.y = m_rotation.z = 0.0f;
    Vec3f offset(0.0f, 0.0f, -30.0f);
    AssignVector(offset, fn_8040D118(&offset, m_params.appearAngleX * (lbl_805FAD38 / 180.0f)));
    AssignVector(offset, fn_8040D0AC(&offset, m_params.appearAngleY * (lbl_805FAD38 / 180.0f)));
    Vec3f end;
    fn_8040CDD0(&end, &m_chara->m_position, &offset);
    fn_80020A20(&segment, &m_chara->m_position, &end);
    fn_802E0AA0(&segment, &lbl_805F06D8, 3.0f);
    if (m_model != 0) {
        m_model->fn_8016108C();
    }
}

void ElecCristalWall::ReadPlacement(UnkPlacement_800C9CD0 *owner) {
    fn_800C9F14(owner, &m_chara->m_position);
    fn_800C9ED4(owner, &m_chara->m_rotation);
    if (m_params.type == 1) {
        Mtx34f matrix;

        fn_800193B8(&matrix, &m_chara->m_rotation, 2);
        fn_8040B884(&matrix, 0.5f * lbl_805FAD38);
        fn_800494F8(&m_chara->m_rotation, &matrix);
    }
    fn_800C9DBC(owner, &m_params, sizeof(m_params));
    fn_800C9E60(owner, &field_0x48);
}

void ElecCristalWall::RebuildTransform() {
    if (m_model != 0) {
        Mtx34f matrix;

        fn_8040C148(&matrix, &lbl_805E0720);
        fn_8040B59C(&matrix, &m_position);
        fn_8001E070(&matrix, &m_rotation, m_spinOrder);
        fn_8001E070(&matrix, &m_chara->m_rotation, 2);
        m_model->fn_801633DC(&matrix);
    }
}

void ElecCristalWall::OnAttack(AttackCallbackParam &param) {
    if (0.0f == m_flashTime && m_state == 4) {
        UnkMaterialAnim_801621F0 *animation = fn_801621F0(m_model, "mat");
        if (animation != 0) {
            animation->color.g = 200;
            animation->color.b = 200;
            animation->color.a = 0;
        }
        m_flashTime = 0.125f;
        m_hits--;
        if (m_hits <= 0) {
            param.m_result = 3;
            m_state = 5;
            fn_801D54F4(fn_80053164(), 0x7241, &m_chara->m_position, 0, 1);
        } else {
            param.m_result = 1;
            fn_801D54F4(fn_80053164(), 0x7240, &m_chara->m_position, 0, 1);
        }
    } else {
        param.m_result = 1;
    }
}

void ElecCristalWall::Break() {
    if (m_model != 0) {
        UnkModelPart_8015F7BC *part = fn_8015F7BC(m_model, 1);
        if (part != 0) {
            UnkPieceManager_800122D4 *manager = fn_800122D4();
            UnkPieces_8015F0B8 pieces;

            fn_8015F0B8(&pieces, m_chara, 0);
            pieces.field_0x34 = 1.5f;
            pieces.field_0x44 = fn_80012700(manager, 1, part->field_0x4);
            pieces.field_0x54 = 0;
            pieces.field_0x4C = 150;
            fn_8015D93C(&pieces);
        }
    }
}

extern "C" void fn_801F8CDC(void) {
    fn_8006B474("WALL.BIN", 1);
}

extern "C" void fn_801F8CD8(void) {}

void ElecCristalWall::Instantiate(UnkPlacement_800C9CD0 *owner) {
    new ElecCristalWall(fn_801EB304(fn_8001C4F8(), 11), owner);
}

/* The labels of the type values. */
static const char *lbl_805E9074[] = {
    "type< Horizontal >",
    "type< Vertical   >",
};

/* The label callback of the type parameter. */
extern "C" const char *fn_801F8C58(UnkLabelQuery_800CBC20 *cursor) {
    const ElecCristalWallParamCursor *fields = (const ElecCristalWallParamCursor *)cursor;

    switch (fields->index) {
    case 0:
        return lbl_805E9074[fields->value];
    }
    return 0;
}

GadgetParamDesc lbl_80542424[] = {
    {1, "", 0, 0, 1, 0.0f, 0.0f, 0.0f, fn_801F8C58, 0},
    {4, "AppearAngleX", 0, 0, 0, 0.0f, -65536.0f, 65536.0f, fn_800CBC20, 0},
    {4, "AppearAngleY", 0, 0, 0, 0.0f, -65536.0f, 65536.0f, fn_800CBC20, 0},
    {0},
};

GadgetClassDesc lbl_805424C4 = {
    "ELEC_WALL", fn_801F8CDC, fn_801F8CD8, ElecCristalWall::Instantiate, 0, 0, 0x7D7, 1, 0, 10,
    lbl_80542424,
};
