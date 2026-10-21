/* BAMine::BAMine (class name from RTTI; its s_taskName is "BAMine"): the BA Mine gadget, a floating
 * mine (BAMINE.BIN) that drifts out along its "X", "Y", "Z" offset and back over "second" seconds
 * (it stays put when "second" is 0), ticking at the turns. A character that touches it, or an
 * attack, sets it off: it blasts with a sound and is done. A vacuum command (type 0x14) pulls it in
 * instead. File name inferred from the class. Names marked "(name inferred)" are not original.
 * Definitions are in reverse address order (deferred inlining emits functions last-defined first).
 * The unit's code reaches its .data through a pooled base address, so the linker keeps that section
 * whole: Game/TNumEdit.h and Game/CharaColliBaseCharInfo.h supply the RTTI base lists it starts
 * with. */
/* Keeps the dead copy of bad_alloc's vtable in .data, as retail has it (see <new>). */
#define MSL_NEW_INLINE_BAD_ALLOC

#include <boost/bind.hpp>
#include <boost/function.hpp>

#include "Game/TNumEdit.h"
/* After TNumEdit.h: the order of the casts sets the order of the RTTI base lists in .data. */
#include "Game/CharaColliBaseCharInfo.h"
#include "Game/CharCommand.h"
#include "Game/CharCommandUnit.h"
#include "Game/CharInfo.h"
#include "Game/CharaColliAttack.h"
#include "Game/Gadget.h"
#include "Game/GadgetCharInfo.h"
#include "Game/GadgetModelInfo.h"
#include "Game/GadgetClassDesc.h"
#include "Game/GadgetUtil.h"
#include "Game/FileQueue.h"
#include "boost/shared_ptr.hpp"
#include "boost/weak_ptr.hpp"
#include "size_assert.h"

using SonicteamUSA::System::CharaColli::AttackCallback;
using SonicteamUSA::System::CharaColli::AttackCallbackParam;
using SonicteamUSA::System::CharaColli::CharaColliAttack;

/* Target (class name from RTTI): a point other objects can aim at (see Spring.cpp). */
class Target {
  public:
    /* Moves the target to position (name inferred). */
    void fn_800CCE38(const Vec3f *position);
};

extern "C" {
/* A 32-bit flag word: fn_80014E04 constructs it cleared, fn_80014E10 clears it, fn_80014E1C sets
 * bit, fn_8006127C clears bit and fn_80014E34 tests bit (behaviour from their bodies). */
void fn_80014E04(u32 *flags);
void fn_80014E10(u32 *flags);
void fn_80014E1C(u32 *flags, int bit);
void fn_8006127C(u32 *flags, int bit);
bool fn_80014E34(u32 *flags, int bit);

/* Gives the character info a command handler unless it has one (behaviour inferred). */
void fn_8015CF7C(GadgetCharInfo *charInfo);
/* Tells the owner that the gadget is done (behaviour inferred). */
void fn_8015A794(Gadget *gadget);

/* GadgetModelInfo helpers (behaviour inferred; see SecretDisk.cpp and Bomb.cpp). */
boost::shared_ptr<GadgetModelInfo> fn_8015F800(int arg0, const char *fileName);
bool fn_80161FC8(GadgetModelInfo *model, int variant, const boost::weak_ptr<CharInfo> &info);
boost::weak_ptr<CharaColliAttack> fn_80161428(GadgetModelInfo *model, int variant);
/* Lets a vacuum pull the model in (behaviour inferred; arg2 unknown; see Bomb.cpp). */
void fn_8033F0F8(CharCommand *command, boost::shared_ptr<GadgetModelInfo> *model, int arg2,
                 CharInfo *charInfo, const Vec3f *rotation);

/* Creates a Target for owner, and removes a gadget's target (behaviour inferred; the flags are
 * unknown). */
boost::shared_ptr<Target> fn_801E7128(const boost::weak_ptr<CharInfo> &owner, int flags);
void fn_801E7108(boost::shared_ptr<Target> *target);

/* A blast at the character info (behaviour inferred; see CityBombBig.cpp). */
void fn_801DD0BC(GadgetCharInfo *charInfo, int arg1, float arg2, int arg3);
/* Plays sound id at a character info (behaviour inferred). */
void fn_801D54F4(void *manager, int id, GadgetCharInfo *source, int arg3, int arg4);

/* Vector helpers (behaviour inferred): vector += value, the vector normalised, a + b. */
void fn_8040CFF8(Vec3f *vector, const Vec3f *value);
Vec3f fn_8040CEE8(const Vec3f *vector);
Vec3f fn_8040CDD0(const Vec3f *a, const Vec3f *b);
}

/* Degrees to radians: an out-of-line function of the game (0x8000AB90). */
float DegToRad(float degrees);

/* The float overload of std::cos, out of line in the game (<cmath> is not included: its sqrtf
 * constants would be emitted into this unit's .sdata2). */
