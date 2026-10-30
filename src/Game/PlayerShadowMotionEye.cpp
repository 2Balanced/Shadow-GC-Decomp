/* Player::Shadow::MotionEye (class name from RTTI): the task that shows Shadow's eyes. Its
 * implementation (class name unknown) loads two eye models of the motion files
 * (Game/PlayerShadowMotionFiles.h) and keeps them on the head node, and its blink (class name
 * unknown) keeps five eyelid models there and shows them one after another. ShadowMotion
 * (Game/PlayerShadowMotion.cpp) calls the task's functions. File name from the class; other
 * names are inferred. Definitions are in reverse address order (deferred inlining emits
 * functions last-defined first). */
#define PLAYER_MOTION_MEMBERS
#include "Game/DebugDispShape.h"
#include "Game/MathTypes.h"
#include "Game/Player.h"
#include "Game/PlayerShadowMotionEye.h"
#include "Game/PlayerShadowMotionFiles.h"
#include "Game/PlayerWeaponBase.h"
#include "Game/Task.h"
#include "boost/scoped_ptr.hpp"
#include "boost/shared_ptr.hpp"
#include "size_assert.h"

using boost::shared_ptr;
using Player::PlayerBase;

extern "C" {
/* The model loader and its files (see Game/PlayerShadowMotionHand.cpp). */
void *fn_800122D4(void);
RpClump *fn_8001277C(void *loader, s32 file, s32 index);
/* Clump helpers (see Game/PlayerShadowMotionHand.cpp). */
RpClump *fn_80457090(RpClump *clump);
void fn_80457468(RpClump *clump);
void *fn_8046256C(RpClump *clump);
void fn_804623EC(void *world, RpClump *clump);
void fn_804622FC(void *world, RpClump *clump);
void fn_80038198(RpClump *clump, UnkClumpFade_80027D5C *fade);
void fn_80038F94(RpClump *clump);
void fn_80038FC0(RpClump *clump);
void *fn_80169364(RpClump *clump);
int fn_80169350(void *frame);
void fn_8006CAE4(Player::UnkClumpList_8006CAE4 *list, RpClump *const *clump);
/* The motion's colour fade and its node of an index (Game/PlayerMotion.cpp). */
UnkClumpFade_80027D5C *fn_800AA584(Player::PlayerMotion *motion);
Player::UnkMotionNode_800AA760 *fn_800AA760(Player::PlayerMotion *motion, int index);
}

/* The areas' worlds (struct unknown; see Game/PlayerShadowMotionHand.cpp). */
struct UnkArea_8051D208 {
    /* 0x00 */ u8 field_0x0[0x10];
    /* 0x10 */ void *m_world;
    /* 0x14 */ u8 field_0x14[0x18 - 0x14];
};
extern UnkArea_8051D208 lbl_8051D208[];

/* The stage (class name unknown; see Game/BulletManagerTask.cpp). */
class UnkStage_8003A840 {
  public:
    UnkArea_8051D208 *GetAreas() {
        return lbl_8051D208;
    }
};

extern "C" UnkStage_8003A840 *fn_8003A840(void);

/* Puts clump in world (name inferred; as in Game/NpcMotion.cpp). */
inline void SetClumpWorld(RpClump *clump, void *world) {
    void *current = fn_8046256C(clump);
    if (current != world) {
        if (current != 0) {
            fn_804623EC(current, clump);
        }
        fn_804622FC(world, clump);
    }
}

/* The frame time a player points at (the word at 0x60; see Game/PlayerShadowMotionHand.cpp). */
#define FrameTime(player) (**reinterpret_cast<float **>(reinterpret_cast<u8 *>(player) + 0x60))

/* The blink's timing: the wait between blinks and the time of each eyelid frame. */
static const float s_blinkWait = 2.0f;
static const float s_blinkFrame = 0.025f;

namespace Player {
namespace Shadow {

/* The blink (class name unknown): five eyelid models on the head node; one of them is shown, and
 * a blink steps through them and back. */
class MotionEyeBlink {
  public:
    MotionEyeBlink(const shared_ptr<PlayerBase> &player, const UnkMotionNode_800AA760 *node,
                   UnkClumpFade_80027D5C *fade, const UnkMotionFiles_800ADE78 &files);
    ~MotionEyeBlink() {
        for (int i = 0; i < 5; i++) {
            if (m_clumps[i] != 0) {
                void *world = fn_8046256C(m_clumps[i]);
                if (world != 0) {
                    fn_804623EC(world, m_clumps[i]);
                }
                fn_80457468(m_clumps[i]);
            }
        }
    }

