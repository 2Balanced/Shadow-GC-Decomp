/* SonicteamUSA::System::Fade::FadeTask and FadeDrawTask (class names from RTTI): the screen fader
 * (UnkFader_80044254, Game/ScreenFade.h) and the two tasks that update and draw it every frame. The
 * fader covers the screen with ten strips of vertices whose alpha is its level; while it is fully
 * dark it can also show the logo of FadeLogo.txd. Other units call the fader's functions by their
 * fn_ names. Names marked "(name inferred)" are not original. Definitions are in reverse address
 * order (deferred inlining emits functions last-defined first).
 *
 * NonMatching: the fader's constructor (96%: float and saved-register numbering, one extra move),
 * and GC/2.7 emits FadeTask's RTTI name and "Task" ahead of the string literals, where retail has
 * them after (every section matches symbol by symbol, not in order). The rest matches. */
#include "Game/CsdRW.h"
#include "Game/ScreenFade.h"
#include "Game/Task.h"
#include <dolphin/types.h>
#include "size_assert.h"

using Chao::CSD::RwMemory;
using Chao::CSD::RwStream;
using Chao::CSD::RwTexDictionary;
using Chao::CSD::RwTexture;

struct UnkFile_8047A178 {
    /* 0x0 */ u32 field_0x0;
    /* 0x4 */ u32 m_size;
};

extern "C" {
/* The file system's path of a file, and its release (behaviours inferred). */
char *fn_80051810(const char *name);
void fn_80051830(char *path);
/* RenderWare's file functions (fopen, fread, fclose) and its file system state (behaviours
 * inferred). */
void *fn_8047A170(void);
void *fn_804799C0(void);
void fn_804799B8(void *a, void *b);
UnkFile_8047A178 *fn_8047A178(const char *path, const char *mode);
u32 fn_8047A3E0(void *buffer, u32 size, u32 count, UnkFile_8047A178 *file);
s32 fn_8047A388(UnkFile_8047A178 *file);
/* RwTexDictionaryFindNamedTexture, RwTextureSetRaster and RwRasterDestroy (names from their use). */
RwTexture *fn_8048CFFC(RwTexDictionary *dict, const char *name);
RwTexture *fn_8048CA00(RwTexture *texture, Chao::CSD::RwRaster *raster);
s32 fn_80489BDC(RwRaster *raster);
/* Whether the game is paused (behaviour inferred). */
s32 fn_802BEB68(void);

void fn_800446D0(UnkFader_80044254 *fader, int withLogo, int keep, float target);
void fn_800447B8(UnkFader_80044254 *fader, int keep, float target);
void fn_800448A0(UnkFader_80044254 *fader);
void fn_80044AD8(UnkFader_80044254 *fader, float dt);
}

namespace SonicteamUSA {
namespace System {
namespace Fade {

class FadeTask : public Task {
  public:
    FadeTask(Task *parent);
    virtual ~FadeTask();
    virtual void Tick(float dt);
};
SIZE_ASSERT(FadeTask, 0x28);

class FadeDrawTask : public Task {
  public:
    FadeDrawTask(Task *parent);
    virtual ~FadeDrawTask();
    virtual void Tick(float dt);
};
SIZE_ASSERT(FadeDrawTask, 0x28);

} // namespace Fade
} // namespace System
} // namespace SonicteamUSA

using namespace SonicteamUSA::System::Fade;

/* The logo's corners on screen and in its texture (names inferred). */
static const float s_logoPositions[4][2] = {{456.0f, 274.0f}, {565.0f, 274.0f}, {456.0f, 409.0f}, {565.0f, 409.0f}};
static const float s_logoUVs[4][2] = {{0.0f, 0.0f}, {1.0f, 0.0f}, {0.0f, 1.0f}, {1.0f, 1.0f}};

/* Gives every strip vertex the fader's colour and level (name inferred). */
inline void ApplyColor(UnkFader_80044254 *fader) {
    int alpha = (int)(255.0f * fader->m_level);
    for (int i = 0; i < 40; i++) {
        fader->m_vertices[i].emissiveColor.red = fader->m_color.red;
        fader->m_vertices[i].emissiveColor.green = fader->m_color.green;
        fader->m_vertices[i].emissiveColor.blue = fader->m_color.blue;
        fader->m_vertices[i].emissiveColor.alpha = alpha;
    }
}

