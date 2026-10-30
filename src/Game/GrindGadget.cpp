/* GrindGadget (class name from RTTI): the GRINDGADGET gadget, a grind object (GRINDGADGET.BIN)
 * that takes a rail path out of the stage's grind path file (A10000GRINDOBJ_GR.PTB, or
 * A20401GRINDOBJ_GR.PTB on stage 13) and places it at its own position the first frame after it
 * is placed or moved. File name inferred from the class. Method names marked "(name inferred)" are
 * not original. Definitions are in reverse address order (deferred inlining emits functions
 * last-defined first).
 *
 * Not yet matching (9 of 10 functions and every data section match): the load hook fn_80228348.
 * Retail keeps the chosen path file in r0 and loads the stage number into r3; GC/2.7 swaps the
 * two (r4 and r0) for every form tried (ternary either way round, if/else, a local, a switch, an
 * inline helper, a pointer to the game state), which changes 7 instructions. */
#include "Game/Gadget.h"
#include "Game/GadgetCharInfo.h"
#include "Game/GadgetClassDesc.h"
#include "Game/GadgetModelInfo.h"
#include "Game/GadgetUtil.h"
#include "Game/FileQueue.h"
#include "Game/MathTypes.h"
#include "Game/PathCtrl.h"
#include "boost/shared_ptr.hpp"
#include "boost/weak_ptr.hpp"
#include "size_assert.h"

using SonicteamUSA::System::PathCtrl;

class CharInfo;
class UnkPathManager_8001B494;
struct UnkVecPair_8001C4B8;

/* The game's state record (struct name unknown; only the field this unit reads). */
struct UnkGameState_8057D6A0 {
    /* 0x00 */ u8 field_0x0[0xAC];
    /* 0xAC */ s32 m_stage; /* (name inferred) */
};
extern UnkGameState_8057D6A0 lbl_8057D6A0;

extern "C" {
/* A 32-bit flag word: fn_8000AAEC constructs one from a value, fn_80014E1C sets bit, fn_8006127C
 * clears bit and fn_80014E34 tests bit (see ArkCannon.cpp). */
u32 *fn_8000AAEC(u32 *flags, u32 value);
void fn_80014E1C(u32 *flags, int bit);
void fn_8006127C(u32 *flags, int bit);
bool fn_80014E34(u32 *flags, int bit);

/* The model loader and a file's model table (see Coaster.cpp). */
void *fn_800122D4(void);
void *fn_80012700(void *loader, int arg1, const char *fileName);

/* The path manager (see EnemyPath.cpp; behaviours inferred): whether a path is set, take a path
 * from a model table and place it, and give it back. */
UnkPathManager_8001B494 *fn_8001B494(void);
bool fn_800ABDCC(PathCtrl *path);
void fn_8001DB08(UnkPathManager_8001B494 *manager, void *model, Vec3f *position, Vec3f rotation,
                 int arg4, PathCtrl *path, const UnkVecPair_8001C4B8 *axes);
void fn_8001E2DC(UnkPathManager_8001B494 *manager, PathCtrl *path);
/* The axes a path is placed with by default (struct name unknown). */
extern const UnkVecPair_8001C4B8 lbl_80570B50;

/* Model helpers (see ArkCannon.cpp). */
boost::shared_ptr<GadgetModelInfo> fn_8015F800(int arg0, const char *fileName);
bool fn_80161FC8(GadgetModelInfo *model, int variant, const boost::weak_ptr<CharInfo> &info);
}

/* The flag word at 0x44 (struct name unknown; bit 0: the path must be placed again). */
struct GrindGadgetFlags {
    GrindGadgetFlags(u32 value) {
        fn_8000AAEC(&m_bits, value);
    }

    u32 m_bits;
};

class GrindGadget : public Gadget {
  public:
    GrindGadget(Task *parent, UnkPlacement_800C9CD0 *owner);
    virtual ~GrindGadget();
    virtual void Behave(float dt);
    virtual void ReloadPlacement(UnkPlacement_800C9CD0 *owner);

    /* Creates the gadget under task layer 11 (name inferred). */
    static void Instantiate(UnkPlacement_800C9CD0 *owner);

