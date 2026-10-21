/* MissionClearCollision (class name from RTTI): the gadget type of the same name, a sphere that
 * clears a mission when the first character comes within its radius. Its parameters pick the
 * mission side ("0:DK 1:NM 2:HR": dark, normal, hero) and the radius. When the character enters the
 * sphere and the entry of the manager at fn_8006D6A8 for that side is in neither state 1 nor 2, it
 * notifies that entry and the owner and finishes. Except on the normal side it also starts a
 * positioned object of the side's kind (a sound source, probably) 40 units above itself. File name inferred from the
 * class. Method and member names are inferred. Definitions are in reverse address order (deferred
 * inlining emits functions last-defined first). */
#include "Game/CharInfo.h"
#include "Game/DebugDraw.h"
#include "Game/Gadget.h"
#include "Game/GadgetCharInfo.h"
#include "Game/GadgetClassDesc.h"
#include "Game/GadgetUtil.h"
#include "Game/Mission.h"
#include "boost/shared_ptr.hpp"
#include "boost/weak_ptr.hpp"
#include "size_assert.h"

/* Four colour bytes built by an out-of-line constructor (see Meteor.cpp). */
struct UnkColor_8040D2D8 {
    UnkColor_8040D2D8(u8 r, u8 g, u8 b, u8 a);

    /* 0x0 */ u8 r;
    /* 0x1 */ u8 g;
    /* 0x2 */ u8 b;
    /* 0x3 */ u8 a;
};
SIZE_ASSERT(UnkColor_8040D2D8, 0x4);

/* The positioned object fn_802B4254 creates (see ElecBomb.cpp; it may be a sound source). */
class UnkSound_802B4254;

class UnkPlayerManager_8006D6A8;

/* fn_8040CD74 and fn_8040CDD0 called as returning a - b and a + b by value (they store the result
 * through their first argument; see ArkCannon.cpp and CityBombSmall.cpp). */
typedef Vec3f (*VectorSubtractFn)(const Vec3f *a, const Vec3f *b);
typedef Vec3f (*VectorAddFn)(const Vec3f *a, const Vec3f *b);

extern "C" {
/* A character of the manager at fn_80074194 by index (behaviour inferred; see CityBombSmall.cpp). */
boost::weak_ptr<CharInfo> fn_800A8C54(UnkCharaList_80074194 *manager, int index);

/* The manager and its entry by index (see ElecBomb.cpp). */
UnkPlayerManager_8006D6A8 *fn_8006D6A8(void);
boost::weak_ptr<Mission::Mission> fn_800740E4(UnkPlayerManager_8006D6A8 *manager, int index);
/* Notifies the entry (behaviour unknown; see ElecCoreProgram.cpp). */
void fn_8016B2A4(Mission::Mission *player);
/* Tells the owner the gadget is done (behaviour inferred; see BreakObj.cpp). */
void fn_800C9B34(UnkPlacement_800C9CD0 *owner);

/* The positioned object (see ElecBomb.cpp). */
boost::shared_ptr<UnkSound_802B4254> fn_802B4254(int kind, int arg1);
void fn_802B4510(UnkSound_802B4254 *sound, u8 flag);
void fn_802B44C8(UnkSound_802B4254 *sound, const Vec3f *position);

void fn_8040CDD0(Vec3f *destination, const Vec3f *a, const Vec3f *b);
}

/* A length (name inferred). As a call it is not a constant expression, so the constant below is
 * stored by the static initializer, as in retail (as in BASwitch.cpp). */
inline float Span(float length) {
    return length;
}

/* The debug colors of the dark, normal and hero sides (names inferred). */
static UnkColor_8040D2D8 s_darkColor(0x80, 0xE0, 0x10, 0x00);
static UnkColor_8040D2D8 s_normalColor(0x80, 0xFF, 0xFF, 0x10);
static UnkColor_8040D2D8 s_heroColor(0x80, 0x35, 0xAD, 0xFF);
/* How high above the sphere the positioned object sits (name inferred). */
static const float s_soundHeight = Span(40.0f);

static UnkColor_8040D2D8 *s_colors[] = {&s_darkColor, &s_normalColor, &s_heroColor};

