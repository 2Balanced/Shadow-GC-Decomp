/* SetGenerator::SetGenerator (class name from RTTI; its s_taskName is "SetGenerator"): the SET
 * GENERATOR gadget. While its switch ("Id") is on, its generator (SetGenerator::Manager) finds the
 * set object whose "ComId" matches and releases it again and again, "delay" seconds apart, "num"
 * times (a "num" of 0 or less: without end); then it turns on the switch the owner names and
 * finishes. How many releases are left is kept in the owner record's adapter state
 * (SetGenerator::SetGeneratorState), so it survives the gadget being recreated. The generator keeps
 * its state in a ManagerImpl (name invented; it has no RTTI) behind a boost::scoped_ptr. File name
 * inferred from the class. Names marked "(name inferred)" are not original. Definitions are in
 * reverse address order (deferred inlining emits functions last-defined first). */
#include "Game/CharCommand.h"
#include "Game/DebugDraw.h"
#include "Game/Gadget.h"
#include "Game/GadgetCharInfo.h"
#include "Game/GadgetClassDesc.h"
#include "Game/GadgetUtil.h"
#include "Game/SetAdapterEventArgs.h"
#include "Game/SetAdapterState.h"
#include "Game/SetGenerator.h"
#include "Game/UnkSwitchBank_800CB69C.h"
#include "boost/scoped_ptr.hpp"
#include "boost/shared_ptr.hpp"
#include "size_assert.h"

/* The color of the generator's debug sphere. */
extern UnkDrawColor_802E0E84 lbl_805F1370;

extern "C" {
/* A 32-bit flag word (see GadgetModelInfo.h). */
void fn_80014E04(u32 *flags);
void fn_80014E10(u32 *flags);
void fn_80014E1C(u32 *flags, int bit);
void fn_8006127C(u32 *flags, int bit);
bool fn_80014E34(u32 *flags, int bit);

/* The switch id the owner names (behaviour inferred). */
void fn_800C9E50(UnkPlacement_800C9CD0 *owner, s32 *id);
/* Tells the gadget's owner that the gadget is finished (behaviour inferred). */
void fn_8015A794(Gadget *gadget);

/* The set object manager, and calling callback with every set object (behaviour inferred; see
 * SavePointSelect.cpp; the callback returns 0 to stop). */
void *fn_800C9B84(void);
void fn_800CB47C(void *manager, void *(*callback)(UnkPlacement_800C9CD0 *owner, void *arg), void *arg);
/* Tests, sets and clears mask in a set object's flags (behaviour inferred). */
bool fn_800C9E80(UnkPlacement_800C9CD0 *owner, u32 mask);
void fn_800C9E9C(UnkPlacement_800C9CD0 *owner, u32 mask);
void fn_800C9EB0(UnkPlacement_800C9CD0 *owner, u32 mask);
/* Hands a command to a set object (behaviour inferred). */
void fn_800C973C(UnkPlacement_800C9CD0 *owner, CharCommand *command);
}

/* A value (name inferred). As a call it is not a constant expression, so the value is stored by
 * the static initializer, as in retail. */
inline float Plain(float value) {
    return value;
}

namespace SetGenerator {

/* The radius of the debug sphere (name inferred). */
static const float s_radius = Plain(20.0f);

/* The flag word of the generator (struct name unknown). Bit 0: running (the switch is on), bit 1:
 * the first release is still to come, bit 2: without end, bit 3: finished (meanings inferred). */
struct ManagerFlags {
    ManagerFlags() {
        fn_80014E04(&m_bits);
    }
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

/* FindArgs (class name from RTTI): finds the set object with the generator's ComId (name
 * inferred). */
class FindArgs : public SetAdapterEventArgs {
  public:
    FindArgs() : m_found(0) {}
    virtual ~FindArgs() {}

