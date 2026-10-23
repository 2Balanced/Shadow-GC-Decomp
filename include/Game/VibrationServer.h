#ifndef GAME_VIBRATIONSERVER_H
#define GAME_VIBRATIONSERVER_H

#include <dolphin/types.h>
#include "size_assert.h"

namespace Vibration {

/* Vibration::Server (class name from RTTI): drives the rumble motor of one pad. The vibration
 * manager keeps one per pad (Game/VibrationManagerTask.cpp); Vibration::ServerGC is the GameCube
 * one. Member names are inferred. */
class Server {
  public:
    Server(int pad);
    virtual ~Server();
    /* Switches the motor for this frame (dt unused by ServerGC). */
    virtual void Update(float dt) = 0;
    /* Asks for a level of vibration this frame (the sum of two strengths). */
    virtual void SetLevel(float level, float extra) = 0;
    /* Stops the motor. */
    virtual void Stop() = 0;

    /* 0x4 */ s32 m_pad;
};
SIZE_ASSERT(Server, 0x8);

/* Vibration::ServerGC (class name from RTTI): the server of a GameCube pad. A level above 0.8 runs
 * the motor, one below 0.2 stops it, and one in between pulses it (names inferred). */
class ServerGC : public Server {
  public:
    ServerGC(int pad);
    virtual ~ServerGC();
    virtual void Update(float dt);
    virtual void SetLevel(float level, float extra);
    virtual void Stop();

    /* 0x08 */ u32 m_flags; /* 0 level set, 1 stopping, 2 motor stopped, 3 on, 4 pulsing */
    /* 0x0C */ float m_level;
    /* 0x10 */ float m_pulse; /* level accumulated while pulsing */
};
SIZE_ASSERT(ServerGC, 0x14);

} // namespace Vibration

#endif
