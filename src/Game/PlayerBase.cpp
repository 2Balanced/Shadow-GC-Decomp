/* Player::PlayerBase (class name from RTTI; base Player::PlayerObject, a CharInfo): what every
 * character driven like a player shares, the player characters and the NPCs alike. Its
 * constructor sets up the transform, the command unit and the character's files; the C functions
 * below (other units call them by their addresses) count nested requests for state bits, keep the
 * motion's blend weights, turn vectors into and out of the character's frame, take the
 * character's tasks down and forward to its collision mover and history. The unit also holds
 * PlayerObject's out-of-line destructor and the accessors of the character file manager
 * (fn_80078C88) and of the stage's player data (fn_80079808). File name from the class; function
 * and member names are inferred. Definitions are in reverse address order (deferred inlining emits
 * functions last-defined first). CharInfo's and PlayerBase's slots 0x0C and PlayerBase's slot 0x14
 * take a parameter their symbols lack, so they are defined as C functions under those symbols.
 *
 * NonMatching (79 of 80 functions; every data section matches): UnkWords_8007A4D4's constructor
 * (0x8007A4D4) differs in two registers only, and GC/2.7 emits it and CharInfo's slot 0x0C at other
 * places among the unit's last weak copies (retail: ~PlayerObject, the control block's destructor,
 * CharInfo::vf_0x0C, UnkWords_8007A4D4(), dispose, get_deleter), which moves 0x80079F58..0x8007A568
 * in the DOL. */
#define PLAYER_OBJECT_MEMBERS
#define PLAYER_BASE_INLINE_SLOTS
#define PLAYER_BASE_IMPLEMENTATION
#define CHARINFO_INLINE_SLOTS
#define CHARINFO_INLINE_SLOT_2C
#define CHARCOMMANDUNIT_CALLBACK_CLEAR
#define CHARACTER_FILES_GET_HANDLE_EXTERN
#define CHARCOMMAND_PLAYER_COMMANDS
#include <string>
#include "Game/CharacterFiles.h"

#include "Game/CharCommand.h"
#include "Game/CharCommandUnit.h"
#include "Game/CharInfo.h"
#include "Game/GadgetCharCommands.h"
#include "Game/MathTypes.h"
#include "Game/Player.h"
#include "Game/PlayerMotionEvent.h"
#include "Game/Quaternion.h"
#include "Game/Task.h"
#include "Game/UnkWords_8007A4D4.h"
#include "boost/shared_ptr.hpp"
#include "boost/weak_ptr.hpp"
#include <dolphin/types.h>
#include "size_assert.h"

using boost::shared_ptr;
using boost::weak_ptr;
using Player::PlayerBase;
using Player::PlayerObject;

/* The pad input of a player (Game/PlayerPadInput.cpp; 0x88 bytes). */
struct UnkPlayerInput_800A85B0 {
    UnkPlayerInput_800A85B0();
    ~UnkPlayerInput_800A85B0();

    /* 0x00 */ u8 field_0x0[0x88];
};

/* The shadow the character casts (Game/PlayerUtil.cpp; 0x28 bytes). The character deletes it
 * without a destructor call. */
struct UnkShadow_8009368C {
    UnkShadow_8009368C();

    /* 0x00 */ u8 field_0x0[0x28];
};

/* The character's attack colliders (Game/BossSonic.cpp). */
struct UnkPlayerPart_80095BD4 {
    ~UnkPlayerPart_80095BD4();
};

/* A character's recent states (Game/PlayerHistory.cpp). */
struct UnkPlayerRecord_800A849C;
class UnkPlayerHistory_800A82C8 {
  public:
    UnkPlayerHistory_800A82C8(PlayerBase *player);
    ~UnkPlayerHistory_800A82C8();
    void Record();
    void Nothing();
    UnkPlayerRecord_800A849C *Get(int back);

    /* 0x00 */ u8 field_0x0[0x20];
};

/* The stage's data on the players (class name unknown; 0x7C4 bytes, made on first use). */
class UnkObject_80079808 {
  public:
    UnkObject_80079808();

    /* 0x000 */ u8 field_0x0[0x7C4];
};

/* A player's entry in it (struct name unknown). */
struct UnkPlayerInfo_80178B5C {
    /* 0x00 */ u8 field_0x0[0x28];
    /* 0x28 */ u8 m_state[1];
};

