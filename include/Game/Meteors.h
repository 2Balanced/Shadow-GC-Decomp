#ifndef GAME_METEORS_H
#define GAME_METEORS_H

#include "Game/Task.h"
#include "Game/MathTypes.h"
#include "boost/shared_ptr.hpp"
#include "size_assert.h"

/* The small meteors of the METEOR_S gadget (Game/MeteorsHolder.cpp), which the large meteor of
 * METEOR_L (Game/Meteor.cpp) also spawns when it breaks. */

/* The parameters of the METEOR_S owner record (names from its parameter table). */
struct MeteorsParams {
    /* 0x00 */ float range;      /* "RANGE m" */
    /* 0x04 */ float speed;      /* "MOVE_SPD m/s" */
    /* 0x08 */ float directionX; /* "DIRECTION_X deg" */
    /* 0x0C */ float directionY; /* "DIRECTION_Y deg" */
    /* 0x10 */ float spinX;      /* "ROT_SPD_X deg/s" */
    /* 0x14 */ float spinZ;      /* "ROT_SPD_Z deg/s" */
    /* 0x18 */ float spinY;      /* "ROT_SPD_Y deg/s" */
    /* 0x1C */ float lifeTime;   /* "LIFETIME s" */
};
SIZE_ASSERT(MeteorsParams, 0x20);

/* How a small meteor flies (struct name unknown; member names inferred): it waits until a character
 * comes within range, then flies along direction at speed, spinning, for lifeTime seconds. */
struct UnkMeteorMotion_8021FCE4 {
    /* From the owner's parameters (metres and degrees). */
    UnkMeteorMotion_8021FCE4(const MeteorsParams &params);
    UnkMeteorMotion_8021FCE4(const Vec3f &direction, Vec3f spin, float range, float speed,
                             float lifeTime);
    UnkMeteorMotion_8021FCE4(const UnkMeteorMotion_8021FCE4 &other);

    /* 0x00 */ float range;
    /* 0x04 */ float speed;
    /* 0x08 */ Vec3f direction;
    /* 0x14 */ Vec3f spin; /* radians per second */
    /* 0x20 */ float lifeTime;
};
SIZE_ASSERT(UnkMeteorMotion_8021FCE4, 0x24);

/* Meteors (class name from the RTTI of its shared_ptr control block): one small meteor. */
class Meteors;

/* MeteorsTask (class name from RTTI): the task that runs a small meteor spawned by something else
 * than a METEOR_S gadget (the large meteor). */
class MeteorsTask : public Task {
  public:
    MeteorsTask(Task *parent, const Vec3f *position, const Vec3f *rotation,
                const UnkMeteorMotion_8021FCE4 *motion, int state);
    virtual ~MeteorsTask();
    virtual void Tick(float dt);

    static const char *s_taskName;

    /* 0x28 */ boost::shared_ptr<Meteors> m_meteors;
};
SIZE_ASSERT(MeteorsTask, 0x30);

#endif
