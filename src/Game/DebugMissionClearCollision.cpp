/* DebugMissionClearCollision (class name from RTTI): the debug version of MissionClearCollision
 * (gadget type "DebugDebugMissionClearCollision"). It clears the mission of its side ("0:DK 1:NM
 * 2:HR 3:BOSS") when the first character comes within s_radius, and draws itself in the debug view
 * as a turning sphere with an "M" in the side's colour. File name inferred from the class. Method,
 * member and static names are inferred. Definitions are in reverse address order (deferred
 * inlining emits functions last-defined first). The unit's code reaches its .data and .bss
 * through one base address each, so the linker keeps those sections whole. */
#include "Game/CharInfo.h"
#include "Game/DebugDisp.h"
#include "Game/DebugDraw.h"
#include "Game/Gadget.h"
#include "Game/GadgetCharInfo.h"
#include "Game/GadgetClassDesc.h"
#include "Game/GadgetUtil.h"

#include "Game/Mission.h"
#include "Game/TNumEdit.h"
#include "boost/shared_ptr.hpp"
#include "boost/weak_ptr.hpp"
#include "size_assert.h"

class UnkPlayerManager_8006D6A8;

/* What fn_80074470 returns (class unknown; see Game/Mission.cpp). */
struct UnkObject_80074470;

/* fn_8040CD74 called as returning a - b by value (see MissionClearCollision.cpp). */
typedef Vec3f (*VectorSubtractFn)(const Vec3f *a, const Vec3f *b);

/* A line segment's storage, built by fn_80020A20 (see Game/DebugDisp.h). */
struct UnkLineStorage {
    /* 0x00 */ Vec3f m_start;
    /* 0x0C */ Vec3f m_end;
};

extern "C" {
/* A character of the manager at fn_80074194 by index (see MissionClearCollision.cpp). */
boost::weak_ptr<CharInfo> fn_800A8C54(UnkCharaList_80074194 *manager, int index);
/* The manager and its entry by index (see MissionClearCollision.cpp). */
UnkPlayerManager_8006D6A8 *fn_8006D6A8(void);
boost::weak_ptr<Mission::Mission> fn_800740E4(UnkPlayerManager_8006D6A8 *manager, int index);
void fn_8016B2A4(Mission::Mission *player);
void fn_800C9B34(UnkPlacement_800C9CD0 *owner);
/* Ends the stage (behaviour inferred; see Game/Mission.cpp). */
UnkObject_80074470 *fn_80074470(void);
void fn_80175EEC(UnkObject_80074470 *object, int arg1, int arg2);

/* Draws a line (see DebugDisp.cpp; fn_80020A20, which builds one, is in Game/GadgetUtil.h). */
void fn_802E0AA0(const UnkDebugLine_80020A20 *line, const UnkColor_8040D2D8 *color, float width);
}

inline const Vec3f &Ref(const Vec3f &vector) {
    return vector;
}

/* A length (name inferred). As a call it is not a constant expression, so the statics below are
 * stored by the static initializer, as in retail (as in MissionClearCollision.cpp). */
inline float Span(float length) {
    return length;
}

/* Degrees to radians (name inferred). */
inline float ToRadian(float degrees) {
    return degrees * (lbl_805FAD38 / 180.0f);
}

/* The radius that clears the mission, the size of the "M", and the width of its lines. */
static const float s_radius = Span(10.0f);
static const float s_size = Span(5.0f);
static const float s_width = Span(1.0f);

/* Nothing reads it (its meaning is unknown). */
static Vec3f s_unused;

/* A vector filled in member by member (no constructor, so that the lines' constructor below has
 * no inline calls and is inlined into the static initializer; as in BlackDoomCtrlUnit.cpp). */
struct UnkVector {
    float x;
    float y;
    float z;
};

/* A line of the "M" (name inferred), built by fn_80020A20 in the static initializer. */
struct DebugMissionLine {
    DebugMissionLine(float y0, float x0, float y1, float x1) {
        UnkVector end;
        UnkVector start;
        end.x = x1;
        end.y = y1;
        end.z = 0.0f;
        start.x = x0;
        start.y = y0;
        start.z = 0.0f;
        fn_80020A20(this, (Vec3f *)&start, (Vec3f *)&end);
    }

    float m_start[3];
    float m_end[3];
};

static DebugMissionLine s_line0(-s_size, -s_size, s_size, -s_size);
static DebugMissionLine s_line1(s_size, -s_size, 0.0f, 0.0f);
static DebugMissionLine s_line2(0.0f, 0.0f, s_size, s_size);
static DebugMissionLine s_line3(s_size, s_size, -s_size, s_size);

/* The lines' and the sphere's colours of the dark, normal, hero and boss sides. */
static UnkColor_8040D2D8 s_darkLine(0xC0, 0xE0, 0x10, 0x00);
static UnkColor_8040D2D8 s_darkSphere(0x80, 0xE0, 0x10, 0x00);
static UnkColor_8040D2D8 s_normalLine(0xC0, 0xFF, 0xFF, 0x10);
static UnkColor_8040D2D8 s_normalSphere(0x80, 0xFF, 0xFF, 0x10);
static UnkColor_8040D2D8 s_heroLine(0xC0, 0x35, 0xAD, 0xFF);
static UnkColor_8040D2D8 s_heroSphere(0x80, 0x35, 0xAD, 0xFF);
static UnkColor_8040D2D8 s_bossLine(0xC0, 0xFF, 0xFF, 0xFF);
static UnkColor_8040D2D8 s_bossSphere(0x80, 0xFF, 0xFF, 0xFF);

