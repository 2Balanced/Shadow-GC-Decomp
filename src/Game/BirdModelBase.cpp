/* Vehicle::BirdBase::BirdModelBase (class name from RTTI; declared in Game/BirdModelBase.h): the
 * model of a flying vehicle. Its Impl (class name unknown) owns the vehicle's motion object and
 * runs a small state machine that switches the flying motion; the steering state follows how far
 * the owner's heading points up or down. Member and function names are inferred. Definitions are
 * in reverse address order (deferred inlining emits functions last-defined first). */
#include "Game/BirdModelBase.h"
#include "Game/UnkMotion_8022F058.h"

extern "C" {
/* A 32-bit flag word: fn_80014E04 constructs it cleared, fn_80014E10 clears it, fn_80014E1C sets
 * bit, fn_8006127C clears bit and fn_80014E34 tests bit (see Game/BAMine.cpp). */
void fn_80014E04(u32 *flags);
void fn_80014E10(u32 *flags);
void fn_80014E1C(u32 *flags, int bit);
void fn_8006127C(u32 *flags, int bit);
bool fn_80014E34(u32 *flags, int bit);

/* The normalized vector, and the dot product (see Game/ArkCannon.cpp). */
Vec3f fn_8040CEE8(const Vec3f *vector);
float fn_8040CF90(const Vec3f *a, const Vec3f *b);
}

namespace Vehicle {
namespace BirdBase {

/* The flag word at 0xC (struct name unknown). */
struct BirdModelFlags {
    BirdModelFlags() {
        fn_80014E04(&m_bits);
    }

    u32 m_bits; /* bit 0: playing, 1: a pause or resume is requested */
};

/* The dot product of two vectors (an inline wrapper: it takes the normalized vector without a
 * copy). */
inline float Dot(const Vec3f &a, const Vec3f &b) {
    return fn_8040CF90(&a, &b);
}

class BirdModelBase::Impl {
  public:
    Impl(CharInfo *owner, const BirdMotionTable *table);
    ~Impl();
    void Load(const BirdMotionTable *table);
    void UpdateLanding(float dt);
    void UpdateTurn(float dt);
    void UpdateSteer(float dt);
    void UpdateFly(float dt);
    void UpdateState(float dt);
    void Stop();
    void SetState(int state);
    void Update(float dt);

    void Pause() {
        if (fn_80014E34(&m_flags.m_bits, 0)) {
            fn_8022EC6C(m_motion);
            fn_8006127C(&m_flags.m_bits, 0);
        }
    }
    void Resume() {
        if (!fn_80014E34(&m_flags.m_bits, 0)) {
            fn_8022EC90(m_motion);
            fn_80014E1C(&m_flags.m_bits, 0);
        }
    }
    void RequestPause() {
        fn_80014E1C(&m_flags.m_bits, 1);
    }
    float GetMotionTime() {
        return fn_8022EEC4(m_motion);
    }
    void *GetFrame(int index) {
        return fn_8022EEE8(m_motion, index);
    }
    void SetPlacement(const Vec3f *position, const Vec3f *rotation) {
        fn_8022EF78(m_motion, position, rotation);
    }

    /* 0x00 */ int m_state;
    /* 0x04 */ int m_step;
    /* 0x08 */ UnkMotion_8022F058 *m_motion;
    /* 0x0C */ BirdModelFlags m_flags;
    /* 0x10 */ CharInfo *m_owner;
};
SIZE_ASSERT(BirdModelBase::Impl, 0x14);

/* Deletes the state (name inferred; see checked_delete below). */
inline void DeleteImpl(BirdModelBase::Impl *impl) {
    delete impl;
}

} // namespace BirdBase
} // namespace Vehicle

/* Retail keeps boost::checked_delete<Impl> out of line (0x802C9134) and ~Impl at its own place
 * (0x802C95D8); this specialization calls an inline helper, which gives retail's code and order
 * (as in Game/Rocket.cpp; see docs/research-notes.md). */
namespace boost {
template <>
inline void checked_delete<Vehicle::BirdBase::BirdModelBase::Impl>(
    Vehicle::BirdBase::BirdModelBase::Impl *x) {
    Vehicle::BirdBase::DeleteImpl(x);
}
} // namespace boost