namespace std {
float cos(float x);
} // namespace std

namespace BAMine {

/* The flag word at 0x70 (struct name unknown). */
struct BAMineFlags {
    BAMineFlags() {
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

    u32 m_bits; /* bit 0: does not move, 1: tick sound due, 2 and 3: the two ticks played */
};

/* The parameters the owner supplies (names from the parameter table). */
struct BAMineParams {
    BAMineParams() : x(0.0f), y(0.0f), z(0.0f), second(0.0f) {}

    /* 0x00 */ float x;
    /* 0x04 */ float y;
    /* 0x08 */ float z;
    /* 0x0C */ float second;
};
SIZE_ASSERT(BAMineParams, 0x10);

class BAMine : public Gadget {
  public:
    BAMine(Task *parent, UnkPlacement_800C9CD0 *owner);
    virtual ~BAMine();
    virtual void Behave(float dt);
    virtual void ReloadPlacement(UnkPlacement_800C9CD0 *owner);

    /* Creates the gadget under task layer 11 (name inferred). */
    static void Instantiate(UnkPlacement_800C9CD0 *owner);

    /* Copies the owner's placement and parameters and resets the state (name inferred). */
    void ReadPlacement(UnkPlacement_800C9CD0 *owner);
    /* Places the model and the target (name inferred). */
    void UpdatePosition();
    /* Drifts along the offset (name inferred). */
    void Move(float dt);
    /* The command unit's callback (name inferred). */
    void OnCommand(CharCommand &command);
    /* The attack collision's callbacks: something touched the mine, and an attack (names
     * inferred). */
    void OnTouch(AttackCallbackParam &param);
    void OnAttack(AttackCallbackParam &param);

    static const char *s_fileName;
    static const char *s_taskName;

