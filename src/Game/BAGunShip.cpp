/* BAGunShip::BAGunShip (class name from RTTI; its s_taskName is "BAGunShip"): the BA GUNSHIP gadget, a
 * gunship (BAGUNSHIP.BIN) with a cylinder collider of the "radius" and "height" parameters. Once its
 * switch is on (at once when it has none) it waits for a character to touch it, then opens (motion 1),
 * stays out for "second" seconds (forever when that is 0) and, with "attack" on, fires a shot at the
 * player every "interval" seconds; then it closes (motion 3) and is done. While it is out it starts
 * a set generator (SetGenerator::Manager) built from "ComId", "num" and "delay". Attacks wear its
 * health down; when it is destroyed it breaks into DM0000GUNSHIPBRK.DFF debris, throws a weapon,
 * rumbles the controller and is done. File name inferred from the class. Names marked "(name
 * inferred)" are not original. Definitions are in reverse address order (deferred inlining emits
 * functions last-defined first). The unit's code reaches its .data through a pooled base address,
 * so the linker keeps that section whole: Game/TNumEdit.h and Game/CharaColliBaseCharInfo.h supply
 * the RTTI base lists it starts with. */
/* Keeps the dead copy of bad_alloc's vtable in .data, as retail has it (see <new>). */
#define MSL_NEW_INLINE_BAD_ALLOC

#include <boost/bind.hpp>
#include <boost/function.hpp>

#include "Game/TNumEdit.h"
/* After TNumEdit.h: the order of the casts sets the order of the RTTI base lists in .data. */
#include "Game/CharaColliBaseCharInfo.h"
#include "Game/CharInfo.h"
#include "Game/CharaColli.h"
#include "Game/CharaColliAttack.h"
#include "Game/Gadget.h"
#include "Game/GadgetCharInfo.h"
#include "Game/GadgetColliDetectArgs.h"
#include "Game/GadgetModelInfo.h"
#include "Game/GadgetParticle.h"
#include "Game/GadgetClassDesc.h"
#include "Game/GadgetUtil.h"
#include "Game/SetGenerator.h"
#include "Game/UnkSwitchBank_800CB69C.h"
#include "Game/Vibration.h"
#include "Game/WeaponBullet.h"
#include "Game/FileQueue.h"
#include "boost/shared_ptr.hpp"
#include "boost/weak_ptr.hpp"
#include "size_assert.h"

using SonicteamUSA::System::CharaColli::AttackCallback;
using SonicteamUSA::System::CharaColli::AttackCallbackParam;
using SonicteamUSA::System::CharaColli::CharaColliAttack;
using SonicteamUSA::System::CharaColli::CharaColliBase;

/* The cursor a parameter's label callback receives (layout as in Helicopter.cpp). */
class UnkLabelQuery_800CBC20 {
  public:
    /* 0x0 */ const GadgetParamDesc *params;
    /* 0x4 */ s32 index; /* the parameter being shown */
    /* 0x8 */ s32 value; /* its current value */
};

/* Target (class name from RTTI): a point characters aim at. Bit 0 of the flag word makes it
 * active (names inferred, as in Bomb.cpp). */
class Target {
  public:
    void TurnOn() {
        m_flags |= 1;
    }
    void TurnOff() {
        m_flags &= ~1;
    }
    /* Moves the target to position (name inferred). */
    void fn_800CCE38(const Vec3f *position);

    /* 0x00 */ u8 field_0x0[0x30];
    /* 0x30 */ u32 m_flags;
};

/* The handle fn_8015B8C4 wraps a collider in (the class name is unknown; layout as in
 * CatchStick.cpp). */
class GadgetCharCollision {
  public:
    /* 0x00 */ boost::function1<void, GadgetColliDetectArgs &> m_onContact;
    /* 0x0C */ u8 field_0xC[0x24 - 0xC];
    /* 0x24 */ CharaColliBase *m_collider;
};

/* The parameters of a debris spawn (layout as in Helicopter.cpp). */
struct UnkDebrisParam_8015F0B8 {
    UnkDebrisParam_8015F0B8(const boost::shared_ptr<CharInfo> &info, int index);