    void Update(float dt);
    void AddTo(UnkClumpList_8006CAE4 *list);
    void Hide() {
        if (m_clumps[m_current] != 0) {
            fn_80038FC0(m_clumps[m_current]);
        }
        m_hidden = 1;
    }
    void Show() {
        if (m_clumps[m_current] != 0) {
            fn_80038F94(m_clumps[m_current]);
        }
        m_hidden = 0;
    }

    /* 0x00 */ const UnkMotionNode_800AA760 *m_node;
    /* 0x04 */ RpClump *m_clumps[5];
    /* 0x18 */ s32 m_state;
    /* 0x1C */ s32 m_step; /* 0: waiting, 1: closing, 2: opening */
    /* 0x20 */ float m_time;
    /* 0x24 */ s32 m_current;
    /* 0x28 */ u8 m_hidden;
};
SIZE_ASSERT(MotionEyeBlink, 0x2C);

/* MotionEye's implementation (class name unknown). */
class MotionEyeImpl {
  public:
    MotionEyeImpl(const shared_ptr<PlayerBase> &player, const UnkMotionFiles_800ADE78 &files);
    ~MotionEyeImpl();

    void Update();
    void AddTo(UnkClumpList_8006CAE4 *list);
    void Show();
    void Hide();
    void Load();
    void Unload();

    int GetBlinkFrame() {
        return m_blink->m_current;
    }

    /* 0x00 */ MotionEyeBlink *m_blink;
    /* 0x04 */ shared_ptr<PlayerBase> m_player;
    /* 0x0C */ const UnkMotionNode_800AA760 *m_node;
    /* 0x10 */ RpClump *m_clumps[2];
    /* 0x18 */ UnkMotionFiles_800ADE78 m_files;
};
SIZE_ASSERT(MotionEyeImpl, 0x90);

/* Deletes an implementation (name inferred; see checked_delete below). */
inline void DeleteImpl(MotionEyeImpl *impl) {
    delete impl;
}

} // namespace Shadow
} // namespace Player

/* Retail keeps boost::checked_delete<MotionEyeImpl> out of line (0x800B5D0C); this specialization
 * calls an inline helper, which gives retail's code and order (see docs/research-notes.md and
 * Game/PlayerShadowMotionHand.cpp). */
namespace boost {
template<> inline void checked_delete<Player::Shadow::MotionEyeImpl>(Player::Shadow::MotionEyeImpl *x) {
    Player::Shadow::DeleteImpl(x);
}
} // namespace boost

