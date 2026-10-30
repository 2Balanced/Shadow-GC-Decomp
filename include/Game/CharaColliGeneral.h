#ifndef GAME_CHARACOLLIGENERAL_H
#define GAME_CHARACOLLIGENERAL_H

#ifndef CHARACOLLI_BASE_MEMBERS
#define CHARACOLLI_BASE_MEMBERS
#endif

#include "Game/CharaColli.h"
#include "Game/CharaColliBaseCharInfo.h"
#include "Game/MathTypes.h"
#include "boost/weak_ptr.hpp"
#include "dolphin/types.h"
#include "size_assert.h"

class CharInfo;
class UnkColliObject_80028CA4;
struct _stMtnPlayWork;

/* The links of a node of a list of pointers; the node keeps its value after them (see
 * Game/BulletManagerTask.cpp and Game/ElecSecurityCode.h, which declare the same list). */
struct UnkPtrListLink_8001B5B8 {
    /* 0x0 */ UnkPtrListLink_8001B5B8 *prev;
    /* 0x4 */ UnkPtrListLink_8001B5B8 *next;
};

/* A position in the list implementation below. */
struct UnkPtrListPosition_8001B674 {
    UnkPtrListPosition_8001B674(UnkPtrListLink_8001B5B8 *node_) : node(node_) {}
    UnkPtrListPosition_8001B674(const UnkPtrListPosition_8001B674 &other) : node(other.node) {}

    /* 0x0 */ UnkPtrListLink_8001B5B8 *node;
};

/* The list implementation the game's lists of pointers share. */
struct UnkPtrList_8001B5B8 {
    /* 0x0 */ u32 m_size;
    /* 0x4 */ UnkPtrListLink_8001B5B8 m_end;
};
SIZE_ASSERT(UnkPtrList_8001B5B8, 0xC);

extern "C" {
/* The list implementation's constructor and destructor, its first and its end position, and
 * appending a value (see Game/BulletManagerTask.cpp). */
void fn_8001BE04(UnkPtrList_8001B5B8 *list);
void fn_8001B5B8(UnkPtrList_8001B5B8 *list, short flag);
UnkPtrListPosition_8001B674 fn_8001B6B0(UnkPtrList_8001B5B8 *list);
UnkPtrListPosition_8001B674 fn_8001B674(UnkPtrList_8001B5B8 *list);
void fn_8001D820(UnkPtrList_8001B5B8 *list, void *const &value);
}

/* Keeps the inline function that calls it out of line, as the list's members are. */
inline void ColliShapeListKeep() {}

/* A copy of a vector made by the out-of-line copy (fn_800091EC); the colliders' position. */
struct UnkCopyVector3_800091EC {
    UnkCopyVector3_800091EC(const Vec3f &v) {
        fn_800091EC(reinterpret_cast<Vec3f *>(this), &v);
    }

    /* The vector itself (an inline the colliders use where they pass it on). */
    Vec3f *operator&() {
        return reinterpret_cast<Vec3f *>(this);
    }

    float x;
    float y;
    float z;
};

/* The descriptions of a collider's shapes: an array ended by an entry whose word 0x4 is 0
 * (0x10 bytes each; layout otherwise unknown). */
struct UnkColliShapeDesc_80037668 {
    /* 0x0 */ s32 m_kind;
    /* 0x4 */ u32 field_0x4;
    /* 0x8 */ u32 field_0x8;
    /* 0xC */ u32 field_0xC;
};

namespace SonicteamUSA {
namespace System {
namespace CharaColli {

/* The shapes of a CharaColliGeneral: the game's list of pointers holding shapes. Its members are
 * inline and emitted after their users, as the game's list template's are (names inferred). */
class ColliShapeList : public UnkPtrList_8001B5B8 {
  public:
    struct Node : public UnkPtrListLink_8001B5B8 {
        /* 0x8 */ UnkColliObject_80028CA4 *value;
    };

