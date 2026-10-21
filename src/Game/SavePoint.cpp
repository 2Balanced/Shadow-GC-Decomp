/* SavePoint::SavePoint (class name from RTTI): the SAVEPOINT gadget. Once the stage owner flags it
 * as reached it shows its second model; a character touching it saves the game there (it records
 * the position and the "priority" parameter, refreshes the game's managers and gives rings back by
 * the ring count). A UseCommand then opens the save point menu (a
 * SavePointSelect::SavePointSelectTask) if saving is possible; choosing a save point in it warps
 * the player there through a WarpHole::ManagerTask. A SavePoint::PauseWatch (class name from RTTI)
 * clears the gadget's request flags while the game is paused. File name from the namespace. Names
 * marked "(name inferred)" are not original. Definitions are in reverse address order (deferred
 * inlining emits functions last-defined first).
 *
 * The constructor addresses its member pointer constants and the vtable through the start of
 * .data, so the linker keeps this unit's .data whole: the orphan RTTI base lists of the two first
 * headers lead it (in this order), and the Player classes' lists close it.
 *
 * The unit also holds the accessors of three game-wide objects (fn_80117514, fn_80117578,
 * fn_80119178; see Game/UnkSingletons.h) and the first copies of the weak functions that destroy
 * the third one's trees. */
#include <boost/bind.hpp>
#include <boost/function.hpp>

/* The game has one shared copy of std::swap for boost::function's any_pointer (0x80019164), which
 * function1::swap calls; an overload declared here, before Game/SavePointSelect.h uses function1's
 * assignment, stands in for it (as in CatchStick.cpp). */
namespace std {
void swap(boost::detail::function::any_pointer &a, boost::detail::function::any_pointer &b);
}

#include "Game/CharCommand.h"
#include "Game/CharCommandUnit.h"
#include "Game/CharInfo.h"
#include "Game/Gadget.h"
#include "Game/GadgetCharInfo.h"
#include "Game/GadgetCollision.h"
#include "Game/GadgetModelInfo.h"
#include "Game/GadgetClassDesc.h"
#include "Game/GadgetUtil.h"
#include "Game/TNumEdit.h"
/* After TNumEdit.h: the order of the casts sets the order of the RTTI base lists in .data. */
#include "Game/CharaColliBaseCharInfo.h"
#include "Game/Player.h"
#include "Game/SavePointSelect.h"
#include "Game/Task.h"
#include "Game/UnkSingletons.h"
#include "Game/FileQueue.h"
#include "boost/shared_ptr.hpp"
#include "boost/weak_ptr.hpp"
#include "size_assert.h"

/* The game state block in .bss (layout as in WarpHole; field_0x84 is set while the game is paused,
 * meaning inferred). */
struct UnkGameState_8057D6A0 {
    /* 0x00 */ u8 field_0x0[0x84];
    /* 0x84 */ u8 field_0x84;
    /* 0x85 */ u8 field_0x85[0xA8 - 0x85];
    /* 0xA8 */ u8 field_0xA8[0x150 - 0xA8];
};
SIZE_ASSERT(UnkGameState_8057D6A0, 0x150);

/* The objects returned by fn_800BADE8 and fn_80074470 (classes unknown; as in WarpHole). */
struct UnkObject_800BADE8 {
    /* Sets the word at 0x30 (meaning unknown). */
    void SetField30(s32 value) {
        field_0x30 = value;
    }

    /* 0x00 */ u8 field_0x0[0x30];
    /* 0x30 */ s32 field_0x30;
};

struct UnkObject_80074470 {
    /* 0x00 */ u8 field_0x0[0x8];
    /* 0x08 */ s32 field_0x8;
};

/* The object fn_80079808 returns (class unknown). */
struct UnkObject_80079808;

namespace WarpHole {
/* The task that warps a player (class name from RTTI; see Game/WarpHole.cpp). */
class ManagerTask {
  public:
    static void Instantiate(const Vec3f &destination, const boost::shared_ptr<Player::Player> &player);
};
} // namespace WarpHole

