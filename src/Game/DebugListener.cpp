/* The application's RenderWare skeleton event handler (fn_80048E90, which the skeleton's main loop
 * calls) and its start-up (fn_80049088), which also creates the default debug listener. The unit
 * holds the first copies of Sonicteam::System::Diagnostics::DebugListener's vtable and RTTI (class
 * name from RTTI; the file is named after it) and of the singleton code of DefaultDebugListener,
 * whose own vtable belongs to the system library (0x80569EAC). The skeleton's functions keep their
 * fn_ names; names of the events and of the helpers are inferred from what they do. Definitions
 * are in reverse address order (deferred inlining emits functions last-defined first). */
#include <dolphin/types.h>
#include "size_assert.h"

namespace Sonicteam {
namespace System {

/* The singleton holder (class names from RTTI): the instance is created on first use by
 * Creator. */
template <class T> class CreateStatic {
  public:
    static T *Create() {
        static T s_instance;
        return &s_instance;
    }
};

template <class T, class Creator> class Singleton {
  public:
    static T *GetInstance() {
        if (ms_instance == 0) {
            ms_instance = Creator::Create();
        }
        return ms_instance;
    }

    /* Defined by the first unit that uses it (0x805EDF80). */
    static T *ms_instance;
};

namespace Diagnostics {

/* Receives the game's debug output (the three functions' meanings are unknown). */
class DebugListener {
  public:
    virtual ~DebugListener() {}
    virtual void Virtual0C() = 0;
    virtual void Virtual10() = 0;
    virtual void Virtual14() = 0;
};

/* The listener installed at start-up; its functions and vtable are in the system library. */
class DefaultDebugListener
    : public DebugListener,
      public Singleton<DefaultDebugListener, CreateStatic<DefaultDebugListener> > {
  public:
    DefaultDebugListener() {}
    virtual ~DefaultDebugListener() {}
    virtual void Virtual0C();
    virtual void Virtual10();
    virtual void Virtual14();

    /* 0x4 */ u8 field_0x4[4]; /* never set here */
};
SIZE_ASSERT(DefaultDebugListener, 0x8);





} // namespace Diagnostics
} // namespace System
} // namespace Sonicteam

using Sonicteam::System::Diagnostics::DefaultDebugListener;

/* RenderWare. */
struct RwRaster;
struct RwRGBA {
    u8 red;
    u8 green;
    u8 blue;
    u8 alpha;
};

/* The skeleton's globals (struct name unknown): the application's name, its maximum screen size
 * and the flag that ends the main loop. */
struct UnkSkeletonGlobals_80575CF0 {
    /* 0x00 */ const char *m_appName;
    /* 0x04 */ int m_maximumWidth;
    /* 0x08 */ int m_maximumHeight;
    /* 0x0C */ int m_quit;
    /* 0x10 */ u8 field_0x10[0x38 - 0x10];
};
extern UnkSkeletonGlobals_80575CF0 lbl_80575CF0;

/* The skeleton's events that this handler processes (names inferred from what it does). */
enum {
    EVENT_CAMERA_SIZE = 0,
    EVENT_INPUT_DEVICE_ATTACH = 4,
    EVENT_PLUGIN_ATTACH = 9,
    EVENT_RW_INITIALIZE = 13,
    EVENT_RW_TERMINATE = 14,
    EVENT_INITIALIZE = 16,
    EVENT_IDLE = 18
};

/* What the handler returns: an error, processed, not processed. */
enum {
    EVENT_ERROR = 0,
    EVENT_PROCESSED = 1,
    EVENT_NOT_PROCESSED = 2
};

