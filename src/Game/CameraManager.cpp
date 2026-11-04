/* The camera manager (class name unknown; UnkCameraManager_80009548, the function-local static
 * behind fn_80009548): the camera factory and the camera unit of each of the two screens. The
 * unit holds the manager's constructor and destructor and the routines other units call by
 * address: a screen's camera unit (fn_8000D0F0), the frame update, the start of the units with a
 * factory, the shakes, and the setters of a screen's current camera mode; and the game's first
 * copies of std::bad_alloc, the screen accessor fn_8000DCC0 and the setters of a placement's eye
 * and point looked at. It is split from CameraFactory before it at the end of that unit's tree
 * code, and from CameraMode after it by the padding at the end of its .data. File name inferred
 * from the class's role; other names are inferred. Definitions are in reverse address order
 * (deferred inlining emits functions last-defined first).
 *
 * NonMatching: every function body matches, but the unit does not link in place, and what is left is
 * one crossed pair of shared_ptr control blocks. Retail's .sbss is 12 bytes at 0x805EDFB0: the byte
 * static s_screenGuard (0x805EDFB0), then the guard bytes of the two control blocks, @505 at
 * 0x805EDFB4 and @482 at 0x805EDFB8, each byte followed by a three-byte gap; ours is nine bytes with
 * the same three records but our CameraManagerUnit guard first. The guards are read through r13, so
 * the inverted pair alone costs two addi immediates: 0x8000d33c, in
 * __ct<CameraManagerUnit>__shared_ptr<CameraManagerUnit>FPQ..., holds 880d2b74 where retail has
 * 880d2b78, and 0x8000d7bc, in the CameraShake one, holds the same two displacements the other way
 * round. The same crossing runs through the checked_deleter half of both control blocks: retail's
 * dispose__ of the CameraManagerUnit block (.text+0xe54), its type name (@749, the 72 bytes at
 * 0x804AA480) and its __RTTI__Q25boostN12checked_deleter...E record (.sdata+0x28) all come first,
 * while ours put CameraShake's three first (dispose__ at .text+0x1150) and cost 0x8000defc, the
 * 84 bytes of type names at 0x804AA4C0..0x804AA514 and two .sdata words. The two shared_ptr and
 * control-block constructors, the two control-block vtables in .data (CameraShake's at +0xc,
 * CameraManagerUnit's at +0x30) and the impl __RTTI__ records at .sdata+0x8 and +0x10 already follow
 * retail, so only the deleter half is crossed; and the order does not answer to the shape of the
 * source: including CameraManagerUnit.h before CameraShake.h, and keeping the CameraShake new behind
 * the CameraManagerUnit one behind a file-scope inline helper, were both tried and changed nothing
 * (the helper also cost an out-of-line MakeShake). The first two macros below are what puts our
 * .data records exactly where retail's are, the 4-byte gap retail has at its end
 * (gap_07_8051C594_data) aside: the shared_ptr base destructor goes to the library, which retires
 * our local copy of the base's vtable, its __RTTI__ record and its .sdata word, and std::exception's
 * what() goes out of line, which retires our local copy of that vtable, its RTTI and the
 * "exception" string; retail's object references all three by name.
 * Retail also has an unused function first (stripped), whose instances the vector's begin and end and
 * fn_8000D0F0 are. */
#define BOOST_SP_EXTERN_BASE_DESTRUCTOR
#define MSL_EXCEPTION_WHAT_OUT_OF_LINE
#define MSL_NEW_INLINE_BAD_ALLOC
#define MSL_VECTOR_AT
#define MSL_VECTOR_DESTROY_ALL_LATE
#define MSL_VECTOR_DESTROY_ALL_FIRST
#define MSL_VECTOR_MIDDLE
#define MSL_LIST_ACCESSORS
#include <new>
#include <vector>

#include "Game/CameraShake.h"
#include "Game/CameraModeUser.h"
#include "Game/CameraManagerUnit.h"
#include "Game/CameraState.h"
#include "Game/MathTypes.h"
#include "Game/ScreenHolder.h"
#include "boost/shared_ptr.hpp"

