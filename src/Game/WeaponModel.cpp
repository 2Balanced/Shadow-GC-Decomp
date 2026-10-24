/* The model of a weapon held by a character or lying in the stage (UnkWeaponModel_80063428, declared
 * in Game/WeaponBase.h; class name unknown). It keeps its state in an Impl (class name unknown)
 * built from the weapon's model description: each part is a clump cloned from the model resource
 * slot into the stage's world, hung on a frame of the first part's clump (or on the model's own
 * frame), with its motions and a node callback that scales its frame; the model keeps the world
 * matrices of some frames of the first clump as nodes (node 0 is the muzzle). Other units call the
 * functions without class names by their addresses, so they keep those names. Game/WeaponBase.h
 * names the description's members after their first known use: the "effects" are the parts and the
 * "sounds" the frame ids of the nodes. File name and member names are inferred. Definitions are in
 * reverse address order (deferred inlining emits functions last-defined first). */
#define WEAPON_MODEL_IMPL_MEMBERS
/* boost::scoped_array needs it (see boost/checked_delete.hpp). */
#define BOOST_CHECKED_ARRAY_DELETE

#include "Game/CharInfo.h"
#include "Game/DebugDispShape.h"
#include "Game/MathTypes.h"
#include "Game/WeaponBase.h"
#include "boost/scoped_array.hpp"
#include "boost/scoped_ptr.hpp"
#include "boost/shared_ptr.hpp"
#include "boost/weak_ptr.hpp"
#include <dolphin/types.h>
#include "size_assert.h"

using Weapon::UnkWeaponEffect_80063428;
using Weapon::UnkWeaponEffectSet_80063428;
using Weapon::UnkWeaponEffectSub_80063428;
using Weapon::UnkWeaponModel_80063428;
using Weapon::UnkWeaponModelImpl_8006465C;

struct _stMtnPlayWork;

/* What a motion passes to its node callbacks (as in Game/VehicleGun.cpp). */
struct _stMtnCallbackParam {
    /* 0x0 */ RwFrame *m_frame;
    /* 0x4 */ void *m_data;
};

/* What fn_804125F4 reports about a motion id (as in Game/DevilDoomCtrlUnit.cpp). */
struct UnkMotionRange_804125F4 {
    /* 0x0 */ float m_end;
    /* 0x4 */ u8 field_0x4[0x10 - 0x4];
};

/* An object that keeps a clump in step with a motion (class name unknown; as in
 * Game/VehicleGun.cpp). */
class UnkClumpUpdater_80015A7C {
  public:
    UnkClumpUpdater_80015A7C(RpClump *clump, _stMtnPlayWork *motion);
    ~UnkClumpUpdater_80015A7C();

    /* 0x00 */ RpClump *m_clump;
    /* 0x04 */ u8 field_0x4[0x1C - 0x4];
};

/* A clump's colour fade (struct name unknown; as in Game/Diablon.h); its constructor is out of line
 * (0x80027D5C). */
struct UnkClumpFade_80027D5C;
extern "C" UnkClumpFade_80027D5C *fn_80027D5C(UnkClumpFade_80027D5C *fade);
struct UnkClumpFade_80027D5C {
    UnkClumpFade_80027D5C() {
        fn_80027D5C(this);
    }

    /* 0x00 */ float m_color[4];
    /* 0x10 */ u8 field_0x10[0x24 - 0x10];
    /* 0x24 */ u8 m_mode;
    /* 0x25 */ u8 field_0x25[0x2C - 0x25];
};

/* The stage (class name unknown; fn_8003A840 returns it): the world of each of its areas (as in
 * Game/BossCtrlUnit.cpp). */
class UnkStage_8003A840 {
  public:
    void *GetWorld(int area);
};

/* What CharInfo's word at 0x60 points to (struct name unknown; see Game/DiablonPartsBody.cpp). */
struct UnkCharTime {
    /* 0x0 */ float m_frameTime;
    /* 0x4 */ float m_speed; /* (name inferred) */
};

/* The game state (struct name unknown; as far as this unit reads it): the frame time at 0x24 (see
 * Game/DevilDoomCtrlUnit.cpp) and the word at 0x88, whose bit 0 a blinking model hides on (meaning
 * inferred). */
