#ifndef GAME_PATHOBJECT_H
#define GAME_PATHOBJECT_H

#include "boost/weak_ptr.hpp"
#include <dolphin/types.h>
#include "size_assert.h"

class CharInfo;

/* A message a path object's controller passes on (struct name unknown; layout from
 * SetEggBalloon's handler, 0x80145BB8; member names inferred). Type 0x303 starts an attack of
 * kind m_kind for m_time seconds, 0x304 stops it. */
struct UnkPathObjectMessage {
    /* 0x00 */ u8 field_0x0[0x4];
    /* 0x04 */ u16 m_type;
    /* 0x06 */ u8 field_0x6[0x14 - 0x6];
    /* 0x14 */ s32 m_kind;
    /* 0x18 */ float m_time;
    /* 0x1C */ float m_interval;
};

/* PathObject (class name from RTTI; vtable at 0x80529B78): the base of the objects a stage
 * controller moves along a path (EggBalloon::SetEggBalloon, EscapePlane::SetEscapePlane,
 * BKTank::SetBKTank). It has no members. Its destructor and the three empty virtual functions are
 * inline: SetEggBalloon's unit emits the copies the linker keeps (0x80147844, 0x80147900,
 * 0x80147904, 0x80147908), and SetEscapePlane's vtable uses the last one. Virtual function names
 * are inferred. */
class PathObject {
  public:
    virtual ~PathObject() {}
    /* Per-frame work. */
    virtual void Tick(float dt) = 0;
    /* Loads the model and sets the object up. */
    virtual void Init() = 0;
    virtual void vf_0x14() = 0;
    /* Places the model. */
    virtual void UpdateMatrix() = 0;
    /* True once the object is done. */
    virtual bool HasEnded() = 0;
    /* The object's character info. */
    virtual boost::weak_ptr<CharInfo> GetCharInfo() = 0;
    virtual void vf_0x24() {}
    /* Called by the controller (0x802742D4; meaning unknown). */
    virtual void vf_0x28() {}
    /* Handles a message from the controller. */
    virtual void OnMessage(UnkPathObjectMessage *message) {}
};
SIZE_ASSERT(PathObject, 0x4);

#endif