    class iterator {
      public:
        iterator() {
            ColliShapeListKeep();
        }
        iterator(const iterator &other) : m_node(other.m_node) {
            ColliShapeListKeep();
        }
        iterator(const UnkPtrListPosition_8001B674 &position) : m_node(position.node) {
            ColliShapeListKeep();
        }

        UnkColliObject_80028CA4 *&operator*() const {
            ColliShapeListKeep();
            return static_cast<Node *>(m_node)->value;
        }
        iterator operator++(int) {
            iterator old(*this);
            m_node = m_node->next;
            return old;
        }
        bool operator!=(const iterator &other) const {
            ColliShapeListKeep();
            return m_node != other.m_node;
        }

        /* 0x0 */ UnkPtrListLink_8001B5B8 *m_node;
    };

    class const_iterator {
      public:
        const_iterator() {
            ColliShapeListKeep();
        }
        const_iterator(const const_iterator &other) : m_node(other.m_node) {
            ColliShapeListKeep();
        }
        const_iterator(UnkPtrListPosition_8001B674 position) : m_node(position.node) {
            ColliShapeListKeep();
        }

        UnkColliObject_80028CA4 *const &operator*() const {
            ColliShapeListKeep();
            return static_cast<const Node *>(m_node)->value;
        }
        const_iterator operator++(int) {
            const_iterator old(*this);
            m_node = m_node->next;
            return old;
        }
        bool operator!=(const const_iterator &other) const {
            ColliShapeListKeep();
            return m_node != other.m_node;
        }

        /* 0x0 */ const UnkPtrListLink_8001B5B8 *m_node;
    };

    ColliShapeList() {
        ColliShapeListKeep();
        fn_8001BE04(this);
    }
    ~ColliShapeList() {
        ColliShapeListKeep();
        fn_8001B5B8(this, 0);
    }

    iterator begin() {
        return iterator(fn_8001B6B0(this));
    }
    iterator end() {
        return iterator(fn_8001B674(this));
    }
    const_iterator begin() const {
        return const_iterator(UnkPtrListPosition_8001B674(BeginPosition()));
    }
    const_iterator end() const {
        return const_iterator(UnkPtrListPosition_8001B674(EndPosition()));
    }
    void push_back(UnkColliObject_80028CA4 *const &value) {
        ColliShapeListKeep();
        fn_8001D820(this, reinterpret_cast<void *const &>(value));
    }

  private:
    UnkPtrListLink_8001B5B8 *EndNode() const {
        ColliShapeListKeep();
        return const_cast<UnkPtrListLink_8001B5B8 *>(&m_end);
    }
    UnkPtrListPosition_8001B674 BeginPosition() const {
        return UnkPtrListPosition_8001B674(EndNode()->next);
    }
    UnkPtrListPosition_8001B674 EndPosition() const {
        return UnkPtrListPosition_8001B674(EndNode());
    }
};
SIZE_ASSERT(ColliShapeList, 0xC);

/* CharaColliGeneral (class name from RTTI): a collider of several shapes (descriptions in an
 * array, see UnkColliShapeDesc_80037668) sharing one position and rotation. Shape ids select a
 * shape in the id-taking slots; -1 means all of them. Names inferred. */
class CharaColliGeneral : public CharaColliBaseCharInfo {
  public:
    CharaColliGeneral(const UnkColliShapeDesc_80037668 *shapes, const Vec3f *position, const Vec3f *rotation,
                      s32 arg, u16 type, u16 typeArg, boost::weak_ptr<CharInfo> info);

