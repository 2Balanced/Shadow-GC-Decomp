/* EnemyCharInfo (class name from RTTI; Game/EnemyCharInfo.h): the character info of an enemy, with
 * the functions that build one for an enemy type and hand the commands it receives to its command
 * unit's callback (the enemy's AI, Game/EnemyBaseAI.cpp). File name inferred from the class. Names
 * marked "(name inferred)" are not original. Definitions are in reverse address order (deferred
 * inlining emits functions last-defined first). */
/* boost::function's operator() puts a bad_function_call on the stack (see the MSL's stdexcept). */
#define MSL_RUNTIME_ERROR_STRING

#include "Game/CharCommand.h"
#include "Game/CharCommandUnit.h"
#include "Game/Enemy.h"
#include "Game/EnemyCharInfo.h"
#include "Game/MathConstants.h"
#include "Game/MathTypes.h"
#include "boost/detail/sp_counted_base_impl.hpp"
#include "boost/function.hpp"
#include "boost/shared_ptr.hpp"

extern "C" {
/* Hands command to the command unit (see Game/GadgetCharInfo.cpp). */
void fn_801D9C0C(boost::shared_ptr<CharCommandUnit> unit, CharCommand *command);
/* Whether the game is paused (behaviour inferred; see Game/StageRecord.cpp). */
u8 fn_802039D8(void *record);
/* Defined below. */
void fn_8019FED0(EnemyCharInfo *info, CharCommand &command);
}

/* Two angles in radians (names and use unknown: nothing reads them). */
static float s_angle240 = 240.0f * (lbl_805FAD38 / 180.0f);
static float s_angle120 = 120.0f * (lbl_805FAD38 / 180.0f);

/* The enemies' character infos are handed out as shared_ptr<UnkEnemyInfo_8017D014>, the name
 * Game/Enemy.h gives the boost instances; retail builds them from an EnemyCharInfo pointer (the
 * RTTI of the control block names EnemyCharInfo). This constructor is that instance, with the
 * pointer converted (not original: the two names are one class in the game). */
namespace boost {
template <>
template <>
inline shared_ptr<UnkEnemyInfo_8017D014>::shared_ptr(EnemyCharInfo *p)
    : px(reinterpret_cast<UnkEnemyInfo_8017D014 *>(p)), pn(p, checked_deleter<EnemyCharInfo>()) {
    detail::sp_enable_shared_from_this(pn, p, p);
}
} // namespace boost

EnemyCharInfo::EnemyCharInfo(int type) : CharInfo(type) {
    fn_800091EC(&m_prevPosition, &m_position);
    field_0x78[0] = 0.0f;
    field_0x78[1] = 0.0f;
    field_0x78[2] = -1.0f;
    field_0x78[3] = 0.0f;
    field_0x78[4] = 1.0f;
    field_0x78[5] = 0.0f;
    field_0x78[6] = 1.0f;
    field_0x78[7] = 1.0f;
    field_0x78[8] = 1.0f;
    fn_800091EC(&field_0x9C, &lbl_805E07A0);
    field_0xA8 = 10.0f;
    field_0xAC = 50.0f;
    field_0xB0 = 50.0f;
    field_0xB4 = 6.0f;
    field_0xB8 = 30.0f * (lbl_805FAD38 / 180.0f);
    field_0x5C = 3;
    field_0x30 = 2;
}

EnemyCharInfo::~EnemyCharInfo() {}

/* The character info of an enemy of type type (name inferred). */
extern "C" boost::shared_ptr<UnkEnemyInfo_8017D014> fn_801A0030(int type) {
    boost::shared_ptr<UnkEnemyInfo_8017D014> info =
        boost::shared_ptr<UnkEnemyInfo_8017D014>(new EnemyCharInfo(type));
    return info;
}

/* The frame time, zero while the game is paused (name inferred). */
extern "C" float fn_8019FFE0(EnemyCharInfo *info) {
    if (fn_802039D8(&lbl_8057D6A0[0xA8])) {
        return 0.0f;
    }
    return (*reinterpret_cast<float **>(info->field_0x60))[0];
}

/* Calls callback with command unless it is empty (name inferred). */
inline void CallCommandCallback(const CharCommandCallback &callback, CharCommand &command) {
    if (!callback.empty()) {
        callback(command);
    }
}

void EnemyCharInfo::PushCommand(CharCommand *command) {
    if (command != 0) {
        if (m_commandUnit.get() != 0) {
            switch (command->TypeId()) {
            case 0x1A:
                fn_8019FED0(this, *command);
                delete command;
                break;
            default:
                fn_801D9C0C(m_commandUnit, command);
                break;
            }
        } else {
            delete command;
        }
    }
}

/* Hands command straight to the command unit's callback (name inferred). */
extern "C" void fn_8019FED0(EnemyCharInfo *info, CharCommand &command) {
    CallCommandCallback(info->m_commandUnit->m_callback.m_function, command);
}

/* Gives the character info a command unit whose callback is callback (name inferred). */
extern "C" void fn_8019FD64(EnemyCharInfo *info, const CharCommandCallback &callback) {
    info->m_commandUnit = boost::shared_ptr<CharCommandUnit>(new CharCommandUnit);
    fn_800729B0(&info->m_commandUnit->m_callback, &callback);
}

bool EnemyCharInfo::vf_0x2C() {
    return true;
}
