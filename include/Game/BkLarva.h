#ifndef GAME_BKLARVA_H
#define GAME_BKLARVA_H

/* The Black Arms larvae (class names from RTTI): BK_LARVA in the stage editor places a
 * BkLarvaGenerator, a SetEnemyBase task that spawns BkLarva tasks around its position (see
 * Game/BkLarva.cpp). Layouts from the constructors and destructors; member names are inferred. */

#include "Game/CharaColli.h"
#include "Game/Enemy.h"
#include "Game/GadgetSound.h"
#include "Game/MathTypes.h"
#include "Game/SetAdapterState.h"
#include "Game/Task.h"
#include "boost/shared_ptr.hpp"
#include <dolphin/types.h>
#include "size_assert.h"

class CharCommand;
class Target;
class UnkSound_802B4254;
namespace SonicteamUSA {
namespace System {
namespace CharaColli {
struct AttackCallbackParam;
} // namespace CharaColli
} // namespace System
} // namespace SonicteamUSA

/* The model part of an enemy (class name unknown; constructor 0x801A3E9C, destructor 0x801A3E48;
 * 8 bytes). Game/EnemyRender.cpp and Game/ThrowBomb.cpp declare it locally as well. */
class UnkModel_801A3E9C {
  public:
    UnkModel_801A3E9C();
    ~UnkModel_801A3E9C();

    /* 0x0 */ u32 field_0x0;
    /* 0x4 */ s32 m_frameCount;
};

extern "C" {
/* The placement record's side of the task link (see Game/Gadget.cpp), and freeing a block of
 * memory (behaviours inferred). */
void fn_800C9CD0(UnkPlacement_800C9CD0 *owner, Task *task);
void fn_800C9BE8(UnkPlacement_800C9CD0 *owner);
bool fn_800C9A64(UnkPlacement_800C9CD0 *owner);
bool fn_800C996C(UnkPlacement_800C9CD0 *owner);
void fn_80049B34(void *data);
}

/* SetEnemyBase (class name from RTTI; vtable 0x805308F0): a Task that runs Behave while its
 * placement record exists, like Gadget. Only BkLarvaGenerator derives from it. The constructor is
 * inline; Tick (0x8018C310) and the destructor (0x8018C3A4, which ~BkLarvaGenerator inlines) are
 * defined in Game/BkLarva.cpp. */
class SetEnemyBase : public Task {
  public:
    SetEnemyBase(Task *parent, UnkPlacement_800C9CD0 *owner) : Task(parent) {
        m_name = s_taskName;
        m_owner = 0;
        m_blocks = 0;
        if (owner != 0) {
            m_owner = owner;
            fn_800C9CD0(m_owner, this);
        }
    }
    virtual ~SetEnemyBase();
    virtual void Tick(float dt);
    /* The object's own frame update, called while its record exists. */
    virtual void Behave(float dt) = 0;
    /* Picks up parameters that were changed on the record. */
    virtual void ReloadPlacement(UnkPlacement_800C9CD0 *owner) = 0;

    /* "SetEnemyBase". */
    static const char *s_taskName;

    /* 0x28 */ UnkPlacement_800C9CD0 *m_owner;
    /* 0x2C */ s32 m_blockCount;
    /* 0x30 */ s16 *m_blocks; /* the ids of the blocks at its position, which it waits for */
    /* 0x34 */ u32 field_0x34;
};
SIZE_ASSERT(SetEnemyBase, 0x38);

/* The parameters of BK_LARVA (names from the parameter table). */
struct BkLarvaParams {
    /* Out of line in retail (0x8018A3D4, a weak copy): it calls Reset, an inline function, so
     * GC/2.7 does not inline it (not original; Reset's name is inferred). */
    BkLarvaParams() {
        Reset();
    }
    void Reset() {
        MoveRange = 200.0f;
        SearchRange = 0.0f;
        SearchAngle = 0.0f;
        SearchWidth = 100.0f;
        SearchHeight = 100.0f;
        SearchHeightOffset = 0.0f;
        MoveSpeedRatio = 1.0f;
        Num = 1;
        AppearRange = 0.0f;
        AppearType = 0;
        Offset_Y = 0.0f;
    }

    /* 0x00 */ float MoveRange;
    /* 0x04 */ float SearchRange;
    /* 0x08 */ float SearchAngle;
    /* 0x0C */ float SearchWidth;
    /* 0x10 */ float SearchHeight;
    /* 0x14 */ float SearchHeightOffset;
    /* 0x18 */ float MoveSpeedRatio;
    /* 0x1C */ s32 Num;
    /* 0x20 */ float AppearRange;
    /* 0x24 */ s32 AppearType; /* 0 NORMAL, 1 DROP */
    /* 0x28 */ float Offset_Y;
};
SIZE_ASSERT(BkLarvaParams, 0x2C);

/* SBkLarvaStateData (class name from RTTI; vtable 0x80530878): what a generator keeps on its
 * placement record: the number of larvae still alive (names inferred). */
