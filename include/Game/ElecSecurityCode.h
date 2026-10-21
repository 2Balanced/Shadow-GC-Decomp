#ifndef GAME_ELECSECURITYCODE_H
#define GAME_ELECSECURITYCODE_H

#include "Game/Gadget.h"
#include "Game/GadgetCharInfo.h"
#include "Game/GadgetModelInfo.h"
#include "Game/MathTypes.h"
#include "boost/shared_ptr.hpp"
#include "boost/weak_ptr.hpp"
#include <dolphin/types.h>
#include "size_assert.h"

/* The ELEC_SECURITY gadget (ElecSecurityCodeGenerator, Game/ElecSecurityCodeGenerator.cpp) and the
 * code objects it launches (ElecSecurityCode, Game/ElecSecurityCode.cpp). Class names are from
 * RTTI; member names are inferred. */

class CharInfo;
/* Target (class name from RTTI): units that use it declare its members. */
class Target;
class ElecSecurityCodeGenerator;

namespace SonicteamUSA {
namespace System {
namespace CharaColli {
class CharaColliAttack;
struct AttackCallbackParam;
} // namespace CharaColli
} // namespace System
} // namespace SonicteamUSA

/* RenderWare's frame and matrix (rw/rwcore.h); only the parts used are declared. */
struct RwFrame;

struct RwV3d {
    float x;
    float y;
    float z;
};

struct RwMatrix {
    /* 0x00 */ RwV3d right;
    /* 0x0C */ u32 flags;
    /* 0x10 */ RwV3d up;
    /* 0x1C */ u32 pad1;
    /* 0x20 */ RwV3d at;
    /* 0x2C */ u32 pad2;
    /* 0x30 */ RwV3d pos;
    /* 0x3C */ u32 pad3;
};
SIZE_ASSERT(RwMatrix, 0x40);

/* ElecSecurityCode (class name from RTTI): a security code a generator launches at the nearest
 * character. It grows out of the generator, then homes in on the character and spins; when an attack
 * hits it, it splits in two smaller codes (types 0 and 1) or breaks into debris (type 2). Its model
 * (SECURITY.BIN) has three spinning frames per type. */
class ElecSecurityCode {
  public:
    /* The second argument is read only for the position at its start (the generator passes its
     * character info; a splitting code passes the place of the new code). */
    ElecSecurityCode(int type, GadgetCharInfo *charInfo, ElecSecurityCodeGenerator *generator,
                     const Vec2f &rotation, int kind);
    ~ElecSecurityCode();

    /* Advances the code by dt; false once it is gone (name inferred). */
    bool Tick(float dt);
    /* Homes in on the nearest character, turning at most a fixed rate (name inferred). */
    void Home(float dt);
    /* Flies a split code off to its side of the nearest character (name inferred). */
    void HomeSplit(float dt);
    /* Moves the character info by its velocity, stopping at what it hits (name inferred). */
    void Move(float dt);
    /* Places the model and the target at the character info (name inferred). */
    void RebuildTransform();
    /* Finds the type's three frames of the model and spins them (name inferred). */
    void UpdateFrames(float dt);
    /* Breaks the code into debris (name inferred). */
    void Break();
    /* The callbacks of the model's attack collision (names inferred). */
    void OnHit(SonicteamUSA::System::CharaColli::AttackCallbackParam &param);
    void OnAttackEnd(SonicteamUSA::System::CharaColli::AttackCallbackParam &param);
    void OnAttack(SonicteamUSA::System::CharaColli::AttackCallbackParam &param);

    /* Sets the scale for growth t (0 to 1): the code widens as it shortens (name inferred). */
    void SetScale(float t) {
        m_scale.x = m_scale.z = t;
        m_scale.y = 2.0f * (1.0f - t) + 1.0f;
    }

    /* 0x000 */ boost::shared_ptr<GadgetCharInfo> m_chara;
    /* 0x008 */ GadgetModelInfo *m_model;
    /* 0x00C */ boost::shared_ptr<Target> m_target;
    /* 0x014 */ s32 m_type;  /* 0 to 2: the model and the size */
    /* 0x018 */ s32 m_state; /* 0 growing, 1 homing, 2 hit */
    /* 0x01C */ float m_time;
    /* 0x020 */ Vec3f m_scale;
    /* 0x02C */ RwFrame *m_frames[3];
    /* 0x038 */ RwMatrix m_frameMatrices[3];
    /* 0x0F8 */ float m_frameAngles[3];
    /* 0x104 */ ElecSecurityCodeGenerator *m_generator;
    /* 0x108 */ Vec2f m_angles;
    /* 0x110 */ boost::weak_ptr<SonicteamUSA::System::CharaColli::CharaColliAttack> m_attack;
    /* 0x118 */ s32 m_kind; /* 0 launched by the generator, 1 and 2 the halves of a split code */
};
SIZE_ASSERT(ElecSecurityCode, 0x11C);

/* The links of a node of the list of codes; the node keeps its element after them (layouts from
 * their accesses). */
struct ElecSecurityCodeLink {
    /* 0x0 */ ElecSecurityCodeLink *prev;
    /* 0x4 */ ElecSecurityCodeLink *next;
};

struct ElecSecurityCodeNode : public ElecSecurityCodeLink {
    /* 0x8 */ ElecSecurityCode *code;
};

