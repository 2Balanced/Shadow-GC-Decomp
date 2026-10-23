/* Player::Shadow::MotionSub (class name from RTTI): a task that clones up to three models of the
 * player's model file and keeps each on a node of the player's skeleton, for the character types
 * 0x10 to 0x13. The file name comes from the RTTI name; function, member and helper names are
 * inferred. Definitions are in reverse address order (deferred inlining emits functions
 * last-defined first). */
#include "Game/MathTypes.h"
#include "Game/Player.h"
#include "Game/PlayerShadowMotionSub.h"
#include "Game/Task.h"
#include "boost/scoped_ptr.hpp"
#include "boost/shared_ptr.hpp"
#include "size_assert.h"

/* RenderWare's vector, matrix and frame, and the start of a clump (see Game/DebugDispShape.h). */
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

struct RwFrame {
    /* 0x00 */ u8 object[0x10];
    /* 0x10 */ RwMatrix modelling;
};

struct RpClump {
    /* 0x0 */ u8 object[0x4];
    /* 0x4 */ RwFrame *frame;
};

/* The world of each stage area (as in Game/BlackDoomCtrlUnit.cpp). */
struct UnkWorldEntry_8051D208 {
    /* 0x00 */ u8 field_0x0[0x10];
    /* 0x10 */ void *m_world;
    /* 0x14 */ u8 field_0x14[0x18 - 0x14];
};
extern UnkWorldEntry_8051D208 lbl_8051D208[];

/* A skeleton node (see Game/PlayerWeaponBase.h). */
struct UnkMotionNode_800AA760 {
    /* 0x00 */ const Mtx34f *m_matrix;
};

extern "C" {
/* The resource loader, and a model of a loaded file (see Game/BossMotion.cpp). */
void *fn_800122D4(void);
void *fn_8001277C(void *loader, void *file, int index);
/* The player's model file (see Game/NpcPartModel.h). */
void *fn_80078C4C(Player::PlayerBase *player);
/* A node of a motion, and the motion's material fade (see Game/NpcPartModel.h and
 * Game/BlackDoomCtrlUnit.cpp). */
UnkMotionNode_800AA760 *fn_800AA760(Player::PlayerMotion *motion, int index);
void *fn_800AA584(Player::PlayerMotion *motion);
void fn_80038198(RpClump *clump, void *fade);
/* Clones and destroys a clump (see Game/BlackDoomCtrlUnit.cpp). */
RpClump *fn_80457090(void *data);
void fn_80457468(RpClump *clump);
/* The clump's world, and taking a clump out of and into a world (see
 * Game/BlackDoomCtrlUnit.cpp). */
void *fn_8046256C(RpClump *clump);
void fn_804623EC(void *world, RpClump *clump);
void fn_804622FC(void *world, RpClump *clump);
/* The current stage area, and the area of a clump (see Game/BlackDoomCtrlUnit.cpp). */
void fn_8003A840(void);
void *fn_80169364(RpClump *clump);
int fn_80169350(void *frame);
/* Adds a clump to a clump list (see Game/NpcEggMonitorMotion.cpp). */
void fn_8006CAE4(Player::UnkClumpList_8006CAE4 *list, RpClump *const *clump);
/* The RenderWare matrix of a matrix, and RwFrameUpdateObjects (see Game/DebugDispShape.h). */
RwMatrix fn_800591F0(const Mtx34f *matrix);
RwFrame *fn_8048722C(RwFrame *frame);
}

/* An extra model: the skeleton node it follows (0xE or more: none) and its index in the player's
 * model file (struct and member names inferred). */
struct UnkMotionSubPart {
    s32 m_node;
    s32 m_model;
};

/* The models of character types 0x10 to 0x13. */
static const UnkMotionSubPart s_parts[4][3] = {
    {{2, 5}, {5, 8}, {5, 9}},
    {{2, 5}, {5, 8}, {5, 9}},
    {{2, 5}, {5, 8}, {0xE, -1}},
    {{2, 5}, {5, 8}, {0xE, -1}},
};

namespace Player {
namespace Shadow {

/* Moves clump into world (as in Game/BlackDoomCtrlUnit.cpp). */
inline void SetClumpWorld(RpClump *clump, void *world) {
    void *current = fn_8046256C(clump);
    if (current != world) {
        if (current != 0) {
            fn_804623EC(current, clump);
        }
        fn_804622FC(world, clump);
    }
}

/* The task's state (class name unknown). */
class MotionSubImpl {
  public:
    MotionSubImpl(const boost::shared_ptr<PlayerBase> &player, int kind);
    ~MotionSubImpl();
    /* Puts the models on their nodes. */
    void Tick();
    void AddClumps(UnkClumpList_8006CAE4 *list);
    /* Adds the models to the world of their stage area. */
    void Attach();
    /* Takes the models out of their worlds. */
    void Detach() {
        for (int i = 0; i < 3; i++) {
            if (m_clumps[i] != 0) {
                void *world = fn_8046256C(m_clumps[i]);
                if (world != 0) {
                    fn_804623EC(world, m_clumps[i]);
                }
            }
        }
    }

