/* GravitySignBoard (class name from RTTI; its s_taskName is "GravitySignBoard"): the gadget type
 * of the same name, a sign (GRAVITYSIGNBOARD.BIN) that stays upright with the stage's current
 * gravity. When the gravity changes it turns from its old attitude to the new one over half a
 * second. File name from the class. Names marked "(name inferred)" are not original. Definitions
 * are in reverse address order (deferred inlining emits functions last-defined first). */
#include "Game/CharInfo.h"
#include "Game/Gadget.h"
#include "Game/GadgetCharInfo.h"
#include "Game/GadgetClassDesc.h"
#include "Game/GadgetModelInfo.h"
#include "Game/GadgetSound.h"
#include "Game/GadgetUtil.h"
#include "Game/FileQueue.h"
#include "Game/MathTypes.h"
#include "boost/shared_ptr.hpp"
#include "boost/weak_ptr.hpp"
#include "size_assert.h"

/* Four colour bytes built by an out-of-line constructor (see ElecBarrier.cpp). */
struct UnkColor_8040D2D8 {
    UnkColor_8040D2D8(u8 r, u8 g, u8 b, u8 a);

    /* 0x0 */ u8 r;
    /* 0x1 */ u8 g;
    /* 0x2 */ u8 b;
    /* 0x3 */ u8 a;
};
SIZE_ASSERT(UnkColor_8040D2D8, 0x4);

/* A debug line segment set up by fn_80020A20 (0x18 bytes; contents unknown). */
struct UnkDebugLine_80020A20 {
    u8 field_0x0[0x18];
};

/* The world's up direction, (0, 1, 0) (defined in GravityDirection.cpp). */
extern Vec3f lbl_80584360;

extern "C" {
/* A 32-bit flag word: fn_8000AAEC constructs one from a value, fn_80014E1C sets bit, fn_8006127C
 * clears bit and fn_80014E34 tests bit (see ArkCannon.cpp). */
u32 *fn_8000AAEC(u32 *flags, u32 value);
void fn_80014E1C(u32 *flags, int bit);
void fn_8006127C(u32 *flags, int bit);
bool fn_80014E34(u32 *flags, int bit);

/* The stage's gravity (behaviour inferred; see GravityChangeSwitch.cpp): the current gravity, its
 * direction for a gravity number, and the set-up and clean-up the type's hooks run. */
int fn_80293AC8(void);
const Vec3f *fn_80293B60(int gravity);
void fn_80293AE4(void);
void fn_80293B1C(void);

/* Whether two vectors are equal (see CameraAdjust.cpp). */
bool fn_8000AD08(const Vec3f *a, const Vec3f *b);
/* Vector cross product, the angle between two vectors and the rotation about an axis (see
 * AdjustCosRot.cpp). */
Vec3f fn_8040CFB8(const Vec3f *a, const Vec3f *b);
float fn_8040F8FC(const Vec3f *a, const Vec3f *b);
Mtx34f fn_8040FD8C(const Vec3f *axis, float angle);
/* Matrix product, the rotation angles of a matrix, and the blend of two matrices (see
 * ArkCannon.cpp and BlackBullCtrlUnit.cpp). */
Mtx34f fn_8001E044(const Mtx34f *a, const Mtx34f *b);
void fn_800494F8(Vec3f *rotation, const Mtx34f *matrix);
Mtx34f fn_8040EFA4(const Mtx34f *a, const Mtx34f *b, float t);

/* GadgetModelInfo helpers (behaviour inferred; see GravityChangeSwitch.cpp). */
boost::shared_ptr<GadgetModelInfo> fn_8015F800(int arg0, const char *fileName);
bool fn_80161FC8(GadgetModelInfo *model, int variant, const boost::weak_ptr<CharInfo> &info);

/* Starts sound soundId at a character info for owner (see ArkLaser.cpp). */
UnkSoundHandle_801D3DC8 fn_801D3E58(int soundId, GadgetCharInfo *charInfo, void *owner);

/* a + b by value, and the debug line draw call (see Meteor.cpp). */
Vec3f fn_8040CDD0(const Vec3f *a, const Vec3f *b);
void fn_802E0AA0(UnkDebugLine_80020A20 *segment, const UnkColor_8040D2D8 &color, float width);
}

/* a + b (name inferred; as in ArkLaser.cpp). */
inline Vec3f AddVectors(const Vec3f &a, const Vec3f &b) {
    return fn_8040CDD0(&a, &b);
}

/* The flag word at 0x44 (struct name unknown). */
struct GravitySignBoardFlags {
    GravitySignBoardFlags(u32 value) {
        fn_8000AAEC(&m_bits, value);
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

    u32 m_bits; /* bit 0: the gravity has changed */
};

class GravitySignBoard : public Gadget {
  public:
    GravitySignBoard(Task *parent, UnkPlacement_800C9CD0 *owner);
    virtual ~GravitySignBoard();
    virtual void Behave(float dt);
    virtual void ReloadPlacement(UnkPlacement_800C9CD0 *owner);

    /* Creates the gadget under task layer 11 (name inferred). */
    static void Instantiate(UnkPlacement_800C9CD0 *owner);

    /* Method names are inferred. */
    /* Copies the owner's position and rotation and takes the current gravity's attitude. */
    void ReadPlacement(UnkPlacement_800C9CD0 *owner);
    /* Places the model with the gravity attitude applied to the owner's rotation. */
    void RebuildTransform();

    static const char *s_taskName;