/* The weights of a motion's two blended layers (struct name unknown). */
struct UnkMotionWeights_800AA584 {
    void SetWeight(int layer, float weight) {
        if (layer != -2) {
            m_weights[layer] = weight;
        } else {
            for (int i = 0; i < 2; i++) {
                m_weights[i] = weight;
            }
        }
    }

    /* 0x00 */ u8 field_0x0[0x10];
    /* 0x10 */ float m_weights[2];
};

/* A contact of the collision mover (Game/PlayerCharaColliMove.cpp). */
struct UnkColliMoveContact_803334A4;
struct CharaColliMoveArgs;
typedef boost::function1<void, CharaColliMoveArgs &> CharaColliMoveCallback;

/* A vector returned through the out-of-line copy constructor (fn_800091EC; as in
 * Game/PlayerCharaColliMove.cpp). */
struct UnkCopyVector3_800091EC {
    UnkCopyVector3_800091EC(const UnkCopyVector3_800091EC &v) {
        fn_800091EC(reinterpret_cast<Vec3f *>(this), reinterpret_cast<const Vec3f *>(&v));
    }

    float x;
    float y;
    float z;
};

inline const UnkCopyVector3_800091EC &AsCopy(const Vec3f &v) {
    return reinterpret_cast<const UnkCopyVector3_800091EC &>(v);
}

namespace Player {

/* The character's collision mover (Game/PlayerCharaColliMove.cpp). */
class CharaColliMove {
  public:
    void Init();
    void SetCallback(const CharaColliMoveCallback &callback);
    void SetContact(const UnkColliMoveContact_803334A4 &contact);
    void ClearContact();
    const UnkColliMoveContact_803334A4 *GetContact() const;
    UnkCopyVector3_800091EC GetVelocity() const;
};

/* The character's aim (only its virtual destructor is used here). */
class PlayerTargetBase {
  public:
    virtual ~PlayerTargetBase();
};

/* The character's optional parts (Game/PlayerOption.cpp). */
class UnkCharaSlots_800AADA4 {
  public:
    UnkCharaSlots_800AADA4(const shared_ptr<PlayerBase> &player);
    ~UnkCharaSlots_800AADA4();

    /* 0x00 */ u8 field_0x0[0x18];
};

/* Game/PlayerEffect.cpp. */
class UnkPlayerData_800A25E0 {
  public:
    ~UnkPlayerData_800A25E0();
};

} // namespace Player

/* What slot 0x14 of the character is given (class name unknown): its slot 0xC is told the
 * character. */
class UnkCharaVisitor_80079918 {
  public:
    virtual void vf_0x08();
    virtual void Visit(const weak_ptr<CharInfo> &chara);
};

extern "C" {
/* Tests a state bit. */
bool fn_8006CD78(UnkWords_8007A4D4 *bits, int bit);
/* Sets and clears a state bit. */
void fn_80076CA8(UnkWords_8007A4D4 *bits, int bit);
void fn_80076CC8(UnkWords_8007A4D4 *bits, int bit);
/* Sets the character's name (Game/CharInfo.cpp). */
void fn_801DC984(CharInfo *info, const char *name);
/* Sends a command to a command unit (Game/CharCommand.cpp). */
void fn_801D9C0C(shared_ptr<CharCommandUnit> unit, CharCommand *command);
/* Tells the character's parts their pad (behaviour inferred). */
void fn_800AAD44(Player::UnkCharaSlots_800AADA4 *slots, int pad);
/* Called first by the destructor (behaviour unknown; it takes no arguments). */
void fn_80094070(void);
/* The weights of a motion. */
UnkMotionWeights_800AA584 *fn_800AA584(Player::PlayerMotion *motion);
/* A player's entry in the stage's data, and its state. */
UnkPlayerInfo_80178B5C *fn_80178B5C(UnkObject_80079808 *data, int pad);
int fn_8016F280(u8 *state);
/* Moves the player (Game/PlayerUtil.cpp). */
void fn_800936F4(shared_ptr<PlayerBase> *player, float dt);
/* Finds the ground below a point along a direction (Game/PlayerUtil.cpp). */
void fn_800935B4(UnkShadow_8009368C *shadow, const Vec3f *position, const Vec3f *direction, int arg3);
/* Vector and matrix routines. */
void fn_8040CE60(Vec3f *vector); /* sets the vector to zero */
void fn_8040C908(Quatf *q);      /* sets the identity */
Mtx34f fn_8040BEF8(const Mtx34f *matrix); /* inverse */
Vec3f fn_8040B67C(const Mtx34f *m, const Vec3f *point);
Vec3f fn_8040D044(const Vec3f *v, float angle); /* turned about z */
Vec3f fn_8040D118(const Vec3f *v, float angle); /* turned about x */
Vec3f fn_8040D0AC(const Vec3f *v, float angle); /* turned about y */
Vec3f fn_8040CEE8(const Vec3f *v);              /* normalized */
Vec3f fn_8040CDA8(const Vec3f *v, float scale);
Vec3f fn_8040CDD0(const Vec3f *a, const Vec3f *b); /* a + b */
}

