/* CharInfo (class name from RTTI; Game/CharInfo.h): what characters and objects publish about
 * themselves. This unit has the constructor, the destructor (which emits the vtable here), the
 * empty PushCommand and slot 0x14, the name setter, and a shared CharInfo pointer that stays empty
 * (lbl_805EF980, passed as "no character" by Game/GunBattery.cpp and others). The other virtual
 * functions are linked from elsewhere (inline in the original, going by their addresses). Names
 * marked "(name inferred)" are not original. Definitions are in reverse address order (deferred
 * inlining emits functions last-defined first). */
#include <string.h>

#define CHARINFO_IMPLEMENTATION
#include "Game/CharInfo.h"
#include "Game/MathTypes.h"
#include "boost/shared_ptr.hpp"
#include <dolphin/types.h>
#include "size_assert.h"

extern "C" {
/* MSL's strncpy (see Game/MemoryCardCtrl.cpp). */
char *fn_803ACABC(char *dst, const char *src, unsigned long size);
/* Sets a vector, and a set of Euler angles, to zero (behaviour inferred). */
void fn_8040CE60(Vec3f *v);
void fn_8040E970(Vec3f *angles);
}

/* The game state (struct name unknown; see Game/BossCtrl.cpp): only the frame time is used here. */
struct UnkGameState_8057D6A0 {
    /* 0x000 */ u8 field_0x0[0x20];
    /* 0x020 */ float m_frameTime;
    /* 0x024 */ u8 field_0x24[0x150 - 0x24];
};
SIZE_ASSERT(UnkGameState_8057D6A0, 0x150);

extern UnkGameState_8057D6A0 lbl_8057D6A0;

/* An empty CharInfo pointer (name unknown). */
boost::shared_ptr<CharInfo> lbl_805EF980;

CharInfo::CharInfo(int type) : m_rotation(0.0f, 0.0f, 0.0f), field_0x30(0) {
    strcpy(m_name, "");
    field_0x4C = 0;
    m_kind = type;
    field_0x54 = -1;
    field_0x58 = -1;
    fn_8040CE60(&m_position);
    fn_8040CE60(&field_0x18);
    fn_8040CE60(&field_0x24);
    fn_8040E970(&m_rotation);
    field_0x5C = 0;
    field_0x34 = 0;
    field_0x60 = &lbl_8057D6A0.m_frameTime;
}

CharInfo::~CharInfo() {}

/* Sets the character's name (name inferred). Retail ends the name past its last character. */
extern "C" void fn_801DC984(CharInfo *info, const char *name) {
    fn_803ACABC(info->m_name, name, sizeof(info->m_name) - 1);
    info->m_name[sizeof(info->m_name)] = '\0';
}

void CharInfo::PushCommand(CharCommand *command) {}

void CharInfo::vf_0x14() {}