    /* 0x00 */ boost::shared_ptr<PlayerBase> m_player;
    /* 0x08 */ UnkMotionNode_800AA760 *m_nodes[3];
    /* 0x14 */ RpClump *m_clumps[3];
};

/* Deletes the state (name inferred; see checked_delete below). */
inline void DeleteMotionSubImpl(MotionSubImpl *impl) {
    delete impl;
}

/* The character type (the word at 0x64; name inferred, as in Game/PlayerNpcTarget.cpp). */
inline s32 CharaType(PlayerBase *player) {
    return *reinterpret_cast<s32 *>(player->field_0x64);
}

} // namespace Shadow
} // namespace Player

/* Retail keeps boost::checked_delete<MotionSubImpl> out of line and the destructor at its own
 * place; this specialization gives that order under GC/2.7 (see docs/research-notes.md and
 * Game/EnergyCore.cpp). */
namespace boost {
template <> inline void checked_delete<Player::Shadow::MotionSubImpl>(Player::Shadow::MotionSubImpl *x) {
    Player::Shadow::DeleteMotionSubImpl(x);
}
} // namespace boost

SIZE_ASSERT(Player::Shadow::MotionSubImpl, 0x20);
SIZE_ASSERT(Player::Shadow::MotionSub, 0x30);

namespace Player {
namespace Shadow {

MotionSubImpl::MotionSubImpl(const boost::shared_ptr<PlayerBase> &player, int kind) : m_player(player) {
    void *loader = fn_800122D4();
    void *file = fn_80078C4C(m_player.get());
    const UnkMotionSubPart *part = s_parts[kind];
    for (int i = 0; i < 3; i++) {
        if (part->m_node < 0xE) {
            m_nodes[i] = fn_800AA760(m_player->m_motion, part->m_node);
            m_clumps[i] = (RpClump *)fn_8001277C(loader, file, part->m_model);
        } else {
            m_nodes[i] = 0;
            m_clumps[i] = 0;
        }
        if (m_clumps[i] != 0) {
            m_clumps[i] = fn_80457090(m_clumps[i]);
            fn_80038198(m_clumps[i], fn_800AA584(m_player->m_motion));
        }
        part++;
    }
    Attach();
}

MotionSubImpl::~MotionSubImpl() {
    Detach();
    for (int i = 0; i < 3; i++) {
        if (m_clumps[i] != 0) {
            fn_80457468(m_clumps[i]);
            m_clumps[i] = 0;
        }
    }
}

void MotionSubImpl::Tick() {
    for (int i = 0; i < 3; i++) {
        if (m_clumps[i] != 0) {
            RwFrame *frame = m_clumps[i]->frame;
            frame->modelling = fn_800591F0(m_nodes[i]->m_matrix);
            fn_8048722C(frame);
        }
    }
}

void MotionSubImpl::AddClumps(UnkClumpList_8006CAE4 *list) {
    for (int i = 0; i < 3; i++) {
        if (m_clumps[i] != 0) {
            fn_8006CAE4(list, &m_clumps[i]);
        }
    }
}

void MotionSubImpl::Attach() {
    void *world;
    fn_8003A840();
    for (int i = 0; i < 3; i++) {
        if (m_clumps[i] != 0) {
            world = lbl_8051D208[fn_80169350(fn_80169364(m_clumps[i]))].m_world;
            if (world != 0) {
                SetClumpWorld(m_clumps[i], world);
            }
        }
    }
}

MotionSub *MotionSub::Create(Task *parent, const boost::shared_ptr<PlayerBase> &player) {
    int kind = -1;
    switch (CharaType(player.operator->())) {
    case 0x10:
        kind = 0;
        break;
    case 0x11:
        kind = 1;
        break;
    case 0x12:
        kind = 2;
        break;
    case 0x13:
        kind = 3;
        break;
    }
    if (kind >= 0) {
        return new MotionSub(parent, player, kind);
    }
    return 0;
}

MotionSub::MotionSub(Task *parent, const boost::shared_ptr<PlayerBase> &player, int kind)
    : Task(parent), m_impl(new MotionSubImpl(player, kind)) {}

MotionSub::~MotionSub() {}

void MotionSub::Tick(float dt) {
    m_impl->Tick();
}

void MotionSub::Refresh() {}

void MotionSub::AddClumps(UnkClumpList_8006CAE4 *list) {
    m_impl->AddClumps(list);
}

void MotionSub::Show() {
    m_impl->Attach();
}

void MotionSub::Hide() {
    m_impl->Detach();
}

} // namespace Shadow
} // namespace Player