extern "C" {
/* A 32-bit flag word: fn_80014E04 constructs it cleared, fn_80014E1C sets bit, fn_8006127C clears
 * bit and fn_80014E34 tests bit (behaviour from their bodies). */
void fn_80014E04(u32 *flags);
void fn_80014E1C(u32 *flags, int bit);
void fn_8006127C(u32 *flags, int bit);
bool fn_80014E34(u32 *flags, int bit);

/* Gives the character info a command handler unless it has one (behaviour inferred). */
void fn_8015CF7C(GadgetCharInfo *charInfo);
/* Sets bits of the owner's flags, and copies the owner's flags (behaviour inferred). */
void fn_800C9EB0(UnkPlacement_800C9CD0 *owner, u32 mask);
void fn_800C9EC4(UnkPlacement_800C9CD0 *owner, u32 *flags);
/* Sets the model's motion speed (behaviour inferred). */
void fn_80161308(GadgetModelInfo *model, float speed);
/* Calls callback with every character in the collision event (behaviour inferred). */
void fn_8015AA84(ColliDetectArgs &args, void (*callback)(boost::shared_ptr<CharInfo> hit, void *user),
                 void *user);
/* Gives target rings or an item (behaviour inferred; as in ScatteringRing). */
void fn_8015F6EC(boost::shared_ptr<CharInfo> target, int id, int arg2, int arg3);
/* boost::dynamic_pointer_cast<Player::Player>(character), a weak instance (defined at the end of
 * this file). */
boost::shared_ptr<Player::Player> fn_8009DC10(const boost::shared_ptr<CharInfo> &character);
/* Returns the character info of the given id from the manager's list (behaviour inferred). */
boost::weak_ptr<CharInfo> fn_800A8B74(UnkCharaList_80074194 *manager, int id);

/* The save data, the number of save points in it, the position of save point index, and the call
 * that records one (behaviour inferred). */
UnkObject_80079808 *fn_80079808(void);
int fn_8016F3A0(UnkObject_80079808 *data);
const Vec3f *fn_8016F344(UnkObject_80079808 *data, int index, int arg2);
void fn_8016F3D4(UnkObject_80079808 *data, const Vec3f *position,
                 const Vec3f *rotation, int priority);
/* The ring count (behaviour inferred). */
void *fn_80178B5C(UnkObject_80079808 *data, int arg1);
int fn_8016F280(void *counter);

/* Queries and switches around saving (behaviour unknown). */
bool fn_8020261C(UnkObject_801175AC *object);
u8 fn_802E5810(UnkObject_802E5B2C *object);
bool fn_80330CB0(void);
void fn_80330CB8(void);
void fn_80330CC4(void);
UnkObject_800BADE8 *fn_800BADE8(void);
UnkObject_80074470 *fn_80074470(void);
void fn_801E2F28(UnkObject_800BADE8 *object);
void fn_801E2EDC(UnkObject_800BADE8 *object);
void fn_80203970(void *object, int arg);
void fn_80301F2C(void);

/* The game's managers, refreshed when the game is saved (behaviour unknown). */
void *fn_8006F754(void);
void *fn_80074AEC(void);
void *fn_8006D6A8(void);
void *fn_8016DFF8(void);
void *fn_800C9B84(void);
void *fn_800CB508(void);
void *fn_801A23F4(void);
void *fn_800CB69C(void);
void *fn_801690F8(void);
}

/* Stops the object fn_800BADE8 returns (name inferred; as in WarpHole's ManagerTask). */
inline void StopObject() {
    UnkObject_800BADE8 *object = fn_800BADE8();
    fn_801E2F28(object);
    fn_801E2EDC(object);
    object->field_0x30 = 0;
}

extern UnkGameState_8057D6A0 lbl_8057D6A0;
/* The id of the player's character info (inferred). */
extern int lbl_805EE588;

/* The height above the save point at which the game is saved (name inferred). Retail fills
 * s_saveHeight in from a static initializer, which a call reproduces. */
inline float SaveHeight() {
    return 10.0f;
}

static float s_saveHeight = SaveHeight();

