/* GravityChangeCollision (class name from RTTI; its s_taskName is "GravityChangeCollision"): the
 * gadget type of the same name, an invisible box ("SIZE X/Y/Z m", in metres) that makes its
 * "GRAVITY" direction the stage's current one when a character enters it, and then ends. File name
 * from the class. Names marked "(name inferred)" are not original. Definitions are in reverse
 * address order (deferred inlining emits functions last-defined first). */
#include "Game/CharInfo.h"
#include "Game/CharaColli.h"
#include "Game/Gadget.h"
#include "Game/GadgetCharInfo.h"
#include "Game/GadgetClassDesc.h"
#include "Game/GadgetCollision.h"
#include "Game/GadgetUtil.h"
#include "boost/shared_ptr.hpp"
#include "boost/weak_ptr.hpp"
#include "size_assert.h"

using SonicteamUSA::System::CharaColli::CharaColliBase;

extern const Vec3f lbl_805E07B8;

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

extern "C" {
/* A 32-bit flag word: fn_8000AAEC constructs one from a value, fn_80014E1C sets bit, fn_8006127C
 * clears bit and fn_80014E34 tests bit (see ArkCannon.cpp). */
u32 *fn_8000AAEC(u32 *flags, u32 value);
void fn_80014E1C(u32 *flags, int bit);
void fn_8006127C(u32 *flags, int bit);
bool fn_80014E34(u32 *flags, int bit);

/* The stage's gravity (behaviour inferred; see GravityChangeSwitch.cpp). */
int fn_80293AC8(void);
const Vec3f *fn_80293B60(int gravity);
void fn_80293928(int gravity);
void fn_80293AE4(void);
void fn_80293B1C(void);

/* Whether a collision's contacts include a character (behaviour inferred; see Coaster.cpp). */
bool fn_8015D2EC(UnkColliContacts_8001EE04 *contacts);
/* Marks the owner record as used up (behaviour inferred; see Coaster.cpp). */
void fn_800C9AD4(UnkPlacement_800C9CD0 *owner);

/* The collider manager and a box collider for a character info (see Door.cpp). */
void *fn_80021B08(void);
boost::shared_ptr<CharaColliBase> fn_800C7C34(void *manager, const boost::weak_ptr<CharInfo> &charInfo,
                                              const Vec3f *size, int flags, int arg4, int arg5,
                                              const Vec3f *offset, const Vec3f *rotation, int arg8);

/* a + b by value, and the debug line draw call (see Meteor.cpp). */
Vec3f fn_8040CDD0(const Vec3f *a, const Vec3f *b);
void fn_802E0AA0(UnkDebugLine_80020A20 *segment, const UnkColor_8040D2D8 &color, float width);
}

/* The contacts of a collider (the object holds them 0xC bytes in; name inferred; as in
 * ChaoHolder.cpp). */
inline UnkColliContacts_8001EE04 *GetContacts(CharaColliBase *collider) {
    return reinterpret_cast<UnkColliContacts_8001EE04 *>(reinterpret_cast<u8 *>(collider) + 0xC);
}

/* a + b (name inferred; as in ArkLaser.cpp). */
inline Vec3f AddVectors(const Vec3f &a, const Vec3f &b) {
    return fn_8040CDD0(&a, &b);
}

/* The flag word at 0x50 (struct name unknown). */
struct GravityChangeCollisionFlags {
    GravityChangeCollisionFlags(u32 value) {
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

    u32 m_bits; /* bit 0: entered */
};

/* The parameters of the owner record (names from the parameter table). The box's size is read in
 * metres and kept as half extents in world units. */
struct GravityChangeCollisionParams {
    GravityChangeCollisionParams() : x(10.0f), y(10.0f), z(10.0f), gravity(0) {}

    /* 0x0 */ float x;
    /* 0x4 */ float y;
    /* 0x8 */ float z;
    /* 0xC */ s32 gravity;
};
SIZE_ASSERT(GravityChangeCollisionParams, 0x10);

class GravityChangeCollision : public Gadget {
  public:
    GravityChangeCollision(Task *parent, UnkPlacement_800C9CD0 *owner);
    virtual ~GravityChangeCollision();
    virtual void Behave(float dt);
    virtual void ReloadPlacement(UnkPlacement_800C9CD0 *owner);

    /* Creates the gadget under task layer 11 (name inferred). */
    static void Instantiate(UnkPlacement_800C9CD0 *owner);

