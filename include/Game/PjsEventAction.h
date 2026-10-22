#ifndef GAME_PJSEVENTACTION_H
#define GAME_PJSEVENTACTION_H

/* The event (cutscene) player: TPjsEventActionManager plays the scenes of one event, each a
 * TPjsEventActionScene (Game/PjsEventActionScene.cpp), below a TPjsEventFolder task. Class names
 * come from RTTI; member names are inferred. */
#include "Game/Task.h"
#include <dolphin/types.h>
#include "size_assert.h"

/* A base class of the event's tasks (class name from RTTI; no members or virtuals are known). */
class TPjsEventActionData {};

/* The task that plays an event: it finds the event's scene files, keeps the current scene and the
 * next one loaded (two at a time), starts the event's music and lets the player skip the event. */
class TPjsEventActionManager : public TPjsEventActionData, public Task {
  public:
    TPjsEventActionManager(Task *parent, int eventId);
    virtual ~TPjsEventActionManager();
    virtual void Tick(float dt);

    /* Finds the event's scene files (event\eventNNNN_sceneX.one). */
    void FindScenes();
    /* Undoes what FindScenes set up (behaviour inferred). */
    void Release();
    /* Lets the player skip the event, if the game allows it. */
    void RequestSkip();
    /* Loads scene sceneNo into slot (the other slot if negative); false if the slot is in use. */
    bool CreateScene(int sceneNo, int slot);
    /* The slot that is not the current one. */
    int OtherIndex();

    static const char *s_taskName;

    /* 0x28 */ s32 m_state;
    /* 0x2C */ s32 m_sceneIndex; /* the slot of the scene playing */
    /* 0x30 */ float m_time; /* the event time at the scene's start */
    /* 0x34 */ u8 m_skip;
    /* 0x38 */ s32 m_savedMode; /* the game state's word at 0x10 before the event */
    /* 0x3C */ u32 field_0x3C;
    /* 0x40 */ s32 m_wait;
    /* 0x44 */ s32 m_endCount;
    /* 0x48 */ s32 m_musicWait;
    /* 0x4C */ s32 m_battleMusic; /* which battle music to play (never set here) */
    /* 0x50 */ u8 m_ended; /* the last scene played to its end */
    /* 0x54 */ s32 m_scenes[12]; /* the numbers of the scene files found */
    /* 0x84 */ s32 m_sceneCount;
    /* 0x88 */ s32 m_nextScene;
    /* 0x8C */ u32 field_0x8C;
};
SIZE_ASSERT(TPjsEventActionManager, 0x90);

/* The task the event tasks hang below (class name from RTTI). */
class TPjsEventFolder : public Task {
  public:
    TPjsEventFolder(Task *parent);
    virtual ~TPjsEventFolder();

    static const char *s_taskName;
};
SIZE_ASSERT(TPjsEventFolder, 0x28);

/* What fn_801723E0 returns (class name unknown; see Game/PjsEventActionFade.cpp): the event
 * playing, the folder its tasks hang below, and whether the event is seen for the first time. */
struct UnkEventHolder_801723E0 {
    /* The event playing, if any (name inferred). */
    Task *GetTask() {
        return m_task ? m_task : 0;
    }

    /* 0x0 */ Task *m_task;
    /* 0x4 */ Task *m_folder;
    /* 0x8 */ u8 m_firstTime;
};

extern "C" UnkEventHolder_801723E0 *fn_801723E0(void);

#endif
