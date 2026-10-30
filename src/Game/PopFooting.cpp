/* PopFooting::SetPopFooting (class name from RTTI): the POP FOOTING gadget, a footing
 * (POPFOOTING.BIN, variant "model", with variant "model" + 1000 as its base) that waits
 * "sec_begin" seconds sunk 80 below its place, rises at 10 a second, stays up "sec_wait" seconds,
 * sinks again and repeats, with a loop sound while it moves. The footing and its base each have a
 * character info. File name inferred from the namespace. Names marked "(name inferred)" are not
 * original. Definitions are in reverse address order (deferred inlining emits functions
 * last-defined first). */
#include "Game/CharInfo.h"
#include "Game/Gadget.h"
#include "Game/GadgetCharInfo.h"
#include "Game/GadgetClassDesc.h"
#include "Game/GadgetModelInfo.h"
#include "Game/GadgetUtil.h"
#include "Game/FileQueue.h"
#include "Game/MathTypes.h"
#include "boost/shared_ptr.hpp"
#include "boost/weak_ptr.hpp"
#include "size_assert.h"

extern "C" {
/* GadgetModelInfo helpers (see Game/BASwitch.cpp). */
boost::shared_ptr<GadgetModelInfo> fn_8015F800(int arg0, const char *fileName);
bool fn_80161FC8(GadgetModelInfo *model, int variant, const boost::weak_ptr<CharInfo> &info);
/* The matrix of a character info's position and rotation (see Game/BKTank.cpp). */
void fn_8015CEA0(GadgetCharInfo *charInfo, Mtx34f *matrix);

/* The 32-bit flag word (see Game/BaseDoor.cpp). */
void fn_80014E04(u32 *flags);
void fn_80014E10(u32 *flags);
void fn_80014E1C(u32 *flags, int bit);
void fn_8006127C(u32 *flags, int bit);
bool fn_80014E34(u32 *flags, int bit);

/* Plays sound id at a character info as a loop for source (see Game/BaseDoor.cpp). */
void fn_801D53C4(void *manager, int id, GadgetCharInfo *charInfo, void *source, int arg4, int arg5,
                 int arg6);
}

/* fn_8040CDD0 called as returning a + b by value (it stores the result through its first
 * argument; see Game/ArkCannon.cpp). */
typedef Vec3f (*VectorOpFn)(const Vec3f *a, const Vec3f *b);
extern "C" void fn_8040CDD0(Vec3f *out, const Vec3f *a, const Vec3f *b);

/* How far the footing sinks and how fast it moves (the game sets them in __sinit; the inline call
 * reproduces that, as in Game/CameraModeFixLook2.cpp). */
inline float DynamicFloat(float value) {
    return value;
}
static const float s_depth = DynamicFloat(80.0f);
static const float s_speed = DynamicFloat(10.0f);

namespace PopFooting {

/* The parameters of the owner record (names from the parameter table). */
struct SetPopFootingParams {
    SetPopFootingParams() : model(0), secBegin(0.0f), secWait(0.0f) {}

