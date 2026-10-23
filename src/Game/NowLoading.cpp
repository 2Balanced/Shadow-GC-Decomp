/* NowLoading and CNowLoadingThread (class names from RTTI), and the "Now Loading" screen they
 * draw (see Game/NowLoading.h). The file name and the names marked "(inferred)" in the header are
 * not original. Definitions are in reverse address order (deferred inlining emits functions
 * last-defined first).
 *
 * NonMatching: NowLoadingScreen::Load (96.9%) has the register swap around sprintf of
 * Game/ResultTask.cpp and Game/Mission.cpp (retail keeps the game state's address in r6), and
 * retail places the RTTI name "Task" after the float constants in .sdata2, where GC/2.7 puts it
 * first. Everything else matches. */
#include "Game/NowLoading.h"

#include <stdio.h>

#include "Game/Task.h"
#include <dolphin/types.h>
#include "size_assert.h"

/* RenderWare (rw/rwcore.h): a colour, a camera as far as it is used here, and the device's
 * render state functions. */
typedef struct RwRGBA {
    u8 red;
    u8 green;
    u8 blue;
    u8 alpha;
} RwRGBA;

struct RwFrame;

typedef struct RwCamera {
    /* 0x00 */ u8 field_0x0[0x4];
    /* 0x04 */ RwFrame *parent; /* object.object.parent: the camera's frame */
    /* 0x08 */ u8 field_0x8[0x60 - 0x8];
    /* 0x60 */ RwRaster *frameBuffer;
} RwCamera;

typedef int (*RwRenderStateSetFunction)(int state, void *value);
typedef int (*RwRenderStateGetFunction)(int state, void *value);

typedef struct RwGlobals {
    /* 0x00 */ u8 field_0x0[0x20];
    /* 0x20 */ RwRenderStateSetFunction fpRenderStateSet; /* dOpenDevice.fpRenderStateSet */
    /* 0x24 */ RwRenderStateGetFunction fpRenderStateGet; /* dOpenDevice.fpRenderStateGet */
} RwGlobals;

/* The engine instance. */
extern RwGlobals *lbl_805F15B4;

enum {
    rwRENDERSTATETEXTURERASTER = 1,
    rwRENDERSTATEZTESTENABLE = 6,
    rwRENDERSTATEZWRITEENABLE = 8,
    rwRENDERSTATEVERTEXALPHAENABLE = 12,
    rwRENDERSTATECULLMODE = 20,
    rwCULLMODECULLNONE = 1
};

/* The game state (struct name unknown; see Game/AdvMemoryCheck.cpp). */
struct UnkGameState_8057D6A0 {
    /* 0x000 */ u8 field_0x0[0x14];
    /* 0x014 */ u8 m_loading; /* (meaning inferred) */
    /* 0x015 */ u8 field_0x15[0x50 - 0x15];
    /* 0x050 */ s32 m_frameRate; /* (meaning inferred) */
    /* 0x054 */ u8 field_0x54[0x58 - 0x54];
    /* 0x058 */ s32 m_language; /* indexes lbl_8051D528 */
    /* 0x05C */ u8 field_0x5C[0xB1 - 0x5C];
    /* 0x0B1 */ u8 field_0xB1; /* no title while set (meaning unknown) */
    /* 0x0B2 */ u8 m_twoPlayer; /* (meaning inferred) */
    /* 0x0B3 */ u8 field_0xB3[0x124 - 0xB3];
    /* 0x124 */ u8 field_0x124; /* the sound keeps playing while set (meaning unknown) */
    /* 0x125 */ u8 field_0x125[0x150 - 0x125];
};
SIZE_ASSERT(UnkGameState_8057D6A0, 0x150);

extern UnkGameState_8057D6A0 lbl_8057D6A0;

/* The language suffixes of the file names (see Game/TGi.h). */
extern const char *lbl_8051D528[];

/* Set while the loading screen runs (meaning inferred; see Game/PjsEventActionManager.cpp). */
extern u8 lbl_805EF948;

/* The screens (struct name unknown; see Game/ScreenViewport.cpp). */
struct UnkScreenHolder_8000DCC0 {
    /* 0x00 */ u8 field_0x0[0x10];
    /* 0x10 */ RwRaster *m_frameBuffer; /* (meaning inferred) */
};

/* The stage title (class name unknown; see Game/StageTitleTask.cpp). */
class UnkObject_801E67EC {
  public:
    bool Load(int stage);
    bool Update();
    void Show();
    void Stop();
};

class UnkCsdHandle_80042DF0 {
  public:
    UnkCsdHandle_80042DF0(const UnkCsdHandle_80042DF0 &other);
    ~UnkCsdHandle_80042DF0();

    /* 0x0 */ void *m_object;
};

class UnkObject_802E5B2C;

