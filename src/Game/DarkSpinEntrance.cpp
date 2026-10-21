/* DarkSpinEntrance (class name from RTTI): the DARKSPINENTRANCE gadget. It creates a collider of
 * the "radius" parameter's size at the owner's position, wraps it in a handle and updates that
 * handle every frame. File name inferred from the class. Method names marked "(name inferred)" are
 * not original. Definitions are in reverse address order (deferred inlining emits functions
 * last-defined first). */
#include "Game/CharInfo.h"
#include "Game/CharaColli.h"
#include "Game/Gadget.h"
#include "Game/GadgetCharInfo.h"
#include "Game/GadgetClassDesc.h"
#include "Game/MathConstants.h"
#include "boost/shared_ptr.hpp"
#include "boost/weak_ptr.hpp"
#include "size_assert.h"

/* The manager returned by fn_80021B08, which creates colliders (behaviour inferred). */
class UnkColliderManager_80021B08;

/* The object at 0x24 of a handle; only the virtual function at vtable offset 0x48 is used here,
 * so the earlier slots are placeholders. */
class UnkHandleBShape_802589F8 {
  public:
    virtual void vf08();
    virtual void vf0C();
    virtual void vf10();
    virtual void vf14();
    virtual void vf18();
    virtual void vf1C();
    virtual void vf20();
    virtual void vf24();
    virtual void vf28();
    virtual void vf2C();
    virtual void vf30();
    virtual void vf34();
    virtual void vf38();
    virtual void vf3C();
    virtual void vf40();
    virtual void vf44();
    /* Resizes the shape (name and meaning of the arguments inferred). */
    virtual void SetSize(int arg0, float radius, float arg2, float height);
};

/* The handle fn_8015B8C4 wraps a collider in (the class name and layout beyond the members used
 * here are unknown). */
class GadgetCharCollision {
  public:
    /* Clears flag bit 1 (name and meaning inferred). */
    void ClearFlag2() {
        m_flags &= ~2;
    }

    /* 0x00 */ u8 field_0x0[0x24];
    /* 0x24 */ UnkHandleBShape_802589F8 *m_shape;
    /* 0x28 */ u32 field_0x28;
    /* 0x2C */ u32 m_flags;
};

extern "C" {
/* Returns the collider manager, constructing it on first use (behaviour inferred). */
UnkColliderManager_80021B08 *fn_80021B08(void);
/* Creates a collider for charInfo with the given flags, offset, rotation and size (behaviour and
 * argument meanings inferred). */
boost::shared_ptr<SonicteamUSA::System::CharaColli::CharaColliBase>
fn_800C7A84(UnkColliderManager_80021B08 *manager, const boost::weak_ptr<CharInfo> &charInfo,
            int flags, int arg3, u16 arg4, const Vec3f *offset,
            const Vec3f *rotation, float height, float radius, int arg9);
/* Wraps collider in a new handle (behaviour inferred). */
boost::shared_ptr<GadgetCharCollision>
fn_8015B8C4(boost::shared_ptr<SonicteamUSA::System::CharaColli::CharaColliBase> collider);
/* Draws the handle's shape for debugging (behaviour inferred). */
void fn_8015BE64(GadgetCharCollision *target);
/* Advances the handle by dt (behaviour inferred). */
void fn_8015BE9C(GadgetCharCollision *target, float dt);
/* destination = matrix * vector (behaviour inferred). */
void fn_8040B67C(Vec3f *destination, const Mtx34f *matrix,
                 const Vec3f *vector);
/* Queues a debug cylinder (behaviour inferred; the meaning of axis is unknown). */
void fn_802E0D6C(const Vec3f *center, const Vec3f *rotation, int axis,
                 float radius, float height);
}

class DarkSpinEntrance : public Gadget {
  public:
    DarkSpinEntrance(Task *parent, UnkPlacement_800C9CD0 *owner);
    virtual ~DarkSpinEntrance();
    virtual void Behave(float dt);
    virtual void fn_800CBC1C();
    virtual void ReloadPlacement(UnkPlacement_800C9CD0 *owner);

    /* Creates the gadget under task layer 11 (name inferred). */
    static void Instantiate(UnkPlacement_800C9CD0 *owner);

    /* Copies the owner's position, rotation and radius into the gadget (name inferred). */
    void ReadPlacement(UnkPlacement_800C9CD0 *owner);
    /* Queues a debug cylinder showing the collider (name inferred). */
    void DrawDebugShape();
    /* Passes the radius to the handle's shape (name inferred). */
    void ApplyRadius();