using SonicteamUSA::System::Camera::CameraFactory;
using SonicteamUSA::System::Camera::CameraManagerUnit;
using SonicteamUSA::System::Camera::CameraMode;
using SonicteamUSA::System::Camera::CameraShake;

typedef boost::shared_ptr<CameraManagerUnit> UnitPtr;

/* The shake's reset (boost/shared_ptr.hpp only declares shared_ptr::reset, which the game has out
 * of line; boost 1.32's body here goes through a helper the game also has out of line). */
inline void ResetShake(boost::shared_ptr<CameraShake> &shake) {
    boost::shared_ptr<CameraShake>().swap(shake);
}

namespace boost {
template <> inline void shared_ptr<CameraShake>::reset() {
    ResetShake(*this);
}
} // namespace boost

/* What fn_80009548 returns (a function-local static of Game/CameraModeStop.cpp). */
class UnkCameraManager_80009548 {
  public:
    UnkCameraManager_80009548();
    ~UnkCameraManager_80009548();

    /* 0x0 */ CameraFactory *m_factory;
    /* 0x4 */ std::vector<UnitPtr> m_units; /* one per screen */
};
SIZE_ASSERT(UnkCameraManager_80009548, 0x10);

typedef UnkCameraManager_80009548 CameraManager;
typedef UnkCameraState_8000AA70 CameraState;

extern "C" {
void *__register_global_object(void *object, void *destructor, void *record);
/* The screen (Game/ScreenHolder.h): its constructor and destructor. */
void *__ct__24UnkScreenHolder_8000DCC0Fv(void *object);
void __dt__24UnkScreenHolder_8000DCC0Fv(void *object, int flags);
/* A shake of a kind (Game/CameraShake.cpp). */
boost::shared_ptr<CameraShake> fn_8000EDD0(int kind, float strength);
/* Placement setters (Game/CameraState.cpp, Game/CameraAdjust.cpp). */
void fn_8000A3F0(CameraState *state, float value);
void fn_8000AF60(CameraState *state, const Vec2f *window);
}

/* The screen accessor's statics: the pointer is placed apart from the unit's .sbss (after the
 * .sbss of three later units), so it is declared extern and left unsplit. */
extern UnkScreenHolder_8000DCC0 *lbl_805EDFC8;
static s8 s_screenGuard;
static u32 s_screenRecord[3];
static u32 s_screen[0x6C / 4];

/* The placement of a mode. */
inline CameraState *ModeState(const boost::shared_ptr<CameraMode> &mode) {
    return reinterpret_cast<CameraState *>(mode.get());
}

inline CameraState *Placement(CameraMode *mode) {
    return reinterpret_cast<CameraState *>(mode);
}

inline void SetFov(CameraMode *mode, float fov) {
    Placement(mode)->m_fov = fov;
}


/* The camera unit of a screen. */
extern "C" inline UnitPtr fn_8000D0F0(CameraManager *manager, int index) {
    return manager->m_units.at(index);
}

/* An empty inline function: calling it keeps the caller out of line (deferred inlining is one
 * level deep). */
inline void Nothing() {}

/* The game's screen (a function-local static). */
extern "C" inline UnkScreenHolder_8000DCC0 *fn_8000DCC0(void) {
    if (lbl_805EDFC8 == 0) {
        if (!s_screenGuard) {
            __register_global_object(__ct__24UnkScreenHolder_8000DCC0Fv(s_screen),
                                     (void *)__dt__24UnkScreenHolder_8000DCC0Fv, s_screenRecord);
            s_screenGuard = 1;
        }
        lbl_805EDFC8 = reinterpret_cast<UnkScreenHolder_8000DCC0 *>(s_screen);
    }
    Nothing();
    return lbl_805EDFC8;
}

UnkCameraManager_80009548::UnkCameraManager_80009548() : m_factory(0) {
    m_units.clear();
    m_units.reserve(2);
    fn_8000DCC0()->CreateScreens();
}