extern "C" {
/* RenderWare: RwCameraCreate, RwFrameCreate, RwCameraSetFrame, RwFrameDestroy, RwCameraDestroy,
 * RwCameraClear, RwCameraBeginUpdate, RwCameraEndUpdate and RwCameraShowRaster (names from the
 * behaviour; the symbols are placeholders). */
RwCamera *fn_80485A58(void);
RwFrame *fn_80486D00(void);
void *fn_8048D7C0(RwCamera *camera, RwFrame *frame);
int fn_80486E2C(RwFrame *frame);
int fn_804859F8(RwCamera *camera);
RwCamera *fn_804857BC(RwCamera *camera, RwRGBA *color, int clearMode);
RwCamera *fn_804854DC(RwCamera *camera);
RwCamera *fn_804854B4(RwCamera *camera);
RwCamera *fn_80485808(RwCamera *camera, void *device, u32 flags);

/* The time in microseconds, and in ticks (behaviour inferred). */
u64 fn_80051328(void);
u64 fn_80051A94(void);

void *fn_8001C4F8(void);
Task *fn_801EB304(void *root, int layer);
UnkScreenHolder_8000DCC0 *fn_8000DCC0(void);
UnkObject_801E67EC *fn_801E67EC(void);
NowLoadingScreen *fn_801E265C(void);

/* The message window: ending it (see Game/SaveCycleMessage.cpp). */
UnkObject_802E5B2C *fn_80117514(void);
void fn_802E582C(UnkObject_802E5B2C *object);

/* Sounds: pausing and resuming them (behaviour inferred). */
void *fn_80053164(void);
void fn_801D51EC(void *sound);
void fn_801D520C(void *sound);
void fn_801D515C(void *sound);

/* CSD projects (see Game/TGi.h). */
void *fn_8003F8D4(void);
const char *fn_800423B8(void *arg0, const char *name, int arg2);
int fn_8036E114(UnkCsdResource_8003E808 *resource, const char *file);
UnkCsdHandle_80042DF0 fn_8036D598(UnkCsdResource_8003E808 *project, int arg1);
void fn_8036DA1C(UnkCsdResource_8003E808 *project, float dt);
void fn_8036D94C(UnkCsdResource_8003E808 *project);
}

NowLoading *NowLoading::s_instance;

static RwRGBA s_clearColor = {0, 0, 0, 0};

/* The project of the "Now Loading" screen, before its language suffix. */
static const char s_projectName[] = "csdFiles/Loading";

extern "C" void fn_802BEBE0(s32 stage) {
    if (lbl_8057D6A0.field_0xB1 || lbl_8057D6A0.m_twoPlayer) {
        stage = -1;
    }
    if (NowLoading::s_instance == 0) {
        return;
    }
    NowLoading::s_instance->m_stage = stage;
    if (NowLoading::s_instance->m_thread == 0) {
        NowLoading::s_instance->m_request |= 1;
        NowLoading::s_instance->m_flags &= ~4;
    } else if (NowLoading::s_instance->m_request & 2) {
        NowLoading::s_instance->m_request &= ~2;
    }
}

extern "C" void fn_802BEB90(void) {
    if (NowLoading::s_instance == 0) {
        return;
    }
    if (NowLoading::s_instance->m_thread != 0) {
        NowLoading::s_instance->m_request |= 2;
    } else if (NowLoading::s_instance->m_request & 1) {
        NowLoading::s_instance->m_request &= ~1;
        NowLoading::s_instance->m_flags |= 4;
    }
}

extern "C" int fn_802BEB68(void) {
    if (NowLoading::s_instance != 0 && NowLoading::s_instance->m_thread != 0) {
        return 1;
    }
    return 0;
}

NowLoading::NowLoading(Task *parent) : Task(parent) {
    m_thread = 0;
    m_request = 0;
    m_name = "NowLoadingTask";
    m_flags |= 0x100;
    s_instance = this;
    m_flags |= 4;
}

NowLoading::~NowLoading() {
    if (m_thread != 0) {
        m_thread->End();
        m_thread = 0;
    }
    s_instance = 0;
    m_request &= ~2;
}

void CNowLoadingThread::End() {
    m_flags |= 1;
}

void NowLoading::Tick(float dt) {
    if (m_request & 2) {
        if (m_thread != 0) {
            m_thread->End();
            m_thread = 0;
        }
        m_request &= ~2;
        fn_801EB304(fn_8001C4F8(), 5)->m_flags &= ~4;
        lbl_805EF948 = 0;
        lbl_8057D6A0.m_loading = 0;
        fn_801D51EC(fn_80053164());
        m_flags |= 4;
    } else if (m_request & 1) {
        RwRaster *frameBuffer = fn_8000DCC0()->m_frameBuffer;
        m_request &= ~1;
        m_current = m_stage;
        fn_802E582C(fn_80117514());
        fn_801EB304(fn_8001C4F8(), 5)->m_flags |= 4;
        lbl_805EF948 = 1;
        lbl_8057D6A0.m_loading = 1;
        if (!lbl_8057D6A0.field_0x124) {
            fn_801D520C(fn_80053164());
            fn_801D515C(fn_80053164());
        }
        m_thread = new CNowLoadingThread(frameBuffer, m_stage);
    } else if (m_stage != m_current && m_stage >= 0) {
        if (m_thread != 0) {
            if (m_current >= 0) {
                fn_801E67EC()->Stop();
            }
            if (m_stage >= 0) {
                fn_801E67EC()->Load(m_stage);
            }
            m_thread->m_stage = m_stage;
        }
        m_current = m_stage;
    }
}

