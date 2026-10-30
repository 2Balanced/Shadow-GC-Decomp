/* TMovieManager (class name from RTTI): the task that plays a full-screen movie (the event movies,
 * the logos, the staff roll's window). The movie library's player (the fn_804xxxxx calls) decodes
 * each frame into one of the manager's rasters; fn_80036570 draws the current raster. The player's
 * state lives in a static block (s_player), the manager instance in lbl_805EE0B0 (other units
 * declare its start as UnkMovieStatus_805EE0B0). Names other than TMovieManager are inferred.
 * Definitions are in reverse address order (deferred inlining emits functions last-defined
 * first). */
#include "Game/Task.h"
#include "Game/TBGRead.h"
#include <dolphin/types.h>
#include "size_assert.h"
#include <string.h>

/* RenderWare (rw/rwcore.h names; the parts used here). */
struct RwRGBA {
    u8 red;
    u8 green;
    u8 blue;
    u8 alpha;
};

struct RwIm2DVertex {
    /* 0x00 */ float x;
    /* 0x04 */ float y;
    /* 0x08 */ float z;
    /* 0x0C */ RwRGBA emissiveColor;
    /* 0x10 */ float u;
    /* 0x14 */ float v;
};

struct RwRaster {
    /* 0x00 */ u8 field_0x0[0xC];
    /* 0x0C */ s32 width;
    /* 0x10 */ s32 height;
    /* 0x14 */ s32 depth;
    /* 0x18 */ s32 stride;
};

struct RwCamera {
    /* 0x00 */ u8 field_0x0[0x60];
    /* 0x60 */ RwRaster *frameBuffer;
};

struct RwGlobals {
    /* 0x00 */ RwCamera *curCamera;
    /* 0x04 */ u8 field_0x4[0x18 - 0x4];
    /* 0x18 */ float nearScreenZ; /* dOpenDevice.zBufferNear (meaning inferred) */
    /* 0x1C */ u8 field_0x1C[0x20 - 0x1C];
    /* 0x20 */ int (*fpRenderStateSet)(int state, void *value);
    /* 0x24 */ int (*fpRenderStateGet)(int state, void *value);
    /* 0x28 */ u8 field_0x28[0x30 - 0x28];
    /* 0x30 */ int (*fpIm2DRenderPrimitive)(int primType, RwIm2DVertex *vertices, int numVertices);
};

extern RwGlobals *lbl_805F15B4;

/* The screen holder (Game/CameraManager.cpp; the part used here): the raster of the screen. */
struct UnkScreenHolder_8000DCC0 {
    /* 0x00 */ u8 field_0x0[0x10];
    /* 0x10 */ RwRaster *m_raster;
};

/* The game state (as in Game/AdvLogo.cpp; the part used here). */
struct UnkGameState_8057D6A0 {
    /* 0x00 */ u8 field_0x0[0x78];
    /* 0x78 */ s32 field_0x78;
};

extern UnkGameState_8057D6A0 lbl_8057D6A0;

/* A decoded frame's description, as the player reports it (layout inferred). */
struct UnkMovieFrame_80402DEC {
    /* 0x00 */ u32 m_ready; /* nonzero when a new frame is ready */
    /* 0x04 */ u32 field_0x4;
    /* 0x08 */ u32 field_0x8;
    /* 0x0C */ s32 m_width;
    /* 0x10 */ s32 m_height;
    /* 0x14 */ u32 field_0x14;
    /* 0x18 */ u32 field_0x18;
    /* 0x1C */ u32 field_0x1C;
    /* 0x20 */ u32 field_0x20;
    /* 0x24 */ u32 field_0x24;
    /* 0x28 */ u32 field_0x28;
    /* 0x2C */ u32 field_0x2C;
    /* 0x30 */ u32 field_0x30;
    /* 0x34 */ u32 field_0x34;
    /* 0x38 */ u32 field_0x38;
    /* 0x3C */ u32 field_0x3C;
    /* 0x40 */ u32 field_0x40;
    /* 0x44 */ u32 field_0x44;
    /* 0x48 */ u32 field_0x48;
    /* 0x4C */ u32 field_0x4C;
    /* 0x50 */ u8 field_0x50[0x38];
    /* 0x88 */ u32 field_0x88;
    /* 0x8C */ u32 field_0x8C;
};
SIZE_ASSERT(UnkMovieFrame_80402DEC, 0x90);