    /* 0x00 */ u8 field_0x0[0x14];
    /* 0x14 */ float field_0x14;
    /* 0x18 */ u8 field_0x18[0x34 - 0x18];
    /* 0x34 */ float scale;
    /* 0x38 */ u8 field_0x38[0x40 - 0x38];
    /* 0x40 */ float field_0x40;
    /* 0x44 */ s32 model;
    /* 0x48 */ u8 field_0x48[0x4A - 0x48];
    /* 0x4A */ u8 field_0x4A;
    /* 0x4B */ u8 field_0x4B;
    /* 0x4C */ s32 field_0x4C;
    /* 0x50 */ u8 field_0x50[0x54 - 0x50];
    /* 0x54 */ u8 field_0x54;
    /* 0x55 */ u8 field_0x55[0x5C - 0x55];
};
SIZE_ASSERT(UnkDebrisParam_8015F0B8, 0x5C);

/* The motion work of a model (struct name from the RTTI of its shared_ptr control block). */
struct _stMtnPlayWork;

/* The RenderWare frame of a model part, and its world matrix (layout from rw/rwcore.h, which the
 * game C++ units do not include; as in TurnOver.cpp). */
struct RwFrame;

struct RwV3d {
    float x;
    float y;
    float z;
};

struct RwMatrix {
    /* 0x00 */ RwV3d right;
    /* 0x0C */ u32 flags;
    /* 0x10 */ RwV3d up;
    /* 0x1C */ u32 pad1;
    /* 0x20 */ RwV3d at;
    /* 0x2C */ u32 pad2;
    /* 0x30 */ RwV3d pos;
    /* 0x3C */ u32 pad3;
};
SIZE_ASSERT(RwMatrix, 0x40);

/* What fn_8015FA78 passes to fn_80169E8C: the id of the model frame to find, and the frame found
 * (as in Rocket.cpp). */
struct UnkFrameSearch_80169E8C {
    UnkFrameSearch_80169E8C() : id(0), frame(0) {}

    /* 0x0 */ u32 id;
    /* 0x4 */ RwFrame *frame;
};
SIZE_ASSERT(UnkFrameSearch_80169E8C, 0x8);

class UnkCharaList_80074194;