extern "C" int fn_80044E10(void) {
    return fn_80044254()->m_state;
}

void FadeDrawTask::Tick(float dt) {
    if (fn_80044E10() != 0) {
        fn_800448A0(fn_80044254());
    }
}

FadeDrawTask::~FadeDrawTask() {}

void FadeTask::Tick(float dt) {
    fn_80044AD8(fn_80044254(), dt);
}

FadeTask::~FadeTask() {}

/* Advances the fade and the logo by dt (name inferred). */
extern "C" void fn_80044AD8(UnkFader_80044254 *fader, float dt) {
    float timer = fader->m_logoTimer;
    fader->m_logoTimer = 0.0f;
    if (fader->m_state >= 2) {
        float step = dt * fader->m_rate;
        if (fader->m_state == 2) {
            fader->m_level -= step;
            if (fader->m_level <= fader->m_target) {
                fader->m_level = fader->m_target;
                if (fader->m_level <= 0.0f) {
                    fader->m_state = 0;
                } else {
                    fader->m_state = 1;
                }
            }
        } else {
            fader->m_level += step;
            if (fader->m_level >= fader->m_target) {
                fader->m_level = fader->m_target;
                fader->m_state = 1;
            }
            if (fader->m_withLogo != 0 && (1.0f - fader->m_level) * fader->m_time <= 0.2f) {
                fader->m_logoTimer = 0.4f;
            }
        }
        ApplyColor(fader);
    } else if (fader->m_state == 1) {
        if ((fader->m_level >= 1.0f || fader->m_withLogo != 0) && fn_802BEB68() == 0) {
            fader->m_logoTimer += timer + dt;
        }
    }
    if (fader->m_logoTimer >= 0.4f) {
        if (fader->m_logoAlpha < 1.0f) {
            fader->m_logoAlpha += 5.0f * dt;
            if (fader->m_logoAlpha > 1.0f) {
                fader->m_logoAlpha = 1.0f;
            }
        }
    } else if (fader->m_logoAlpha > 0.0f) {
        fader->m_logoAlpha -= 5.0f * dt;
        if (fader->m_logoAlpha < 0.0f) {
            fader->m_logoAlpha = 0.0f;
        }
    }
}

