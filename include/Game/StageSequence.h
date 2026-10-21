#ifndef GAME_STAGESEQUENCE_H
#define GAME_STAGESEQUENCE_H

/* The stage sequence classes that Game/StageSequenceMovieTest.cpp uses (class names from RTTI). The
 * sequence manager (StageSequence::Manager, around 0x802CE158) runs one phase at a time; a phase plays
 * stages, such as a StageMovie, through a shared_ptr<StageBase::Base>. Only the members used so
 * far are declared; method names are inferred. */

#include <dolphin/types.h>

namespace StageBase {

/* Base (vtable at 0x80558448): a stage a phase plays. */
class Base {
  public:
    virtual ~Base();
    /* Runs the stage; 1 while it is still running (meaning inferred). */
    virtual int Tick();
    virtual void vf_0x10();
};

} // namespace StageBase

/* StageMovie (vtable at 0x80558478): a stage that plays a movie file. 0x58 bytes. */
class StageMovie : public StageBase::Base {
  public:
    StageMovie(const char *fileName);
    virtual ~StageMovie();
    virtual int Tick();
    virtual void vf_0x10();

    /* 0x04 */ u8 field_0x4[0x58 - 0x4];
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

} // namespace StageSequence

#endif