    /* Copies the owner's position, rotation and parameters (name inferred). */
    void ReadPlacement(UnkPlacement_800C9CD0 *owner);

    static const char *s_taskName;

    /* Member names are inferred. */
    /* 0x30 */ GravityChangeCollisionParams m_params;
    /* 0x40 */ boost::shared_ptr<GadgetCharInfo> m_chara;
    /* 0x48 */ boost::shared_ptr<CharaColliBase> m_collider;
    /* 0x50 */ GravityChangeCollisionFlags m_touch;
    /* 0x54 */ u32 pad_0x54; /* the class size is a multiple of 8 (as in JumpBoard.cpp) */
};
SIZE_ASSERT(GravityChangeCollision, 0x58);

const char *GravityChangeCollision::s_taskName = "GravityChangeCollision";

GravityChangeCollision::GravityChangeCollision(Task *parent, UnkPlacement_800C9CD0 *owner)
    : Gadget(parent, owner), m_touch(0) {
    m_name = s_taskName;
    m_chara = fn_8015CD90(0x68, 4);
    ReadPlacement(owner);
    Vec3f size(m_params.x, m_params.y, m_params.z);
    m_collider = fn_800C7C34(fn_80021B08(), m_chara, &size, 0x140100, 7, 0, &lbl_805E07A0,
                             &lbl_805E07B8, 0);
}

GravityChangeCollision::~GravityChangeCollision() {}

extern "C" void fn_80247914(void) {
    fn_80293B1C();
}

extern "C" void fn_802478F4(void) {
    fn_80293AE4();
}

void GravityChangeCollision::Instantiate(UnkPlacement_800C9CD0 *owner) {
    new GravityChangeCollision(fn_801EB304(fn_8001C4F8(), 11), owner);
}

void GravityChangeCollision::Behave(float dt) {
    if (fn_8015D2EC(GetContacts(m_collider.get()))) {
        m_touch.Set(0);
    }
    if (m_touch.Test(0)) {
        if (m_params.gravity != fn_80293AC8()) {
            fn_80293928(m_params.gravity);
        }
        m_touch.Clear(0);
        fn_800C9AD4(m_owner);
        m_flags |= 1;
    }
}

void GravityChangeCollision::ReloadPlacement(UnkPlacement_800C9CD0 *owner) {
    ReadPlacement(owner);
    if (m_collider) {
        m_collider->SetSize(m_params.x, m_params.y, m_params.z, -2);
        m_collider->Reset();
    }
    Vec3f direction;
    fn_800091EC(&direction, fn_80293B60(m_params.gravity));
    fn_8040CE38(&direction, -50.0f);
    UnkDebugLine_80020A20 segment;
    fn_80020A20(&segment, &m_chara->m_position, &(const Vec3f &)AddVectors(m_chara->m_position, direction));
    fn_802E0AA0(&segment, UnkColor_8040D2D8(0xC0, 0xFF, 0xFF, 0x00), 1.0f);
}

void GravityChangeCollision::ReadPlacement(UnkPlacement_800C9CD0 *owner) {
    fn_800C9F14(owner, &m_chara->m_position);
    fn_800C9ED4(owner, &m_chara->m_rotation);
    fn_800C9DBC(owner, &m_params, sizeof(m_params));
    m_params.x = 0.5f * (10.0f * m_params.x);
    m_params.y = 0.5f * (10.0f * m_params.y);
    m_params.z = 0.5f * (10.0f * m_params.z);
}

static GadgetParamDesc lbl_80548968[] = {
    {4, "SIZE X m", 0, 0, 0, 10.0f, 0.0f, 65536.0f, fn_800CBC20, 0},
    {4, "SIZE Y m", 0, 0, 0, 10.0f, 0.0f, 65536.0f, fn_800CBC20, 0},
    {4, "SIZE Z m", 0, 0, 0, 10.0f, 0.0f, 65536.0f, fn_800CBC20, 0},
    {1, "GRAVITY", 0, 0, 5, 0.0f, 0.0f, 0.0f, fn_800CBC20, 0},
    {0},
};

GadgetClassDesc lbl_80548A30 = {
    "GravityChangeCollision", fn_80247914, fn_802478F4, GravityChangeCollision::Instantiate, 0, 0, 0x14B6,
    1, 0, 10, lbl_80548968,
};