/* fn_8040CD74 as a function that returns its result (see Game/EnemyRender.cpp). */
typedef Vec3f (*VectorSubtractFn)(const Vec3f *lhs, const Vec3f *rhs);
#define Sub(a, b) ((VectorSubtractFn)fn_8040CD74)(a, b)

#define TestBit(bits, bit) fn_8006CD78(bits, bit)

/* Returns x (an inline call, so that the accessors below are not inlined themselves; deferred
 * inlining is one level deep). */
template <class T> inline T &Ref(T &x) {
    return x;
}

/* The instance pointer of a class made on first use (as in Game/NpcControlRestartFollow.cpp). */
template <class T> struct Instance {
    static T *s_instance;
};

template <class T> T *Instance<T>::s_instance;

/* A quaternion's assignment (other units call it by its address). */
inline void CopyQuat(Quatf *q, const Quatf *value) {
    q->x = value->x;
    q->y = value->y;
    q->z = value->z;
    q->w = value->w;
}

extern "C" inline void fn_8007A34C(Quatf *q, const Quatf *value) {
    CopyQuat(q, value);
}

/* CharInfo's slot 0x0C, inline in the game (its symbol has no parameter; the game's copy
 * stores the player number at 0x54). */
extern "C" void vf_0x0C__8CharInfoFv(CharInfo *info, int pad) {
    info->field_0x54 = pad;
}

namespace Player {

PlayerBase::PlayerBase(int type, int kind, const char *name)
    : PlayerObject(1), m_input(new UnkPlayerInput_800A85B0) {
    fn_800938C8(field_0x270);
    SetPlayerFlag(type);
    fn_801DC984(this, name);
    SetKind(kind);
    field_0x30 = 2;
    m_counter80 = 0;
    m_counter84 = 0;
    m_counter88 = 0;
    m_time8C = 0.0f;
    m_counter90 = 0;
    m_time94 = 0.0f;
    field_0x98 = 0.0f;
    field_0x9C = 0.0f;
    fn_8040CE60(&field_0xB8);
    fn_8040CE60(&field_0xC4);
    fn_800091D0(&field_0xD0, &m_position);
    Vec3f up(0.0f, 1.0f, 0.0f);
    fn_800091D0(&m_up, &up);
    fn_800091D0(&field_0xF4, &up);
    fn_8040CE60(&field_0x100);
    fn_8040B6F8(&field_0x11C);
    fn_800091D0(&field_0x17C, &up);
    fn_8040C908(&field_0x188);
    fn_8040B6F8(&field_0x198);
    fn_800091D0(&field_0x1C8, &field_0x17C);
    fn_8007A34C(&field_0x1D4, &field_0x188);
    fn_8040C148(&field_0x1E4, &field_0x198);
    m_commandUnit = shared_ptr<CharCommandUnit>(new CharCommandUnit);
    m_task = 0;
    m_colliMove = 0;
    m_behaviors = 0;
    m_motion = 0;
    m_attack = 0;
    m_weapon = 0;
    field_0x23C = 0;
    m_sound = 0;
    m_data = 0;
    m_part = 0;
    m_target = 0;
    m_charaSlots = 0;
    m_history = 0;
    m_layerTask = 0;
    m_rootTask = 0;
    field_0x274 = 0.0f;
    m_shadow = new UnkShadow_8009368C;
    m_time27C = 0.0f;
    for (int i = 0; i < 2; i++) {
        m_weightCount[i] = 0;
        m_weight[i] = 1.0f;
    }
    fn_80078C88()->Load(fn_8027A994(m_kind));
}

} // namespace Player