class MissionClearCollision : public Gadget {
  public:
    MissionClearCollision(Task *parent, UnkPlacement_800C9CD0 *owner);
    virtual ~MissionClearCollision();
    virtual void Behave(float dt);
    virtual void ReloadPlacement(UnkPlacement_800C9CD0 *owner);

    /* Creates the gadget under task layer 11. */
    static void Instantiate(UnkPlacement_800C9CD0 *owner);

    /* Copies the owner's position, rotation and parameters into the gadget. */
    void ReadPlacement(UnkPlacement_800C9CD0 *owner);
    /* Draws the sphere in the color of its side. */
    void DebugDraw();

    static const char *s_taskName;

    /* 0x30 */ boost::shared_ptr<GadgetCharInfo> m_chara;
    /* 0x38 */ struct {
        u32 side; /* 0 dark, 1 normal, 2 hero */
        float radius;
    } m_params;
    /* 0x40 */ boost::shared_ptr<UnkSound_802B4254> m_sound;
};
SIZE_ASSERT(MissionClearCollision, 0x48);

const char *MissionClearCollision::s_taskName = "MissionClearCollision";

MissionClearCollision::MissionClearCollision(Task *parent, UnkPlacement_800C9CD0 *owner)
    : Gadget(parent, owner) {
    m_name = s_taskName;
    m_chara = fn_8015CD90(0x85, 4);
    ReadPlacement(owner);
    if (m_params.side != 1) {
        m_sound = fn_802B4254(m_params.side, 0);
        fn_802B4510(m_sound.get(), 1);
        Vec3f offset(0.0f, s_soundHeight, 0.0f);
        fn_802B44C8(m_sound.px, &(const Vec3f &)((VectorAddFn)fn_8040CDD0)(
                                    &m_chara->m_position, &offset));
    }
}

MissionClearCollision::~MissionClearCollision() {}

extern "C" void fn_8023192C(void) {}

extern "C" void fn_80231928(void) {}

void MissionClearCollision::Instantiate(UnkPlacement_800C9CD0 *owner) {
    new MissionClearCollision(fn_801EB304(fn_8001C4F8(), 11), owner);
}

void MissionClearCollision::Behave(float dt) {
    boost::shared_ptr<CharInfo> character = fn_800A8C54(fn_80074194(), 0).lock();
    if (character.get() != 0) {
        Vec3f offset;

        fn_800091EC(&offset, &(const Vec3f &)((VectorSubtractFn)fn_8040CD74)(
                                 &character->m_position, &m_chara->m_position));
        if (fn_8040CF50(&offset) < m_params.radius) {
            boost::shared_ptr<Mission::Mission> player =
                fn_800740E4(fn_8006D6A8(), m_params.side).lock();
            if (player.get() != 0 && !player->IsState(2) && !player->IsState(1)) {
                fn_8016B2A4(player.get());
                fn_800C9B34(m_owner);
                m_flags |= 1;
                return;
            }
        }
    }
}

void MissionClearCollision::ReloadPlacement(UnkPlacement_800C9CD0 *owner) {
    ReadPlacement(owner);
    DebugDraw();
}

void MissionClearCollision::ReadPlacement(UnkPlacement_800C9CD0 *owner) {
    fn_800C9F14(owner, &m_chara->m_position);
    fn_800C9ED4(owner, &m_chara->m_rotation);
    fn_800C9DBC(owner, &m_params, sizeof(m_params));
}

void MissionClearCollision::DebugDraw() {
    fn_802E0E84(&m_chara->m_position, (const UnkDrawColor_802E0E84 *)s_colors[m_params.side],
                m_params.radius);
}

static GadgetParamDesc lbl_80547074[] = {
    {2, "0:DK 1:NM 2:HR", 0, 0, 2, 0.0f, 0.0f, 0.0f, fn_800CBC20, 0},
    {4, "COLLI_RADIUS point", 0, 0, 0, 50.0f, 0.0f, 65536.0f, fn_800CBC20, 0},
    {0},
};

GadgetClassDesc lbl_805470EC = {
    "MissionClearCollision", fn_8023192C, fn_80231928, MissionClearCollision::Instantiate, 0, 0,
    0x2595, 1, 0, 10, lbl_80547074,
};
