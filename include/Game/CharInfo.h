#ifndef SHADOW_GAME_CHARINFO_H
#define SHADOW_GAME_CHARINFO_H

#include "Game/MathTypes.h"
#include "boost/weak_ptr.hpp"
#include <dolphin/types.h>
#include "size_assert.h"

class CharCommand;

/* CharInfo, named by the game's RTTI (vtable 0x8053D540). Characters and objects publish
 * themselves through it: where they are, how they are turned, and a mailbox for commands from
 * others. The transform data comes first and the vtable pointer follows it at 0x38, because the
 * first virtual function is declared after those members.
 *
 * Virtual slots that no matched unit calls keep a vf_<slot> name so the vtable stays in shape;
 * what the named ones do is taken from their callers. */
class CharInfo {
  public:
    /* 0x00 */ Vec3f m_position;
    /* 0x0C */ Vec3f m_rotation; /* Euler angles in radians */
    /* 0x18 */ Vec3f field_0x18;
    /* 0x24 */ Vec3f field_0x24; /* BOMB accelerates the object with it */
    /* 0x30 */ s32 field_0x30;
    /* 0x34 */ u32 field_0x34;

    /* 0x801DCA08; type ends up in the word at 0x50. */
    CharInfo(int type);

    virtual ~CharInfo(); /* the vtable pointer is at 0x38 */
    virtual void vf_0x0C();
    /* Slot 0x10: hands a command to the character. */
    virtual void PushCommand(CharCommand *command);
    virtual void vf_0x14();
    /* Slot 0x18: asked by BombServer of each character in contact. */
    virtual bool vf_0x18();
    /* Slot 0x1C: asked before a character may collect a ring. */
    virtual bool vf_0x1C();
    virtual bool vf_0x20();
    /* Slot 0x24: asked before an item box sends HomingJumpCommand. */
    virtual bool vf_0x24();
    virtual bool vf_0x28();
    /* Slot 0x2C: false in CharInfo, true in EnemyCharInfo and ResearcherCharInfo. */
    virtual bool vf_0x2C();

#ifdef CHARINFO_IMPLEMENTATION
    /* The members from 0x3C as Game/CharInfo.cpp sees them (names inferred; other units keep the
     * byte arrays below). */
    /* 0x3C */ char m_name[0x10];
    /* 0x4C */ s32 field_0x4C;
    /* 0x50 */ s32 m_kind;
    /* 0x54 */ s32 field_0x54;
    /* 0x58 */ s32 field_0x58;
    /* 0x5C */ s32 field_0x5C;
    /* 0x60 */ void *field_0x60; /* points to the frame time (see Game/DiablonController.cpp) */
#else
    /* The word at 0x50: the character's kind (meaning inferred; see Game/ElecCircuitEntrance.cpp).
     * An inline function in the original: Game/BirdBase.cpp calls the shared_ptr's operator-> out
     * of line before it. */
    s32 GetKind() const {
        return *reinterpret_cast<const s32 *>(&field_0x3C[0x50 - 0x3C]);
    }

    /* 0x3C */ u8 field_0x3C[0x54 - 0x3C];
    /* 0x54 */ s32 field_0x54; /* player number, going by BlockChange */
    /* 0x58 */ u8 field_0x58[0x5C - 0x58];
    /* 0x5C */ s32 field_0x5C; /* 1 for players, going by BlockChange */
    /* 0x60 */ u8 field_0x60[0x64 - 0x60];
#endif
};
SIZE_ASSERT(CharInfo, 0x64);

/* The list of all characters (a singleton from fn_80074194; class name unknown). */
class UnkCharaList_80074194;

extern "C" {
UnkCharaList_80074194 *fn_80074194(void);
/* The character closest to `from`. */
boost::weak_ptr<CharInfo> fn_800A8A7C(UnkCharaList_80074194 *list, const Vec3f *from);
}

#endif