    /* 0x8 */ UnkPlacement_800C9CD0 *m_found;
};

/* The callback that picks a generated object (bit 0x40000 of its flags) for FindArgs (name
 * inferred). */
static void *FindTarget(UnkPlacement_800C9CD0 *owner, void *arg) {
    FindArgs *args = (FindArgs *)arg;
    if (fn_800C9E80(owner, 0x40000) && args != 0) {
        args->m_found = owner;
        return 0;
    }
    return owner;
}

/* The generator's state (class name invented; member names inferred). */
class ManagerImpl {
  public:
    ManagerImpl(UnkGeneratorParam_8027C3C4 &param);

    /* Steps the generator. */
    void Update(float dt);

    ~ManagerImpl() {}

    /* Whether the generator is finished. */
    bool IsFinished() {
        return fn_80014E34(&m_flags.m_bits, 3);
    }
    /* Pauses and resumes the generator. */
    void Stop() {
        fn_8006127C(&m_flags.m_bits, 0);
    }
    void Start() {
        fn_80014E1C(&m_flags.m_bits, 0);
    }

    /* How many releases are left. */
    s32 GetCount() {
        return m_param.num;
    }

    /* 0x00 */ s32 m_state; /* 0 find, 1 wait for the switch, 2 hide, 3 release, 4 wait until it
                             * is gone, 5 delay, 6 finished */
    /* 0x04 */ float m_timer;
    /* 0x08 */ UnkGeneratorParam_8027C3C4 m_param;
    /* 0x14 */ UnkPlacement_800C9CD0 *m_target;
    /* 0x18 */ ManagerFlags m_flags;
};
SIZE_ASSERT(ManagerImpl, 0x1C);

ManagerImpl::ManagerImpl(UnkGeneratorParam_8027C3C4 &param) {
    m_state = 0;
    m_timer = 0.0f;
    m_target = 0;
    const UnkGeneratorParam_8027C3C4 *src = &param;
    m_param.comId = src->comId;
    m_param.num = src->num;
    m_param.delay = src->delay;
    m_flags.Clear();
    m_flags.Set(1);
    m_flags.Reset(0);
    if (m_param.num <= 0) {
        m_flags.Set(2);
    }
}

void ManagerImpl::Update(float dt) {
    switch (m_state) {
    case 0: {
        FindArgs args;
        args.field_0x0 = m_param.comId;
        fn_800CB47C(fn_800C9B84(), FindTarget, &args);
        m_target = args.m_found;
        if (m_target == 0) {
            m_state = 6;
        } else {
            m_timer = m_param.delay;
            m_state = 5;
        }
        break;
    }
    case 1:
        if (m_flags.Test(0)) {
            if (m_flags.Test(1)) {
                if (fn_800C9E80(m_target, 0x20000)) {
                    m_state = 2;
                } else {
                    m_state = 4;
                }
                m_flags.Reset(1);
            } else if (!fn_800C9E80(m_target, 2)) {
                fn_800C9EB0(m_target, 0x40);
                m_state = 2;
            } else {
                m_state = 4;
            }
        }
        break;
    case 2:
        if (m_target != 0) {
            fn_800C9E9C(m_target, 0x20000);
            m_state = 3;
        } else {
            m_state = 6;
        }
        break;
    case 3:
        if (m_target != 0) {
            Vec3f position;
            Vec3f rotation(0.0f, 0.0f, 0.0f);
            fn_800C9F14(m_target, &position);
            fn_800C9ED4(m_target, &rotation);
            fn_800C973C(m_target, new GenerateCommand(position, rotation));
        }
        m_state = 4;
        break;
    case 4:
        if (m_target != 0) {
            if (!fn_800C9E80(m_target, 1)) {
                m_timer = m_param.delay;
                m_state = 5;
                if (!m_flags.Test(2)) {
                    m_param.num--;
                    if (m_param.num <= 0) {
                        m_flags.Set(3);
                        m_state = 6;
                    }
                }
            }
        } else {
            m_state = 6;
        }
        break;
    case 5:
        m_timer -= dt;
        if (m_timer <= 0.0f) {
            m_state = 1;
        }
        if (!m_flags.Test(2) && m_param.num <= 0) {
            m_state = 6;
        }
        break;
    case 6:
        m_flags.Set(3);
        break;
    }
}

/* Manager (class name from the RTTI of its shared_ptr control block): the pimpl wrapper of
 * ManagerImpl. Its members keep the fn_ names Game/SetGenerator.h declares them by. */
class Manager {
  public:
    Manager(UnkGeneratorParam_8027C3C4 &param);
    ~Manager();