class SBkLarvaStateData : public SetAdapterState {
  public:
    SBkLarvaStateData(s32 remaining) : m_remaining(remaining), m_total(remaining) {}

    /* 0x4 */ s32 m_remaining;
    /* 0x8 */ s32 m_total;
};
SIZE_ASSERT(SBkLarvaStateData, 0xC);

/* BkLarvaGenerator (class name from RTTI; vtable 0x80530860): spawns the larvae, plays their
 * shared motions and checks whether the player is in its area. */
class BkLarvaGenerator : public SetEnemyBase {
  public:
    BkLarvaGenerator(Task *parent, UnkPlacement_800C9CD0 *owner);
    virtual ~BkLarvaGenerator();
    virtual void Behave(float dt);
    virtual void ReloadPlacement(UnkPlacement_800C9CD0 *owner);

    /* Creates the generator under task layer 14. */
    static void Instantiate(UnkPlacement_800C9CD0 *owner);

    /* Reads the placement, the parameters and the saved state, and builds the matrices. */
    void ReadPlacement(UnkPlacement_800C9CD0 *owner);
    /* Stores the number of larvae alive on the placement record. */
    void SaveState();
    /* The command unit's callback. */
    void OnCommand(CharCommand &command);
    /* The slowest and fastest speed of a larva hop (GetMinSpeed is out of line at 0x80189600 and
     * GetMaxSpeed has a weak copy at 0x8018B114; GetBaseSpeed is not original). */
    float GetMinSpeed();
    float GetBaseSpeed() {
        return 12.5f * m_params.MoveSpeedRatio;
    }
    float GetMaxSpeed() {
        return field_0x13C + GetBaseSpeed();
    }

    static const char *s_taskName;

    /* 0x038 */ boost::shared_ptr<UnkEnemyInfo_8017D014> m_chara;
    /* 0x040 */ Vec3f m_position;
    /* 0x04C */ Vec3f m_rotation;
    /* 0x058 */ BkLarvaParams m_params;
    /* 0x084 */ s32 m_state; /* 0 loading, 1 waiting for the blocks, 2 spawning, 3 running */
    /* 0x088 */ UnkEnemyArea_8018A374 m_area;
    /* 0x0CC */ Mtx34f m_matrix;
    /* 0x0FC */ Mtx34f m_inverse;
    /* 0x12C */ Vec3f m_up;
    /* 0x138 */ u8 m_playerInArea;
    /* 0x13C */ float field_0x13C; /* 20: added to the fastest hop speed */
    /* 0x140 */ s32 m_spawned;
    /* 0x144 */ s32 m_soundCount; /* the larvae playing their hop sound (at most three) */
    /* 0x148 */ s32 m_remaining;
    /* 0x14C */ u32 field_0x14C;
};
SIZE_ASSERT(BkLarvaGenerator, 0x150);

/* BkLarva (class name from RTTI; vtable 0x805308E0): one larva, a Task below its generator. */
class BkLarva : public Task {
  public:
    BkLarva(BkLarvaGenerator *generator, s32 index);
    virtual ~BkLarva();
    virtual void Tick(float dt);

    /* Places the larva at a random point of the generator's area. */
    void Place();
    /* Moves the larva and its model. */
    void Move(float dt);
    /* Pushes the larva away from the larvae it touches. */
    void Separate();
    /* The command unit's callback. */
    void OnCommand(CharCommand &command);
    /* The collider's callbacks. */
    void OnHit(SonicteamUSA::System::CharaColli::AttackCallbackParam &param);
    void OnAttacked(SonicteamUSA::System::CharaColli::AttackCallbackParam &param);

    static const char *s_taskName;

    /* 0x28 */ s32 m_index;
    /* 0x2C */ s32 m_motion;
    /* 0x30 */ BkLarvaGenerator *m_generator;
    /* 0x34 */ boost::shared_ptr<UnkEnemyInfo_8017D014> m_chara;
    /* 0x3C */ boost::shared_ptr<SonicteamUSA::System::CharaColli::CharaColliBase> m_collider;
    /* 0x44 */ UnkModel_801A3E9C m_model;
    /* 0x4C */ u8 m_frame[0x40]; /* a RenderWare matrix: the model's frame in the motion */
    /* 0x8C */ float m_timer;
    /* 0x90 */ boost::shared_ptr<Target> m_target;
    /* 0x98 */ u8 m_falling; /* under gravity */
    /* 0x9C */ float m_spinX;
    /* 0xA0 */ float field_0xA0;
    /* 0xA4 */ float m_spinZ;
    /* 0xA8 */ UnkSoundHandle_801D3DC8 m_sound;
    /* 0xC4 */ boost::shared_ptr<UnkSound_802B4254> m_loop;
    /* 0xCC */ s32 m_state; /* 0 hidden (DROP), 1 dropping, 2 waiting, 3 hopping, 4 hit, 5 thrown */
};
SIZE_ASSERT(BkLarva, 0xD0);

#endif