/* The player's creation parameters (layout inferred). */
struct UnkMovieParams_80406634 {
    /* 0x00 */ u32 field_0x0;
    /* 0x04 */ u32 field_0x4;
    /* 0x08 */ u32 field_0x8;
    /* 0x0C */ u32 field_0xC;
    /* 0x10 */ u32 field_0x10;
    /* 0x14 */ u32 field_0x14;
    /* 0x18 */ void *m_work;
    /* 0x1C */ u32 m_workSize;
    /* 0x20 */ u32 field_0x20;
    /* 0x24 */ u8 field_0x24[0x30 - 0x24];
};
SIZE_ASSERT(UnkMovieParams_80406634, 0x30);

/* The library's set-up parameters (layout inferred). */
struct UnkMovieConfig_80406B90 {
    /* 0x00 */ float m_frameRate;
    /* 0x04 */ u32 field_0x4;
    /* 0x08 */ u32 field_0x8;
    /* 0x0C */ u32 field_0xC;
    /* 0x10 */ u8 field_0x10[0x20 - 0x10];
};
SIZE_ASSERT(UnkMovieConfig_80406B90, 0x20);

/* The player's state (struct name unknown). */
struct UnkMoviePlayer_80571188 {
    /* 0x000 */ void *m_handle;
    /* 0x004 */ UnkMovieConfig_80406B90 m_config;
    /* 0x024 */ UnkMovieParams_80406634 m_params;
    /* 0x054 */ s32 m_status;
    /* 0x058 */ UnkMovieFrame_80402DEC m_frame;
    /* 0x0E8 */ void *m_work;
    /* 0x0EC */ char m_fileName[0x20];
    /* 0x10C */ u32 field_0x10C;
    /* 0x110 */ u32 field_0x110;
    /* 0x114 */ void *field_0x114;
    /* 0x118 */ u8 field_0x118[0x120 - 0x118];
};
SIZE_ASSERT(UnkMoviePlayer_80571188, 0x120);

/* One movie type: the player's parameters and the number of rasters (meanings inferred). */
struct UnkMovieType_804AB138 {
    /* 0x00 */ u32 field_0x0;
    /* 0x04 */ u32 field_0x4;
    /* 0x08 */ u32 field_0x8;
    /* 0x0C */ u32 field_0xC;
    /* 0x10 */ u32 field_0x10;
    /* 0x14 */ u32 field_0x14;
    /* 0x18 */ u32 field_0x18;
    /* 0x1C */ s32 m_rasterCount;
};
SIZE_ASSERT(UnkMovieType_804AB138, 0x20);

typedef void (*MovieEndCallback)(void *arg);

extern "C" {
/* The movie library (behaviours inferred). */
void fn_80402DEC(void *handle, UnkMovieFrame_80402DEC *frame);
void fn_804029D0(void *handle);
void fn_804039C4(void *handle, s32 stride, s32 height);
void fn_80403EC4(void *handle);
void *fn_804042FC(UnkMovieParams_80406634 *params);
u32 fn_80406634(UnkMovieParams_80406634 *params);
void fn_80406A98(void);
void fn_80406B90(UnkMovieConfig_80406B90 *config);
void fn_80406F64(void *handle, s32 pause);
void fn_80407484(void *handle, const char *fileName);
s32 fn_804074A4(void *handle);
s32 fn_80407568(void *handle);
void fn_8040760C(void *handle, s32 volume);
s32 fn_80407688(void *handle);
void fn_80408680(void *handle, UnkMovieFrame_80402DEC *frame, void *pixels);

/* RenderWare rasters: create, lock, unlock, destroy. */
RwRaster *fn_80489E74(s32 width, s32 height, s32 depth, s32 flags);
void *fn_80489F70(RwRaster *raster, u8 level, s32 lockMode);
void fn_80489B40(RwRaster *raster);
void fn_80489BDC(RwRaster *raster);

/* The game's heap (allocate, free). */
void *fn_80049B90(u32 size, int heap);
void fn_80049B34(void *memory);

UnkScreenHolder_8000DCC0 *fn_8000DCC0(void);
int fn_80305BE8(void);
int fn_80342388(void);

bool fn_800361D8(void);
void fn_80036244(void);
void fn_8003636C(void);
float fn_80036464(float volume);
bool fn_80036570(int x, int y, float scaleX, float scaleY);
bool fn_80036E78(const char *fileName, int type, MovieEndCallback callback, void *arg);
void fn_8003723C(void);
bool fn_8003727C(Task *parent, int pal);
}

