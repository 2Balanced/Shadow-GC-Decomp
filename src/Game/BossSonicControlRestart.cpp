/* @unnamed@BossSonicControlRestart_cpp@::ControlRestart (class name from RTTI; its name is
 * "BossSonicRestart"): the control Boss Sonic's AI (ControlAI) switches to to bring the boss back
 * into the arena. Once fn_80073908 of the NPC reaches 1, it puts the NPC on whichever of four
 * points around the arena centre is farthest from the boss's target, facing along the zero angle,
 * and switches the NPC back to control 2. The file name comes from the RTTI name. Function and
 * member names are inferred. Definitions are in reverse address order (deferred inlining emits
 * functions last-defined first). */
#include "Game/BossSonic.h"
#include "Game/MathTypes.h"
#include "Game/Npc.h"
#include "Game/Player.h"
#include "Game/PlayerControl.h"
#include "Game/Task.h"
#include "boost/shared_ptr.hpp"
#include "boost/weak_ptr.hpp"
#include "size_assert.h"

extern "C" {
/* The distance between two points (behaviour inferred). */
float fn_8040D180(const Vec3f *a, const Vec3f *b);
/* Puts chara at position, turned to angle, and resets its behaviour (behaviour inferred from its
 * commands). */
void fn_800B8000(const boost::weak_ptr<CharInfo> &chara, const Vec3f &position, const Vec3f &angle);
/* NPC helpers (behaviour unknown; fn_80073908 returns a time or ratio the control waits on). */
float fn_80073908(Player::Npc::Npc *npc);
void fn_8007393C(Player::Npc::Npc *npc);
void fn_80073964(Player::Npc::Npc *npc);
/* Switches the control of the character task (see Game/BossSonicExecuter.cpp). */
void fn_8009EBA4(Task *task, const Player::Control::InitializeArgs *args);
}

using boost::shared_ptr;
using Player::Npc::Npc;
using Player::Npc::BossSonic::BossSonicWork;

/* The player's weak pointer to itself (at 0x260; as in Game/BossSonicExecuter.cpp). */
inline const boost::weak_ptr<Player::PlayerBase> &GetSelf(Player::PlayerBase *player) {
    return *reinterpret_cast<boost::weak_ptr<Player::PlayerBase> *>(reinterpret_cast<u8 *>(player) +
                                                                     0x260);
}

/* The four points the boss is put back at (names inferred). The original copy-constructs them
 * from temporaries through the vector type's out-of-line copy constructor, fn_800091EC; this
 * wrapper's constructor writes those copies out so that the startup code stays the same (as in
 * Game/Cage.cpp). */
static struct RestartPoints {
    RestartPoints() {
        float point3[3];
        float point2[3];
        float point1[3];
        float point0[3];

        point0[0] = 400.0f;
        point0[1] = 20.0f;
        point0[2] = 0.0f;
        fn_800091EC(reinterpret_cast<Vec3f *>(m_points[0]), reinterpret_cast<Vec3f *>(point0));
        point1[0] = -400.0f;
        point1[1] = 20.0f;
        point1[2] = 0.0f;
        fn_800091EC(reinterpret_cast<Vec3f *>(m_points[1]), reinterpret_cast<Vec3f *>(point1));
        point2[0] = 0.0f;
        point2[1] = 20.0f;
        point2[2] = 400.0f;
        fn_800091EC(reinterpret_cast<Vec3f *>(m_points[2]), reinterpret_cast<Vec3f *>(point2));
        point3[0] = 0.0f;
        point3[1] = 20.0f;
        point3[2] = -400.0f;
        fn_800091EC(reinterpret_cast<Vec3f *>(m_points[3]), reinterpret_cast<Vec3f *>(point3));
    }

    /* 0x00 */ float m_points[4][3];
} lbl_80585788;

static const char *s_name = "BossSonicRestart";

namespace {

class ControlRestart : public Player::Control::ControlBase {
  public:
    ControlRestart(const shared_ptr<BossSonicWork> &work);
    virtual ~ControlRestart();
    virtual void vf_0x0C();
    virtual void vf_0x10();
    virtual void vf_0x14(float dt);

    /* 0x14 */ shared_ptr<BossSonicWork> m_work;
};
SIZE_ASSERT(ControlRestart, 0x1C);

ControlRestart::ControlRestart(const shared_ptr<BossSonicWork> &work)
    : ControlBase(work->m_npc.lock(), s_name, 4), m_work(work) {}

ControlRestart::~ControlRestart() {}

void ControlRestart::vf_0x0C() {
    shared_ptr<Npc> npc = m_work->m_npc.lock();
    fn_80073964(npc.get());
}

void ControlRestart::vf_0x10() {}

} // namespace

/* The point farthest from the boss's target, or the first when there is none (returned through
 * out; name inferred). */
static void GetRestartPoint(Vec3f *out, ControlRestart *control);
/* GetRestartPoint called as returning the point by value. */
typedef Vec3f (*RestartPointFn)(ControlRestart *control);

void ControlRestart::vf_0x14(float dt) {
    shared_ptr<Npc> npc = m_work->m_npc.lock();
    if (fn_80073908(npc.get()) >= 1.0f) {
        fn_800B8000(GetSelf(npc.get()), ((RestartPointFn)GetRestartPoint)(this),
                    Vec3f(0.0f, 0.0f, 0.0f));
        fn_8007393C(npc.get());
        fn_8009EBA4(npc->m_task, &Player::Control::InitializeArgs(2));
    }
}

static void GetRestartPoint(Vec3f *out, ControlRestart *control) {
    if (control->m_work->m_target.expired()) {
        fn_800091EC(out, reinterpret_cast<Vec3f *>(lbl_80585788.m_points[0]));
        return;
    }
    shared_ptr<Npc> npc = control->m_work->m_npc.lock();
    shared_ptr<CharInfo> target = control->m_work->m_target.lock();
    float farthest = -1.0f;
    int index = 0;
    for (int i = 0; i < 4; i++) {
        float distance =
            fn_8040D180(&target->m_position, reinterpret_cast<Vec3f *>(lbl_80585788.m_points[i]));
        if (farthest < distance) {
            farthest = distance;
            index = i;
        }
    }
    fn_800091EC(out, reinterpret_cast<Vec3f *>(lbl_80585788.m_points[index]));
}

/* Creates the control for work (called by ControlAI; name inferred). */
extern "C" Player::Control::ControlBase *fn_8033D79C(const shared_ptr<BossSonicWork> &work) {
    return new ControlRestart(work);
}