struct UnkGameState_8057D6A0 {
    /* 0x000 */ u8 field_0x0[0x24];
    /* 0x024 */ float m_dt;
    /* 0x028 */ u8 field_0x28[0x88 - 0x28];
    /* 0x088 */ u32 field_0x88;
    /* 0x08C */ u8 field_0x8C[0x150 - 0x8C];
};
SIZE_ASSERT(UnkGameState_8057D6A0, 0x150);

extern UnkGameState_8057D6A0 lbl_8057D6A0;
/* Set while the game is paused (meaning inferred; see Game/VehicleArrow.cpp): a blinking model stays
 * shown. */
extern u8 lbl_805EE1DC;

extern "C" {
/* A 32-bit flag word (see Game/BKTankController.h): construct cleared, clear all, set, clear and
 * test a bit. */
void fn_80014E04(u32 *flags);
void fn_80014E10(u32 *flags);
void fn_80014E1C(u32 *flags, int bit);
void fn_8006127C(u32 *flags, int bit);
bool fn_80014E34(u32 *flags, int bit);
/* The resource manager, a model of a slot, the first motion number of a slot and a motion's data
 * (see Game/DiablonPartsModelBase.cpp). */
void *fn_800122D4(void);
RpClump *fn_8001277C(void *manager, s32 slot, int model);
int fn_80012608(void *manager, s32 slot, int type);
void *fn_80012684(void *manager, s32 slot, int anim);
/* The stage, the area a clump's frame lies in (see Game/BossCtrlUnit.cpp). */
UnkStage_8003A840 *fn_8003A840(void);
void *fn_80169364(RpClump *clump);
int fn_80169350(void *frame);
/* RpClumpClone, the clump's set-up, its fade, showing and hiding it, advancing its material
 * animations (behaviour inferred), and RpClumpDestroy. */
RpClump *fn_80457090(RpClump *clump);
void fn_80038C84(RpClump *clump);
void fn_80038198(RpClump *clump, UnkClumpFade_80027D5C *fade);
void fn_80038F94(RpClump *clump);
void fn_80038FC0(RpClump *clump);
void fn_80039B94(RpClump *clump, float dt, float arg2);
void fn_80457468(RpClump *clump);
/* The world a clump is in, and taking it out of and adding it to a world. */
void *fn_8046256C(RpClump *clump);
void fn_804623EC(void *world, RpClump *clump);
void fn_804622FC(void *world, RpClump *clump);
/* The frame of clump with hierarchy id id, the root of a frame, the world matrix of a frame
 * (behaviour inferred; see Game/DiablonPartsModelBase.cpp), RwFrameCreate, RwFrameDestroy,
 * RwFrameGetLTM and RwMatrixScale. */
RwFrame *fn_80038D24(RpClump *clump, s32 id);
RwFrame *fn_804872D8(RwFrame *frame);
Mtx34f fn_802C8068(RwFrame *frame);
RwFrame *fn_80486D00(void);
int fn_80486E2C(RwFrame *frame);
RwMatrix *RwFrameGetLTM(RwFrame *frame);
RwMatrix *fn_8047F398(RwMatrix *matrix, const RwV3d *scale, int combine);
/* A matrix of a RenderWare matrix (another copy than fn_800635A4 below; behaviour inferred), a
 * frame's matrix written, a scale matrix, and a matrix multiplied in place (see
 * Game/WeaponItem.cpp). */
Mtx34f fn_80016414(const RwMatrix *matrix);
void fn_800163F4(RwFrame *frame, const Mtx34f *matrix);
void fn_8040B07C(Mtx34f *matrix, const Vec3f *scale);
void fn_8040BAC4(Mtx34f *matrix, const Mtx34f *other);
/* The motion player (see Game/VehicleGun.cpp and Game/DiablonPartsModelBase.cpp): creating and
 * freeing one, a node's callback, placing it under a matrix, its speed, running it, the clump
 * updater's step, choosing a motion, its flags, the range of a motion, and setting its time. */
_stMtnPlayWork *fn_8041B908(void *anim, int arg1);
void fn_8041B884(_stMtnPlayWork *work);
void fn_80415808(_stMtnPlayWork *work, int id, void (*callback)(_stMtnCallbackParam *), void *data);
void fn_804172D8(_stMtnPlayWork *work, const Mtx34f *matrix);
void fn_80416DC8(_stMtnPlayWork *work, int arg1, float dt);
void fn_80415EA4(_stMtnPlayWork *work);
void fn_800159AC(UnkClumpUpdater_80015A7C *updater);
void fn_80417568(_stMtnPlayWork *work, u16 motion);
int fn_80416F28(_stMtnPlayWork *work, int arg1, int arg2, int arg3);
int fn_804125F4(UnkMotionRange_804125F4 *range, u16 motion);
void fn_80416E20(_stMtnPlayWork *work, int arg1, float frame);
}