namespace SavePoint {

/* The request flags (class name unknown; its out-of-line members are the fn_80014E04 family).
 * Bit 0: a character used the save point; bit 1: the menu may open. */
struct SavePointFlags {
    SavePointFlags() {
        fn_80014E04(&m_bits);
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

/* The task that clears the request flags while the game is paused (class name from RTTI). */
class PauseWatch : public Task {
  public:
    PauseWatch(Task *parent, SavePointFlags *flags);
    virtual ~PauseWatch();
    virtual void Tick(float dt);

    static const char *s_taskName;

    /* 0x28 */ SavePointFlags *m_watched;
    /* 0x2C */ u32 field_0x2C;
};
SIZE_ASSERT(PauseWatch, 0x30);

/* The parameters the owner supplies (names from the parameter table). */
struct SavePointParams {
    SavePointParams() : priority(0) {}

    /* 0x0 */ s32 priority;
};

class SavePoint : public Gadget {
  public:
    SavePoint(Task *parent, UnkPlacement_800C9CD0 *owner);
    virtual ~SavePoint();
    virtual void Behave(float dt);
    virtual void ReloadPlacement(UnkPlacement_800C9CD0 *owner);

    /* Creates the gadget under task layer 11 (name inferred). */
    static void Instantiate(UnkPlacement_800C9CD0 *owner);

    /* Method names are inferred. */
    bool CanSave();
    void LoadFromOwner(UnkPlacement_800C9CD0 *owner);
    void UpdateModel();
    void OnColliDetect(ColliDetectArgs &args);
    void OnSelect(SavePointSelect::Args &args);
    void OnCancel(SavePointSelect::Args &args);
    void OnCommand(CharCommand &command);

    static const char *s_taskName;

    /* Member names are inferred. */
    /* 0x30 */ boost::shared_ptr<GadgetCharInfo> m_chara;
    /* 0x38 */ boost::shared_ptr<GadgetModelInfo> m_model;
    /* 0x40 */ boost::shared_ptr<GadgetModelInfo> m_activeModel; /* shown once reached */
    /* 0x48 */ s32 m_state; /* 0 start, 1 not reached, 2 reached, 3 menu open */
    /* 0x4C */ SavePointParams m_params;
    /* 0x50 */ SavePointFlags m_flags;
    /* 0x54 */ u32 field_0x54;
};
SIZE_ASSERT(SavePoint, 0x58);

/* What the collision callback passes to SaveGame (name inferred). */
struct SaveSearch {
    SaveSearch() : m_params(0), m_saved(false) {}

    /* 0x0 */ boost::weak_ptr<GadgetCharInfo> m_chara;
    /* 0x8 */ const SavePointParams *m_params;
    /* 0xC */ bool m_saved;
};

const char *PauseWatch::s_taskName = "PauseWatch::PauseWatch";
const char *SavePoint::s_taskName = "SavePoint";

PauseWatch::PauseWatch(Task *parent, SavePointFlags *flags) : Task(parent) {
    m_name = s_taskName;
    m_watched = flags;
    m_flags |= 0x100;
}

PauseWatch::~PauseWatch() {
    m_watched = 0;
}

void PauseWatch::Tick(float dt) {
    if (m_watched != 0 && lbl_8057D6A0.field_0x84 != 0) {
        m_watched->Reset(0);
        m_watched->Reset(1);
    }
}

} // namespace SavePoint

/* The object behind fn_80119178 (name inferred; inlined into it). */
inline UnkObject_801A2AF4 &UnkObject_801A2AF4_Instance() {
    static UnkObject_801A2AF4 s_object;
    return s_object;
}

extern "C" UnkObject_801A2AF4 *fn_80119178(void) {
    if (UnkObject_801A2AF4::s_cached == 0) {
        UnkObject_801A2AF4::s_cached = &UnkObject_801A2AF4_Instance();
    }
    return UnkObject_801A2AF4::s_cached;
}

namespace SavePoint {

/* Saves the game at the save point, once per collision event (name inferred). */
static void SaveGame(boost::shared_ptr<CharInfo> hit, void *user) {
    SaveSearch *search = static_cast<SaveSearch *>(user);
    if (search != 0 && !search->m_saved) {
        if (boost::shared_ptr<GadgetCharInfo> charInfo = search->m_chara.lock()) {
            Vec3f position(0.0f, s_saveHeight, 0.0f);
            Mtx34f matrix;
            fn_8040B6F8(&matrix);
            fn_8040B59C(&matrix, &charInfo->m_position);
            fn_8040B848(&matrix, charInfo->m_rotation.y);
            fn_8040B884(&matrix, charInfo->m_rotation.x);
            fn_8040B80C(&matrix, charInfo->m_rotation.z);
            AssignVector(position, fn_8040B67C(&matrix, &position));
            fn_8016F3D4(fn_80079808(), &position, &charInfo->m_rotation, search->m_params->priority);
            fn_801D560C(fn_80053164(), 0xE013, 0, 0);
        }
        fn_8006F754();
        fn_80074AEC();
        fn_8006D6A8();
        fn_8016DFF8();
        fn_800C9B84();
        fn_800CB508();
        fn_80119178();
        fn_801A23F4();
        fn_800CB69C();
        fn_801690F8();
        int rings = fn_8016F280((u8 *)fn_80178B5C(fn_80079808(), 0) + 0x28);
        if (rings >= 100) {
            fn_8015F6EC(hit, 6, 1, 1);
        } else if (rings >= 50) {
            fn_8015F6EC(hit, 0, 20, 1);
        } else if (rings >= 0) {
            fn_8015F6EC(hit, 0, 10, 1);
        }
        fn_80301F2C();
        search->m_saved = true;
    }
}

SavePoint::SavePoint(Task *parent, UnkPlacement_800C9CD0 *owner) : Gadget(parent, owner) {
    m_name = s_taskName;
    m_chara = fn_8015CD90(0xC, 4);
    fn_8015CF7C(m_chara.get());
    fn_800729B0(&m_chara->m_commandUnit->m_callback,
                &(const CharCommandCallback &)CharCommandCallback(
                    boost::bind(&SavePoint::OnCommand, this, _1)));
    LoadFromOwner(owner);
    m_model = fn_8015F9D8(0, 0xDC);
    m_model->fn_80162A68(0, 0);
    m_model->fn_80161F08("col", m_chara);
    fn_800D0B64(m_model->field_0x4,
                boost::function1<void, ColliDetectArgs &>(
                    boost::bind(&SavePoint::OnColliDetect, this, _1)));
    UpdateModel();
    new PauseWatch(this, &m_flags);
}

SavePoint::~SavePoint() {}

void SavePoint::Behave(float dt) {
    if (m_model.get() != 0) {
        m_model->fn_80162FB0(dt);
    }
    if (m_activeModel.get() != 0) {
        m_activeModel->fn_80162FB0(dt);
    }

    switch (m_state) {
    case 0: {
        u32 ownerFlags;
        fn_800C9EC4(m_owner, &ownerFlags);
        if (ownerFlags & 0x400000) {
            if (m_activeModel.get() == 0) {
                m_activeModel = fn_8015F9D8(0, 0xDC);
                m_activeModel->fn_80162A68(1, 0);
                UpdateModel();
            }
            m_state = 2;
        } else {
            m_state = 1;
        }
        break;
    }
    case 2:
        if (m_flags.Test(1)) {
            if (CanSave()) {
                if (fn_80330CB0() && fn_8016F3A0(fn_80079808()) > 0) {
                    {
                        Task *parent = fn_801EB304(fn_8001C4F8(), 11);
                        SavePointSelect::SavePointSelectTask *task =
                            new SavePointSelect::SavePointSelectTask(parent, m_params.priority);
                        task->m_onSelect = boost::function1<void, SavePointSelect::Args &>(
                            boost::bind(&SavePoint::OnSelect, this, _1));
                        task->m_onCancel = boost::function1<void, SavePointSelect::Args &>(
                            boost::bind(&SavePoint::OnCancel, this, _1));
                    }
                    m_state = 3;
                    fn_80203970(&lbl_8057D6A0.field_0xA8, 1);
                    m_flags.Reset(1);
                    StopObject();
                }
            } else {
                m_flags.Reset(1);
            }
        }
        break;
    case 3:
        break;
    }

    if (m_flags.Test(0)) {
        m_flags.Reset(0);
        m_flags.Set(1);
    }
}

void SavePoint::ReloadPlacement(UnkPlacement_800C9CD0 *owner) {
    LoadFromOwner(owner);
    UpdateModel();
    if (m_model.get() != 0) {
        m_model->fn_8016108C();
    }
}

void SavePoint::LoadFromOwner(UnkPlacement_800C9CD0 *owner) {
    fn_800C9F14(owner, &m_chara->m_position);
    fn_800C9ED4(owner, &m_chara->m_rotation);
    fn_800C9DBC(owner, &m_params, sizeof(m_params));
    m_state = 0;
    fn_800C9EB0(owner, 0x200);
}

void SavePoint::UpdateModel() {
    Mtx34f matrix;
    fn_8040C148(&matrix, &lbl_805E0720);
    if (m_model.get() != 0) {
        fn_8040B59C(&matrix, &m_chara->m_position);
        fn_8040B848(&matrix, m_chara->m_rotation.y);
        fn_8040B884(&matrix, m_chara->m_rotation.x);
        fn_8040B80C(&matrix, m_chara->m_rotation.z);
        m_model->fn_801633DC(&matrix);
    }
    if (m_activeModel.get() != 0) {
        m_activeModel->fn_801633DC(&matrix);
    }
}

void SavePoint::OnColliDetect(ColliDetectArgs &args) {
    if (m_state == 1) {
        SaveSearch search;
        search.m_chara = m_chara;
        search.m_params = &m_params;
        fn_8015AA84(args, SaveGame, &search);
        if (search.m_saved) {
            if (m_activeModel.get() == 0) {
                m_activeModel = fn_8015F9D8(0, 0xDC);
                m_activeModel->fn_80162A68(1, 0);
                UpdateModel();
            }
            fn_800C9EB0(m_owner, 0x400000);
            m_state = 2;
        }
    }
}

void SavePoint::OnSelect(SavePointSelect::Args &args) {
    if (args.field_0x0 != -1) {
        Vec3f position;
        fn_800091EC(&position, fn_8016F344(fn_80079808(), args.field_0x0, 0));
        boost::weak_ptr<CharInfo> player = fn_800A8B74(fn_80074194(), lbl_805EE588);
        if (boost::shared_ptr<CharInfo> character = player.lock()) {
            boost::shared_ptr<Player::Player> target = fn_8009DC10(character);
            WarpHole::ManagerTask::Instantiate(position, target);
        }
    }
    m_state = 2;
    fn_80161308(m_model.get(), 10.0f);
}

void SavePoint::OnCancel(SavePointSelect::Args &args) {
    m_state = 2;
    if (fn_80074470()->field_0x8 != 6) {
        fn_80203970(&lbl_8057D6A0.field_0xA8, 0);
    }
    fn_800BADE8()->SetField30(1);
}

void SavePoint::OnCommand(CharCommand &command) {
    switch (command.TypeId()) {
    case 0xD:
        if (m_state == 2) {
            m_flags.Set(0);
        }
        break;
    }
}

} // namespace SavePoint

extern "C" UnkObject_801175AC &fn_801175AC(void) {
    static UnkObject_801175AC s_object;
    return s_object;
}

extern "C" UnkObject_801175AC *fn_80117578(void) {
    if (UnkObject_801175AC::s_cached == 0) {
        UnkObject_801175AC::s_cached = &fn_801175AC();
    }
    return UnkObject_801175AC::s_cached;
}

/* The object behind fn_80117514 (name inferred; inlined into it). */
inline UnkObject_802E5B2C &UnkObject_802E5B2C_Instance() {
    static UnkObject_802E5B2C s_object;
    return s_object;
}

extern "C" UnkObject_802E5B2C *fn_80117514(void) {
    if (UnkObject_802E5B2C::s_cached == 0) {
        UnkObject_802E5B2C::s_cached = &UnkObject_802E5B2C_Instance();
    }
    return UnkObject_802E5B2C::s_cached;
}

namespace SavePoint {

bool SavePoint::CanSave() {
    if (lbl_8057D6A0.field_0x84 != 0) {
        return false;
    }
    if (!fn_8020261C(fn_80117578())) {
        return false;
    }
    if (fn_80074470()->field_0x8 == 6) {
        return false;
    }
    return fn_802E5810(fn_80117514());
}

} // namespace SavePoint

extern "C" void fn_80117468(void) {
    fn_8006B50C(0xDC, 1);
    fn_80330CC4();
}

extern "C" void fn_80117448(void) {
    fn_80330CB8();
}

namespace SavePoint {

void SavePoint::Instantiate(UnkPlacement_800C9CD0 *owner) {
    Task *parent = fn_801EB304(fn_8001C4F8(), 11);
    new SavePoint(parent, owner);
}

static GadgetParamDesc lbl_80526E50[] = {
    {1, "priority", 1, 1, 0x1F, 0.0f, 0.0f, 0.0f, fn_800CBC20, 0},
    {0},
};

} // namespace SavePoint

GadgetClassDesc lbl_80526EA0 = {
    "SAVEPOINT", fn_80117468, fn_80117448, SavePoint::SavePoint::Instantiate, 0, 0x200, 5, 1, 0,
    10, SavePoint::lbl_80526E50,
};

/* The accessors' caches: retail places them after the unit's other .sbss objects. */
UnkObject_802E5B2C *UnkObject_802E5B2C::s_cached;
UnkObject_801175AC *UnkObject_801175AC::s_cached;
UnkObject_801A2AF4 *UnkObject_801A2AF4::s_cached;

/* After the unit's code: see the header. */
#include "Game/UnkTreeDestroy.h"

/* The weak copy of boost::dynamic_pointer_cast<Player::Player> this unit emitted. The linker keeps
 * the player code's (0x8009DC10, which game code calls by its fn_ name), so this code is dropped;
 * the template it instantiates is compiled with the unit's last functions, and its RTTI base lists
 * for the Player classes stay at the end of this unit's .data, which is kept whole. */
extern "C" inline boost::shared_ptr<Player::Player> fn_8009DC10(const boost::shared_ptr<CharInfo> &character) {
    return boost::dynamic_pointer_cast<Player::Player>(character);
}
