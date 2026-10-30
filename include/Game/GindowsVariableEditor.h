#ifndef GAME_GINDOWSVARIABLEEDITOR_H
#define GAME_GINDOWSVARIABLEEDITOR_H

#include <boost/function.hpp>
#include <dolphin/types.h>

/* Sonicteam::Gindows::Object, Sonicteam::Gindows::Utility::VariableEditor::Variable and
 * VariableBasic<T> (class names from RTTI): by their names, the objects of a debug window system
 * ("Gindows") and the variables its editor shows. Only Game/TVehicleCarMove.cpp's retail .data
 * holds traces of them: the RTTI base lists of Variable and VariableBasic<float>, and a dead,
 * zero-filled copy of VariableBasic<float>'s vtable (ten words: eight virtual functions). Object's
 * and Variable's own vtables are not in that unit, so their constructors are taken to be out of
 * line. Everything else (members, the meaning of the virtual functions) is unknown: the slots are
 * placeholders, and the callback member is an assumption that accounts for the boost::function
 * call (and its bad_function_call) that unit's .rodata and .data hold after the class's RTTI. */
namespace Sonicteam {
namespace Gindows {

class Object {
  public:
    Object();
    virtual ~Object();
};

namespace Utility {
namespace VariableEditor {

class Variable : public Object {
  public:
    Variable();
    virtual ~Variable();
};

template <class T> class VariableBasic : public Variable {
  public:
    VariableBasic(T *value) : m_value(value) {}
    virtual ~VariableBasic() {}
    virtual void vf_0x0C() {}
    virtual void vf_0x10() {}
    virtual void vf_0x14() {}
    virtual void vf_0x18() {}
    virtual void vf_0x1C() {}
    virtual void vf_0x20() {}
    /* Calls the callback (meaning inferred). */
    virtual void vf_0x24() {
        m_onChange();
    }

    T *m_value;
    boost::function0<void> m_onChange;
};

} // namespace VariableEditor
} // namespace Utility
} // namespace Gindows
} // namespace Sonicteam

#endif