/* Draws the strips and, over a black screen, the logo (name inferred). */
extern "C" void fn_800448A0(UnkFader_80044254 *fader) {
    int k = 0;
    s32 alphaEnable;
    s32 zTest;
    s32 zWrite;
    s32 cullMode;
    lbl_805F15B4->fpRenderStateGet(Chao::CSD::rwRENDERSTATEVERTEXALPHAENABLE, &alphaEnable);
    lbl_805F15B4->fpRenderStateGet(Chao::CSD::rwRENDERSTATEZTESTENABLE, &zTest);
    lbl_805F15B4->fpRenderStateGet(Chao::CSD::rwRENDERSTATEZWRITEENABLE, &zWrite);
    lbl_805F15B4->fpRenderStateGet(Chao::CSD::rwRENDERSTATECULLMODE, &cullMode);
    lbl_805F15B4->fpRenderStateSet(Chao::CSD::rwRENDERSTATEVERTEXALPHAENABLE, (void *)1);
    lbl_805F15B4->fpRenderStateSet(Chao::CSD::rwRENDERSTATEZTESTENABLE, (void *)0);
    lbl_805F15B4->fpRenderStateSet(Chao::CSD::rwRENDERSTATEZWRITEENABLE, (void *)0);
    lbl_805F15B4->fpRenderStateSet(Chao::CSD::rwRENDERSTATECULLMODE, (void *)1);
    lbl_805F15B4->fpRenderStateSet(Chao::CSD::rwRENDERSTATETEXTURERASTER, (void *)0);
    for (int i = 0; i < 10; i++) {
        lbl_805F15B4->fpIm2DRenderPrimitive(Chao::CSD::rwPRIMTYPETRISTRIP,
                                            (Chao::CSD::RwIm2DVertex *)&fader->m_vertices[k], 4);
        k += 4;
    }
    if (fader->m_logoAlpha > 0.0f && fader->m_color.red == 0 && fader->m_color.green == 0 &&
        fader->m_color.blue == 0) {
        u32 alpha = 255.0f * fader->m_logoAlpha;
        for (int i = 0; i < 4; i++) {
            fader->m_logo[i].emissiveColor.red = 0xFF;
            fader->m_logo[i].emissiveColor.green = 0xFF;
            fader->m_logo[i].emissiveColor.blue = 0xFF;
            fader->m_logo[i].emissiveColor.alpha = alpha;
        }
        lbl_805F15B4->fpRenderStateSet(Chao::CSD::rwRENDERSTATETEXTURERASTER, fader->m_logoRaster);
        lbl_805F15B4->fpIm2DRenderPrimitive(Chao::CSD::rwPRIMTYPETRISTRIP,
                                            (Chao::CSD::RwIm2DVertex *)fader->m_logo, 4);
    }
    lbl_805F15B4->fpRenderStateSet(Chao::CSD::rwRENDERSTATEVERTEXALPHAENABLE, (void *)alphaEnable);
    lbl_805F15B4->fpRenderStateSet(Chao::CSD::rwRENDERSTATEZTESTENABLE, (void *)zTest);
    lbl_805F15B4->fpRenderStateSet(Chao::CSD::rwRENDERSTATEZWRITEENABLE, (void *)zWrite);
    lbl_805F15B4->fpRenderStateSet(Chao::CSD::rwRENDERSTATECULLMODE, (void *)cullMode);
}

/* Starts fading out to target; keep leaves a fade already out alone (names inferred). */
extern "C" void fn_800447B8(UnkFader_80044254 *fader, int keep, float target) {
    fader->m_withLogo = 0;
    if (keep != 0) {
        if (fader->m_state != 0 && fader->m_state != 2) {
            fader->m_state = 2;
            fader->m_target = target;
            ApplyColor(fader);
        }
    } else {
        fader->m_state = 2;
        fader->m_level = 1.0f;
        fader->m_target = target;
        ApplyColor(fader);
    }
}

/* Starts fading in to target, with the logo if withLogo is set; keep leaves a fade already in alone
 * (names inferred). */
extern "C" void fn_800446D0(UnkFader_80044254 *fader, int withLogo, int keep, float target) {
    fader->m_withLogo = withLogo;
    if (keep != 0) {
        if (fader->m_state != 1 && fader->m_state != 3) {
            fader->m_state = 3;
            fader->m_target = target;
            ApplyColor(fader);
        }
    } else {
        fader->m_state = 3;
        fader->m_level = 0.0f;
        fader->m_target = target;
        ApplyColor(fader);
    }
}

extern "C" void fn_80044648(UnkFader_80044254 *fader, float level) {
    fader->m_level = level;
    if (0.0f == level) {
        fader->m_state = 0;
    } else if (1.0f == level) {
        fader->m_state = 1;
    }
    ApplyColor(fader);
}

extern "C" void fn_800445D0(UnkFader_80044254 *fader, const RwRGBA *color) {
    fader->m_color = *color;
    ApplyColor(fader);
}

/* The colour a fader starts with (name inferred; copied as a word). */
static const RwRGBA s_black = {0, 0, 0, 0};