    /* Copies the owner's position and rotation (name inferred). */
    void LoadPlacement(UnkPlacement_800C9CD0 *owner);
    /* Places the model at the position and rotation (name inferred). */
    void RebuildTransform();

    static const char *s_taskName;

    /* 0x30 */ boost::shared_ptr<GadgetCharInfo> m_chara;
    /* 0x38 */ boost::shared_ptr<GadgetModelInfo> m_model;
    /* 0x40 */ s32 field_0x40;
    /* 0x44 */ GrindGadgetFlags m_flags;
    /* 0x48 */ PathCtrl m_path;
};
SIZE_ASSERT(GrindGadget, 0xA0);

/* The stage's grind path files. */
const char *const lbl_805F6F18 = "A10000GRINDOBJ_GR.PTB";
const char *const lbl_805F6F1C = "A20401GRINDOBJ_GR.PTB";

/* The grind path file of this stage (set by the load hook). */
static const char *const *s_pathFile = &lbl_805F6F18;

const char *GrindGadget::s_taskName = "GrindGadget";

GrindGadget::GrindGadget(Task *parent, UnkPlacement_800C9CD0 *owner)
    : Gadget(parent, owner), field_0x40(1), m_flags(0) {
    m_name = s_taskName;
    m_chara = fn_8015CD90(0x63, 4);
    LoadPlacement(owner);
    m_model = fn_8015F800(1, "GRINDGADGET.BIN");
    m_model->fn_80162A68(0, 0);
    fn_80161FC8(m_model.get(), 0, boost::weak_ptr<CharInfo>(m_chara));
    RebuildTransform();
    fn_80014E1C(&m_flags.m_bits, 0);
}

GrindGadget::~GrindGadget() {
    if (fn_800ABDCC(&m_path)) {
        fn_8001E2DC(fn_8001B494(), &m_path);
    }
}

extern "C" void fn_80228348(void) {
    s_pathFile = (lbl_8057D6A0.m_stage == 13) ? &lbl_805F6F1C : &lbl_805F6F18;
    fn_8006B474("GRINDGADGET.BIN", 1);
    fn_8006B474(*s_pathFile, 0);
}

extern "C" void fn_80228344(void) {}

void GrindGadget::Instantiate(UnkPlacement_800C9CD0 *owner) {
    new GrindGadget(fn_801EB304(fn_8001C4F8(), 11), owner);
}

void GrindGadget::Behave(float dt) {
    if (fn_80014E34(&m_flags.m_bits, 0)) {
        if (fn_800ABDCC(&m_path)) {
            fn_8001E2DC(fn_8001B494(), &m_path);
        }
        void *table = fn_80012700(fn_800122D4(), 1, *s_pathFile);
        fn_8001DB08(fn_8001B494(), table, &m_chara->m_position, m_chara->m_rotation,
                    m_chara->field_0x30, &m_path, &lbl_80570B50);
        fn_8006127C(&m_flags.m_bits, 0);
    }
    m_model->fn_80162FB0(dt);
}

void GrindGadget::ReloadPlacement(UnkPlacement_800C9CD0 *owner) {
    LoadPlacement(owner);
    RebuildTransform();
    m_model->fn_80162EF4();
    m_model->fn_8016108C();
    fn_80014E1C(&m_flags.m_bits, 0);
}

void GrindGadget::LoadPlacement(UnkPlacement_800C9CD0 *owner) {
    fn_800C9F14(owner, &m_chara->m_position);
    fn_800C9ED4(owner, &m_chara->m_rotation);
}

void GrindGadget::RebuildTransform() {
    Mtx34f matrix;
    fn_8040C148(&matrix, &lbl_805E0720);
    fn_8040B59C(&matrix, &m_chara->m_position);
    fn_8040B848(&matrix, m_chara->m_rotation.y);
    fn_8040B884(&matrix, m_chara->m_rotation.x);
    fn_8040B80C(&matrix, m_chara->m_rotation.z);
    m_model->fn_801633DC(&matrix);
}

GadgetClassDesc lbl_80546700 = {
    "GRINDGADGET", fn_80228348, fn_80228344, GrindGadget::Instantiate, 0, 0, 0x138D, 1, 0, 10, 0,
};
