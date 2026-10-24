/* AdvReal::Logo::Phase (class name from RTTI): the front-end phase that shows the logo at start-up.
 * Its state lives in a Logo::Impl (class name inferred) that waits for the screen fade, plays the
 * CSD project "csdFiles/Logo" (or "csdFiles/Logo_JP" for the Japanese region), starts loading the
 * opening movie after a second and moves on to the movie (phase 8) when the logo ends, or to the
 * title (phase 1) when the player skips it. The file name and all names marked "(name inferred)"
 * are not original. Definitions are in reverse address order (deferred inlining emits functions
 * last-defined first). */
#include <boost/scoped_ptr.hpp>

#include "Game/AdvBase.h"
#include "Game/AdvPhaseManager.h"
#include "Game/CsdRCPtr.h"
#include "Game/CsdSceneCtrl.h"
#include "Game/CsdSceneSet.h"
#include <dolphin/types.h>
#include "size_assert.h"

using Chao::CSD::CTexList;
using Chao::CSD::RCPtr;

/* The loaded CSD project (Chao::CSD::CProject, Game/csdmProject.cpp; declared as in TGi.h, which
 * gives its size). */
namespace Chao {
namespace CSD {
class CProject {
  public:
    CProject();
    virtual ~CProject();

    /* 0x04 */ u8 field_0x4[0x4C - 0x4];
};
} // namespace CSD
} // namespace Chao
SIZE_ASSERT(Chao::CSD::CProject, 0x4C);

/* The value a project returns from fn_8036D598 (class name unknown; declared as in TGi.h). */
class UnkCsdHandle_80042DF0 {
  public:
    UnkCsdHandle_80042DF0(const UnkCsdHandle_80042DF0 &other);
    ~UnkCsdHandle_80042DF0();

    /* 0x0 */ void *m_object;
};

/* An empty texture list (meaning inferred; see Game/AdvOption.cpp). */
extern RCPtr<CTexList> lbl_805EE110;

/* The game state (struct name unknown; see Game/AdvMemoryCheck.cpp). */
struct UnkGameState_8057D6A0 {
    /* 0x000 */ u8 field_0x0[0x40];
    /* 0x040 */ s32 m_region; /* 0 Japan (meaning inferred) */
    /* 0x044 */ u8 field_0x44[0x7F - 0x44];
    /* 0x07F */ u8 m_logoLock; /* set while the logo may not be skipped (meaning inferred) */
    /* 0x080 */ u8 field_0x80[0x150 - 0x80];
};
SIZE_ASSERT(UnkGameState_8057D6A0, 0x150);

extern UnkGameState_8057D6A0 lbl_8057D6A0;

/* The movie playing (struct name unknown; see Game/AdvMovie.cpp). */
struct UnkMovieStatus_805EE0B0 {
    /* 0x00 */ u8 field_0x0[0x4];
    /* 0x04 */ u16 m_flags;
};

extern UnkMovieStatus_805EE0B0 *lbl_805EE0B0;

extern "C" {
/* The resource manager and a resource of it by name (behaviour inferred). */
void *fn_8003F8D4(void);
void *fn_800423B8(void *manager, const char *name, int arg2);
/* Builds the project from loaded data; nonzero on success (behaviour inferred). */
int fn_8036E114(Chao::CSD::CProject *project, void *data);
/* Projects (behaviour inferred; see Game/AdvStoryLibrary.cpp). */
UnkCsdHandle_80042DF0 fn_8036D598(Chao::CSD::CProject *project, int arg1);
RCPtr<CTexList> fn_8036C628(Chao::CSD::CProject *project);

/* Starts loading a movie file, and a call made once it started (behaviour inferred; see
 * Game/AdvMovie.cpp). */
bool fn_80036E78(const char *fileName, int arg1, int arg2, int arg3);
void fn_8003636C(void);

/* The voice manager and muting it (behaviour inferred; see Game/AdvMovie.cpp). */
void *fn_801E5CA8(void);
void fn_802C839C(void *manager, int arg1);

/* The screen fade and whether it has finished (behaviour inferred; see Game/AdvMemoryCheck.cpp). */
void fn_80173834(int arg0, float time);
void fn_80117514(void);
bool fn_802E5810(void);

/* Pad input (Game/AdvPadInput.cpp). */
bool fn_802B7D5C(int button, int pad);
}

/* The opening movie, loaded while the logo plays. */
static const char s_movieFile[] = "Op.sfd";

