#ifndef GAME_THROWBOMB_H
#define GAME_THROWBOMB_H

#include "Game/MathTypes.h"
#include "boost/weak_ptr.hpp"
#include <dolphin/types.h>

class Task;

/* The parameters of a throw (struct name unknown; layout from its constructor, 0x80194C1C; units
 * that build one define it, as in Game/HeavyDog0510CtrlUnit.cpp and Game/ThrowBomb.cpp). */
struct UnkEffectParam_80194C1C;

/* ThrowBomb (class name from RTTI): a bomb that bosses and enemies throw. Its only implementation
 * is ThrowBombImpl (Game/ThrowBomb.cpp); a ThrowBombTask drives it until it explodes. All its
 * virtual functions are pure (its vtable at 0x80544214 has empty slots). Names are inferred. */
class ThrowBomb {
  public:
    /* Places the bomb's model (slot 0x08). */
    virtual void SetMatrix(const Mtx34f &matrix) = 0;
    /* Throws the bomb from param's position with its velocity (slot 0x0C). */
    virtual void Throw(const UnkEffectParam_80194C1C &param) = 0;
    /* Removes the bomb (slot 0x10). */
    virtual void Remove() = 0;
    /* The model's alpha (slots 0x14 and 0x18). */
    virtual void SetAlpha(float alpha) = 0;
    virtual float GetAlpha() = 0;
    virtual ~ThrowBomb() = 0;
};

inline ThrowBomb::~ThrowBomb() {}

extern "C" {
/* Creates a bomb of the given kind under layer (0x80209908; behaviour inferred). */
boost::weak_ptr<ThrowBomb> fn_80209908(Task *layer, int kind);
/* Creates a bomb of the given kind under layer and throws it at once (0x80209B58). */
void fn_80209B58(Task *layer, int kind, const UnkEffectParam_80194C1C &param);
}

#endif