    /* 0x0 */ boost::scoped_ptr<ManagerImpl> m_impl;
};
SIZE_ASSERT(Manager, 0x4);

Manager::Manager(UnkGeneratorParam_8027C3C4 &param) : m_impl(new ManagerImpl(param)) {}

Manager::~Manager() {}

} // namespace SetGenerator

using SetGenerator::Manager;

/* Deletes the generator (name inferred; see checked_delete below). */
inline void DeleteManager(Manager *manager) {
    delete manager;
}

/* Retail keeps boost::checked_delete<Manager> out of line (0x8027C5A0) and the destructor at its
 * definition; see Rocket.cpp and docs/research-notes.md. */
namespace boost {
template<> inline void checked_delete<Manager>(Manager *x) {
    DeleteManager(x);
}
} // namespace boost

extern "C" void fn_8027C6A8(Manager *manager, float dt) {
    manager->m_impl->Update(dt);
}

extern "C" void fn_8027C67C(Manager *manager) {
    manager->m_impl->Start();
}

extern "C" void fn_8027C650(Manager *manager) {
    manager->m_impl->Stop();
}

/* Whether the generator is finished (name inferred). */
extern "C" bool fn_8027C624(Manager *manager) {
    return manager->m_impl->IsFinished();
}

/* How many releases are left (name inferred). */
extern "C" s32 fn_8027C5F8(Manager *manager) {
    return manager->m_impl->GetCount();
}

extern "C" boost::shared_ptr<Manager> fn_8027C3C4(const UnkGeneratorParam_8027C3C4 *param) {
    boost::shared_ptr<Manager> manager = boost::shared_ptr<Manager>(new Manager(*const_cast<UnkGeneratorParam_8027C3C4 *>(param)));
    return manager;
}

namespace SetGenerator {

/* SetGeneratorState (class name from RTTI): the releases left, kept by the owner record (member
 * name inferred). */
class SetGeneratorState : public SetAdapterState {
  public:
    SetGeneratorState() : m_count(0) {}
    virtual ~SetGeneratorState() {}

    /* 0x4 */ s32 m_count;
};
SIZE_ASSERT(SetGeneratorState, 0x8);

/* The parameters of the owner record (names from the parameter table). */
struct SetGeneratorParams {
    SetGeneratorParams() : comId(0), id(0), num(0), delay(0.0f), scoreId(0) {}

    /* 0x00 */ s32 comId;
    /* 0x04 */ s32 id; /* the switch that runs the generator */
    /* 0x08 */ s32 num;
    /* 0x0C */ float delay;
    /* 0x10 */ s32 scoreId;
};

class SetGenerator : public Gadget {
  public:
    SetGenerator(Task *parent, UnkPlacement_800C9CD0 *owner);
    virtual ~SetGenerator();
    virtual void Behave(float dt);
    virtual void fn_800CBC1C();
    virtual void ReloadPlacement(UnkPlacement_800C9CD0 *owner);

    /* Creates the gadget under task layer 11 (name inferred). */
    static void Instantiate(UnkPlacement_800C9CD0 *owner);

    /* Copies the owner's placement and parameters (name inferred). */
    void ReadPlacement(UnkPlacement_800C9CD0 *owner);

    static const char *s_taskName;