    /* Member names are inferred. */
    /* 0x30 */ boost::shared_ptr<GadgetCharInfo> m_chara;
    /* 0x38 */ boost::shared_ptr<GadgetModelInfo> m_model;
    /* 0x40 */ boost::shared_ptr<Target> m_target;
    /* 0x48 */ s32 m_state; /* 0 new, 1 drifting, 2 pulled in, 3 set off */
    /* 0x4C */ BAMineParams m_params;
    /* 0x5C */ Vec3f m_origin;
    /* 0x68 */ float m_speed; /* degrees of the cycle per second */
    /* 0x6C */ float m_time;
    /* 0x70 */ BAMineFlags m_flags2;
    /* 0x74 */ u32 pad_0x74; /* the class size is a multiple of 8 */
};
SIZE_ASSERT(BAMine, 0x78);

const char *BAMine::s_fileName = "BAMINE.BIN";
const char *BAMine::s_taskName = "BAMine";

BAMine::BAMine(Task *parent, UnkPlacement_800C9CD0 *owner) : Gadget(parent, owner) {
    m_name = s_taskName;
    m_chara = fn_8015CD90(0x8E, 4);
    fn_8015CF7C(m_chara.px);
    fn_800729B0(&m_chara->m_commandUnit->m_callback,
                &(const CharCommandCallback &)CharCommandCallback(
                    boost::bind(&BAMine::OnCommand, this, _1)));
    ReadPlacement(owner);
    m_model = fn_8015F800(1, s_fileName);
    m_model->fn_80162A68(0, 0);
    fn_80161FC8(m_model.px, 0, m_chara);
    boost::weak_ptr<CharaColliAttack> attack = fn_80161428(m_model.px, 0);
    if (boost::shared_ptr<CharaColliAttack> collision = attack.lock()) {
        collision->field_0x54 = 1;
        collision->m_kind = 1;
        collision->m_power = 1.0f;
        collision->m_hitCallback = AttackCallback(boost::bind(&BAMine::OnTouch, this, _1));
        collision->field_0x55 = 1;
        collision->m_attackCallback = AttackCallback(boost::bind(&BAMine::OnAttack, this, _1));
    }
    m_target = fn_801E7128(m_chara, 0x880);
    UpdatePosition();
}

BAMine::~BAMine() {
    fn_801E7108(&m_target);
}

void BAMine::Behave(float dt) {
    if (m_model.px != 0) {
        m_model->fn_80162FB0(dt);
    }
    switch (m_state) {
    case 0:
        m_state = 1;
        break;
    case 1:
        Move(dt);
        UpdatePosition();
        break;
    case 2:
        fn_8015A794(this);
        m_flags |= 1;
        break;
    case 3:
        fn_801DD0BC(m_chara.px, 1, 1.0f, 7);
        fn_801D54F4(fn_80053164(), 0x7934, m_chara.px, 0, 1);
        fn_8015A794(this);
        m_flags |= 1;
        break;
    }
}

void BAMine::ReloadPlacement(UnkPlacement_800C9CD0 *owner) {
    ReadPlacement(owner);
    UpdatePosition();
    Vec3f start;
    fn_800091EC(&start, &m_chara->m_position);
    Vec3f end(m_params.x, m_params.y, m_params.z);
    fn_8040CFF8(&end, &start);
    u8 segment[0x18];
    fn_802E0B58(fn_80020A20(segment, &start, &end), &lbl_805F06D8);
}

void BAMine::ReadPlacement(UnkPlacement_800C9CD0 *owner) {
    fn_800C9F14(owner, &m_chara->m_position);
    fn_800C9ED4(owner, &m_chara->m_rotation);
    fn_800C9DBC(owner, &m_params, sizeof(m_params));
    m_state = 0;
    fn_800091D0(&m_origin, &m_chara->m_position);
    float speed = 0.0f;
    if (0.0f != m_params.second) {
        speed = 360.0f / m_params.second;
    }
    m_speed = speed;
    m_time = 0.0f;
    if (m_params.second <= 0.0f) {
        m_flags2.Set(0);
    } else {
        m_flags2.Clear(0);
    }
}

void BAMine::UpdatePosition() {
    if (m_model.px != 0) {
        Mtx34f matrix;

        fn_8040C148(&matrix, &lbl_805E0720);
        fn_8040B59C(&matrix, &m_chara->m_position);
        fn_8040B848(&matrix, m_chara->m_rotation.y);
        m_model->fn_801633DC(&matrix);
    }
    if (m_target.px != 0) {
        m_target->fn_800CCE38(&m_chara->m_position);
    }
}

void BAMine::Move(float dt) {
    if (!m_flags2.Test(0)) {
        m_time += dt;
        if (m_time > m_params.second) {
            m_time = 0.0f;
            m_flags2.Clear();
        } else if (m_time > 0.5f * m_params.second + 0.5f) {
            if (!m_flags2.Test(3)) {
                m_flags2.Set(3);
                m_flags2.Set(1);
            }
        } else if (m_time > 0.5f) {
            if (!m_flags2.Test(2)) {
                m_flags2.Set(2);
                m_flags2.Set(1);
            }
        }
        if (m_flags2.Test(1)) {
            fn_801D54F4(fn_80053164(), 0x7933, m_chara.px, 0, 1);
            m_flags2.Clear(1);
        }
    }
    float angle = m_speed * m_time;
    Vec3f offset(m_params.x, m_params.y, m_params.z);
    float half = fn_8040CF50(&offset);
    AssignVector(offset, fn_8040CEE8(&offset));
    half = 0.5f * half;
    fn_8040CE38(&offset, half - half * std::cos(DegToRad(angle)));
    AssignVector(m_chara.px->m_position, fn_8040CDD0(&m_origin, &offset));
}

void BAMine::OnAttack(AttackCallbackParam &param) {
    param.m_result = 5;
    if (m_state == 1) {
        if (boost::shared_ptr<CharaColliAttack> attack = param.m_attacker.lock()) {
            if ((attack->m_flags & 0x3D1) == 0) {
                return;
            }
            param.m_result = 3;
            m_state = 3;
        }
    }
}

void BAMine::OnTouch(AttackCallbackParam &param) {
    if (m_state == 1) {
        m_state = 3;
    }
}

void BAMine::OnCommand(CharCommand &command) {
    switch (command.TypeId()) {
    case 0x14:
        if (m_state == 1) {
            m_model->fn_80161880();
            fn_8033F0F8(&command, &m_model, 0, m_chara.px, &m_chara.px->m_rotation);
            m_model.reset();
            m_state = 2;
        }
        break;
    }
    command.field_0x4[3] |= 2;
}

} // namespace BAMine

extern "C" void fn_80300038(void) {
    fn_8006B474(BAMine::BAMine::s_fileName, 1);
}

extern "C" void fn_80300034(void) {}

namespace BAMine {

void BAMine::Instantiate(UnkPlacement_800C9CD0 *owner) {
    Task *parent = fn_801EB304(fn_8001C4F8(), 11);
    new BAMine(parent, owner);
}

} // namespace BAMine

static GadgetParamDesc lbl_8055AB34[] = {
    {4, "X", 0, 0, 0, 0.0f, -65536.0f, 65536.0f, fn_800CBC20, 0},
    {4, "Y", 0, 0, 0, 0.0f, -65536.0f, 65536.0f, fn_800CBC20, 0},
    {4, "Z", 0, 0, 0, 0.0f, -65536.0f, 65536.0f, fn_800CBC20, 0},
    {4, "second", 0, 0, 0, 0.0f, 0.0f, 65536.0f, fn_800CBC20, 0},
    {0},
};

GadgetClassDesc lbl_8055ABFC = {
    "BA Mine", fn_80300038, fn_80300034, BAMine::BAMine::Instantiate, 0, 0, 0x17D5, 1, 0, 10,
    lbl_8055AB34,
};