extern "C" {
RwMatrix *RwFrameGetLTM(RwFrame *frame);

/* A 32-bit flag word: fn_80014E04 constructs it cleared, fn_80014E1C sets bit, fn_8006127C clears
 * bit and fn_80014E34 tests bit (behaviour from their bodies). */
void fn_80014E04(u32 *flags);
void fn_80014E1C(u32 *flags, int bit);
void fn_8006127C(u32 *flags, int bit);
bool fn_80014E34(u32 *flags, int bit);

/* The switch id the owner was placed with (behaviour inferred). */
void fn_800C9E50(UnkPlacement_800C9CD0 *owner, s32 *id);
/* Tells the owner that the gadget is done (behaviour inferred). */
void fn_8015A794(Gadget *gadget);

/* GadgetModelInfo helpers (behaviour inferred; see Fan.cpp, Door.cpp and Bomb.cpp). */
boost::shared_ptr<GadgetModelInfo> fn_8015F800(int arg0, const char *fileName);
boost::weak_ptr<_stMtnPlayWork> fn_801602B4(GadgetModelInfo *model, int index);
bool fn_80161BD8(GadgetModelInfo *model, int index, const boost::weak_ptr<CharInfo> &info,
                 int arg3);
void fn_80161230(GadgetModelInfo *model, int index, int arg2);
boost::weak_ptr<CharaColliAttack> fn_80161428(GadgetModelInfo *model, int variant);
void fn_80160A84(GadgetModelInfo *model, int index, int motion);
void fn_8016111C(GadgetModelInfo *model, int index, int arg2);
bool fn_801626F4(GadgetModelInfo *model);
void fn_8016271C(GadgetModelInfo *model);
void fn_80162790(GadgetModelInfo *model);
void fn_8015FA78(GadgetModelInfo *model, void *(*callback)(void *part, void *data), void *data);
void *fn_80169E8C(void *part, void *data);
void *fn_80169D00(void *part, void *data);
/* A flag of the motion work (behaviour inferred; see TurnOver.cpp), and a position the motion
 * work reports (behaviour inferred). */
int fn_80416F28(_stMtnPlayWork *work, int arg1, int arg2, int arg3);
void fn_80415DF8(_stMtnPlayWork *work, Vec3f *position);

/* Particle controller helpers (behaviour inferred). */
boost::shared_ptr<GadgetParticle::GadgetParticleCtrl> fn_80166AF4(void);
void fn_80166B70(GadgetParticle::GadgetParticleCtrl *ctrl);
void fn_80166C70(GadgetParticle::GadgetParticleCtrl *ctrl);
void fn_80167010(GadgetParticle::GadgetParticleCtrl *ctrl, int id);
void fn_801672E4(GadgetParticle::GadgetParticleCtrl *ctrl, int id);
void fn_8016761C(GadgetParticle::GadgetParticleCtrl *ctrl, float dt);

/* Creates a target with the given flags, and removes a gadget's target (behaviour inferred). */
boost::shared_ptr<Target> fn_801E72DC(int flags);
void fn_801E7108(boost::shared_ptr<Target> *target);

/* The collider manager; a cylinder collider for charInfo (see Fan.cpp); the handle a collider is
 * wrapped in, re-registered and advanced by dt (behaviour inferred). */
void *fn_80021B08(void);
boost::shared_ptr<CharaColliBase> fn_800C7A84(void *manager,
                                              const boost::weak_ptr<CharInfo> &charInfo, int flags,
                                              int arg3, u16 arg4, const Vec3f *offset,
                                              const Vec3f *rotation, float height,
                                              float radius, int arg9);
boost::shared_ptr<GadgetCharCollision> fn_8015B8C4(boost::shared_ptr<CharaColliBase> collider);
void fn_8015BE64(GadgetCharCollision *handle);
void fn_8015BE9C(GadgetCharCollision *handle, float dt);
/* True when a character stands in the collision event's contacts (behaviour inferred). */
bool fn_8015AC84(ColliDetectArgs &args);

/* The character manager's character by id, and the player's id (see SecretDoor.cpp). */
UnkCharaList_80074194 *fn_80074194(void);
boost::weak_ptr<CharInfo> fn_800A8B74(UnkCharaList_80074194 *manager, int id);
extern int lbl_805EE588;
/* Sends message to the character (behaviour and meaning of message inferred). */
int fn_8020642C(const boost::shared_ptr<CharInfo> &character, int message);

/* Fires a bullet of type for a shot (behaviour inferred; see WeaponSplitter.cpp). */
void fn_80051C34(int type, const Weapon::ShotInfo *info);
/* Throws a weapon of type from position (see ScatteringWeapon.cpp). */
void fn_80221BA8(const Vec3f *position, const Vec3f *direction,
                 const Vec3f *velocity, int type, int arg);
/* Controller vibrations near charInfo (behaviour inferred). */
boost::weak_ptr<Vibration::RequestHandle> fn_80169B80(int type, GadgetCharInfo *charInfo);

/* Plays sound id at a character info, once and as a loop for source (behaviour inferred). */
void fn_801D54F4(void *manager, int id, GadgetCharInfo *source, int arg3, int arg4);
void fn_801D53C4(void *manager, int id, GadgetCharInfo *charInfo, void *source, int arg4, int arg5,
                 int arg6);
/* Debris (see GadgetDebris.h) and the model loader (see BreakObj.cpp). */
void fn_8015D93C(UnkDebrisParam_8015F0B8 *param);
void *fn_800122D4(void);
s32 fn_80012700(void *manager, int arg1, const char *fileName);
}

/* Degrees to radians: an out-of-line function of the game (0x8000AB90). */
float DegToRad(float degrees);

/* A length (name inferred). As a call it is not a constant expression, so the constant below is
 * stored by a static initializer, as in retail (as in SecretDisk.cpp). */
inline float Span(float length) {
    return length;
}

/* Never read in this unit (meaning unknown). */
static float s_unused = Span(500.0f);

namespace BAGunShip {

/* The flag word at 0xA8 (struct name unknown). */
struct BAGunShipFlags {
    BAGunShipFlags() {
        fn_80014E04(&m_bits);
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

    u32 m_bits; /* bit 0: attack, 1: shot warning shown, 2: "second" is 0, 3: loop sound */
};

/* The parameters the owner supplies (names from the parameter table). */
struct BAGunShipParams {
    BAGunShipParams()
        : radius(0.0f), height(0.0f), second(0.0f), attack(0), unused(0.0f), comId(0), num(0),
          delay(0.0f), interval(0.0f) {}