    /* Member names are inferred. */
    /* 0x30 */ boost::shared_ptr<GadgetCharInfo> m_chara;
    /* 0x38 */ SetGeneratorParams m_params;
    /* 0x4C */ s32 m_state; /* 0 start, 1 running */
    /* 0x50 */ float field_0x50;
    /* 0x54 */ s32 field_0x54;
    /* 0x58 */ boost::shared_ptr<Manager> m_manager;
};
SIZE_ASSERT(SetGenerator, 0x60);

const char *SetGenerator::s_taskName = "SetGenerator";

SetGenerator::SetGenerator(Task *parent, UnkPlacement_800C9CD0 *owner) : Gadget(parent, owner) {
    m_name = s_taskName;
    m_chara = fn_8015CD90(0x88, 4);
    ReadPlacement(owner);
}

SetGenerator::~SetGenerator() {
    if (m_manager.get() != 0) {
        SetGeneratorState *state = static_cast<SetGeneratorState *>(m_owner->m_adapterState);
        if (state != 0) {
            state->m_count = fn_8027C5F8(m_manager.get());
        }
    }
}

void SetGenerator::Behave(float dt) {
    switch (m_state) {
    case 0: {
        UnkGeneratorParam_8027C3C4 param;
        param.comId = m_params.comId;
        param.num = m_params.num;
        param.delay = m_params.delay;
        UnkPlacement_800C9CD0 *owner = m_owner;
        if (owner->m_adapterState != 0) {
            param.num = static_cast<SetGeneratorState *>(owner->m_adapterState)->m_count;
        } else {
            SetGeneratorState *state = new SetGeneratorState;
            state->m_count = m_params.num;
            param.num = state->m_count;
            fn_800C9930(owner, state);
        }
        m_manager = fn_8027C3C4(&param);
        m_state = 1;
        break;
    }
    case 1:
        if (fn_8016904C(fn_800CB69C(), m_params.id) == 1) {
            fn_8027C67C(m_manager.get());
        } else {
            fn_8027C650(m_manager.get());
        }
        fn_8027C6A8(m_manager.get(), dt);
        if (fn_8027C624(m_manager.get())) {
            if (m_owner != 0) {
                s32 switchId;
                fn_800C9E50(m_owner, &switchId);
                if (switchId > 0) {
                    fn_801690D0(fn_800CB69C(), switchId, 1);
                }
            }
            fn_8015A794(this);
            m_flags |= 1;
        }
        break;
    case 6:
        break;
    }
}

void SetGenerator::fn_800CBC1C() {
    fn_802E0E84(&m_chara->m_position, &lbl_805F1370, s_radius);
}

void SetGenerator::ReloadPlacement(UnkPlacement_800C9CD0 *owner) {
    ReadPlacement(owner);
}

void SetGenerator::ReadPlacement(UnkPlacement_800C9CD0 *owner) {
    fn_800C9F14(owner, &m_chara->m_position);
    fn_800C9ED4(owner, &m_chara->m_rotation);
    fn_800C9DBC(owner, &m_params, sizeof(m_params));
    m_state = 0;
    field_0x54 = 0;
    field_0x50 = 0.0f;
}

} // namespace SetGenerator

extern "C" void fn_8027BF70(void) {}

extern "C" void fn_8027BF6C(void) {}

void SetGenerator::SetGenerator::Instantiate(UnkPlacement_800C9CD0 *owner) {
    Task *parent = fn_801EB304(fn_8001C4F8(), 11);
    new SetGenerator(parent, owner);
}

static GadgetParamDesc lbl_8054DCC8[] = {
    {1, "ComId", 0, 0, 0xFF, 0.0f, 0.0f, 0.0f, fn_800CBC20, 0},
    {1, "Id", 0, 0, 0xFF, 0.0f, 0.0f, 0.0f, fn_800CBC20, 0},
    {1, "num", 0, 0, 0x10000, 0.0f, 0.0f, 0.0f, fn_800CBC20, 0},
    {4, "delay", 0, 0, 0, 0.0f, 0.0f, 65536.0f, fn_800CBC20, 0},
    {1, "scoreId", 0, 0, 0x10000, 0.0f, 0.0f, 0.0f, fn_800CBC20, 0},
    {0},
};

GadgetClassDesc lbl_8054DDB8 = {
    "SET GENERATOR", fn_8027BF70, fn_8027BF6C, SetGenerator::SetGenerator::Instantiate, 0, 0, 0x2593, 1, 0, 10,
    lbl_8054DCC8,
};