    static const char *s_taskName;

    /* 0x30 */ boost::shared_ptr<GadgetCharInfo> m_chara;
    /* 0x38 */ boost::shared_ptr<GadgetCharCollision> m_handle;
    /* 0x40 */ float m_radius; /* the "radius" parameter */
    /* 0x44 */ u32 field_0x44;
};
SIZE_ASSERT(DarkSpinEntrance, 0x48);

const char *DarkSpinEntrance::s_taskName = "DarkSpinEntrance";

/* The collider's rotation (radians), a quarter turn about x, built at startup. */
static Vec3f lbl_80584068(0.5f * lbl_805FAD38, 0.0f, 0.0f);

DarkSpinEntrance::DarkSpinEntrance(Task *parent, UnkPlacement_800C9CD0 *owner)
    : Gadget(parent, owner) {
    m_name = s_taskName;
    fn_800C9DBC(owner, &m_radius, sizeof(m_radius));
    /* The original builds the character info twice; this first copy is never used. */
    boost::shared_ptr<GadgetCharInfo> charInfo = fn_8015CB88(0x86, 4, "DarkSpinEntrance");
    m_chara = fn_8015CB88(0x86, 4, "DarkSpinEntrance");
    m_chara->field_0x30 = 2;
    boost::shared_ptr<SonicteamUSA::System::CharaColli::CharaColliBase> collider =
        fn_800C7A84(fn_80021B08(), m_chara, 0x40000, 10, 0, &lbl_805E07A0,
                    &lbl_80584068, 2.5f, m_radius, 0);
    m_handle = fn_8015B8C4(collider);
    m_handle->ClearFlag2();
    ReadPlacement(m_owner);
}

DarkSpinEntrance::~DarkSpinEntrance() {}

void DarkSpinEntrance::Behave(float dt) {
    fn_8015BE9C(m_handle.px, dt);
}

void DarkSpinEntrance::ReloadPlacement(UnkPlacement_800C9CD0 *owner) {
    ReadPlacement(owner);
    ApplyRadius();
}

void DarkSpinEntrance::fn_800CBC1C() {
    DrawDebugShape();
}

void DarkSpinEntrance::ApplyRadius() {
    m_handle->m_shape->SetSize(-2, m_radius, 0.0f, 2.5f);
}

void DarkSpinEntrance::DrawDebugShape() {
    if (m_handle.get() != 0) {
        fn_8015BE64(m_handle.get());

        Mtx34f matrix;
        Mtx34f translation;
        SetTranslationMtx(&translation, m_chara->m_position);
        fn_8040C148(&matrix, &translation);
        fn_8040B848(&matrix, m_chara->m_rotation.y);
        fn_8040B884(&matrix, m_chara->m_rotation.x);
        fn_8040B80C(&matrix, m_chara->m_rotation.z);

        Vec3f center;
        Vec3f transformed;
        Vec3f offset(0.0f, 0.0f, 0.5f * -m_radius);
        fn_8040B67C(&transformed, &matrix, &offset);
        fn_800091EC(&center, &transformed);
        fn_802E0D6C(&center, &m_chara->m_rotation, 2, 0.5f * m_radius, 1.25f);
    }
}

void DarkSpinEntrance::ReadPlacement(UnkPlacement_800C9CD0 *owner) {
    fn_800C9F14(owner, &m_chara->m_position);
    fn_800C9ED4(owner, &m_chara->m_rotation);
    fn_800C9DBC(owner, &m_radius, sizeof(m_radius));
}

extern "C" void fn_802588B4(void) {}

extern "C" void fn_802588B0(void) {}

void DarkSpinEntrance::Instantiate(UnkPlacement_800C9CD0 *owner) {
    new DarkSpinEntrance(fn_801EB304(fn_8001C4F8(), 11), owner);
}

static GadgetParamDesc lbl_80549E88[] = {
    {4, "radius", 0, 0, 0, 10.0f, 0.0f, 65536.0f, fn_800CBC20, 0},
    {0},
};

GadgetClassDesc lbl_80549ED8 = {
    "DARKSPINENTRANCE", fn_802588B4, fn_802588B0, DarkSpinEntrance::Instantiate, 0, 0, 0x61, 1, 0, 10,
    lbl_80549E88,
};
