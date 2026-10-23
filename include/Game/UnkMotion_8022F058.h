#ifndef GAME_UNKMOTION_8022F058_H
#define GAME_UNKMOTION_8022F058_H

#include "Game/CharInfo.h"
#include "Game/MathTypes.h"
#include "boost/scoped_ptr.hpp"
#include <dolphin/types.h>
#include "size_assert.h"

/* The tables a model's motion is built from (Game/AirBoltModel.cpp has one; struct names unknown,
 * member names inferred). */
struct BirdMotionEntry {
    /* 0x0 */ s32 m_file;
    /* 0x4 */ s32 m_motion;
    /* 0x8 */ s32 m_slot;
};

struct BirdMotionSet {
    /* 0x00 */ s32 field_0x0;
    /* 0x04 */ s32 m_file;
    /* 0x08 */ s32 field_0x8;
    /* 0x0C */ const BirdMotionEntry *m_entries;
    /* 0x10 */ s32 m_entryCount;
};

/* Three frame numbers of each motion (meaning unknown). */
struct BirdMotionFrames {
    /* 0x0 */ s32 m_frames[3];
};

struct BirdMotionTable {
    /* 0x00 */ const BirdMotionSet *m_sets;
    /* 0x04 */ s32 m_setCount;
    /* 0x08 */ const s32 *m_ids;
    /* 0x0C */ s32 m_idCount;
    /* 0x10 */ const BirdMotionFrames *m_frames;
    /* 0x14 */ s32 m_file;
    /* 0x18 */ s32 field_0x18;
};

/* A model's motion player (class name unknown; constructor 0x8022F058, destructor 0x8022EF9C, code
 * around 0x8022EC24, not yet matched): a pointer to its state, which the game allocates (0x6C
 * bytes, constructor 0x8023037C) and deletes through a scoped_ptr. Method names are inferred. */
class UnkMotion_8022F058 {
  public:
    class Impl;

    UnkMotion_8022F058(CharInfo *owner, int kind, const BirdMotionTable *table);
    ~UnkMotion_8022F058();

    /* 0x0 */ boost::scoped_ptr<Impl> m_impl;
};
SIZE_ASSERT(UnkMotion_8022F058, 0x4);

extern "C" {
/* The motion's methods (names inferred). */
void fn_8022EC24(UnkMotion_8022F058 *motion, void *arg);
bool fn_8022EC48(UnkMotion_8022F058 *motion); /* the motion has ended */
void fn_8022EC6C(UnkMotion_8022F058 *motion); /* pauses */
void fn_8022EC90(UnkMotion_8022F058 *motion); /* resumes */
float fn_8022EEC4(UnkMotion_8022F058 *motion);
void *fn_8022EEE8(UnkMotion_8022F058 *motion, int index);
void fn_8022EF30(UnkMotion_8022F058 *motion, int motionId, float blend); /* changes the motion */
void fn_8022EF54(UnkMotion_8022F058 *motion, float dt);
void fn_8022EF78(UnkMotion_8022F058 *motion, const Vec3f *position, const Vec3f *rotation);
}

#endif