    /* 0x0 */ s32 model;
    /* 0x4 */ float secBegin;
    /* 0x8 */ float secWait;
};

/* The flag word at 0x78 (struct name unknown; bit 0: the footing moves). */
struct SetPopFootingFlags {
    SetPopFootingFlags() {
        fn_80014E04(&m_bits);
    }
    void Clear() {
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

class SetPopFooting : public Gadget {
  public:
    SetPopFooting(Task *parent, UnkPlacement_800C9CD0 *owner);
    virtual ~SetPopFooting();
    virtual void Behave(float dt);
    virtual void ReloadPlacement(UnkPlacement_800C9CD0 *owner);

    /* Creates the gadget under task layer 11 (name inferred). */
    static void Instantiate(UnkPlacement_800C9CD0 *owner);

    /* Method names are inferred. */
    /* Copies the owner's position, rotation and parameters and resets the state. */
    void LoadPlacement(UnkPlacement_800C9CD0 *owner);
    /* Places both models at their character infos. */
    void RebuildTransform();
    /* Moves the footing to its offset below its place. */
    void UpdateFooting();

    static const char *s_fileName;
    static const char *s_taskName;

    /* 0x30 */ boost::shared_ptr<GadgetCharInfo> m_chara;     /* the footing */
    /* 0x38 */ boost::shared_ptr<GadgetCharInfo> m_baseChara; /* its base */
    /* 0x40 */ boost::shared_ptr<GadgetModelInfo> m_model;
    /* 0x48 */ boost::shared_ptr<GadgetModelInfo> m_baseModel; /* variant m_modelIndex + 1000 */
    /* 0x50 */ SetPopFootingParams m_params;
    /* 0x5C */ s32 m_state; /* 0 placed, 1 sunk, 2 rising, 3 up, 4 sinking */
    /* 0x60 */ s32 m_modelIndex;
    /* 0x64 */ float m_timer;
    /* 0x68 */ Vec3f m_position;
    /* 0x74 */ float m_offset;
    /* 0x78 */ SetPopFootingFlags m_flags;
    /* 0x7C */ u32 field_0x7C;
};
SIZE_ASSERT(SetPopFooting, 0x80);

const char *SetPopFooting::s_fileName = "POPFOOTING.BIN";
const char *SetPopFooting::s_taskName = "SetPopFooting";

SetPopFooting::SetPopFooting(Task *parent, UnkPlacement_800C9CD0 *owner) : Gadget(parent, owner) {
    m_name = s_taskName;
    m_chara = fn_8015CD90(0x7A, 4);
    m_baseChara = fn_8015CD90(0x7A, 4);
    LoadPlacement(owner);
    m_modelIndex = m_params.model;
    m_model = fn_8015F800(1, s_fileName);
    m_model->fn_80162A68(m_modelIndex, 0);
    m_baseModel = fn_8015F800(1, s_fileName);
    m_baseModel->fn_80162A68(m_modelIndex + 1000, 0);
    fn_80161FC8(m_model.px, m_modelIndex, boost::weak_ptr<CharInfo>(m_chara));
    fn_80161FC8(m_baseModel.px, m_modelIndex + 1000, boost::weak_ptr<CharInfo>(m_baseChara));
    RebuildTransform();
}

SetPopFooting::~SetPopFooting() {}

void SetPopFooting::Behave(float dt) {
    if (m_model.px != 0) {
        m_model->fn_80162FB0(dt);
    }
    if (m_baseModel.px != 0) {
        m_baseModel->fn_80162FB0(dt);
    }
    if (m_flags.Test(0)) {
        fn_801D53C4(fn_80053164(), 0x783A, m_chara.px, this, 3, 0, 2);
    }
    switch (m_state) {
    case 0:
        m_timer = m_params.secBegin;
        m_offset = -s_depth;
        m_state = 1;
        break;
    case 1:
        m_timer -= dt;
        if (m_timer <= 0.0f) {
            m_state = 2;
            m_flags.Set(0);
        }
        break;
    case 2:
        m_offset += s_speed * dt;
        if (m_offset > 0.0f) {
            m_offset = 0.0f;
            m_state = 3;
            m_timer = m_params.secWait;
            m_flags.Clear(0);
        }
        break;
    case 3:
        m_timer -= dt;
        if (m_timer <= 0.0f) {
            m_state = 4;
            m_flags.Set(0);
        }
        break;
    case 4:
        m_offset -= s_speed * dt;
        if (m_offset < -s_depth) {
            m_offset = -s_depth;
            m_state = 1;
            m_timer = m_params.secBegin;
            m_flags.Clear(0);
        }
        break;
    case 5:
        break;
    }
    UpdateFooting();
    RebuildTransform();
}

void SetPopFooting::ReloadPlacement(UnkPlacement_800C9CD0 *owner) {
    LoadPlacement(owner);
    RebuildTransform();
    if (m_params.model != m_modelIndex) {
        m_model->fn_80161880();
        m_model->fn_80162804();
        m_baseModel->fn_80162804();
        m_modelIndex = m_params.model;
        m_model = fn_8015F800(1, s_fileName);
        m_model->fn_80162A68(m_modelIndex, 0);
        m_baseModel = fn_8015F800(1, s_fileName);
        m_baseModel->fn_80162A68(m_modelIndex + 1000, 0);
        fn_80161FC8(m_model.px, m_modelIndex, boost::weak_ptr<CharInfo>(m_chara));
        fn_80161FC8(m_baseModel.px, m_modelIndex + 1000, boost::weak_ptr<CharInfo>(m_baseChara));
    }
}

void SetPopFooting::LoadPlacement(UnkPlacement_800C9CD0 *owner) {
    fn_800C9F14(owner, &m_position);
    fn_800C9ED4(owner, &m_chara->m_rotation);
    fn_800C9DBC(owner, &m_params, sizeof(m_params));
    fn_800091D0(&m_chara->m_position, &m_position);
    fn_800091D0(&m_baseChara->m_position, &m_chara->m_position);
    m_baseChara->m_rotation = m_chara->m_rotation;
    m_state = 0;
    m_timer = 0.0f;
    m_offset = 0.0f;
    m_flags.Clear();
}

void SetPopFooting::RebuildTransform() {
    if (m_model.px != 0) {
        Mtx34f matrix;
        fn_8015CEA0(m_chara.px, &matrix);
        m_model->fn_801633DC(&matrix);
    }
    if (m_baseModel.px != 0) {
        Mtx34f matrix;
        fn_8015CEA0(m_baseChara.px, &matrix);
        m_baseModel->fn_801633DC(&matrix);
    }
}

void SetPopFooting::UpdateFooting() {
    Mtx34f matrix;
    Vec3f offset(0.0f, 0.0f, m_offset);
    fn_8040B6F8(&matrix);
    fn_8040B848(&matrix, m_chara->m_rotation.y);
    fn_8040B884(&matrix, m_chara->m_rotation.x);
    fn_8040B80C(&matrix, m_chara->m_rotation.z);
    fn_800091D0(&offset, &(const Vec3f &)fn_8040B67C(&matrix, &offset));
    const Vec3f &position = ((VectorOpFn)fn_8040CDD0)(&m_position, &offset);
    fn_800091D0(&m_chara->m_position, &position);
}

} // namespace PopFooting

extern "C" void fn_8022BC84(void) {
    fn_8006B474("POPFOOTING.BIN", 1);
}

extern "C" void fn_8022BC80(void) {}

namespace PopFooting {

void SetPopFooting::Instantiate(UnkPlacement_800C9CD0 *owner) {
    Task *parent = fn_801EB304(fn_8001C4F8(), 11);
    new SetPopFooting(parent, owner);
}

} // namespace PopFooting

static GadgetParamDesc lbl_80546B00[] = {
    {1, "model", 0, 0, 1, 0.0f, 0.0f, 0.0f, fn_800CBC20, 0},
    {4, "sec_begin", 0, 0, 0, 1.0f, -65536.0f, 65536.0f, fn_800CBC20, 0},
    {4, "sec_wait", 0, 0, 0, 1.0f, -65536.0f, 65536.0f, fn_800CBC20, 0},
    {0},
};

GadgetClassDesc lbl_80546BA0 = {
    "POP FOOTING", fn_8022BC84, fn_8022BC80, PopFooting::SetPopFooting::Instantiate, 0, 0, 0x13F0, 1,
    0, 10, lbl_80546B00,
};