/* How fast the "M" turns, in radians a second. */
static const float s_turnSpeed = ToRadian(90.0f);

static DebugMissionLine *s_lineTable[4] = {&s_line0, &s_line1, &s_line2, &s_line3};
static UnkColor_8040D2D8 *s_sphereColors[4] = {&s_darkSphere, &s_normalSphere, &s_heroSphere,
                                                &s_bossSphere};
static UnkColor_8040D2D8 *s_lineColors[4] = {&s_darkLine, &s_normalLine, &s_heroLine, &s_bossLine};

class DebugMissionClearCollision : public Gadget {
  public:
    DebugMissionClearCollision(Task *parent, UnkPlacement_800C9CD0 *owner);
    virtual ~DebugMissionClearCollision();
    virtual void Behave(float dt);
    virtual void ReloadPlacement(UnkPlacement_800C9CD0 *owner);

    /* Creates the gadget under task layer 11. */
    static void Instantiate(UnkPlacement_800C9CD0 *owner);

    /* Copies the owner's position, rotation and parameters into the gadget. */
    void ReadPlacement(UnkPlacement_800C9CD0 *owner);
    /* Draws the sphere and the "M" in the colour of its side. */
    void DebugDraw();

    static const char *s_taskName;

    /* 0x30 */ boost::shared_ptr<GadgetCharInfo> m_chara;
    /* 0x38 */ struct {
        u32 side; /* 0 dark, 1 normal, 2 hero, 3 boss */
    } m_params;
    /* 0x3C */ u32 pad_0x3C; /* the class size is a multiple of 8 */
};
SIZE_ASSERT(DebugMissionClearCollision, 0x40);

const char *DebugMissionClearCollision::s_taskName = "DebugMissionClearCollision";

DebugMissionClearCollision::DebugMissionClearCollision(Task *parent, UnkPlacement_800C9CD0 *owner)
    : Gadget(parent, owner) {
    m_name = s_taskName;
    m_chara = fn_8015CD90(0x84, 4);
    ReadPlacement(owner);
}

DebugMissionClearCollision::~DebugMissionClearCollision() {}

extern "C" void fn_802C8820(void) {}

extern "C" void fn_802C881C(void) {}

void DebugMissionClearCollision::Instantiate(UnkPlacement_800C9CD0 *owner) {
    new DebugMissionClearCollision(fn_801EB304(fn_8001C4F8(), 11), owner);
}

void DebugMissionClearCollision::Behave(float dt) {
    boost::shared_ptr<CharInfo> character = fn_800A8C54(fn_80074194(), 0).lock();
    if (character.get() != 0) {
        Vec3f offset;

        fn_800091EC(&offset, &(const Vec3f &)((VectorSubtractFn)fn_8040CD74)(
                                 &character->m_position, &m_chara->m_position));
        if (fn_8040CF50(&offset) < s_radius) {
            u32 side = m_params.side;
            if (side == 3) {
                fn_80175EEC(fn_80074470(), 6, 0);
                fn_800C9B34(m_owner);
                m_flags |= 1;
                return;
            }
            boost::shared_ptr<Mission::Mission> player = fn_800740E4(fn_8006D6A8(), side).lock();
            if (player.get() != 0 && !player->IsState(1)) {
                player->SetFlag();
                fn_8016B2A4(player.get());
                fn_800C9B34(m_owner);
                m_flags |= 1;
                return;
            }
        }
    }
    m_chara->m_rotation.y += s_turnSpeed * dt;
    DebugDraw();
}

void DebugMissionClearCollision::ReloadPlacement(UnkPlacement_800C9CD0 *owner) {
    ReadPlacement(owner);
    DebugDraw();
}

void DebugMissionClearCollision::ReadPlacement(UnkPlacement_800C9CD0 *owner) {
    fn_800C9F14(owner, &m_chara->m_position);
    fn_800C9ED4(owner, &m_chara->m_rotation);
    fn_800C9DBC(owner, &m_params, sizeof(m_params));
}

void DebugMissionClearCollision::DebugDraw() {
    fn_802E0E84(&m_chara->m_position, (const UnkDrawColor_802E0E84 *)s_sphereColors[m_params.side],
                s_radius);
    Mtx34f matrix;
    fn_8040C148(&matrix, &lbl_805E0720);
    fn_8040B59C(&matrix, &m_chara->m_position);
    fn_8040B848(&matrix, m_chara->m_rotation.y);
    for (u32 i = 0; i < 4; i++) {
        UnkDebugLine_80020A20 line(*(UnkDebugLine_80020A20 *)s_lineTable[i]);
        fn_800091D0(&line.m_start, &Ref(fn_8040B67C(&matrix, &line.m_start)));
        fn_800091D0(&line.m_end, &Ref(fn_8040B67C(&matrix, &line.m_end)));
        fn_802E0AA0(&line, s_lineColors[m_params.side], s_width);
    }
}

static GadgetParamDesc lbl_80557C04[] = {
    {2, "0:DK 1:NM 2:HR 3:BOSS", 0, 0, 3, 0.0f, 0.0f, 0.0f, fn_800CBC20, 0},
    {0},
};

GadgetClassDesc lbl_80557C54 = {
    "DebugDebugMissionClearCollision", fn_802C8820, fn_802C881C,
    DebugMissionClearCollision::Instantiate, 0, 0, 0x2592, 1, 0, 10, lbl_80557C04,
};
