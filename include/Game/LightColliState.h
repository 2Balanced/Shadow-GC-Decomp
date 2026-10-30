#ifndef GAME_LIGHTCOLLISTATE_H
#define GAME_LIGHTCOLLISTATE_H

#include <dolphin/types.h>
#include "size_assert.h"
#include <boost/shared_ptr.hpp>
#include <boost/weak_ptr.hpp>
#include <map>

/* The shared state of the LIGHT_COLI gadgets (Game/LightColli.cpp): the singleton fn_801E6500
 * returns (Game/UnkSystemManager_801E2128.cpp makes it on first use). Class names other than
 * LightColli and LightColliMasterTask are unknown; member names are inferred. */

class CharInfo;
class LightColli;
class LightColliMasterTask;

namespace Effect {
namespace System {
class EffectElement;
} // namespace System
} // namespace Effect

/* The list implementation the game's lists of pointers share (see Game/ElecSecurityCode.h). */
struct UnkPtrListLink_8001B5B8 {
    /* 0x0 */ UnkPtrListLink_8001B5B8 *prev;
    /* 0x4 */ UnkPtrListLink_8001B5B8 *next;
};

struct UnkPtrList_8001B5B8 {
    /* 0x0 */ u32 m_size;
    /* 0x4 */ UnkPtrListLink_8001B5B8 m_end;
};
SIZE_ASSERT(UnkPtrList_8001B5B8, 0xC);

/* A position in that list. */
struct UnkPtrListPosition_801E6500 {
    UnkPtrListPosition_801E6500(const UnkPtrListPosition_801E6500 &other) : node(other.node) {}

    /* 0x0 */ UnkPtrListLink_8001B5B8 *node;
};

extern "C" {
/* The list implementation's constructor and destructor (the usual destructor flag in flag), its
 * first and end positions, and appending, removing and clearing (behaviour inferred). */
void fn_8001BE04(UnkPtrList_8001B5B8 *list);
void fn_8001B5B8(UnkPtrList_8001B5B8 *list, short flag);
UnkPtrListPosition_801E6500 fn_8001B6B0(UnkPtrList_8001B5B8 *list);
UnkPtrListPosition_801E6500 fn_8001B674(UnkPtrList_8001B5B8 *list);
void fn_8001D820(UnkPtrList_8001B5B8 *list, void *const &value);
void fn_8001B42C(UnkPtrList_8001B5B8 *list, void *const &value);
void fn_8001E404(UnkPtrList_8001B5B8 *list);
}

/* Keeps an inline function out of line (deferred inlining is one level deep). */
inline void UnkLightColliListKeep() {}

/* The light colliders' list (a std::list of pointers by its shape; name inferred). Its members are
 * the game's weak copies, emitted out of line after their first user. */
class UnkLightColliList_801E6500 : public UnkPtrList_8001B5B8 {
  public:
    struct Node : public UnkPtrListLink_8001B5B8 {
        /* 0x8 */ LightColli *value;
    };

    class iterator {
      public:
        iterator() {
            UnkLightColliListKeep();
        }
        iterator(const UnkPtrListPosition_801E6500 &position) : m_node(position.node) {
            UnkLightColliListKeep();
        }
        iterator(const iterator &other) : m_node(other.m_node) {
            UnkLightColliListKeep();
        }

        LightColli *&operator*() const {
            UnkLightColliListKeep();
            return static_cast<Node *>(m_node)->value;
        }
        iterator operator++(int) {
            iterator old(*this);
            m_node = m_node->next;
            return old;
        }
        bool operator!=(const iterator &other) const {
            UnkLightColliListKeep();
            return m_node != other.m_node;
        }

        /* 0x0 */ UnkPtrListLink_8001B5B8 *m_node;
    };

    UnkLightColliList_801E6500() {
        UnkLightColliListKeep();
        fn_8001BE04(this);
    }
    ~UnkLightColliList_801E6500() {
        UnkLightColliListKeep();
        fn_8001B5B8(this, 0);
    }

    iterator begin() {
        return iterator(fn_8001B6B0(this));
    }
    iterator end() {
        return iterator(fn_8001B674(this));
    }
    void push_back(LightColli *const &value) {
        UnkLightColliListKeep();
        fn_8001D820(this, reinterpret_cast<void *const &>(value));
    }
    void remove(LightColli *const &value) {
        UnkLightColliListKeep();
        fn_8001B42C(this, reinterpret_cast<void *const &>(value));
    }
    void clear() {
        fn_8001E404(this);
    }
};

/* The light colliders' shared state (class name unknown; constructed by fn_801EEA70): the master
 * task, the colliders, and per character the light effect, the light number last sent and the
 * light number the character returns to. */
class UnkLightColliState_801E6500 {
  public:
    typedef std::map<boost::weak_ptr<CharInfo>, int> LightMap;

    UnkLightColliState_801E6500();
    ~UnkLightColliState_801E6500();

    /* The light number last sent to chara, or -1 (name inferred). */
    int GetLight(boost::weak_ptr<CharInfo> chara);
    /* Records the light number sent to chara (name inferred). */
    void SetLight(boost::weak_ptr<CharInfo> chara, int light);
    /* The light number chara returns to, or 0 (name inferred). */
    int GetDefaultLight(boost::weak_ptr<CharInfo> chara);
    /* Sets the light number chara returns to; -1 only registers chara, with 0 (name inferred). */
    void SetDefaultLight(boost::weak_ptr<CharInfo> chara, int light);
    /* Takes chara out of the colliders of type 1 (name inferred). */
    void Leave(boost::weak_ptr<CharInfo> chara);

    /* 0x00 */ LightColliMasterTask *m_task;
    /* 0x04 */ UnkLightColliList_801E6500 m_colliders;
    /* 0x10 */ std::map<boost::weak_ptr<CharInfo>, boost::weak_ptr<Effect::System::EffectElement> >
        m_effects;
    /* 0x20 */ LightMap m_lights;
    /* 0x30 */ LightMap m_defaultLights;
};
SIZE_ASSERT(UnkLightColliState_801E6500, 0x40);

extern "C" {
/* The state (made on first use). */
UnkLightColliState_801E6500 *fn_801E6500(void);
}

#endif
