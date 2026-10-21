/* Player::Npc::NpcMotion (class name from RTTI): the base of the non-player characters' motions
 * (the MotionImpl classes of Sonic, Tails, Knuckles, ...). It keeps the NPC, moves it by the
 * motion's root, places the motion and the model on it, finds the skeleton nodes and creates the
 * model's parts. The file name and all function and member names are inferred. Definitions are in
 * reverse address order (deferred inlining emits functions last-defined first). */
#include "Game/DevilDoomCtrlUnit.h"
#include "Game/MathTypes.h"
#include "Game/Npc.h"
#include "Game/NpcMotion.h"
#include "Game/NpcPartModel.h"
#include "Game/PlayerWeaponBase.h"
#include "boost/shared_ptr.hpp"
#include <dolphin/types.h>

/* The stage (class name unknown; fn_8003A840 returns it): the world of each of its areas
 * (behaviour inferred). */
class UnkStage_8003A840 {
  public:
    void *GetWorld(int area);
};

extern "C" {
/* PlayerMotion's own functions (behaviour inferred). */
void fn_800A9EA0(Player::PlayerMotion *motion);
void fn_800AA0D4(Player::PlayerMotion *motion);
void fn_800AA404(Player::PlayerMotion *motion);
float fn_800A9EF8(Player::PlayerMotion *motion);
void *fn_800AA584(Player::PlayerMotion *motion);

/* The resource loader: a resource's state and a clump of it (see Game/BossRender.cpp). */
void *fn_800122D4(void);
int fn_80012408(void *loader, void *resource);
void *fn_8001277C(void *loader, void *resource, int index);

/* RenderWare clumps (see Game/BossRender.cpp), and the stage (behaviour inferred). */
void *fn_8046256C(RpClump *clump);
void fn_804623EC(void *world, RpClump *clump);
void fn_804622FC(void *world, RpClump *clump);
RpClump *fn_80457090(void *data);
void fn_80457468(RpClump *clump);
void *fn_80169364(RpClump *clump);
int fn_80169350(void *frame);
UnkStage_8003A840 *fn_8003A840(void);
void fn_80038198(RpClump *clump, void *fade);

/* Motions (see Game/DevilDoomCtrlUnit.cpp; behaviour inferred). */
void fn_80415E34(_stMtnPlayWork *work, Vec3f *position);
void fn_80415BDC(_stMtnPlayWork *work, const Vec3f *position);
void fn_8041734C(_stMtnPlayWork *work, const Mtx34f *matrix);
void fn_80416DC8(_stMtnPlayWork *work, int arg1, float dt);
void fn_80416E20(_stMtnPlayWork *work, int arg1, float frame);
void *fn_80415E70(_stMtnPlayWork *work, int id);
void fn_800163F4(Mtx34f *out, const Mtx34f *matrix); /* a copy of matrix */

/* The player's state bits (see Game/PlayerShadowWeapon.cpp). */
bool fn_8006CD78(u32 *bits, int bit);
void fn_80076CA8(u32 *bits, int bit);

/* Math (see Game/DevilDoomCtrlUnit.cpp). */
void fn_8040BAC4(Mtx34f *matrix, const Mtx34f *other);
void fn_8040FA20(Mtx34f *matrix, const Vec3f *translation);
Vec3f fn_8040B67C(const Mtx34f *matrix, const Vec3f *point);
Vec3f fn_8040CDD0(const Vec3f *a, const Vec3f *b);
}

/* fn_8040CD74 called as returning a - b by value (see Game/ArkCannon.cpp). */
typedef Vec3f (*VectorSubtractFn)(const Vec3f *a, const Vec3f *b);

/* m becomes m * other (name inferred; as in Game/PlayerBehaviorFirstMeet.cpp). */
inline void MultiplyMtx(Mtx34f *m, const Mtx34f &other) {
    fn_8040BAC4(m, &other);
}

/* Passes a returned vector on by address (as in Game/ArkCannon.cpp's casts). */
inline const Vec3f &Ref(const Vec3f &vector) {
    return vector;
}

/* Puts clump in world (name inferred; as in Game/BossRender.cpp). */
inline void SetClumpWorld(RpClump *clump, void *world) {
    void *current = fn_8046256C(clump);
    if (current != world) {
        if (current != 0) {
            fn_804623EC(current, clump);
        }
        fn_804622FC(world, clump);
    }
}

/* The stage and the area clump is in (names inferred). */
inline UnkStage_8003A840 *GetStage() {
    return fn_8003A840();
}

inline int GetClumpArea(RpClump *clump) {
    return fn_80169350(fn_80169364(clump));
}