UnkFader_80044254::UnkFader_80044254() {
    m_level = 0.0f;
    m_time = 1.0f;
    m_rate = 1.0f;
    *(u32 *)&m_color = *(const u32 *)&s_black;
    m_state = 0;
    m_logoTimer = 0.0f;
    m_logoAlpha = 0.0f;
    m_logoRaster = 0;
    m_withLogo = 0;
    int k = 0;
    int alpha = (int)(255.0f * m_level);
    float x = 0.0f;
    float y = 0.0f;
    for (int i = 0; i < 10; i++) {
        for (int j = 0; j < 2; j++) {
            m_vertices[k].x = x;
            m_vertices[k].y = y;
            m_vertices[k].z = 0.1f;
            m_vertices[k].emissiveColor.red = m_color.red;
            m_vertices[k].emissiveColor.green = m_color.green;
            m_vertices[k].emissiveColor.blue = m_color.blue;
            m_vertices[k].emissiveColor.alpha = alpha;
            k++;
            m_vertices[k].x = x + 64.0f;
            m_vertices[k].y = y;
            m_vertices[k].z = 0.1f;
            m_vertices[k].emissiveColor.red = m_color.red;
            m_vertices[k].emissiveColor.green = m_color.green;
            m_vertices[k].emissiveColor.blue = m_color.blue;
            m_vertices[k].emissiveColor.alpha = alpha;
            y += 480.0f;
            k++;
        }
        x += 64.0f;
        y = 0.0f;
    }
    float scale = 1.0f;
    for (int i = 0; i < 4; i++) {
        m_logo[i].x = s_logoPositions[i][0];
        m_logo[i].y = scale * s_logoPositions[i][1];
        m_logo[i].z = 0.1f;
        m_logo[i].u = s_logoUVs[i][0];
        m_logo[i].v = s_logoUVs[i][1];
        m_logo[i].emissiveColor.red = 0xFF;
        m_logo[i].emissiveColor.green = 0xFF;
        m_logo[i].emissiveColor.blue = 0xFF;
        m_logo[i].emissiveColor.alpha = 0;
    }
    void *state = fn_8047A170();
    void *previous = fn_804799C0();
    fn_804799B8(state, 0);
    char *path = fn_80051810("FadeLogo.txd");
    UnkFile_8047A178 *file = fn_8047A178(path, "r");
    if (file != 0) {
        u32 size = file->m_size;
        void *buffer = lbl_805F15B4->fpMalloc(size, 0);
        RwMemory memory;
        memory.length = size;
        memory.start = buffer;
        fn_8047A3E0(buffer, size, 1, file);
        fn_8047A388(file);
        RwStream *stream = fn_80481368(Chao::CSD::rwSTREAMMEMORY, Chao::CSD::rwSTREAMREAD, &memory);
        if (fn_8047CF50(stream, Chao::CSD::rwID_TEXDICTIONARY, 0, 0) != 0) {
            RwTexDictionary *dict = fn_80483FBC(stream);
            if (dict != 0) {
                RwTexture *texture = fn_8048CFFC(dict, "logo");
                if (texture != 0) {
                    m_logoRaster = (RwRaster *)texture->raster;
                    fn_8048CA00(texture, 0);
                }
                fn_8048CB38(dict);
            }
            fn_8048124C(stream, &memory);
        }
        lbl_805F15B4->fpFree(buffer);
    }
    fn_80051830(path);
    fn_804799B8(state, previous);
}

UnkFader_80044254::~UnkFader_80044254() {
    if (m_logoRaster != 0) {
        fn_80489BDC(m_logoRaster);
        m_logoRaster = 0;
    }
}

/* The game's fader (name inferred). */
static UnkFader_80044254 *s_fader;

inline UnkFader_80044254 &FaderInstance() {
    static UnkFader_80044254 s_instance;
    return s_instance;
}

extern "C" UnkFader_80044254 *fn_80044254(void) {
    if (s_fader == 0) {
        s_fader = &FaderInstance();
    }
    return s_fader;
}

FadeTask::FadeTask(Task *parent) : Task(parent) {
    m_name = "FadeTask";
    m_flags |= 0x100;
}

FadeDrawTask::FadeDrawTask(Task *parent) : Task(parent) {
    m_name = "FadeDrawTask";
    m_flags |= 0x100;
}

/* Creates the fader and its tasks (name inferred). */
extern "C" void fn_8004414C(Task *updateParent, Task *drawParent) {
    fn_80044254();
    new FadeTask(updateParent);
    new FadeDrawTask(drawParent);
}

/* Does nothing (name inferred; the game system calls it when it stops). */
extern "C" void fn_80044148(void) {}