CNowLoadingThread::CNowLoadingThread(RwRaster *raster, int stage)
    : Task(fn_801EB304(fn_8001C4F8(), 0)) {
    m_stage = stage;
    m_stopped = 0;
    m_name = "NowLoadingThread";
    m_flags |= 0x100;
    m_camera = fn_80485A58();
    fn_8048D7C0(m_camera, fn_80486D00());
    m_camera->frameBuffer = raster;
    m_lastTime = fn_80051328();
    if (stage >= 0) {
        fn_801E67EC()->Load(m_stage);
    }
    field_0x30 = 1;
}

CNowLoadingThread::~CNowLoadingThread() {
    if (NowLoading::s_instance != 0) {
        NowLoading::s_instance->m_thread = 0;
    }
    if (m_stage >= 0) {
        if (m_stopped == 0) {
            fn_801E67EC()->Show();
        } else {
            fn_801E67EC()->Stop();
        }
    }
    if (m_camera != 0) {
        RwFrame *frame = m_camera->parent;
        if (frame != 0) {
            fn_8048D7C0(m_camera, 0);
            fn_80486E2C(frame);
        }
        m_camera->frameBuffer = 0;
        fn_804859F8(m_camera);
        m_camera = 0;
    }
}

int CNowLoadingThread::Run() {
    u64 now = fn_80051328();
    if (now - m_lastTime >= 16683) {
        fn_804857BC(m_camera, &s_clearColor, 1);
        if (fn_804854DC(m_camera) != 0) {
            void *vertexAlpha;
            void *zTest;
            void *zWrite;
            void *cullMode;
            lbl_805F15B4->fpRenderStateGet(rwRENDERSTATEVERTEXALPHAENABLE, &vertexAlpha);
            lbl_805F15B4->fpRenderStateGet(rwRENDERSTATEZTESTENABLE, &zTest);
            lbl_805F15B4->fpRenderStateGet(rwRENDERSTATEZWRITEENABLE, &zWrite);
            lbl_805F15B4->fpRenderStateGet(rwRENDERSTATECULLMODE, &cullMode);
            lbl_805F15B4->fpRenderStateSet(rwRENDERSTATEVERTEXALPHAENABLE, (void *)0);
            lbl_805F15B4->fpRenderStateSet(rwRENDERSTATEZTESTENABLE, (void *)0);
            lbl_805F15B4->fpRenderStateSet(rwRENDERSTATEZWRITEENABLE, (void *)0);
            lbl_805F15B4->fpRenderStateSet(rwRENDERSTATECULLMODE, (void *)rwCULLMODECULLNONE);
            lbl_805F15B4->fpRenderStateSet(rwRENDERSTATETEXTURERASTER, (void *)0);
            if (m_stage < 0) {
                fn_801E265C()->Update();
            } else {
                fn_801E67EC()->Update();
            }
            lbl_805F15B4->fpRenderStateSet(rwRENDERSTATEVERTEXALPHAENABLE, vertexAlpha);
            lbl_805F15B4->fpRenderStateSet(rwRENDERSTATEZTESTENABLE, zTest);
            lbl_805F15B4->fpRenderStateSet(rwRENDERSTATEZWRITEENABLE, zWrite);
            lbl_805F15B4->fpRenderStateSet(rwRENDERSTATECULLMODE, cullMode);
            fn_804854B4(m_camera);
            fn_80485808(m_camera, 0, 0);
            m_lastTime = now;
        }
    }
    return 1;
}

void CNowLoadingThread::Tick(float dt) {
    Run();
}

NowLoadingScreen::NowLoadingScreen() {
    m_time = 0.0f;
}

/* NonMatching (96.9%): see the note at the top of the file. */
bool NowLoadingScreen::Load() {
    char path[0x20];
    sprintf(path, "%s%s", s_projectName, lbl_8051D528[lbl_8057D6A0.m_language]);
    m_loaded = fn_8036E114(&m_project, fn_800423B8(fn_8003F8D4(), path, 0)) != 0;
    if (m_loaded) {
        fn_8036D598(&m_project, 0);
    }
    return m_loaded;
}

bool NowLoadingScreen::Update() {
    if (m_loaded) {
        float time = 1e-6f * fn_80051A94();
        fn_8036DA1C(&m_project, 1.0f / lbl_8057D6A0.m_frameRate);
        fn_8036D94C(&m_project);
        m_time = time;
    }
    return m_loaded;
}