/* Never used (kept with the unit's .rodata). */
static const float lbl_804AB0F8[16] = {1.0f, 0.0f, 0.0f, 0.0f, 1.0f, 0.0f, 0.0f, 0.0f,
                                       1.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f};

static const UnkMovieType_804AB138 s_movieTypes[3] = {
    {0x11, 1, 3500000, 640, 480, 2, 2, 1},
    {0x11, 1, 3000000, 256, 256, 2, 1, 1},
    {0x11, 1, 2000000, 128, 128, 2, 1, 1},
};

class TMovieManager : public Task {
  public:
    TMovieManager(Task *parent, MovieEndCallback callback, void *arg);
    virtual ~TMovieManager();
    virtual void Tick(float dt);

    void UploadFrame(UnkMovieFrame_80402DEC *frame);

    static const char *s_taskName;

    /* 0x28 */ RwRaster *m_rasters[2];
    /* 0x30 */ RwIm2DVertex m_vertices[4];
    /* 0x90 */ bool m_endOnStop;
    /* 0x94 */ s32 m_raster;
    /* 0x98 */ MovieEndCallback m_callback;
    /* 0x9C */ void *m_callbackArg;
    /* 0xA0 */ UnkMovieFrame_80402DEC m_frame;
};
SIZE_ASSERT(TMovieManager, 0x130);

const char *TMovieManager::s_taskName = "TMovieManager";

static RwRaster *s_raster;
static Task *s_parent;
TMovieManager *lbl_805EE0B0;
static s32 s_movieType;
static u8 s_initialized;
static s32 s_pal;
static float s_volume;
static u8 s_keepWork;
static UnkMoviePlayer_80571188 s_player;

extern UnkFileSystem_800371D8 *lbl_805EDFC4;

inline TMovieManager::TMovieManager(Task *parent, MovieEndCallback callback, void *arg)
    : Task(parent), m_endOnStop(true) {
    m_name = s_taskName;
    lbl_805EE0B0 = this;
    s_raster = 0;
    memset(m_rasters, 0, sizeof(m_rasters));
    m_raster = 0;
    m_callback = callback;
    m_callbackArg = arg;
    memset(&m_frame, 0, sizeof(m_frame));
}

/* Resets the player for NTSC (pal 0) or PAL (pal 1). */
inline bool InitPlayer(Task *parent, int pal) {
    memset(&s_player, 0, sizeof(s_player));
    s_player.m_handle = 0;
    s_player.field_0x110 = 1;
    s_player.field_0x10C = 0;
    s_player.m_fileName[0] = 0;
    UnkMovieConfig_80406B90 *config = &s_player.m_config;
    memset(config, 0, sizeof(*config));
    switch (pal) {
    case 1:
        config->m_frameRate = 50.0f;
        break;
    default:
        config->m_frameRate = 59.94f;
        break;
    }
    s_pal = pal;
    config->field_0x4 = 1;
    config->field_0x8 = 1;
    config->field_0xC = 0;
    fn_80406B90(config);
    s_parent = parent;
    lbl_805EE0B0 = 0;
    s_player.field_0x114 = 0;
    s_initialized = 1;
    return true;
}

/* Sets the volume (0 silent, 1 full) and returns the previous one; -1 if nothing plays. */
inline float SetVolume(float volume) {
    s32 status = fn_80407688(s_player.m_handle);
    if (status != 3 && status != 4) {
        s32 previous = fn_80407568(s_player.m_handle);
        if (volume == 0.0f) {
            fn_8040760C(s_player.m_handle, -960);
        } else if (volume == 1.0f) {
            fn_8040760C(s_player.m_handle, 0);
        } else {
            fn_8040760C(s_player.m_handle, (s32)(960.0f * volume + -960.0f));
        }
        return (previous + 960) / 960.0f;
    }
    return -1.0f;
}

inline bool FileExists(UnkFileSystem_800371D8 *fileSystem, const char *fileName) {
    return fileSystem->m_exists(fileName);
}

extern "C" bool fn_8003727C(Task *parent, int pal) {
    return InitPlayer(parent, pal);
}

extern "C" void fn_8003723C(void) {
    if (lbl_805EE0B0 != 0) {
        lbl_805EE0B0->m_flags |= 1;
    }
    fn_80406A98();
    s_initialized = 0;
}