/* A position in the list implementation below. */
struct UnkPtrListPosition_8001B674 {
    UnkPtrListPosition_8001B674(ElecSecurityCodeLink *node_) : node(node_) {}
    UnkPtrListPosition_8001B674(const UnkPtrListPosition_8001B674 &other) : node(other.node) {}

    /* 0x0 */ ElecSecurityCodeLink *node;
};

struct UnkPtrList_8001B5B8;

extern "C" {
/* The list implementation that the game's lists of pointers share (inferred: its functions are
 * shared by several units, and the list of codes passes itself to them). fn_8001BE04 constructs
 * it, fn_8001B5B8 destroys it (the usual destructor flag in arg1), fn_8001B6B0 and fn_8001B674
 * return its first and its end position; fn_8001BDE8 returns its element count, fn_8001BDEC and
 * fn_8001BDF4 return its node allocator and its allocator (behaviour inferred). fn_8001D820
 * appends value (it inserts at fn_8001B674's end position). */
void fn_8001BE04(UnkPtrList_8001B5B8 *list);
void fn_8001B5B8(UnkPtrList_8001B5B8 *list, short flag);
UnkPtrListPosition_8001B674 fn_8001B6B0(UnkPtrList_8001B5B8 *list);
UnkPtrListPosition_8001B674 fn_8001B674(UnkPtrList_8001B5B8 *list);
u32 *fn_8001BDE8(UnkPtrList_8001B5B8 *list);
void *fn_8001BDEC(UnkPtrList_8001B5B8 *list);
void *fn_8001BDF4(UnkPtrList_8001B5B8 *list);
void fn_8001D820(UnkPtrList_8001B5B8 *list, ElecSecurityCode *const &value);
/* Returns the number of elements of the list (behaviour inferred). */
u32 fn_80058F00(UnkPtrList_8001B5B8 *list);
}

struct UnkPtrList_8001B5B8 {
    /* 0x0 */ u32 m_size;
    /* 0x4 */ ElecSecurityCodeLink m_end;
};
SIZE_ASSERT(UnkPtrList_8001B5B8, 0xC);

/* The list of the codes a generator launched: a std::list of ElecSecurityCode pointers by its
 * shape (inferred). In the original its members are template instances emitted in the units that
 * use them first; their names are not known. */
class ElecSecurityCodeList : public UnkPtrList_8001B5B8 {
  public:
    class iterator {
      public:
        iterator();
        iterator(const iterator &other);
        iterator(const UnkPtrListPosition_8001B674 &position);

        ElecSecurityCode *&operator*() const;
        iterator operator++(int);
        bool operator!=(const iterator &other) const;
        /* The position in the list implementation. */
        operator UnkPtrListPosition_8001B674() const;

        /* 0x0 */ ElecSecurityCodeLink *m_node;
    };

    ElecSecurityCodeList();
    ~ElecSecurityCodeList();

    iterator begin();
    iterator end();
    iterator erase(iterator where);
    /* Appends code (defined in Game/ElecSecurityCode.cpp, its first user). */
    void push_back(ElecSecurityCode *const &code);
    /* Unlinks and frees the node at where and returns the next position (name inferred). */
    UnkPtrListPosition_8001B674 EraseNode(UnkPtrListPosition_8001B674 where);
};

/* The two parameters the owner supplies (names from the parameter table). */
struct ElecSecurityCodeGeneratorParams {
    /* 0x0 */ int type;
    /* 0x4 */ float range;
};
SIZE_ASSERT(ElecSecurityCodeGeneratorParams, 0x8);

class ElecSecurityCodeGenerator : public Gadget {
  public:
    ElecSecurityCodeGenerator(Task *parent, UnkPlacement_800C9CD0 *owner);
    virtual ~ElecSecurityCodeGenerator();
    virtual void Behave(float dt);
    virtual void fn_800CBC1C();
    virtual void ReloadPlacement(UnkPlacement_800C9CD0 *owner);

    /* Creates the generator under task layer 11 (name inferred). */
    static void Instantiate(UnkPlacement_800C9CD0 *owner);

    /* True when the nearest character info is within the range (name inferred). */
    bool IsWithinReach();
    /* Places the model at the character info's translation (name inferred). */
    void RebuildTransform();
    /* Copies the owner's position, rotation, parameters and circuit id into the gadget (name
     * inferred). */
    void ReadPlacement(UnkPlacement_800C9CD0 *owner);

    static const char *s_taskName;

    /* 0x30 */ ElecSecurityCodeList m_codes;
    /* 0x3C */ boost::shared_ptr<CharInfo> field_0x3C; /* the character the last split code saw */
    /* 0x44 */ ElecSecurityCodeGeneratorParams m_params;
    /* 0x4C */ boost::shared_ptr<GadgetCharInfo> m_chara;
    /* 0x54 */ GadgetModelInfo *m_model;
    /* 0x58 */ u8 field_0x58; /* copied from the owner; the circuit id it sets (inferred) */
    /* 0x5C */ int m_modelIndex; /* the model shown (name inferred) */
    /* 0x60 */ int m_state;      /* 0 waiting, 1 codes launched (inferred) */
    /* 0x64 */ u32 field_0x64;
};
SIZE_ASSERT(ElecSecurityCodeGenerator, 0x68);

#endif