extern "C" {
/* The skeleton (the main unit): its start-up, RenderWare's start and end, and an error message. */
int fn_80051528(void);
int fn_80051598(void *param);
void fn_800517A0(void);
void fn_80051A10(const char *message);
/* The debug text's character set: created from two colours, and destroyed. */
RwRaster *fn_80478DE0(const RwRGBA *foreground, const RwRGBA *background);
s32 fn_80489BDC(RwRaster *raster);
/* RenderWare plugins attached at start-up. */
int fn_8046080C(void);
int fn_80441028(void);
int fn_80447200(void);
int fn_8044CB90(void);
int fn_80037E0C(void);
int fn_804A1E74(void);
int fn_80453A78(void);
int fn_8044484C(void);
int fn_80452CDC(void);
int fn_80447640(void);
/* The game's start and end with RenderWare, and its frame (false when it is to end). */
bool fn_801E2234(void);
bool fn_801E21E4(void);
bool fn_801E1EB8(void);

int fn_80049088(void);
}

/* The debug text's character set (Game/StageSequenceMovieTest.cpp draws with it). */
RwRaster *lbl_805EE170;

static const RwRGBA s_foreground = {200, 200, 200, 255};
static const RwRGBA s_background = {64, 64, 64, 0};

static inline int AttachPlugins() {
    if (fn_8046080C() == 0) {
        return 0;
    }
    if (fn_80441028() == 0) {
        return 0;
    }
    if (fn_80447200() == 0) {
        return 0;
    }
    if (fn_8044CB90() == 0) {
        return 0;
    }
    if (fn_80037E0C() == 0) {
        return 0;
    }
    if (fn_804A1E74() == 0) {
        return 0;
    }
    if (fn_80453A78() == 0) {
        return 0;
    }
    if (fn_8044484C() == 0) {
        return 0;
    }
    if (fn_80452CDC() == 0) {
        return 0;
    }
    if (fn_80447640() == 0) {
        return 0;
    }
    return 1;
}

static inline int Initialize3D(void *param) {
    if (fn_80051598(param) == 0) {
        fn_80051A10("Error initializing RenderWare.");
        return 0;
    }
    lbl_805EE170 = fn_80478DE0(&s_foreground, &s_background);
    if (lbl_805EE170 == 0) {
        fn_80051A10("Cannot create raster charset.");
        return 0;
    }
    if (!fn_801E2234()) {
        return 0;
    }
    return 1;
}

static inline void Terminate3D() {
    if (fn_801E21E4()) {
        if (lbl_805EE170 != 0) {
            fn_80489BDC(lbl_805EE170);
        }
        fn_800517A0();
    }
}

static inline void Idle() {
    if (!fn_801E1EB8()) {
        lbl_80575CF0.m_quit = 1;
    }
}

/* Starts the skeleton and sets the application's defaults. */
extern "C" int fn_80049088(void) {
    if (fn_80051528() != 0) {
        if (lbl_80575CF0.m_maximumWidth == 0) {
            lbl_80575CF0.m_maximumWidth = 640;
        }
        if (lbl_80575CF0.m_maximumHeight == 0) {
            lbl_80575CF0.m_maximumHeight = 480;
        }
        lbl_80575CF0.m_appName = "RW3 Static Geometry Construction Example";
        DefaultDebugListener::GetInstance();
        return 1;
    }
    return 0;
}

/* The skeleton's event handler. */
extern "C" int fn_80048E90(int event, void *param) {
    switch (event) {
    case EVENT_INITIALIZE:
        return fn_80049088() ? EVENT_PROCESSED : EVENT_ERROR;
    case EVENT_CAMERA_SIZE:
        return EVENT_PROCESSED;
    case EVENT_RW_INITIALIZE:
        return Initialize3D(param) ? EVENT_PROCESSED : EVENT_ERROR;
    case EVENT_RW_TERMINATE:
        Terminate3D();
        return EVENT_PROCESSED;
    case EVENT_PLUGIN_ATTACH:
        return AttachPlugins() ? EVENT_PROCESSED : EVENT_ERROR;
    case EVENT_INPUT_DEVICE_ATTACH:
        return EVENT_PROCESSED;
    case EVENT_IDLE:
        Idle();
        return EVENT_PROCESSED;
    }
    return EVENT_NOT_PROCESSED;
}