extern "C" UnkFileSystem_800371D8 *fn_800371D8(void) {
    if (lbl_805EDFC4 == 0) {
        static UnkFileSystem_800371D8 s_fileSystem;
        lbl_805EDFC4 = &s_fileSystem;
    }
    return lbl_805EDFC4;
}

extern "C" bool fn_80036E78(const char *fileName, int type, MovieEndCallback callback, void *arg) {
    if (!FileExists(fn_800371D8(), fileName)) {
        return false;
    }
    if (s_initialized == 1) {
        fn_8003723C();
        if (fn_80305BE8()) {
            InitPlayer(s_parent, 1);
        } else {
            InitPlayer(s_parent, 0);
        }
    }
    if (fn_80342388() == 0) {
        s_keepWork = 0;
    } else if (lbl_8057D6A0.field_0x78 == 2) {
        s_keepWork = 1;
    } else {
        s_keepWork = 0;
    }
    s_movieType = type;
    UnkMovieParams_80406634 *params = &s_player.m_params;
    memset(params, 0, sizeof(*params));
    params->field_0x20 = s_movieTypes[s_movieType].field_0x0;
    params->field_0x0 = s_movieTypes[s_movieType].field_0x4;
    params->field_0x4 = s_movieTypes[s_movieType].field_0x8;
    params->field_0x8 = s_movieTypes[s_movieType].field_0xC;
    params->field_0xC = s_movieTypes[s_movieType].field_0x10;
    params->field_0x10 = s_movieTypes[s_movieType].field_0x14;
    params->field_0x14 = s_movieTypes[s_movieType].field_0x18;
    params->m_workSize = fn_80406634(params);
    params->m_work = fn_80049B90(params->m_workSize, 1);
    if (params->m_work == 0) {
        return false;
    }
    s_player.m_work = params->m_work;
    void *handle = fn_804042FC(params);
    if (handle == 0) {
        fn_80049B34(s_player.m_work);
        return false;
    }
    s_player.m_handle = handle;
    strcpy(s_player.m_fileName, fileName);
    if (s_keepWork == 0) {
        s_player.field_0x114 = 0;
    }
    fn_80407484(handle, s_player.m_fileName);
    new TMovieManager(s_parent, callback, arg);
    return true;
}

TMovieManager::~TMovieManager() {
    if (m_callback != 0) {
        m_callback(m_callbackArg);
    }
    fn_80403EC4(s_player.m_handle);
    s_player.m_handle = 0;
    fn_80049B34(s_player.m_work);
    for (int i = 0; i < s_movieTypes[s_movieType].m_rasterCount; i++) {
        if (m_rasters[i] != 0) {
            fn_80489BDC(m_rasters[i]);
        }
    }
    s_raster = 0;
    if (s_player.field_0x114 != 0) {
        fn_80049B34(s_player.field_0x114);
        s_player.field_0x114 = 0;
    }
    lbl_805EE0B0 = 0;
}

void TMovieManager::Tick(float dt) {
    UnkMovieFrame_80402DEC frame;
    fn_80402DEC(s_player.m_handle, &frame);
    if (frame.m_ready != 0) {
        UploadFrame(&frame);
        s_player.m_frame = frame;
        fn_804029D0(s_player.m_handle);
        m_frame = frame;
    }
    s32 status = fn_80407688(s_player.m_handle);
    if ((status == 3 || status == 4) && m_endOnStop) {
        m_flags |= 1;
    } else {
        s_player.m_status = status;
    }
}

/* The smallest power of 2 not below n (0 for n <= 0). */
inline s32 RoundUpToPowerOf2(s32 n) {
    if (n <= 0) {
        return 0;
    }
    s32 power;
    for (s32 i = 0; n > (power = 1 << i); i++) {
    }
    return power;
}

