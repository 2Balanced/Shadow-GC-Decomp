/* SonicteamUSA::System::CharaColli::CharaColliBaseCharInfo (class name from RTTI): the base of the
 * colliders that follow a character info (Game/CharaColliBaseCharInfo.h). This unit holds its
 * out-of-line constructor and destructor, which emit its vtable and RTTI, and the two empty pointers
 * the colliders hand out when they have no character or attack collision (lbl_805EE698 and
 * lbl_805EE6A0, read by other units under those names). The file name is not original.
 * Definitions are in reverse address order (deferred inlining emits functions last-defined
 * first). */
#define CHARACOLLI_BASE_MEMBERS
#define CHARACOLLI_BASE_PURE
#include "Game/CharaColliBaseCharInfo.h"
#include "boost/shared_ptr.hpp"
#include "boost/weak_ptr.hpp"
#include <dolphin/types.h>

using SonicteamUSA::System::CharaColli::CharaColliAttack;
using SonicteamUSA::System::CharaColli::CharaColliBase;
using SonicteamUSA::System::CharaColli::CharaColliBaseCharInfo;

/* The colliders' manager (class unknown). */
struct UnkColliManager_80021B08;

extern "C" {
/* The colliders' manager, and removing a collider from it (behaviour inferred). */
UnkColliManager_80021B08 *fn_80021B08(void);
void fn_800C46DC(UnkColliManager_80021B08 *manager, CharaColliBase *colli);
}

/* The empty character and attack collision (names unknown). */
boost::shared_ptr<CharInfo> lbl_805EE698;
boost::shared_ptr<CharaColliAttack> lbl_805EE6A0;

namespace SonicteamUSA {
namespace System {
namespace CharaColli {

CharaColliBaseCharInfo::CharaColliBaseCharInfo(u16 type, u16 arg, boost::weak_ptr<CharInfo> info)
    : CharaColliBase(type, arg), m_info(info) {}

CharaColliBaseCharInfo::~CharaColliBaseCharInfo() {
    fn_800C46DC(fn_80021B08(), this);
}

} // namespace CharaColli
} // namespace System
} // namespace SonicteamUSA
