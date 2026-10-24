#ifndef GAME_STAGESEQUENCE_H
#define GAME_STAGESEQUENCE_H

/* The stage sequence classes (class names from RTTI). The sequence manager
 * (StageSequence::Manager, Game/StageSequenceManager.cpp) runs one phase at a time; a phase plays
 * stages (StageManager, StageMovie, StageRouteDisplay, StageStaffroll) through a
 * shared_ptr<StageBase::Base>. Only the members used so far are declared; method names are
 * inferred. */

#include "Game/GameSystem.h"
#include "boost/scoped_ptr.hpp"
#include "boost/shared_ptr.hpp"
#include <dolphin/types.h>

namespace StageBase {

/* Base (vtable at 0x80558448; Game/StageBase.cpp): a stage a phase plays. */
class Base {
  public:
    /* 0x802D2560; kind is 1 for a StageManager, 2 for a StageMovie, 3 for a StageStaffroll and 0
     * for a StageRouteDisplay (meaning inferred). */
    Base(int kind);
    virtual ~Base();
    /* Runs the stage; 1 while it is still running (meaning inferred). */
    virtual int Tick();
    virtual void vf_0x10();

    /* 0x04 */ s32 m_kind;
};

} // namespace StageBase

class Render;
struct RednerDrawArgs;
namespace Chao {
namespace CSD {
class CProject;
} // namespace CSD
} // namespace Chao
class StageMovieScript;

/* StageMovie (vtable at 0x80558478; Game/StageMovie.cpp): a stage that plays a movie file, with
 * the subtitles of its event script (StageMovieScript). 0x58 bytes. */
class StageMovie : public StageBase::Base {
  public:
    StageMovie(const char *fileName);
    virtual ~StageMovie();
    virtual int Tick();
    /* Ends the movie (meaning inferred). */
    virtual void vf_0x10();

    /* Starts the movie; false when it cannot (name inferred). */
    bool Start();
    /* The Render's draw callback: shows the subtitles up to the frame played (name inferred). */
    void Draw(RednerDrawArgs &args);

    /* 0x08 */ boost::shared_ptr<Render> m_render;
    /* 0x10 */ s32 m_frame; /* the movie's frame */
    /* 0x14 */ s32 m_prevFrame;
    /* 0x18 */ u8 m_firstView; /* not seen before: the player cannot skip it (meaning inferred) */
    /* 0x1C */ s32 m_movieId; /* the number in the file name */
    /* 0x20 */ char m_fileName[0x20];
    /* 0x40 */ s32 m_delay; /* frames before the movie starts */
    /* 0x44 */ s32 m_failures; /* the failed starts */
    /* 0x48 */ u8 m_ended; /* set by vf_0x10 */
    /* 0x4C */ float m_wait;
    /* 0x50 */ s32 m_state; /* 0 starting, 1 playing, 2 failed, 3 ended, 4 waiting */
    /* 0x54 */ boost::scoped_ptr<StageMovieScript> m_script;
};

/* StageRouteDisplay (vtable at 0x8055F560): a stage that shows the story's route up to a stage
 * (meaning inferred; StageSequence::PlayStory plays it). 0x1C bytes. */
class StageRouteDisplay : public StageBase::Base {
  public:
    /* 0x80352448 */
    StageRouteDisplay(int stageId);
    virtual ~StageRouteDisplay();
    virtual int Tick();

    /* The Render's draw callback (Game/StageRouteDisplay.cpp). */
    void Draw(RednerDrawArgs &args);

    /* 0x08 */ boost::shared_ptr<Render> m_render;
    /* 0x10 */ Chao::CSD::CProject *m_project; /* the CSD project "CsdFiles/RouteDisplay" */
    /* 0x14 */ float m_time;
    /* 0x18 */ s32 m_state;
};

/* StageStaffroll (vtable at 0x8055F5AC): a stage that plays the staff roll of an ending (meaning
 * inferred; StageSequence::PlayStory plays it). 8 bytes. */
class StageStaffroll : public StageBase::Base {
  public:
    /* 0x80353B90 */
    StageStaffroll(int ending, int arg);
    virtual ~StageStaffroll();
    virtual int Tick();
};