/* The game's character file manager, made on first use (other units call it by its address). */
extern "C" inline CharacterFileManager *fn_80078C88(void) {
    if (Instance<CharacterFileManager>::s_instance == 0) {
        static CharacterFileManager s_manager;
        Instance<CharacterFileManager>::s_instance = &Ref(s_manager);
    }
    return Instance<CharacterFileManager>::s_instance;
}

namespace Player {

PlayerBase::~PlayerBase() {
    fn_80094070();
    if (m_shadow) {
        delete m_shadow;
        m_shadow = 0;
    }
    if (m_history) {
        delete m_history;
        m_history = 0;
    }
    if (m_data) {
        delete m_data;
        m_data = 0;
    }
    fn_80078C88()->Unload(fn_8027A994(m_kind));
    __dt__18UnkShadow_8009368CFv(field_0x270, -1);
}

/* Makes the character's history and parts (slot 0x30). */
void PlayerBase::vf_0x30() {
    m_history = new UnkPlayerHistory_800A82C8(this);
    m_charaSlots = new UnkCharaSlots_800AADA4(m_self.lock());
}

void PlayerBase::PushCommand(CharCommand *command) {
    fn_801D9C0C(m_commandUnit, command);
}

} // namespace Player

/* PlayerBase's slots 0x14 and 0x0C (their symbols have no parameters): tells visitor the
 * character, and sets the player number (also telling the character's parts). */
extern "C" void vf_0x14__Q26Player10PlayerBaseFv(PlayerBase *player, UnkCharaVisitor_80079918 *visitor) {
    visitor->Visit(weak_ptr<CharInfo>(player->m_self));
}

extern "C" void vf_0x0C__Q26Player10PlayerBaseFv(PlayerBase *player, int pad) {
    player->field_0x54 = pad;
    if (player->m_charaSlots) {
        fn_800AAD44(player->m_charaSlots, pad);
    }
}

extern "C" {

/* Casts the character's shadow on the ground below it. */
void fn_80079860(PlayerBase *player) {
    Vec3f start;
    fn_800091EC(&start, &(const Vec3f &)fn_8040CDD0(
                            &(const Vec3f &)Sub(&player->m_position, &(const Vec3f &)fn_8040CDA8(
                                                                         &player->field_0x17C,
                                                                         player->field_0x274)),
                            &player->m_up));
    fn_800935B4(player->m_shadow, &start, &(const Vec3f &)fn_8040CDA8(&player->m_up, -300.0f), 0);
}

/* The stage's data on the players, made on first use (other units call it by its address). */
inline UnkObject_80079808 *fn_80079808(void) {
    if (Instance<UnkObject_80079808>::s_instance == 0) {
        static UnkObject_80079808 s_data;
        Instance<UnkObject_80079808>::s_instance = &Ref(s_data);
    }
    return Instance<UnkObject_80079808>::s_instance;
}

} // extern "C"

namespace Player {

/* Whether the character's pad may act (slot 0x50). */
bool PlayerBase::vf_0x50() {
    int pad = field_0x54;
    if (TestBit(&m_stateBits, 0x24)) {
        return false;
    }
    if (TestBit(&m_stateBits, 0x20)) {
        return false;
    }
    if (pad >= 0) {
        UnkPlayerInfo_80178B5C *info = fn_80178B5C(fn_80079808(), pad);
        if (info) {
            return fn_8016F280(info->m_state) == 0;
        }
    }
    return false;
}

} // namespace Player