/* Moves clump into world (as in Game/DiablonPartsModelBase.cpp). */
inline void SetClumpWorld(RpClump *clump, void *world) {
    void *current = fn_8046256C(clump);
    if (current != world) {
        if (current != 0) {
            fn_804623EC(current, clump);
        }
        fn_804622FC(world, clump);
    }
}

/* Copies value into matrix (as in Game/DiablonPartsModelBase.cpp). */
inline void AssignMatrix(Mtx34f &matrix, const Mtx34f &value) {
    fn_8040C148(&matrix, &value);
}

/* Fills matrix row by row (name inferred). */
inline void SetRows(Mtx34f &matrix, float m00, float m01, float m02, float m03, float m10, float m11,
                    float m12, float m13, float m20, float m21, float m22, float m23) {
    matrix.values[0][0] = m00;
    matrix.values[0][1] = m01;
    matrix.values[0][2] = m02;
    matrix.values[0][3] = m03;
    matrix.values[1][0] = m10;
    matrix.values[1][1] = m11;
    matrix.values[1][2] = m12;
    matrix.values[1][3] = m13;
    matrix.values[2][0] = m20;
    matrix.values[2][1] = m21;
    matrix.values[2][2] = m22;
    matrix.values[2][3] = m23;
}

/* The 3x4 matrix of a RenderWare matrix, stored through out (other units call it by its address,
 * declared as returning the matrix, which passes out the same way; Game/GadgetCrash3D.cpp declares it
 * like this). It calls SetRows, so it is emitted as a weak copy after its first caller. */
extern "C" inline void fn_800635A4(Mtx34f *out, const RwMatrix *matrix) {
    Mtx34f rows;
    SetRows(rows, matrix->right.x, matrix->up.x, matrix->at.x, matrix->pos.x, matrix->right.y, matrix->up.y,
            matrix->at.y, matrix->pos.y, matrix->right.z, matrix->up.z, matrix->at.z, matrix->pos.z);
    fn_8040C148(out, &rows);
}

/* A flag word (see Game/BKTankController.h; struct name unknown, as in
 * Game/WeaponLockonBase.cpp). */
struct UnkFlags_80014E04 {
    UnkFlags_80014E04() {
        fn_80014E04(&m_bits);
    }
    void ClearAll() {
        fn_80014E10(&m_bits);
    }
    void Set(int bit) {
        fn_80014E1C(&m_bits, bit);
    }
    void Clear(int bit) {
        fn_8006127C(&m_bits, bit);
    }
    bool Test(int bit) {
        return fn_80014E34(&m_bits, bit);
    }

    u32 m_bits;
};

namespace Weapon {

/* The model's state. Flag bit 0: hidden; bit 1: blinking. */
class UnkWeaponModelImpl_8006465C {
  public:
    /* A frame of the first part's clump and its last world matrix. */
    struct Node {
        Node() {}
        ~Node() {}

        /* 0x00 */ RwFrame *m_frame;
        /* 0x04 */ Mtx34f m_matrix;
    };

    /* A motion of a part: its number, and the word stored when it is chosen (meaning unknown). */
    struct Motion {
        /* 0x0 */ u32 m_id;
        /* 0x4 */ s32 m_arg;
    };

    /* A part: a clump hung on a frame, with its motions. */
    struct Part {
        Part() : m_attach(0), m_frame(0), m_clump(0), m_updater(0), m_motion(0), m_motionCount(0) {
            m_scale.z = 1.0f;
            m_scale.y = 1.0f;
            m_scale.x = 1.0f;
        }
        ~Part() {}

        /* 0x00 */ RwFrame *m_attach; /* the frame the part hangs on */
        /* 0x04 */ RwFrame *m_frame;  /* the part's own frame */
        /* 0x08 */ RpClump *m_clump;
        /* 0x0C */ UnkClumpUpdater_80015A7C *m_updater;
        /* 0x10 */ _stMtnPlayWork *m_motion;
        /* 0x14 */ Vec3f m_scale;
        /* 0x20 */ s32 m_motionCount;
        /* 0x24 */ s32 m_motionArg;
        /* 0x28 */ boost::scoped_array<Motion> m_motions;
    };