namespace StageSequence {

/* StageSequenceBase (vtable at 0x80558160): a phase of the stage sequence. */
class StageSequenceBase {
  public:
    StageSequenceBase();
    virtual ~StageSequenceBase();
    /* Runs the phase; false once it finished (meaning inferred). */
    virtual bool Tick();
    /* Ends the phase early (name inferred). */
    virtual void Skip();
};

/* The phases (class names from RTTI): each keeps its state in a phaseImpl of its own unit
 * (Game/StageSequence<Name>.cpp); Manager creates them. */
namespace PlayOnce {
class phaseImpl;
class Phase : public StageSequenceBase {
  public:
    Phase();
    virtual ~Phase();
    virtual bool Tick();
    virtual void Skip();

    /* 0x4 */ boost::scoped_ptr<phaseImpl> m_impl;
};
} // namespace PlayOnce

namespace PlayStory {
class phaseImpl;
class Phase : public StageSequenceBase {
  public:
    Phase();
    virtual ~Phase();
    virtual bool Tick();
    virtual void Skip();

    /* 0x4 */ boost::scoped_ptr<phaseImpl> m_impl;
};
} // namespace PlayStory

namespace PlayEvent {
class phaseImpl;
class Phase : public StageSequenceBase {
  public:
    Phase();
    virtual ~Phase();
    virtual bool Tick();
    virtual void Skip();

    /* 0x4 */ boost::scoped_ptr<phaseImpl> m_impl;
};
} // namespace PlayEvent

namespace Play2POnce {
class phaseImpl;
class Phase : public StageSequenceBase {
  public:
    Phase();
    virtual ~Phase();
    virtual bool Tick();
    virtual void Skip();

    /* 0x4 */ boost::scoped_ptr<phaseImpl> m_impl;
};
} // namespace Play2POnce

namespace Play2PThree {
class phaseImpl;
class Phase : public StageSequenceBase {
  public:
    Phase();
    virtual ~Phase();
    virtual bool Tick();
    virtual void Skip();

    /* 0x4 */ boost::scoped_ptr<phaseImpl> m_impl;
};
} // namespace Play2PThree

namespace MovieTest {
class phaseImpl;
class Phase : public StageSequenceBase {
  public:
    Phase();
    virtual ~Phase();
    virtual bool Tick();
    virtual void Skip();

    /* 0x4 */ boost::scoped_ptr<phaseImpl> m_impl;
};
} // namespace MovieTest

namespace PlayHard {
class phaseImpl;
class Phase : public StageSequenceBase {
  public:
    Phase();
    virtual ~Phase();
    virtual bool Tick();
    virtual void Skip();

    /* 0x4 */ boost::scoped_ptr<phaseImpl> m_impl;
};
} // namespace PlayHard

namespace PlayFlashback {
class phaseImpl;
class Phase : public StageSequenceBase {
  public:
    Phase();
    virtual ~Phase();
    virtual bool Tick();
    virtual void Skip();

    /* 0x4 */ boost::scoped_ptr<phaseImpl> m_impl;
};
} // namespace PlayFlashback

/* Manager (vtable at 0x80558184; Game/StageSequenceManager.cpp): the GameSystem that runs the
 * stage sequence. Initialize starts the phase SetNext chose (0 PlayOnce, 1 PlayStory, 2
 * PlayEvent, 3 Play2POnce, 4 Play2PThree, 5 MovieTest, 6 PlayHard, 7 PlayFlashback; -1 none). */
class Manager : public GameSystem {
  public:
    Manager();
    virtual ~Manager();
    virtual bool Initialize();
    virtual bool IsFinished();
    virtual bool Finalize();
    virtual void Abort();

    /* The sequence playing (0x802CF1D4). */
    static s32 GetCurrent();
    /* Chooses the sequence the next Initialize starts (0x802CF1DC). */
    static void SetNext(s32 sequence);

    /* 0x4 */ s32 m_sequence;
    /* 0x8 */ boost::shared_ptr<StageSequenceBase> m_phase;
};

} // namespace StageSequence

#endif