namespace Player {
namespace Npc {

NpcMotion::NpcMotion(Task *parent, const boost::shared_ptr<Npc> &npc)
    : PlayerMotion(parent, npc), m_npc(npc) {}

NpcMotion::~NpcMotion() {}

bool NpcMotion::UpdateVisibility() {
    bool visible = true;
    if (m_npc.get()->IsWeaponHidden()) {
        visible = false;
    }
    if (fn_8006CD78(m_npc.get()->m_stateBits, 0x18)) {
        if (++m_blinkCount & 1) {
            visible = false;
        }
    }
    if (visible) {
        Show();
    } else {
        Hide();
    }
    return visible;
}

bool NpcMotion::LoadWhenReady() {
    void *loader = fn_800122D4();
    if (fn_80012408(loader, fn_80078C4C(m_npc.get())) == 3) {
        Load();
        fn_80076CA8(m_npc.get()->m_stateBits, 0x47);
        fn_80076CA8(m_npc.get()->m_stateBits, 0x46);
        int motion = m_motionId;
        if (motion != 0x30) {
            m_motionId = 0x30;
            vf_0x10(motion, 0.0f);
        }
        return true;
    }
    return false;
}

void NpcMotion::BindNodes(const int *nodes) {
    if (m_work) {
        for (int i = 0; i < 14; i++) {
            const Mtx34f *matrix;
            u8 valid = 1;
            switch (nodes[i]) {
            case -1:
                matrix = &m_bodyMatrix;
                break;
            case -2:
                matrix = &m_baseMatrix;
                break;
            default:
                matrix = (const Mtx34f *)fn_80415E70(m_work, nodes[i]);
                if (!matrix) {
                    valid = 0;
                }
                break;
            }
            m_nodes[i].m_matrix = matrix;
            m_nodes[i].m_valid = valid;
        }
    }
}

void NpcMotion::PlaceOnNpc(const Vec3f &offset) {
    Mtx34f matrix;
    Vec3f root;
    Vec3f body;
    fn_800AA404(this);
    fn_80415E34(m_work, &root);
    fn_800091D0(&root,
                &Ref(((VectorSubtractFn)fn_8040CD74)(
                    &Ref(fn_8040CDD0(&reinterpret_cast<UnkNpcMotionFields_801FD130 *>(m_npc.get())->m_position,
                                     &offset)),
                    &root)));
    fn_80415BDC(m_work, &root);
    fn_8040B6F8(&m_baseMatrix);
    MultiplyMtx(&m_baseMatrix, ((UnkNpcMotionFields_801FD130 *)fn_8006CA70(&m_npc))->m_modelMatrix);
    fn_8040B848(&m_baseMatrix, reinterpret_cast<UnkNpcMotionFields_801FD130 *>(m_npc.get())->m_rotation.y);
    fn_8040B884(&m_baseMatrix, reinterpret_cast<UnkNpcMotionFields_801FD130 *>(m_npc.get())->m_rotation.x);
    fn_8040B80C(&m_baseMatrix, reinterpret_cast<UnkNpcMotionFields_801FD130 *>(m_npc.get())->m_rotation.z);
    fn_8040C148(&m_bodyMatrix, &m_baseMatrix);
    fn_800163F4(&matrix, &m_baseMatrix);
    fn_8041734C(m_work, &matrix);
    /* The offset is 1.2 and -1.6 rounded down (retail's float bits are 0x3F999999 and
     * 0xBFCCCCCC). */
    fn_800091D0(&body, &Ref(fn_8040B67C(&m_bodyMatrix, &Vec3f(0.0f, 1.19999993f, -1.5999999f))));
    fn_8040FA20(&m_bodyMatrix,
                &Ref(fn_8040CDD0(&reinterpret_cast<UnkNpcMotionFields_801FD130 *>(m_npc.get())->m_position,
                                 &body)));
    fn_8040FA20(&m_baseMatrix, &reinterpret_cast<UnkNpcMotionFields_801FD130 *>(m_npc.get())->m_position);
    m_frameTime = fn_800A9EF8(this);
    fn_80416DC8(m_work, 0, m_frameTime);
    if (m_startFrame >= 0.0f) {
        fn_80416E20(m_work, 0, m_startFrame);
        m_startFrame = -1.0f;
    }
}

void NpcMotion::MoveByRoot(const Vec3f &offset) {
    fn_800A9EA0(this);
    Vec3f root;
    fn_80415E34(m_work, &root);
    fn_800091D0(&reinterpret_cast<UnkNpcMotionFields_801FD130 *>(m_npc.get())->m_velocity,
                &Ref(((VectorSubtractFn)fn_8040CD74)(
                    &Ref(((VectorSubtractFn)fn_8040CD74)(&root, &offset)),
                    &reinterpret_cast<UnkNpcMotionFields_801FD130 *>(m_npc.get())->m_position)));
    fn_800091D0(&reinterpret_cast<UnkNpcMotionFields_801FD130 *>(m_npc.get())->m_position,
                &Ref(((VectorSubtractFn)fn_8040CD74)(&root, &offset)));
    fn_800AA0D4(this);
}

UnkClumpUpdater_80015A7C *NpcMotion::CreatePart(boost::shared_ptr<PlayerBase> player,
                                                _stMtnPlayWork *work, void *model, int index) {
    RpClump *clump = (RpClump *)fn_8001277C(fn_800122D4(), model, index);
    UnkClumpUpdater_80015A7C *part = 0;
    if (work && clump) {
        clump = fn_80457090(clump);
        part = new UnkClumpUpdater_80015A7C(clump, work);
        UnkStage_8003A840 *stage = fn_8003A840();
        int area = fn_80169350(fn_80169364(clump));
        SetClumpWorld(clump, stage->GetWorld(area));
        if (player.get()->m_motion) {
            fn_80038198(clump, fn_800AA584(player.get()->m_motion));
        }
    }
    return part;
}

void NpcMotion::DestroyPart(UnkClumpUpdater_80015A7C *part) {
    if (part) {
        RpClump *clump = *reinterpret_cast<RpClump **>(part);
        void *world = fn_8046256C(clump);
        if (world) {
            fn_804623EC(world, clump);
        }
        delete part;
        fn_80457468(clump);
    }
}

} // namespace Npc
} // namespace Player
