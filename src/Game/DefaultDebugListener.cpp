/* Sonicteam::System::Diagnostics::DefaultDebugListener (class names and bases from RTTI): the debug
 * listener Game/DebugListener.cpp installs at start-up. Its three functions pass the text they are
 * given straight to the C library's output (fn_803A7D24); this unit holds them with the class's
 * vtable and RTTI records. The classes are declared as in Game/DebugListener.cpp, here with the
 * text parameter the three functions take (their names stay placeholders). Definitions are in
 * reverse address order (deferred inlining emits functions last-defined first). */
#include <dolphin/types.h>
#include "size_assert.h"

extern "C" int fn_803A7D24(const char *text);

namespace Sonicteam {
namespace System {

template <class T> class CreateStatic {
  public:
    static T *Create();
};

template <class T, class Creator> class Singleton {
  public:
    static T *ms_instance;
};

namespace Diagnostics {

class DebugListener {
  public:
    virtual ~DebugListener() {}
    virtual void Virtual0C(const char *text) = 0;
    virtual void Virtual10(const char *text) = 0;
    virtual void Virtual14(const char *text) = 0;
};

class DefaultDebugListener
    : public DebugListener,
      public Singleton<DefaultDebugListener, CreateStatic<DefaultDebugListener> > {
  public:
    DefaultDebugListener() {}
    virtual ~DefaultDebugListener() {}
    virtual void Virtual0C(const char *text);
    virtual void Virtual10(const char *text);
    virtual void Virtual14(const char *text);

    /* 0x4 */ u8 field_0x4[4];
};
SIZE_ASSERT(DefaultDebugListener, 0x8);

void DefaultDebugListener::Virtual0C(const char *text) {
    fn_803A7D24(text);
}

void DefaultDebugListener::Virtual10(const char *text) {
    fn_803A7D24(text);
}

void DefaultDebugListener::Virtual14(const char *text) {
    fn_803A7D24(text);
}

} // namespace Diagnostics
} // namespace System
} // namespace Sonicteam