    UnkWeaponModelImpl_8006465C(s32 slot, const boost::weak_ptr<CharInfo> &owner,
                                const UnkWeaponEffectSet_80063428 *desc);
    ~UnkWeaponModelImpl_8006465C();

    /* Places the model by position and rotation, and by a world matrix. */
    void SetTransform(const Vec3f *position, const Vec3f *rotation);
    void SetMatrix(const Mtx34f *matrix);
    /* Runs the blink, the parts and the nodes. */
    void Update(float dt);
    /* Restarts the parts' motions, and chooses motion state of each part. */
    void Restart();
    void SetState(int state);
    /* The matrix of node id (node 0 for an id out of range). */
    const Mtx34f *NodeMatrix(int id);
    /* Ends the blink, and shows and hides the parts. */
    void StopBlink();
    void Hide();
    void Show();
    /* Gives the parts' clumps a fade. */
    void SetFade(UnkClumpFade_80027D5C *fade);
    /* Moves the parts with their frames and motions, advancing their clumps by dt. */
    void UpdateParts(float dt, float speed);
    /* Loads and frees the parts. */
    void LoadParts(s32 slot, const UnkWeaponEffectSet_80063428 *desc);
    void DestroyParts();
    /* Gives the parts' clumps the default fade. */
    void ResetFade();
    /* Sets up the motions of part. */
    void SetupPart(s32 slot, Part *part, const UnkWeaponEffect_80063428 *desc);
    /* Finds the nodes' frames, and keeps their world matrices. */
    void CreateNodes(const UnkWeaponEffectSet_80063428 *desc);
    void UpdateNodes();

    /* 0x00 */ s32 m_partCount;
    /* 0x04 */ s32 m_nodeCount;
    /* 0x08 */ boost::scoped_array<Part> m_parts;
    /* 0x0C */ boost::scoped_array<Node> m_nodes;
    /* 0x10 */ UnkFlags_80014E04 m_flags;
    /* 0x14 */ s32 m_state;
    /* 0x18 */ RwFrame *m_frame;
    /* 0x1C */ boost::weak_ptr<CharInfo> m_owner;
};
SIZE_ASSERT(UnkWeaponModelImpl_8006465C, 0x24);

} // namespace Weapon

/* Deletes the Impl (see checked_delete below). */
inline void DeleteImpl(UnkWeaponModelImpl_8006465C *impl) {
    delete impl;
}

/* Retail keeps boost::checked_delete of the Impl out of line and its destructor at its own place;
 * this specialization gives that order under GC/2.7 (see docs/research-notes.md and
 * Game/EnergyCore.cpp). */
namespace boost {
template <>
inline void checked_delete<UnkWeaponModelImpl_8006465C>(UnkWeaponModelImpl_8006465C *x) {
    DeleteImpl(x);
}
} // namespace boost

typedef UnkWeaponModelImpl_8006465C Impl;

/* The fade every part starts with. */
static UnkClumpFade_80027D5C s_fade;

/* The parts' node callback (defined after Impl::UpdateNodes). */
static void ScaleNode(_stMtnCallbackParam *param);