namespace Player {
namespace Shadow {

/* NonMatching (99%): register allocation only (the file entry and the second loop's world). */
MotionEyeBlink::MotionEyeBlink(const shared_ptr<PlayerBase> &player, const UnkMotionNode_800AA760 *node,
                               UnkClumpFade_80027D5C *fade, const UnkMotionFiles_800ADE78 &files)
    : m_state(0), m_step(0), m_time(0.0f), m_current(0), m_hidden(0) {
    m_node = node;
    void *loader = fn_800122D4();
    for (int i = 0; i < 5; i++) {
        if (files.m_set2[i].m_index >= 0) {
            m_clumps[i] = fn_8001277C(loader, files.m_set2[i].m_file, files.m_set2[i].m_index);
        } else {
            m_clumps[i] = 0;
        }
    }
    UnkStage_8003A840 *stage = fn_8003A840();
    for (int i = 0; i < 5; i++) {
        if (m_clumps[i] != 0) {
            m_clumps[i] = fn_80457090(m_clumps[i]);
            void *world = stage->GetAreas()[fn_80169350(fn_80169364(m_clumps[i]))].m_world;
            if (world != 0) {
                SetClumpWorld(m_clumps[i], world);
            }
            fn_80038FC0(m_clumps[i]);
            fn_80038198(m_clumps[i], fade);
        }
    }
}

void MotionEyeBlink::Update(float dt) {
    switch (m_state) {
    case 0: {
        int current = m_current;
        if (!m_hidden) {
            m_time += dt;
            switch (m_step) {
            case 0:
                if (m_time >= s_blinkWait) {
                    m_time -= s_blinkWait;
                    m_step++;
                }
                break;
            case 1:
                if (m_time >= s_blinkFrame) {
                    m_time -= s_blinkFrame;
                    if (++current >= 5) {
                        current = 4;
                        m_step++;
                    }
                }
                break;
            case 2:
                if (m_time >= s_blinkFrame) {
                    m_time -= s_blinkFrame;
                    if (--current <= 0) {
                        m_step = current = 0;
                    }
                }
                break;
            }
            if (m_current != current) {
                if (m_clumps[m_current] != 0) {
                    fn_80038FC0(m_clumps[m_current]);
                }
                m_current = current;
                if (m_clumps[m_current] != 0) {
                    fn_80038F94(m_clumps[m_current]);
                }
            }
        }
        break;
    }
    case 1:
        break;
    }
    if (m_clumps[m_current] != 0) {
        RwFrame *frame = m_clumps[m_current]->frame;
        frame->modelling = fn_800591F0(m_node->m_matrix);
        fn_8048722C(frame);
    }
}

void MotionEyeBlink::AddTo(UnkClumpList_8006CAE4 *list) {
    RpClump *clump = m_clumps[m_current];
    if (clump != 0) {
        fn_8006CAE4(list, &clump);
    }
}

/* NonMatching (90%): retail loads the player's motion into r0 for the test and copies it to r29
 * after it; every source shape tried either loads it straight into r29 or reloads it. */
MotionEyeImpl::MotionEyeImpl(const shared_ptr<PlayerBase> &player, const UnkMotionFiles_800ADE78 &files)
    : m_blink(0), m_player(player), m_files(files) {
    if (m_player.get()->m_motion != 0) {
        m_blink = new MotionEyeBlink(m_player, fn_800AA760(m_player.get()->m_motion, 2),
                                     fn_800AA584(m_player.get()->m_motion), m_files);
    }
    m_clumps[0] = 0;
    m_clumps[1] = 0;
    Load();
}

MotionEyeImpl::~MotionEyeImpl() {
    Unload();
    delete m_blink;
}

void MotionEyeImpl::Update() {
    for (int i = 0; i < 2; i++) {
        if (m_clumps[i] != 0) {
            RwFrame *frame = m_clumps[i]->frame;
            frame->modelling = fn_800591F0(m_node->m_matrix);
            fn_8048722C(frame);
        }
    }
    m_blink->Update(FrameTime(m_player.get()));
}

void MotionEyeImpl::AddTo(UnkClumpList_8006CAE4 *list) {
    fn_8006CAE4(list, &m_clumps[0]);
    fn_8006CAE4(list, &m_clumps[1]);
    m_blink->AddTo(list);
}

void MotionEyeImpl::Show() {
    UnkStage_8003A840 *stage = fn_8003A840();
    void *world;
    for (int i = 0; i < 2; i++) {
        if (m_clumps[i] != 0) {
            world = stage->GetAreas()[fn_80169350(fn_80169364(m_clumps[i]))].m_world;
            if (world != 0) {
                SetClumpWorld(m_clumps[i], world);
            }
        }
    }
    m_blink->Show();
}

void MotionEyeImpl::Hide() {
    for (int i = 0; i < 2; i++) {
        if (m_clumps[i] != 0) {
            void *world = fn_8046256C(m_clumps[i]);
            if (world != 0) {
                fn_804623EC(world, m_clumps[i]);
            }
        }
    }
    m_blink->Hide();
}

void MotionEyeImpl::Load() {
    int i;
    if (m_player.get()->m_motion != 0) {
        PlayerMotion *motion = m_player.get()->m_motion;
        m_node = fn_800AA760(motion, 2);
        void *loader = fn_800122D4();
        for (i = 0; i < 2; i++) {
            if (m_files.m_set3[i].m_index >= 0) {
                m_clumps[i] = fn_8001277C(loader, m_files.m_set3[i].m_file, m_files.m_set3[i].m_index);
            }
        }
        for (i = 0; i < 2; i++) {
            if (m_clumps[i] != 0) {
                m_clumps[i] = fn_80457090(m_clumps[i]);
                fn_80038198(m_clumps[i], fn_800AA584(motion));
            }
        }
    }
    Show();
}

void MotionEyeImpl::Unload() {
    Hide();
    for (int i = 0; i < 2; i++) {
        if (m_clumps[i] != 0) {
            fn_80457468(m_clumps[i]);
            m_clumps[i] = 0;
        }
    }
}

MotionEye::MotionEye(Task *parent, const shared_ptr<PlayerBase> &player, const UnkMotionFiles_800ADE78 &files)
    : Task(parent), m_impl(new MotionEyeImpl(player, files)) {
    m_name = "MotionEye";
}

MotionEye::~MotionEye() {}

void MotionEye::Tick(float dt) {
    m_impl->Update();
}

void MotionEye::Update() {}

void MotionEye::AddTo(UnkClumpList_8006CAE4 *list) {
    m_impl->AddTo(list);
}

void MotionEye::Show() {
    m_impl->Show();
}

void MotionEye::Hide() {
    m_impl->Hide();
}

int MotionEye::GetBlinkFrame() {
    return m_impl->GetBlinkFrame();
}

} // namespace Shadow
} // namespace Player