    /* 0x00 */ float radius;
    /* 0x04 */ float height;
    /* 0x08 */ float second;
    /* 0x0C */ s32 attack;
    /* 0x10 */ float unused; /* "N/A" */
    /* 0x14 */ s32 comId;
    /* 0x18 */ s32 num;
    /* 0x1C */ float delay;
    /* 0x20 */ float interval;
};
SIZE_ASSERT(BAGunShipParams, 0x24);

class BAGunShip : public Gadget {
  public:
    BAGunShip(Task *parent, UnkPlacement_800C9CD0 *owner);
    virtual ~BAGunShip();
    virtual void Behave(float dt);
    virtual void ReloadPlacement(UnkPlacement_800C9CD0 *owner);

    /* Creates the gadget under task layer 11 (name inferred). */
    static void Instantiate(UnkPlacement_800C9CD0 *owner);

    /* Copies the owner's placement and parameters and resets the state (name inferred). */
    void ReadPlacement(UnkPlacement_800C9CD0 *owner);
    /* Places the model and the target (name inferred). */
    void UpdatePosition();
    /* The collider's callback (name inferred). */
    void OnColliDetect(ColliDetectArgs &args);
    /* The attack collision's callback (name inferred). */
    void OnAttack(AttackCallbackParam &param);
    /* Breaks the gunship into its debris (name inferred). */
    void Break();
    /* Fires at the player; returns the time to the next shot (name inferred). */
    float Shoot();

    static const char *s_fileName;
    static const char *s_debrisFileName;
    static const char *s_taskName;