    /* Member names are inferred. */
    /* 0x30 */ boost::shared_ptr<GadgetCharInfo> m_chara;
    /* 0x38 */ boost::shared_ptr<GadgetModelInfo> m_model;
    /* 0x40 */ s32 m_state; /* 1 while turning */
    /* 0x44 */ GravitySignBoardFlags m_flags;
    /* 0x48 */ s32 m_gravity;
    /* 0x4C */ UnkSoundHandle_801D3DC8 m_sound;
    /* 0x68 */ float m_timer;
    /* 0x6C */ Mtx34f m_attitude;
    /* 0x9C */ Mtx34f m_from;
    /* 0xCC */ Mtx34f m_to;
    /* 0xFC */ u8 field_0xFC[4]; /* unused */
};
SIZE_ASSERT(GravitySignBoard, 0x100);

/* The rotation that turns the world's up direction into up (name inferred). */
void GetGravityAttitude(Mtx34f *attitude, const Vec3f *up) {
    if (!fn_8000AD08(up, &lbl_80584360)) {
        Vec3f axis;
        fn_800091EC(&axis, &(const Vec3f &)fn_8040CFB8(&lbl_80584360, up));
        float angle = fn_8040F8FC(&lbl_80584360, up);
        fn_8040C148(attitude, &(const Mtx34f &)fn_8040FD8C(&axis, angle));
    } else {
        fn_8040C148(attitude, &lbl_805E0720);
    }
}

const char *GravitySignBoard::s_taskName = "GravitySignBoard";

GravitySignBoard::GravitySignBoard(Task *parent, UnkPlacement_800C9CD0 *owner)
    : Gadget(parent, owner), m_flags(0), m_timer(0.0f) {
    m_name = s_taskName;
    m_chara = fn_8015CD90(0x69, 4);
    ReadPlacement(owner);
    m_model = fn_8015F800(1, "GRAVITYSIGNBOARD.BIN");
    m_model->fn_80162A68(0, 0);
    fn_80161FC8(m_model.get(), 0, m_chara);
    RebuildTransform();
    m_sound = fn_801D3E58(0x7747, m_chara.get(), this);
}

GravitySignBoard::~GravitySignBoard() {
    m_sound.fn_801D455C();
}

extern "C" void fn_802590F0(void) {
    fn_80293B1C();
    fn_8006B474("GRAVITYSIGNBOARD.BIN", 1);
}

extern "C" void fn_802590D0(void) {
    fn_80293AE4();
}

void GravitySignBoard::Instantiate(UnkPlacement_800C9CD0 *owner) {
    new GravitySignBoard(fn_801EB304(fn_8001C4F8(), 11), owner);
}

void GravitySignBoard::Behave(float dt) {
    if (m_gravity != fn_80293AC8()) {
        m_flags.Set(0);
    }
    if (m_flags.Test(0)) {
        fn_8040C148(&m_from, &m_attitude);
        m_gravity = fn_80293AC8();
        GetGravityAttitude(&m_to, fn_80293B60(m_gravity));
        m_timer = 0.0f;
        m_state = 1;
        m_flags.Clear(0);
    }
    switch (m_state) {
    case 1:
        m_timer += dt;
        if (m_timer >= 0.5f) {
            GetGravityAttitude(&m_attitude, fn_80293B60(m_gravity));
            m_state = 0;
        } else {
            fn_8040C148(&m_attitude, &(const Mtx34f &)fn_8040EFA4(&m_from, &m_to, m_timer / 0.5f));
        }
        RebuildTransform();
        break;
    }
    m_model->fn_80162FB0(dt);
}

void GravitySignBoard::ReloadPlacement(UnkPlacement_800C9CD0 *owner) {
    ReadPlacement(owner);
    RebuildTransform();
    m_model->fn_80162EF4();
    m_model->fn_8016108C();
    Vec3f direction;
    fn_800091EC(&direction, fn_80293B60(m_gravity));
    fn_8040CE38(&direction, -50.0f);
    UnkDebugLine_80020A20 segment;
    fn_80020A20(&segment, &m_chara->m_position, &(const Vec3f &)AddVectors(m_chara->m_position, direction));
    fn_802E0AA0(&segment, UnkColor_8040D2D8(0xC0, 0xFF, 0xFF, 0x00), 1.0f);
}

void GravitySignBoard::ReadPlacement(UnkPlacement_800C9CD0 *owner) {
    fn_800C9F14(owner, &m_chara->m_position);
    fn_800C9ED4(owner, &m_chara->m_rotation);
    m_state = 0;
    m_gravity = fn_80293AC8();
    GetGravityAttitude(&m_attitude, fn_80293B60(m_gravity));
}

void GravitySignBoard::RebuildTransform() {
    Mtx34f matrix;
    Mtx34f placed;

    fn_8040C148(&matrix, &lbl_805E0720);
    fn_800C9ED4(m_owner, &m_chara->m_rotation);
    fn_8040B848(&matrix, m_chara->m_rotation.y);
    fn_8040B884(&matrix, m_chara->m_rotation.x);
    fn_8040B80C(&matrix, m_chara->m_rotation.z);
    fn_8040C148(&matrix, &(const Mtx34f &)fn_8001E044(&m_attitude, &matrix));
    fn_800494F8(&m_chara->m_rotation, &matrix);
    fn_8040C148(&placed, &lbl_805E0720);
    fn_8040B59C(&placed, &m_chara->m_position);
    fn_8040C148(&placed, &(const Mtx34f &)fn_8001E044(&placed, &matrix));
    m_model->fn_801633DC(&placed);
}

GadgetClassDesc lbl_80549F40 = {
    "GravitySignBoard", fn_802590F0, fn_802590D0, GravitySignBoard::Instantiate, 0, 0, 0x14B7, 1, 0,
    10, 0,
};