namespace Vehicle {
namespace BirdBase {

BirdModelBase::Impl::Impl(CharInfo *owner, const BirdMotionTable *table)
    : m_motion(0), m_owner(owner) {
    fn_80014E10(&m_flags.m_bits);
    Load(table);
    m_state = 0;
    m_step = 0;
}

BirdModelBase::Impl::~Impl() {
    if (m_motion) {
        delete m_motion;
    }
    m_motion = 0;
}

void BirdModelBase::Impl::Update(float dt) {
    if (fn_80014E34(&m_flags.m_bits, 1)) {
        if (fn_80014E34(&m_flags.m_bits, 0)) {
            Pause();
        } else {
            Resume();
        }
    }
    UpdateState(dt);
    fn_8022EF54(m_motion, dt);
}

void BirdModelBase::Impl::SetState(int state) {
    m_state = state;
    m_step = 0;
}

void BirdModelBase::Impl::Stop() {
    fn_8006127C(&m_flags.m_bits, 1);
    Pause();
}

void BirdModelBase::Impl::UpdateState(float dt) {
    switch (m_state) {
    case 0:
        UpdateFly(dt);
        break;
    case 1:
        UpdateSteer(dt);
        break;
    case 2:
        UpdateTurn(dt);
        break;
    case 3:
        UpdateLanding(dt);
        break;
    }
}

void BirdModelBase::Impl::UpdateFly(float dt) {
    switch (m_step) {
    case 0:
        fn_8022EF30(m_motion, 0, 0.0f);
        m_step++;
        break;
    }
}

/* Picks the motion from how far the owner's heading points up (2), level (1) or down (3). */
void BirdModelBase::Impl::UpdateSteer(float dt) {
    Vec3f up(0.0f, 1.0f, 0.0f);
    float dot = Dot(fn_8040CEE8(&m_owner->field_0x18), up);
    int step;
    if (dot > 0.259f) {
        step = 2;
    } else if (dot >= -0.259f) {
        step = 1;
    } else {
        step = 3;
    }
    if (m_step != step) {
        m_step = step;
        int motionId;
        switch (m_step) {
        case 1:
            motionId = 1;
            break;
        case 2:
            motionId = 2;
            break;
        case 3:
            motionId = 3;
            break;
        }
        fn_8022EF30(m_motion, motionId, 30.0f);
    }
}

void BirdModelBase::Impl::UpdateTurn(float dt) {
    switch (m_step) {
    case 0:
        fn_8022EF30(m_motion, 2, 30.0f);
        m_step++;
        break;
    }
}

void BirdModelBase::Impl::UpdateLanding(float dt) {
    switch (m_step) {
    case 0:
        fn_8022EF30(m_motion, 4, 0.0f);
        m_step++;
        break;
    case 1:
        if (fn_8022EC48(m_motion)) {
            m_state = 1;
            m_step = 0;
        }
        break;
    }
}

void BirdModelBase::Impl::Load(const BirdMotionTable *table) {
    if (m_motion) {
        if (m_motion) {
            delete m_motion;
        }
        m_motion = 0;
    }
    m_motion = new UnkMotion_8022F058(m_owner, 3, table);
}

BirdModelBase::BirdModelBase(CharInfo *owner, const BirdMotionTable *table)
    : m_impl(new Impl(owner, table)) {}

BirdModelBase::~BirdModelBase() {}

void BirdModelBase::Update(float dt) {
    m_impl->Update(dt);
}

void BirdModelBase::SetState(int state) {
    m_impl->SetState(state);
}

void BirdModelBase::SetPlacement(const Vec3f *position, const Vec3f *rotation) {
    m_impl->SetPlacement(position, rotation);
}

void *BirdModelBase::GetFrame(int index) {
    return m_impl->GetFrame(index);
}

float BirdModelBase::GetMotionTime() {
    return m_impl->GetMotionTime();
}

void BirdModelBase::RequestPause() {
    m_impl->RequestPause();
}

void BirdModelBase::Stop() {
    m_impl->Stop();
}

void BirdModelBase::SetMotionArg(void *arg) {
    fn_8022EC24(m_impl->m_motion, arg);
}

} // namespace BirdBase
} // namespace Vehicle
