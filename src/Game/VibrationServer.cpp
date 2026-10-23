/* Vibration::Server (class name from RTTI; declared in Game/VibrationServer.h): the base of the
 * pads' rumble servers. File name from the class. Definitions are in reverse address order
 * (deferred inlining emits functions last-defined first). */
#include "Game/VibrationServer.h"

namespace Vibration {

Server::Server(int pad) : m_pad(pad) {}

Server::~Server() {}

} // namespace Vibration
