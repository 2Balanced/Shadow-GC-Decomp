#ifndef GAME_MISSION_H
#define GAME_MISSION_H

#include <dolphin/types.h>
#include "size_assert.h"

/* A stage's missions (class names from RTTI; the code is at 0x8016AA90..0x8016ECE8, not yet
 * matched). A Mission tracks one goal for one player: its state (0 running, 1 cleared, 2 failed)
 * and a message manager that puts the goal on screen. CountMission and its subclasses count
 * towards a target and share Mission as a virtual base. Member names are inferred from what the
 * functions do; the virtuals of Mission are listed in vtable order (offsets in the vtable).
 *
 * Gadget units reach a player's mission through fn_800740E4 (a weak_ptr<Mission>; the shared_ptr /
 * weak_ptr instances at 0x80073EA0..0x800740E4). */
namespace Mission {

#ifdef MISSION_MESSAGE_MEMBERS
/* The message managers of Mission and CountMission, as far as the mission units call them (their
 * definitions are in Game/BasicMessage.cpp and Game/CountMessage.cpp). Behind a macro: the units
 * that only read the missions keep the members as bytes. */
enum MissionState {
    kMissionRunning,
    kMissionCleared,
    kMissionFailed,
};

class BasicMessageManage {
  public:
    BasicMessageManage(int id, int player);
    ~BasicMessageManage();
    void Show(MissionState state);

    u8 field_0x0[0x28];
};

class CountMessageManage {
  public:
    CountMessageManage(int id, int player, int target);
    ~CountMessageManage();
    void SetCount(s32 count);

    u8 field_0x0[0x30];
};
#endif

class Mission {
  public:
    Mission(int id, int player);

    /* 0x08 */ virtual ~Mission();
    /* 0x0C */ virtual int GetType(); /* 0 here; 1 Goal, 2 Count, 3 Enemy, 7 Ring */
    /* 0x10 */ virtual bool IsTimed();
    /* 0x14 */ virtual float GetTimeLimit();
    /* 0x18 */ virtual int GetCount(int index);
    /* 0x1C */ virtual int GetTarget();
    /* 0x20 */ virtual int GetSubTarget();
    /* 0x24 */ virtual void AddCount(int delta, int index);
    /* 0x28 */ virtual void SetCount(int value, int index);
    /* 0x2C */ virtual void SetTarget(int value, int index);
    /* 0x30 */ virtual int vf_0x30();
    /* 0x34 */ virtual void vf_0x34();
    /* 0x38 */ virtual void OnSave();
    /* 0x3C */ virtual void OnRestore();
    /* 0x40 */ virtual void OnReset();
    /* 0x44 */ virtual void OnUpdate(); /* called each frame while the mission runs */

    /* Inline helpers the gadget units use (names inferred). */
    bool IsState(s32 state) const {
        return m_state == state;
    }
    void GetState(s32 *state) {
        *state = m_state;
    }
    void SetFlag() {
        m_flag = 1;
    }
    int GetId() const {
        return m_id;
    }
    u8 GetFlag() const {
        return m_flag;
    }

    /* Defined in Game/Mission.cpp (names inferred). Clearing and failing a mission are
     * fn_8016B2A4 and fn_8016B270 there, which the gadget units call under those names. */
    void Update(float dt);
    void Save();
    void Restore();
    void Reset();
    void OnCleared();
    void OnFailed();

    /* 0x04 */ int m_id;
    /* 0x08 */ int m_player;
    /* 0x0C */ int m_state;
    /* 0x10 */ int m_savedState;
    /* 0x14 */ u8 m_flag;
    /* 0x15 */ u8 m_savedFlag;
    /* 0x16 */ u8 field_0x16[2];
#ifdef MISSION_MESSAGE_MEMBERS
    /* 0x18 */ BasicMessageManage m_message; /* the message manager */
#else
    /* 0x18 */ u8 m_message[0x28]; /* the message manager (Mission::BasicMessageManage) */
#endif
};
SIZE_ASSERT(Mission, 0x40);

/* Counts up to m_target (and a second count up to m_subTarget); clears the mission when the first
 * count reaches its target and fails it when the second one does. */
class CountMission : public virtual Mission {
  public:
    CountMission(int id, int player, int target, int subTarget, int count, int subCount);

    virtual ~CountMission();
    virtual int GetType();
    virtual int GetCount(int index);
    virtual int GetTarget();
    virtual int GetSubTarget();
    virtual void AddCount(int delta, int index);
    virtual void SetCount(int value, int index);
    virtual void SetTarget(int value, int index);
    virtual void OnSave();
    virtual void OnRestore();
    virtual void OnReset();

    /* 0x08 */ int m_target;
    /* 0x0C */ int m_subTarget;
    /* 0x10 */ int m_initCount;
    /* 0x14 */ int m_initSubCount;
    /* 0x18 */ int m_count;
    /* 0x1C */ int m_subCount;
    /* 0x20 */ int m_savedCount;
    /* 0x24 */ int m_savedSubCount;
#ifdef MISSION_MESSAGE_MEMBERS
    /* 0x28 */ CountMessageManage m_message; /* the count display */
#else
    /* 0x28 */ u8 m_message[0x30]; /* the count display (Mission::CountMessageManage) */
#endif
};

} // namespace Mission

#endif