void TMovieManager::UploadFrame(UnkMovieFrame_80402DEC *frame) {
    if (s_raster == 0) {
        s32 width = RoundUpToPowerOf2(frame->m_width);
        s32 height = RoundUpToPowerOf2(frame->m_height);
        for (int i = 0; i != s_movieTypes[s_movieType].m_rasterCount; i++) {
            m_rasters[i] = fn_80489E74(width, height, 32, 4);
            if (m_rasters[i] != 0) {
                void *pixels = fn_80489F70(m_rasters[i], 0, 9);
                if (pixels != 0) {
                    memset(pixels, 0, m_rasters[i]->stride * m_rasters[i]->height);
                    fn_80489B40(m_rasters[i]);
                }
            }
        }
        m_raster = 0;
        s_raster = m_rasters[m_raster];
        float u = (float)frame->m_width / (float)width;
        float v = (float)frame->m_height / (float)height;
        float uv[4][2] = {0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f};
        uv[1][0] = u;
        uv[2][1] = v;
        uv[3][0] = u;
        uv[3][1] = v;
        RwRaster *screen = fn_8000DCC0()->m_raster;
        float screenWidth = screen->width;
        float screenHeight = screen->height;
        float position[4][2] = {0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f};
        position[1][0] = screenWidth;
        position[2][1] = screenHeight;
        position[3][0] = screenWidth;
        position[3][1] = screenHeight;
        float z = lbl_805F15B4->nearScreenZ;
        for (int i = 0; i < 4; i++) {
            m_vertices[i].x = position[i][0];
            m_vertices[i].y = position[i][1];
            m_vertices[i].z = z;
            m_vertices[i].u = uv[i][0];
            m_vertices[i].v = uv[i][1];
            m_vertices[i].emissiveColor.red = 0xFF;
            m_vertices[i].emissiveColor.green = 0xFF;
            m_vertices[i].emissiveColor.blue = 0xFF;
            m_vertices[i].emissiveColor.alpha = 0xFF;
        }
        fn_804039C4(s_player.m_handle, s_raster->stride, s_raster->height);
    }
    if (s_raster != 0) {
        m_raster = (m_raster + 1) % s_movieTypes[s_movieType].m_rasterCount;
        s_raster = m_rasters[m_raster];
        void *pixels = fn_80489F70(s_raster, 0, 9);
        if (pixels != 0) {
            fn_80408680(s_player.m_handle, frame, pixels);
            fn_80489B40(s_raster);
        }
    }
}

/* Draws the movie at (x, y), scaled from the screen's size. */
extern "C" bool fn_80036570(int x, int y, float scaleX, float scaleY) {
    if (lbl_805EE0B0 != 0) {
        RwRaster *raster;
        RwRaster *frameBuffer = lbl_805F15B4->curCamera->frameBuffer;
        if ((raster = s_raster) != 0) {
            float width = frameBuffer->width;
            float height = frameBuffer->height;
            RwIm2DVertex vertices[4];
            memcpy(vertices, lbl_805EE0B0->m_vertices, sizeof(vertices));
            width *= scaleX;
            height *= scaleY;
            vertices[0].x = x;
            vertices[0].y = y;
            vertices[1].x = x + width;
            vertices[1].y = y;
            vertices[2].x = x;
            vertices[2].y = y + height;
            vertices[3].x = x + width;
            vertices[3].y = y + height;
            lbl_805F15B4->fpRenderStateSet(6, (void *)0);
            lbl_805F15B4->fpRenderStateSet(8, (void *)0);
            lbl_805F15B4->fpRenderStateSet(12, (void *)0);
            lbl_805F15B4->fpRenderStateSet(9, (void *)2);
            lbl_805F15B4->fpRenderStateSet(20, (void *)1);
            lbl_805F15B4->fpRenderStateSet(1, raster);
            lbl_805F15B4->fpIm2DRenderPrimitive(4, vertices, 4);
            return true;
        }
    }
    return false;
}

extern "C" float fn_80036464(float volume) {
    return SetVolume(volume);
}

/* Pauses the movie and silences it. */
extern "C" void fn_8003636C(void) {
    s32 status;
    if (lbl_805EE0B0 != 0 && (status = fn_80407688(s_player.m_handle)) != 3 && status != 4 &&
        fn_804074A4(s_player.m_handle) == 0) {
        fn_80406F64(s_player.m_handle, 1);
        s_volume = SetVolume(0.0f);
    }
}

/* Resumes the movie at the volume it had. */
extern "C" void fn_80036244(void) {
    s32 status;
    if (lbl_805EE0B0 != 0 && (status = fn_80407688(s_player.m_handle)) != 3 && status != 4 &&
        fn_804074A4(s_player.m_handle) != 0) {
        fn_80406F64(s_player.m_handle, 0);
        SetVolume(s_volume);
    }
}

/* True while the movie is paused. */
extern "C" bool fn_800361D8(void) {
    s32 status;
    if (lbl_805EE0B0 != 0 && (status = fn_80407688(s_player.m_handle)) != 3 && status != 4 &&
        fn_804074A4(s_player.m_handle) != 0) {
        return true;
    }
    return false;
}