namespace Weapon {

UnkWeaponModelImpl_8006465C::UnkWeaponModelImpl_8006465C(s32 slot, const boost::weak_ptr<CharInfo> &owner,
                                                         const UnkWeaponEffectSet_80063428 *desc)
    : m_owner(owner) {
    m_flags.ClearAll();
    m_state = 0;
    m_frame = fn_80486D00();
    LoadParts(slot, desc);
    CreateNodes(desc);
}

UnkWeaponModelImpl_8006465C::~UnkWeaponModelImpl_8006465C() {
    DestroyParts();
    fn_80486E2C(m_frame);
}

void UnkWeaponModelImpl_8006465C::SetTransform(const Vec3f *position, const Vec3f *rotation) {
    RwFrame *frame = m_frame;
    Mtx34f matrix;
    Mtx34f translation;
    fn_8040B034(&translation, position);
    fn_8040C148(&matrix, &translation);
    fn_8040B848(&matrix, rotation->y);
    fn_8040B884(&matrix, rotation->x);
    fn_8040B80C(&matrix, rotation->z);
    frame->modelling = fn_800591F0(&matrix);
    fn_8048722C(m_frame);
    UpdateParts(0.0f, 0.0f);
}

void UnkWeaponModelImpl_8006465C::SetMatrix(const Mtx34f *matrix) {
    RwFrame *frame = m_frame;
    frame->modelling = fn_800591F0(matrix);
    fn_8048722C(m_frame);
    UpdateParts(0.0f, 0.0f);
}

void UnkWeaponModelImpl_8006465C::Update(float dt) {
    if (m_flags.Test(1)) {
        if (lbl_805EE1DC || !(lbl_8057D6A0.field_0x88 & 1)) {
            Show();
        } else {
            Hide();
        }
    }
    float speed;
    if (m_owner.expired()) {
        speed = lbl_8057D6A0.m_dt;
    } else {
        speed = reinterpret_cast<UnkCharTime *&>(m_owner.lock()->field_0x60)->m_speed;
    }
    UpdateParts(dt, speed);
    UpdateNodes();
}

void UnkWeaponModelImpl_8006465C::Restart() {
    for (int i = 0; i < m_partCount; i++) {
        _stMtnPlayWork *motion = m_parts[i].m_motion;
        if (motion != 0) {
            fn_80416F28(motion, 0, 1, 0);
        }
    }
}

void UnkWeaponModelImpl_8006465C::SetState(int state) {
    if (m_state != state) {
        m_state = state;
        for (int i = 0; i < m_partCount; i++) {
            Part *part = &m_parts[i];
            if (part->m_motion != 0) {
                fn_80417568(part->m_motion, part->m_motions[m_state].m_id);
                fn_80416F28(part->m_motion, 0, 0xB, 0);
                part->m_motionArg = part->m_motions[m_state].m_arg;
                if (m_state == 0) {
                    fn_80416F28(part->m_motion, 0, 1, 0);
                }
            }
        }
    } else {
        for (int i = 0; i < m_partCount; i++) {
            Part *part = &m_parts[i];
            if (part->m_motion != 0) {
                UnkMotionRange_804125F4 range;
                if (fn_804125F4(&range, part->m_motions[m_state].m_id)) {
                    fn_80416E20(part->m_motion, 0, range.m_end);
                }
            }
        }
    }
}

const Mtx34f *UnkWeaponModelImpl_8006465C::NodeMatrix(int id) {
    if (id < m_nodeCount) {
        return &m_nodes[id].m_matrix;
    }
    return &m_nodes[0].m_matrix;
}

void UnkWeaponModelImpl_8006465C::StopBlink() {
    m_flags.Clear(1);
    if (m_flags.Test(0)) {
        Show();
    }
}

void UnkWeaponModelImpl_8006465C::Hide() {
    if (!m_flags.Test(0)) {
        for (int i = 0; i < m_partCount; i++) {
            RpClump *clump = m_parts[i].m_clump;
            if (clump != 0) {
                fn_80038FC0(clump);
            }
        }
        m_flags.Set(0);
    }
}

void UnkWeaponModelImpl_8006465C::Show() {
    if (m_flags.Test(0)) {
        for (int i = 0; i < m_partCount; i++) {
            RpClump *clump = m_parts[i].m_clump;
            if (clump != 0) {
                fn_80038F94(clump);
            }
        }
        m_flags.Clear(0);
    }
}

void UnkWeaponModelImpl_8006465C::SetFade(UnkClumpFade_80027D5C *fade) {
    for (int i = 0; i < m_partCount; i++) {
        RpClump *clump = m_parts[i].m_clump;
        if (clump != 0) {
            fn_80038198(clump, fade);
        }
    }
}

void UnkWeaponModelImpl_8006465C::UpdateParts(float dt, float speed) {
    for (int i = 0; i < m_partCount; i++) {
        Part *part = &m_parts[i];
        if (part->m_clump != 0) {
            if (part->m_motion != 0) {
                RwMatrix *ltm = RwFrameGetLTM(part->m_attach);
                fn_804172D8(part->m_motion, &(const Mtx34f &)fn_80016414(ltm));
                fn_80416DC8(part->m_motion, 0, speed);
                fn_80415EA4(part->m_motion);
                if (part->m_updater != 0) {
                    fn_800159AC(part->m_updater);
                }
            } else if (part->m_frame != 0) {
                RwMatrix *modelling = &part->m_frame->modelling;
                *modelling = *RwFrameGetLTM(part->m_attach);
                if (part->m_scale.x != 1.0f || part->m_scale.y != 1.0f || part->m_scale.z != 1.0f) {
                    RwV3d scale;
                    scale.x = part->m_scale.x;
                    scale.y = part->m_scale.y;
                    scale.z = part->m_scale.z;
                    fn_8047F398(modelling, &scale, 1);
                }
                fn_8048722C(part->m_frame);
            }
            if (dt != 0.0f) {
                fn_80039B94(part->m_clump, dt, -1.0f);
            }
        }
    }
}

void UnkWeaponModelImpl_8006465C::LoadParts(s32 slot, const UnkWeaponEffectSet_80063428 *desc) {
    m_partCount = desc->m_effectCount;
    m_parts.reset(new Part[m_partCount]);
    void *manager = fn_800122D4();
    UnkStage_8003A840 *stage = fn_8003A840();
    for (int i = 0; i < desc->m_effectCount; i++) {
        const UnkWeaponEffect_80063428 *partDesc = &desc->m_effects[i];
        Part *part = &m_parts[i];
        RpClump *clump = fn_8001277C(manager, slot, partDesc->m_resource);
        if (clump != 0) {
            part->m_clump = fn_80457090(clump);
            int area = fn_80169350(fn_80169364(part->m_clump));
            fn_80038C84(part->m_clump);
            RpClump *cloned = part->m_clump;
            SetClumpWorld(cloned, stage->GetWorld(area));
            if (partDesc->field_0x0 >= 0) {
                part->m_attach = fn_80038D24(m_parts[0].m_clump, partDesc->field_0x0);
            }
            if (part->m_attach == 0) {
                part->m_attach = m_frame;
            }
            if (partDesc->field_0x4 >= 0) {
                part->m_frame = fn_80038D24(part->m_clump, partDesc->field_0x4);
            }
            if (part->m_frame == 0) {
                part->m_frame = fn_804872D8(part->m_clump->frame);
            }
        }
        SetupPart(slot, part, partDesc);
    }
    ResetFade();
}

void UnkWeaponModelImpl_8006465C::DestroyParts() {
    for (int i = 0; i < m_partCount; i++) {
        Part *part = &m_parts[i];
        if (part->m_updater != 0) {
            delete part->m_updater;
        }
        if (part->m_motion != 0) {
            fn_8041B884(part->m_motion);
        }
        part->m_updater = 0;
        part->m_motion = 0;
        if (part->m_clump != 0) {
            void *world = fn_8046256C(part->m_clump);
            if (world != 0) {
                fn_804623EC(world, part->m_clump);
            }
            fn_80457468(part->m_clump);
        }
    }
}

void UnkWeaponModelImpl_8006465C::ResetFade() {
    SetFade(&s_fade);
}

void UnkWeaponModelImpl_8006465C::SetupPart(s32 slot, Part *part, const UnkWeaponEffect_80063428 *desc) {
    part->m_motionCount = desc->m_subCount;
    part->m_motions.reset(new Motion[part->m_motionCount]);
    if (part->m_motionCount > 0) {
        void *manager = fn_800122D4();
        for (int i = 0; i < part->m_motionCount; i++) {
            const UnkWeaponEffectSub_80063428 *motion = &desc->m_subs[i];
            if (motion->m_resource < 0) {
                part->m_motions[i].m_id = 0xFFFF;
            } else {
                part->m_motions[i].m_id = fn_80012608(manager, slot, motion->m_resource);
                part->m_motions[i].m_id += motion->field_0x4;
            }
            part->m_motions[i].m_arg = motion->field_0x8;
        }
        if (desc->field_0xC >= 0) {
            part->m_motion = fn_8041B908(fn_80012684(manager, slot, desc->field_0xC), 1);
            part->m_updater = new UnkClumpUpdater_80015A7C(part->m_clump, part->m_motion);
            if (desc->field_0x4 >= 0) {
                fn_80415808(part->m_motion, desc->field_0x4, ScaleNode, part);
            }
        }
        m_state = -1;
        SetState(0);
    }
}

void UnkWeaponModelImpl_8006465C::CreateNodes(const UnkWeaponEffectSet_80063428 *desc) {
    m_nodeCount = desc->m_soundCount;
    m_nodes.reset(new Node[m_nodeCount]);
    for (int i = 0; i < m_nodeCount; i++) {
        m_nodes[i].m_frame = fn_80038D24(m_parts[0].m_clump, desc->m_sounds[i]);
    }
}

void UnkWeaponModelImpl_8006465C::UpdateNodes() {
    for (int i = 0; i < m_nodeCount; i++) {
        AssignMatrix(m_nodes[i].m_matrix, fn_802C8068(m_nodes[i].m_frame));
    }
}

} // namespace Weapon