/* Not original: an unused table that pools the unit's .rodata strings in retail's order (see
 * Game/GiGameOver.cpp; the linker drops the table): the CSD projects of the logo, the export one and
 * the Japanese one. */
static const char *s_rodataPool[2] = {"csdFiles/Logo", "csdFiles/Logo_JP"};

/* The scene of the project. */
static const char *const s_sceneNames[1] = {"GC"};

namespace AdvReal {
namespace Logo {

/* The phase's state (class name inferred). */
class Impl {
  public:
    Impl();
    ~Impl() {
        lbl_8057D6A0.m_logoLock = 0;
    }

    bool Tick();
    bool Draw();
    void WaitFade();
    void Start();
    bool Load();

    /* Member names are inferred. */
    /* 0x00 */ u8 m_busy;
    /* 0x01 */ u8 field_0x1[3]; /* (the scene set's own type has byte alignment) */
    /* 0x04 */ UnkCsdSceneSet_8003D5D8 m_scenes;
    /* 0x10 */ Chao::CSD::CProject m_project;
    /* 0x5C */ u8 m_loaded;
    /* 0x60 */ s32 m_frame;
    /* 0x64 */ s32 m_state; /* 0 fading in, 1 loading, 2 playing, 3 done, 4 failed */
};
SIZE_ASSERT(Impl, 0x68);

class Phase : public AdvBase {
  public:
    Phase();
    virtual ~Phase();
    virtual bool Tick();
    virtual bool Draw();

    /* 0x4 */ boost::scoped_ptr<Impl> m_impl;
};
SIZE_ASSERT(Phase, 0x8);

/* Whether two texture lists are the same (name inferred; as in Game/AdvOption.cpp). */
static inline int SameTexList(const RCPtr<CTexList> &a, const RCPtr<CTexList> &b) {
    if (a.m_object != 0 && b.m_object != 0) {
        return a.m_object->m_object == b.m_object->m_object;
    }
    return a.m_object == b.m_object;
}

Impl::Impl() : m_loaded(0), m_frame(0), m_state(1) {
    fn_80173834(0, 0.0f);
}

bool Impl::Tick() {
    switch (m_state) {
    case 0:
        WaitFade();
        return true;
    case 1:
        Start();
        break;
    case 2:
        if (!m_busy) {
            m_state = 3;
        }
        if (++m_frame > 60 && lbl_805EE0B0 == 0 && fn_80036E78(s_movieFile, 0, 0, 0)) {
            fn_8003636C();
        }
        if (!lbl_8057D6A0.m_logoLock && fn_802B7D5C(0, 4)) {
            fn_802B7528(fn_801E5E00(), 1);
            if (lbl_805EE0B0 != 0) {
                lbl_805EE0B0->m_flags |= 1;
            }
        }
        break;
    case 3:
        fn_802B7528(fn_801E5E00(), 8);
        break;
    case 4:
        break;
    }
    m_busy = fn_8003CDF4(&m_scenes);
    return true;
}

bool Impl::Draw() {
    if (m_loaded) {
        if (SameTexList(fn_8036C628(&m_project), lbl_805EE110)) {
            return false;
        }
        fn_8003C92C(&m_scenes, fn_8036C628(&m_project));
    }
    return true;
}

void Impl::WaitFade() {
    fn_80117514();
    if (fn_802E5810()) {
        m_state = 1;
    }
}

void Impl::Start() {
    if (Load()) {
        fn_8003CEAC(&m_scenes, s_sceneNames[0])->Play("Open", 1);
        fn_8003C8D8(&m_scenes, s_sceneNames[0], true);
        m_state = 2;
        fn_802C839C(fn_801E5CA8(), 0);
        fn_80173834(1, 0.0f);
        m_loaded = 1;
    } else {
        m_state = 4;
    }
}

bool Impl::Load() {
    const char *file;
    switch (lbl_8057D6A0.m_region) {
    case 0:
        file = "csdFiles/Logo_JP";
        break;
    default:
        file = "csdFiles/Logo";
        break;
    }
    if (fn_8036E114(&m_project, fn_800423B8(fn_8003F8D4(), file, 0))) {
        UnkCsdHandle_80042DF0 project = fn_8036D598(&m_project, 0);
        fn_8003CEF8(&m_scenes, project, s_sceneNames, 1);
        return true;
    }
    return false;
}

Phase::Phase() : m_impl(new Impl) {}

Phase::~Phase() {}

bool Phase::Tick() {
    return m_impl->Tick();
}

bool Phase::Draw() {
    return m_impl->Draw();
}

} // namespace Logo
} // namespace AdvReal