    /* Member names are inferred. */
    /* 0x30 */ boost::shared_ptr<GadgetCharInfo> m_chara;
    /* 0x38 */ boost::shared_ptr<GadgetModelInfo> m_model;
    /* 0x40 */ boost::shared_ptr<GadgetParticle::GadgetParticleCtrl> m_particles;
    /* 0x48 */ boost::shared_ptr<GadgetCharCollision> m_handle;
    /* 0x50 */ boost::shared_ptr<Target> m_target;
    /* 0x58 */ s32 m_state;
    /* 0x5C */ BAGunShipParams m_params;
    /* 0x80 */ s32 m_switchId;
    /* 0x84 */ boost::weak_ptr<_stMtnPlayWork> m_motion;
    /* 0x8C */ float m_timer;
    /* 0x90 */ float m_hitCooldown;
    /* 0x94 */ float m_health;
    /* 0x98 */ boost::shared_ptr<SetGenerator::Manager> m_generator;
    /* 0xA0 */ float m_shotTimer;
    /* 0xA4 */ RwFrame *m_muzzle;
    /* 0xA8 */ BAGunShipFlags m_flags2;
    /* 0xAC */ u32 pad_0xAC; /* the class size is a multiple of 8 */
};
SIZE_ASSERT(BAGunShip, 0xB0);

const char *BAGunShip::s_fileName = "BAGUNSHIP.BIN";
const char *BAGunShip::s_debrisFileName = "DM0000GUNSHIPBRK.DFF";
const char *BAGunShip::s_taskName = "BAGunShip";

BAGunShip::BAGunShip(Task *parent, UnkPlacement_800C9CD0 *owner) : Gadget(parent, owner) {
    m_name = s_taskName;
    m_chara = fn_8015CD90(0x8D, 4);
    ReadPlacement(owner);
    m_model = fn_8015F800(1, s_fileName);
    m_model->fn_80162A68(0, 0);
    m_model->fn_80160C28(0);
    m_model->fn_8015FF60();
    m_motion = fn_801602B4(m_model.px, 0);
    fn_80161BD8(m_model.px, 0, m_chara, 0);
    fn_80161230(m_model.px, 0, -1);
    boost::weak_ptr<CharaColliAttack> attack = fn_80161428(m_model.px, 0);
    if (boost::shared_ptr<CharaColliAttack> collision = attack.lock()) {
        collision->field_0x54 = 0;
        collision->field_0x55 = 1;
        collision->m_attackCallback = AttackCallback(boost::bind(&BAGunShip::OnAttack, this, _1));
    }
    {
        Vec3f offset(0.0f, 0.0f, 0.0f);
        Vec3f rotation(DegToRad(90.0f), 0.0f, 0.0f);
        boost::shared_ptr<CharaColliBase> collider =
            fn_800C7A84(fn_80021B08(), m_chara, 0x40100, 10, 0, &offset, &rotation,
                        m_params.height, m_params.radius, 0);
        m_handle = fn_8015B8C4(collider);
        fn_800F5980(m_handle.px, boost::bind(&BAGunShip::OnColliDetect, this, _1));
    }
    {
        UnkFrameSearch_80169E8C search;
        search.id = 7;
        search.frame = 0;
        fn_8015FA78(m_model.px, fn_80169E8C, &search);
        m_muzzle = search.frame;
    }
    if (m_model.px != 0) {
        m_particles = fn_80166AF4();
        UnkParticleAttach_80169D00 ref;
        ref.ctrl = m_particles;
        fn_8015FA78(m_model.px, fn_80169D00, &ref);
    }
    m_target = fn_801E72DC(0x800);
    m_target->TurnOff();
    UpdatePosition();
}

BAGunShip::~BAGunShip() {
    fn_801E7108(&m_target);
}

void BAGunShip::Behave(float dt) {
    if (m_model.px != 0) {
        m_model->fn_80162FB0(dt);
    }
    if (m_handle.px != 0) {
        fn_8015BE9C(m_handle.px, dt);
    }
    if (m_particles.px != 0) {
        fn_8016761C(m_particles.px, dt);
    }
    m_hitCooldown -= dt;
    if (m_hitCooldown < 0.0f) {
        m_hitCooldown = 0.0f;
    }
    m_shotTimer -= dt;
    if (m_shotTimer < 0.0f) {
        m_shotTimer = 0.0f;
    }
    if (m_flags2.Test(3)) {
        fn_801D53C4(fn_80053164(), 0x792F, m_chara.px, this, 3, 0, 2);
    }

    switch (m_state) {
    case 0:
        if (m_switchId > 0) {
            if (fn_8016904C(fn_800CB69C(), m_switchId) == 1) {
                m_state = 2;
            }
        } else {
            m_state = 2;
        }
        if (!fn_801626F4(m_model.px)) {
            fn_80162790(m_model.px);
            fn_80166C70(m_particles.px);
        }
        {
            UnkGeneratorParam_8027C3C4 param;
            param.comId = m_params.comId;
            param.num = m_params.num;
            param.delay = m_params.delay;
            m_generator = fn_8027C3C4(&param);
        }
        break;
    case 1:
        if (fn_801626F4(m_model.px)) {
            m_timer -= dt;
            if (m_timer <= 0.0f) {
                fn_8016271C(m_model.px);
                fn_8016111C(m_model.px, 0, -1);
                fn_80166B70(m_particles.px);
                if (m_particles.px != 0) {
                    fn_801672E4(m_particles.px, -1);
                    fn_80167010(m_particles.px, -1);
                }
                fn_801D54F4(fn_80053164(), 0x792E, m_chara.px, 0, 1);
                m_flags2.Set(3);
                m_target->TurnOn();
            }
        }
        if (!m_motion.expired()) {
            if (boost::shared_ptr<_stMtnPlayWork> work = m_motion.lock()) {
                if (fn_80416F28(work.px, 0, 3, 1) != 0) {
                    if (m_flags2.Test(2)) {
                        m_state = 4;
                    } else {
                        m_state = 3;
                    }
                    fn_80160A84(m_model.px, 0, 2);
                    if (m_generator.px != 0) {
                        fn_8027C67C(m_generator.px);
                    }
                    m_timer = m_params.second;
                }
            }
        }
        break;
    case 3:
        m_timer -= dt;
        if (m_timer <= 0.0f) {
            fn_80160A84(m_model.px, 0, 3);
            m_state = 5;
            if (m_generator.px != 0) {
                fn_8027C650(m_generator.px);
            }
        } else if (m_flags2.Test(0)) {
            if (m_shotTimer <= 0.0f) {
                m_shotTimer = Shoot();
                m_flags2.Clear(1);
            } else if (m_shotTimer <= 1.0f && !m_flags2.Test(1)) {
                m_flags2.Set(1);
                if (m_particles.px != 0) {
                    fn_801672E4(m_particles.px, 1);
                }
            }
        }
        break;
    case 4:
        if (m_flags2.Test(0)) {
            if (m_shotTimer <= 0.0f) {
                m_shotTimer = Shoot();
                m_flags2.Clear(1);
            } else if (m_shotTimer <= 1.0f && !m_flags2.Test(1)) {
                m_flags2.Set(1);
                if (m_particles.px != 0) {
                    fn_801672E4(m_particles.px, 1);
                }
            }
        }
        break;
    case 5:
        if (!m_motion.expired()) {
            if (boost::shared_ptr<_stMtnPlayWork> work = m_motion.lock()) {
                if (fn_80416F28(work.px, 0, 3, 1) != 0) {
                    m_state = 7;
                    m_flags2.Clear(3);
                }
            }
        }
        break;
    case 6: {
        Break();
        Vec3f position;
        fn_800091EC(&position, &m_chara->m_position);
        Vec3f rotation = m_chara->m_rotation;
        Vec3f velocity(0.0f, 1.0f, 0.0f);
        fn_80221BA8(&position, &rotation, &velocity, 0x10, -1);
        if (m_generator.px != 0) {
            fn_8027C650(m_generator.px);
        }
        fn_80169B80(3, m_chara.px);
        fn_8015A794(this);
        m_flags |= 1;
        break;
    }
    case 7:
        fn_8015A794(this);
        m_flags |= 1;
        break;
    }

    if (m_generator.px != 0) {
        fn_8027C6A8(m_generator.px, dt);
    }
    UpdatePosition();
}

void BAGunShip::ReloadPlacement(UnkPlacement_800C9CD0 *owner) {
    ReadPlacement(owner);
    UpdatePosition();
    if (m_model.px != 0) {
        m_model->fn_80162EF4();
    }
    if (m_handle.px != 0) {
        m_handle->m_collider->SetSize(m_params.radius, 0.0f, m_params.height, -2);
        fn_8015BE64(m_handle.px);
    }
}

void BAGunShip::ReadPlacement(UnkPlacement_800C9CD0 *owner) {
    fn_800C9F14(owner, &m_chara->m_position);
    fn_800C9ED4(owner, &m_chara->m_rotation);
    fn_800C9DBC(owner, &m_params, sizeof(m_params));
    fn_800C9E50(owner, &m_switchId);
    m_state = 0;
    m_timer = 0.0f;
    m_hitCooldown = 0.0f;
    m_health = 40.0f;
    m_shotTimer = m_params.interval;
    if (m_params.attack != 0) {
        m_flags2.Set(0);
    } else {
        m_flags2.Clear(0);
    }
    if (m_params.second <= 0.0f) {
        m_flags2.Set(2);
    } else {
        m_flags2.Clear(2);
    }
}

void BAGunShip::UpdatePosition() {
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

void BAGunShip::OnColliDetect(ColliDetectArgs &args) {
    if (m_state == 2 && fn_8015AC84(args)) {
        m_timer = 1.0f;
        m_state = 1;
        fn_80160A84(m_model.px, 0, 1);
        UpdatePosition();
    }
}

void BAGunShip::OnAttack(AttackCallbackParam &param) {
    param.m_result = 5;
    if (m_hitCooldown > 0.0f) {
        return;
    }
    if (m_state == 3 || m_state == 4) {
        if (boost::shared_ptr<CharaColliAttack> attack = param.m_attacker.lock()) {
            if ((attack->m_flags & 0x3D0) == 0) {
                return;
            }
            m_health -= param.m_damage;
            m_hitCooldown = 0.5f;
            if (m_health > 0.0f) {
                param.m_result = 1;
                fn_801D54F4(fn_80053164(), 0x7931, m_chara.px, 0, 1);
            } else {
                param.m_result = 3;
                m_state = 6;
                if (boost::shared_ptr<CharInfo> attacker = attack->m_owner.lock()) {
                    fn_8020642C(attacker, 0x4E);
                }
                fn_801D54F4(fn_80053164(), 0x7932, m_chara.px, 0, 1);
            }
        }
    }
}

void BAGunShip::Break() {
    s32 model = fn_80012700(fn_800122D4(), 1, s_debrisFileName);
    if (model != 0) {
        Vec3f position(0.0f, 0.0f, 0.0f);
        boost::weak_ptr<_stMtnPlayWork> motion = fn_801602B4(m_model.px, 0);
        if (boost::shared_ptr<_stMtnPlayWork> work = motion.lock()) {
            fn_80415DF8(work.px, &position);
        }
        UnkDebrisParam_8015F0B8 debris(m_chara, 0);

        debris.scale = 1.3f;
        debris.field_0x14 = position.y;
        debris.model = model;
        debris.field_0x54 = 1;
        debris.field_0x4C = 0x96;
        debris.field_0x40 = m_chara->m_position.y;
        debris.field_0x4A = 0;
        fn_8015D93C(&debris);
    }
}

float BAGunShip::Shoot() {
    if (m_muzzle != 0) {
        RwMatrix *ltm = RwFrameGetLTM(m_muzzle);
        Vec3f muzzle(ltm->pos.x, ltm->pos.y, ltm->pos.z);
        Vec3f target(0.0f, 0.0f, 0.0f);
        boost::weak_ptr<CharInfo> player = fn_800A8B74(fn_80074194(), lbl_805EE588);
        if (boost::shared_ptr<CharInfo> character = player.lock()) {
            fn_800091D0(&target, &character->m_position);
        }
        Vec3f direction;
        {
            Vec3f delta;
            fn_8040CD74(&delta, &target, &muzzle);
            fn_800091EC(&direction, &delta);
        }
        if (direction.y > 0.0f) {
            direction.y = 0.0f;
        }
        Weapon::ShotInfo shot(m_chara, true, muzzle, direction, Vec3f(0.0f, 0.0f, 0.0f));
        fn_80051C34(0x1D, &shot);
        fn_801D54F4(fn_80053164(), 0x7930, m_chara.px, 0, 1);
    }
    return m_params.interval;
}

} // namespace BAGunShip

extern "C" void fn_802FE2A8(void) {
    fn_8006B474(BAGunShip::BAGunShip::s_fileName, 1);
}

extern "C" void fn_802FE2A4(void) {}

namespace BAGunShip {

void BAGunShip::Instantiate(UnkPlacement_800C9CD0 *owner) {
    Task *parent = fn_801EB304(fn_8001C4F8(), 11);
    new BAGunShip(parent, owner);
}

} // namespace BAGunShip

extern "C" const char *fn_802FE218(UnkLabelQuery_800CBC20 *cursor) {
    static const char *s_attackLabels[2] = {"attack OFF", "attack ON"};

    if (cursor->index == 3) {
        if (cursor->value == 0) {
            return s_attackLabels[0];
        }
        return s_attackLabels[1];
    }
    return 0;
}

static GadgetParamDesc lbl_8055A888[] = {
    {4, "radius", 0, 0, 0, 0.0f, 0.0f, 65536.0f, fn_800CBC20, 0},
    {4, "height", 0, 0, 0, 0.0f, 0.0f, 65536.0f, fn_800CBC20, 0},
    {4, "second", 0, 0, 0, 0.0f, 0.0f, 65536.0f, fn_800CBC20, 0},
    {1, "attack", 0, 0, 1, 0.0f, 0.0f, 0.0f, fn_802FE218, 0},
    {4, "N/A", 0, 0, 0, 0.0f, 0.0f, 65536.0f, fn_800CBC20, 0},
    {1, "ComId", -1, -1, 0xFF, 0.0f, 0.0f, 0.0f, fn_800CBC20, 0},
    {1, "num", 0, 0, 0x10000, 0.0f, 0.0f, 0.0f, fn_800CBC20, 0},
    {4, "delay", 0, 0, 0, 0.0f, 0.0f, 65536.0f, fn_800CBC20, 0},
    {4, "interval", 0, 0, 0, 0.0f, 0.0f, 65536.0f, fn_800CBC20, 0},
    {0},
};

GadgetClassDesc lbl_8055AA18 = {
    "BA GUNSHIP", fn_802FE2A8, fn_802FE2A4, BAGunShip::BAGunShip::Instantiate, 0, 0, 0x17D4, 1, 0, 10,
    lbl_8055A888,
};