/* The parts' node callback: puts the part's frame on the frame it hangs on, scaled by the part's
 * scale. */
static void ScaleNode(_stMtnCallbackParam *param) {
    Impl::Part *part = static_cast<Impl::Part *>(param->m_data);
    Mtx34f matrix;
    RwMatrix *ltm = RwFrameGetLTM(part->m_attach);
    Mtx34f frameMatrix;
    fn_800635A4(&frameMatrix, ltm);
    fn_8040C148(&matrix, &frameMatrix);
    if (part->m_scale.x != 1.0f || part->m_scale.y != 1.0f || part->m_scale.z != 1.0f) {
        Mtx34f scale;
        fn_8040B07C(&scale, &part->m_scale);
        fn_8040BAC4(&matrix, &scale);
    }
    fn_800163F4(param->m_frame, &matrix);
}

namespace Weapon {

UnkWeaponModel_80063428::UnkWeaponModel_80063428(void *resource, const UnkWeaponEffectSet_80063428 *effects)
    : m_impl(new Impl((s32)resource, boost::weak_ptr<CharInfo>(), effects)) {}

UnkWeaponModel_80063428::UnkWeaponModel_80063428(const boost::weak_ptr<CharInfo> &owner,
                                                 const UnkWeaponEffectSet_80063428 *effects)
    : m_impl(new Impl(2, owner, effects)) {}

UnkWeaponModel_80063428::UnkWeaponModel_80063428(const boost::weak_ptr<CharInfo> &owner, int arg,
                                                 const UnkWeaponEffectSet_80063428 *effects)
    : m_impl(new Impl(arg, owner, effects)) {}

UnkWeaponModel_80063428::~UnkWeaponModel_80063428() {}

void UnkWeaponModel_80063428::fn_800632E8(const Vec3f *position, const Vec3f *rotation) {
    m_impl->SetTransform(position, rotation);
}

void UnkWeaponModel_80063428::fn_800632C4(const Mtx34f *matrix) {
    m_impl->SetMatrix(matrix);
}

void UnkWeaponModel_80063428::fn_800632A0(float dt) {
    m_impl->Update(dt);
}

void UnkWeaponModel_80063428::fn_8006327C() {
    m_impl->Restart();
}

void UnkWeaponModel_80063428::fn_80063258(int state) {
    m_impl->SetState(state);
}

const Mtx34f *UnkWeaponModel_80063428::fn_80063234(int id) {
    return m_impl->NodeMatrix(id);
}

} // namespace Weapon