extern "C" {

/* Moves the character by its velocity over the frame. */
void fn_80079720(PlayerBase *player) {
    fn_800936F4(&const_cast<shared_ptr<PlayerBase> &>((const shared_ptr<PlayerBase> &)player->m_self.lock()),
                **reinterpret_cast<float **>(player->field_0x60));
}

Vec3f fn_80079628(PlayerBase *player, const Vec3f *offset);

/* The character's forward and up directions. */
Vec3f fn_800796DC(PlayerBase *player) {
    return fn_80079628(player, &Vec3f(0.0f, 0.0f, -1.0f));
}

Vec3f fn_80079698(PlayerBase *player) {
    return fn_80079628(player, &Vec3f(0.0f, 1.0f, 0.0f));
}

/* offset turned into the character's frame. */
Vec3f fn_80079628(PlayerBase *player, const Vec3f *offset) {
    Mtx34f matrix;
    fn_8040C148(&matrix, &player->field_0x1E4);
    fn_8040B848(&matrix, player->m_rotation.y);
    fn_8040B884(&matrix, player->m_rotation.x);
    fn_8040B80C(&matrix, player->m_rotation.z);
    return fn_8040B67C(&matrix, offset);
}

/* The direction to target in the character's own frame (forward if target is at the character). */
Vec3f fn_80079508(PlayerBase *player, const Vec3f *target) {
    Vec3f direction;
    fn_800091EC(&direction, &(const Vec3f &)Sub(target, &player->m_position));
    if (direction.x * direction.x + direction.y * direction.y + direction.z * direction.z < 1e-4f) {
        return Vec3f(0.0f, 0.0f, -1.0f);
    }
    fn_800091D0(&direction, &(const Vec3f &)fn_8040B67C(&(const Mtx34f &)fn_8040BEF8(&player->field_0x1E4),
                                                       &direction));
    fn_800091D0(&direction, &(const Vec3f &)fn_8040D0AC(&direction, -player->m_rotation.y));
    fn_800091D0(&direction, &(const Vec3f &)fn_8040D118(&direction, -player->m_rotation.x));
    fn_800091D0(&direction, &(const Vec3f &)fn_8040D044(&direction, -player->m_rotation.z));
    return fn_8040CEE8(&direction);
}

void fn_800794C8(PlayerBase *player) {
    fn_80076CC8(&player->m_stateBits, 0x23);
    fn_80076CC8(&player->m_stateBits, 0x22);
}

} // extern "C"

namespace Player {

StartActionCommand::StartActionCommand(int arg) : PlayerCommand(0x106, arg) {}

} // namespace Player