UnkCameraManager_80009548::~UnkCameraManager_80009548() {
    if (m_factory != 0) {
        delete m_factory;
        m_factory = 0;
    }
}

/* A placement's eye and point looked at (the point looked at, then the eye, kept). */
extern "C" void fn_8000DB78(CameraState *state, const Vec3f *eye) {
    fn_8000A5D4(reinterpret_cast<CameraMode *>(state), eye, &state->m_target);
}

extern "C" void fn_8000DB50(CameraState *state, const Vec3f *target) {
    fn_8000A5D4(reinterpret_cast<CameraMode *>(state), &state->m_eye, target);
}

/* Sets the eye and the point looked at of a screen's current mode. */
extern "C" void fn_8000DAB8(CameraManager *manager, const Vec3f *eye, const Vec3f *target, int index) {
    boost::shared_ptr<CameraMode> mode(fn_8000D0F0(manager, index)->GetCurrentMode());
    if (mode) {
        fn_8000A5D4(reinterpret_cast<CameraMode *>(ModeState(mode)), eye, target);
    }
}

/* Sets the field of view of a screen's current mode. */
extern "C" void fn_8000DA24(CameraManager *manager, int index, float fov) {
    boost::shared_ptr<CameraMode> mode(fn_8000D0F0(manager, index)->GetCurrentMode());
    if (mode.get() != 0) {
        SetFov(mode.get(), fov);
    }
}

/* Resets the view window of a screen's current mode. */
extern "C" void fn_8000D988(CameraManager *manager, int index, float value) {
    boost::shared_ptr<CameraMode> mode(fn_8000D0F0(manager, index)->GetCurrentMode());
    if (mode) {
        fn_8000A3F0(ModeState(mode), value);
    }
}

extern "C" void fn_8000D968(CameraState *state, const Vec2f *window) {
    fn_8000AF60(state, window);
}

/* Starts a shake of a kind on a screen's camera. */
extern "C" void fn_8000D8EC(CameraManager *manager, int kind, int index, float strength) {
    fn_8000D0F0(manager, index)->m_shake = fn_8000EDD0(kind, strength);
}

/* A shake from its values. */
extern "C" void fn_8000D664(CameraManager *manager, int index, float duration, float amplitude,
                            float frequency) {
    fn_8000D0F0(manager, index)->m_shake =
        boost::shared_ptr<CameraShake>(new CameraShake(duration, amplitude, frequency));
}

/* Stops a screen's shake. */
extern "C" void fn_8000D534(CameraManager *manager, int index) {
    fn_8000D0F0(manager, index)->m_shake.reset();
}

/* Turns a screen's camera on. */
extern "C" inline void fn_8000D270(CameraManager *manager, int index) {
    fn_8000D0F0(manager, index)->field_0x0 = 1;
}

/* Makes the camera units with the factory, once. */
extern "C" void fn_8000D1E0(CameraManager *manager, CameraFactory *factory) {
    if (manager->m_factory == 0) {
        manager->m_factory = factory;
        for (u32 i = 0; i < 2; i++) {
            UnitPtr unit(new CameraManagerUnit(i));
            manager->m_units.push_back(unit);
        }
        fn_8000D270(manager, 0);
    }
}

/* The frame update of the cameras that are on. */
extern "C" void fn_8000D170(CameraManager *manager, float dt) {
    for (UnitPtr *it = manager->m_units.begin(); it != manager->m_units.end(); it++) {
        if ((*it)->field_0x0) {
            (*it)->Update(dt);
        }
    }
}

/* Not called anywhere: the linker strips it from the game. Retail emits the vector's begin and end
 * and fn_8000D0F0 before the frame update, where the instances of a function defined here would go
 * (inferred). */
void UnusedUpdate_8000D094(CameraManager *manager, float dt) {
    fn_8000D0F0(manager, 0);
    for (UnitPtr *it = manager->m_units.begin(); it != manager->m_units.end(); it++) {
    }
}

#include <vector_impl>