/* Sets the scale of part (Game/WeaponSpecialVacuumEgg.cpp scales its egg's part 1 with it). */
extern "C" void fn_800631FC(UnkWeaponModel_80063428 *model, int part, const Vec3f *scale) {
    fn_800091D0(&model->m_impl->m_parts[part].m_scale, scale);
}

/* The Impl (an inline function in the original: it calls the inline scoped_ptr::get, so it is
 * emitted as a weak copy after its first caller). */
inline Impl *GetImpl(UnkWeaponModel_80063428 *model) {
    return model->m_impl.get();
}

namespace Weapon {

void UnkWeaponModel_80063428::fn_800631D0() {
    UnkWeaponModelImpl_8006465C *impl = GetImpl(this);
    impl->m_flags.Set(1);
}

} // namespace Weapon

/* Ends the blink, and whether the model is blinking (other units call them by their addresses). */
extern "C" void fn_800631AC(UnkWeaponModel_80063428 *model) {
    model->m_impl->StopBlink();
}

extern "C" bool fn_80063178(UnkWeaponModel_80063428 *model) {
    UnkWeaponModelImpl_8006465C *impl = GetImpl(model);
    return impl->m_flags.Test(1);
}

namespace Weapon {

void UnkWeaponModel_80063428::fn_80063154() {
    m_impl->Hide();
}

void UnkWeaponModel_80063428::fn_80063130() {
    m_impl->Show();
}

void UnkWeaponModel_80063428::fn_8006310C(void *arg) {
    m_impl->SetFade(static_cast<UnkClumpFade_80027D5C *>(arg));
}

} // namespace Weapon