extern "C" {

/* Starts the character's action if it may. */
void fn_80079404(PlayerBase *player) {
    if (TestBit(&player->m_stateBits, 0x14) || TestBit(&player->m_stateBits, 0x15)) {
        player->PushCommand(new Player::StartActionCommand(0));
    }
}

void fn_8007939C(PlayerBase *player, bool on) {
    if (on) {
        player->m_counter80++;
    } else {
        player->m_counter80--;
    }
    if (player->m_counter80 > 0) {
        fn_80076CA8(&player->m_stateBits, 0x17);
    } else {
        fn_80076CC8(&player->m_stateBits, 0x17);
    }
}

void fn_80079334(PlayerBase *player, bool on) {
    if (on) {
        player->m_counter84++;
    } else {
        player->m_counter84--;
    }
    if (player->m_counter84 > 0) {
        fn_80076CA8(&player->m_stateBits, 0x3F);
    } else {
        fn_80076CC8(&player->m_stateBits, 0x3F);
    }
}

void fn_8007922C(PlayerBase *player);

void fn_800792D4(PlayerBase *player, float time) {
    if (player->m_time8C < time) {
        player->m_time8C = time;
    }
    if (!TestBit(&player->m_stateBits, 0x2B)) {
        fn_80076CA8(&player->m_stateBits, 0x2B);
        fn_8007922C(player);
    }
}

void fn_80079284(PlayerBase *player) {
    if (!TestBit(&player->m_stateBits, 0x2D)) {
        fn_80076CA8(&player->m_stateBits, 0x2D);
        fn_8007922C(player);
    }
}

void fn_8007922C(PlayerBase *player) {
    player->m_counter88++;
    fn_80076CA8(&player->m_stateBits, 0x2A);
    player->m_counter90++;
    fn_80076CA8(&player->m_stateBits, 0x2E);
}

void fn_800791BC(PlayerBase *player) {
    player->m_counter88--;
    if (player->m_counter88 == 0) {
        fn_80076CC8(&player->m_stateBits, 0x2A);
    }
    player->m_counter90--;
    if (player->m_counter90 == 0) {
        fn_80076CC8(&player->m_stateBits, 0x2E);
    }
}

void fn_8007914C(PlayerBase *player, float time) {
    if (player->m_time94 < time) {
        player->m_time94 = time;
    }
    if (!TestBit(&player->m_stateBits, 0x2F)) {
        fn_80076CA8(&player->m_stateBits, 0x2F);
        player->m_counter90++;
        fn_80076CA8(&player->m_stateBits, 0x2E);
    }
}

void fn_800790EC(PlayerBase *player) {
    if (!TestBit(&player->m_stateBits, 0x30)) {
        fn_80076CA8(&player->m_stateBits, 0x30);
        player->m_counter90++;
        fn_80076CA8(&player->m_stateBits, 0x2E);
    }
}

void fn_800790B4(PlayerBase *player) {
    player->m_counter90++;
    fn_80076CA8(&player->m_stateBits, 0x2E);
}

void fn_80079074(PlayerBase *player) {
    player->m_counter90--;
    if (player->m_counter90 == 0) {
        fn_80076CC8(&player->m_stateBits, 0x2E);
    }
}

/* Keeps state bit 0x18 set for time. */
void fn_80079028(PlayerBase *player, float time) {
    if (time >= player->m_time27C) {
        player->m_time27C = time;
    }
    if (player->m_time27C > 0.0f) {
        fn_80076CA8(&player->m_stateBits, 0x18);
    }
}

void fn_80078FEC(PlayerBase *player) {
    if (player->m_time27C > 0.0f) {
        player->m_time27C = 0.0f;
        fn_80076CC8(&player->m_stateBits, 0x18);
    }
}

void fn_80078F64(PlayerBase *player, float dt) {
    if (player->m_time27C > 0.0f) {
        fn_80076CA8(&player->m_stateBits, 0x18);
        player->m_time27C -= dt;
        if (player->m_time27C <= 0.0f) {
            player->m_time27C = 0.0f;
            fn_80076CC8(&player->m_stateBits, 0x18);
        }
    }
}

/* Counts the requests to hold the weight of a layer at 1. */
void fn_80078F2C(PlayerBase *player, int layer, bool on) {
    if (on) {
        player->m_weightCount[layer]++;
    } else {
        player->m_weightCount[layer]--;
    }
}

void fn_80078F1C(PlayerBase *player, int layer, float weight) {
    player->m_weight[layer] = weight;
}

/* Gives the motion its layers' weights. */
void fn_80078E84(PlayerBase *player) {
    if (player->m_motion) {
        UnkMotionWeights_800AA584 *weights = fn_800AA584(player->m_motion);
        for (int i = 0; i < 2; i++) {
            float weight = player->m_weight[i];
            if (player->m_weightCount[i] > 0) {
                weight = 1.0f;
            }
            weights->SetWeight(i, weight);
        }
    }
}

void fn_80078E58(PlayerBase *player, const CharaColliMoveCallback &callback) {
    if (player->m_colliMove) {
        player->m_colliMove->SetCallback(callback);
    }
}

void fn_80078E2C(PlayerBase *player) {
    if (player->m_colliMove) {
        player->m_colliMove->Init();
    }
}

void fn_80078E00(PlayerBase *player, const UnkColliMoveContact_803334A4 &contact) {
    if (player->m_colliMove) {
        player->m_colliMove->SetContact(contact);
    }
}

void fn_80078DD4(PlayerBase *player) {
    if (player->m_colliMove) {
        player->m_colliMove->ClearContact();
    }
}

const UnkColliMoveContact_803334A4 *fn_80078DA0(const PlayerBase *player) {
    return player->m_colliMove ? player->m_colliMove->GetContact() : 0;
}

UnkCopyVector3_800091EC fn_80078D58(const PlayerBase *player) {
    if (player->m_colliMove) {
        return player->m_colliMove->GetVelocity();
    }
    return AsCopy(lbl_805E07A0);
}

void fn_80078D34(PlayerBase *player) {
    player->m_history->Record();
}

void fn_80078D10(PlayerBase *player) {
    player->m_history->Nothing();
}

UnkPlayerRecord_800A849C *fn_80078CEC(PlayerBase *player, int back) {
    return player->m_history->Get(back);
}


/* The loader slot of the character's file. */
s32 fn_80078C4C(PlayerBase *player) {
    return fn_80078C88()->GetHandle(fn_8027A994(player->m_kind));
}

/* Takes the character's tasks and parts down (Game/PlayerManageUnit.cpp and
 * Game/NpcManageUnit.cpp call it when the character leaves). */
void fn_80078B54(PlayerBase *player) {
    if (player->m_rootTask) {
        player->m_rootTask->m_flags |= 1;
        player->m_rootTask = 0;
    }
    if (player->m_layerTask) {
        player->m_layerTask->m_flags |= 1;
        player->m_layerTask = 0;
    }
    if (player->m_part) {
        delete player->m_part;
        player->m_part = 0;
    }
    if (player->m_target) {
        delete player->m_target;
        player->m_target = 0;
    }
    if (player->m_charaSlots) {
        delete player->m_charaSlots;
        player->m_charaSlots = 0;
    }
    player->m_commandUnit->m_callback.Clear();
}

} // extern "C"