    virtual u16 GetShapeCount();
    virtual void TurnOn(u32 id);
    virtual void TurnOff(u32 id);
    virtual Vec3f *vf_0x14();
    virtual Vec3f *vf_0x18();
    virtual s32 vf_0x24();
    virtual UnkColliObject_80028CA4 *vf_0x28(int index) const;
    virtual UnkColliObject_80028CA4 *vf_0x2C(u32 id) const;
    virtual u32 GetAttributes(int arg) const;
    virtual u32 vf_0x34(u32 id) const;
    virtual void SetCharInfo(CharInfo *info);
    virtual void SetRotation(const Vec3f *rotation);
    virtual void SetScale(const Vec3f *scale);
    virtual void SetDebugColor(const UnkColor_8040D2D8 *color);
    virtual void vf_0x54(const boost::weak_ptr<CharaColliBase> &owner);
    virtual void SetAttributes(u32 attributes, int arg1);
    virtual void vf_0x5C(u32 attributes, u32 id);
    virtual UnkColliObject_80028CA4 *vf_0x60(const UnkDebugLine_80020A20 *line, UnkColliContact_8001FF00 *result,
                                             Filter filter, u32 arg);
    virtual UnkColliObject_80028CA4 *vf_0x64(const UnkDebugLine_80020A20 *segment, UnkColliContact_8001FF00 *result,
                                             Filter filter, u32 arg);
    virtual UnkColliObject_80028CA4 *vf_0x68(const UnkSphere_8004E6CC *sphere, UnkColliContact_8001FF00 *result,
                                             Filter filter, u32 arg);
    virtual UnkColliObject_80028CA4 *vf_0x6C(const UnkSphere_8004E6CC *sphere, const Vec3f *move,
                                             UnkColliContact_8001FF00 *result, Filter filter, u32 arg);
    virtual void Reset();
    virtual void vf_0x74();
    virtual ~CharaColliGeneral();
    virtual void vf_0x7C();
    virtual bool vf_0x80(CharaColliBase *other, UnkColliObject_80028CA4 **mine, UnkColliObject_80028CA4 **theirs,
                         UnkColliHit_8002686C **mineHit, UnkColliHit_8002686C **theirHit);
    virtual void vf_0x84();
    virtual void vf_0x88();
    virtual void vf_0x8C();

    /* 0x40 */ UnkCopyVector3_800091EC m_position;
    /* 0x4C */ Vec3f m_rotation;
    /* 0x58 */ s32 field_0x58;
    /* 0x5C */ u16 m_count;
    /* 0x60 */ ColliShapeList m_shapes;
};
SIZE_ASSERT(CharaColliGeneral, 0x6C);

/* CharaColliGeneralCharInfo (class name from RTTI): a CharaColliGeneral that takes its position,
 * rotation and word 0x30 from its character (slot 0x88) instead of being moved; the setters do
 * nothing. Names inferred. */
class CharaColliGeneralCharInfo : public CharaColliGeneral {
  public:
    CharaColliGeneralCharInfo(boost::weak_ptr<CharInfo> info, const UnkColliShapeDesc_80037668 *shapes, u16 type,
                              u16 typeArg);

    virtual void SetCharInfo(CharInfo *info) {}
    virtual void SetRotation(const Vec3f *rotation) {}
    virtual ~CharaColliGeneralCharInfo() {}
    virtual void vf_0x88();
};
SIZE_ASSERT(CharaColliGeneralCharInfo, 0x6C);

/* CharaColliGeneralBone (class name from RTTI): a CharaColliGeneralCharInfo whose shapes follow
 * the bones of a motion (each shape's id names its bone; slot 0x90). Names inferred. */
class CharaColliGeneralBone : public CharaColliGeneralCharInfo {
  public:
    CharaColliGeneralBone(boost::weak_ptr<CharInfo> info, const UnkColliShapeDesc_80037668 *shapes,
                          _stMtnPlayWork *motion, u16 type, u16 typeArg);

    virtual ~CharaColliGeneralBone();
    virtual void vf_0x7C();
    virtual void vf_0x84();
    virtual void vf_0x88();
    /* Moves the shapes to their bones. */
    virtual void vf_0x90();

    /* 0x6C */ Mtx34f **m_bones;
};
SIZE_ASSERT(CharaColliGeneralBone, 0x70);

} // namespace CharaColli
} // namespace System
} // namespace SonicteamUSA

#endif
