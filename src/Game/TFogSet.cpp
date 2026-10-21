/* TFogSet (class name from RTTI): the task that fades the camera's fog towards the fog areas a
 * player's active blocks are in. The stage's fog table (stgNNNN/stgNNNN.fog: 24 areas, each with a
 * block mask, a fog start and end, a type and, for type 1, a density and a colour; see
 * Game/FogTable.h) is loaded once into a static FogTable. Each frame the task averages the areas whose blocks are on and moves the
 * camera's fog 2% of the way there. File name from the class; all other names are inferred.
 * Definitions are in reverse address order (deferred inlining emits functions last-defined
 * first). */
#include "Game/FogTable.h"
#include "Game/Task.h"
#include "size_assert.h"
#include <cstring>

/* A colour as the render state takes it. */
struct UnkColor_8016A520 {
    u8 red;
    u8 green;
    u8 blue;
    u8 alpha;
};

/* The fog colour the game state keeps, blue first (field names inferred from the order it is
 * written in). */
struct UnkFogColor_8057D6A0 {
    u8 blue;
    u8 green;
    u8 red;
    u8 alpha;
};

/* The game state (struct name unknown; see Game/CityBombSmall.cpp). */
struct UnkGameState_8057D6A0 {
    /* 0x000 */ UnkFogColor_8057D6A0 m_fogColor; /* (name inferred) */
    /* 0x004 */ u8 field_0x4[0xA8 - 0x4];
    /* 0x0A8 */ s32 m_stageFile; /* numbers the stage's files (see Game/GadgetStageData.cpp) */
    /* 0x0AC */ u8 field_0xAC[0x150 - 0xAC];
};
SIZE_ASSERT(UnkGameState_8057D6A0, 0x150);

extern UnkGameState_8057D6A0 lbl_8057D6A0;

/* The camera of the screen (class unknown): the fog start, end and density (names inferred from
 * how this task uses them). */
struct UnkCamera_8000DCC0 {
    /* 0x00 */ u8 field_0x0[0x80];
    /* 0x80 */ float m_fogStart;
    /* 0x84 */ float m_fogEnd;
    /* 0x88 */ float m_fogDensity;
};

/* What fn_8000DCC0 returns (see Game/ScreenViewport.cpp), reduced to the camera. */
struct UnkScreenHolder_8000DCC0 {
    /* 0x0 */ UnkCamera_8000DCC0 *m_camera;
};

/* What fn_8003A840 returns (class unknown): the flag at 0x1809 turns the stage's fog on. */
struct UnkStage_8003A840 {
    /* 0x0000 */ u8 field_0x0[0x1809];
    /* 0x1809 */ u8 m_fogOn;
};

/* The engine instance's render state functions (at 0x20 and 0x24 of the instance). */
struct UnkRwGlobals_805F15B4 {
    /* 0x00 */ u8 field_0x0[0x20];
    /* 0x20 */ int (*m_renderStateSet)(int state, u32 value);
    /* 0x24 */ int (*m_renderStateGet)(int state, void *value);
};

extern UnkRwGlobals_805F15B4 *lbl_805F15B4;

/* A player's set of switchable blocks (class name unknown; see Game/BlockChange.cpp). */
class UnkBlockSet_8003AAB0;

/* Each player's block set, by player index (see Game/BlockChange.cpp). */
extern UnkBlockSet_8003AAB0 *lbl_8051CE88[4];

extern "C" {
UnkScreenHolder_8000DCC0 *fn_8000DCC0(void);
UnkStage_8003A840 *fn_8003A840(void);
/* Sets the camera's fog start and end (behaviour inferred). */
void fn_80485550(UnkCamera_8000DCC0 *camera, float start);
void fn_8048560C(UnkCamera_8000DCC0 *camera, float end);
/* Whether block id of the block set is in state (behaviour inferred). */
bool fn_8003AA30(UnkBlockSet_8003AAB0 *blocks, int id, int state);
}

class TFogSet : public Task {
  public:
    TFogSet(Task *parent, int player);
    virtual ~TFogSet();
    virtual void Tick(float dt);

    void GetActiveAreas(u8 *active);
    void TurnFogOff();

    static const char *s_taskName;

    /* 0x28 */ s32 m_player;
    /* 0x2C */ u8 m_first;
};
SIZE_ASSERT(TFogSet, 0x30);

const char *TFogSet::s_taskName = "TFogSet";

TFogSet::TFogSet(Task *parent, int player) : Task(parent), m_first(true) {
    m_name = s_taskName;
    FogTable::Get()->Load();
    m_player = player;
    fn_80485550(fn_8000DCC0()->m_camera, 1.0f);
    fn_8048560C(fn_8000DCC0()->m_camera, 30000.0f);
    UnkStage_8003A840 *stage = fn_8003A840();
    stage->m_fogOn = 0;
}

TFogSet::~TFogSet() {
    FogTable::Get()->Release();
    TurnFogOff();
}

void TFogSet::Tick(float dt) {
    if (FogTable::Get()->m_state == FogTable::STATE_LOADED && FogTable::Get()->m_disabled == 0) {
        u8 active[kFogAreaCount];
        GetActiveAreas(active);
        int colored = 0;
        int count = 0;
        float start = fn_8000DCC0()->m_camera->m_fogStart;
        float end = fn_8000DCC0()->m_camera->m_fogEnd;
        float density = fn_8000DCC0()->m_camera->m_fogDensity;
        UnkColor_8016A520 color;
        lbl_805F15B4->m_renderStateGet(15, &color);
        if (m_first) {
            start = 1.0f;
            m_first = false;
            density = start;
            end = 30000.0f;
        }
        float sumStart = start;
        float sumEnd = end;
        float sumDensity = density;
        int red = color.red;
        int green = color.green;
        int blue = color.blue;
        for (int i = 0; i < kFogAreaCount; i++) {
            if (active[i]) {
                FogArea *area = &FogTable::Get()->m_areas[i];
                sumStart += area->m_start;
                sumEnd += area->m_end;
                switch (area->m_type) {
                case 0:
                    break;
                case 1:
                    colored++;
                    sumDensity += area->m_density;
                    red += area->m_red;
                    green += area->m_green;
                    blue += area->m_blue;
                    break;
                }
                count++;
            }
        }
        if (count != 0) {
            sumStart /= count + 1;
            sumEnd /= count + 1;
            start = 0.98f * start + 0.02f * sumStart;
            end = 0.98f * end + 0.02f * sumEnd;
            if (start < 1.0f) {
                start = 1.0f;
            }
            fn_80485550(fn_8000DCC0()->m_camera, start);
            fn_8048560C(fn_8000DCC0()->m_camera, end);
        }
        if (colored != 0) {
            int n = colored + 1;
            red /= n;
            green /= n;
            blue /= n;
            sumDensity /= n;
            density = 0.98f * density + 0.02f * sumDensity;
            UnkColor_8016A520 fog;
            fog.red = 0.98f * color.red + 0.02f * red;
            fog.green = 0.98f * color.green + 0.02f * green;
            fog.blue = 0.98f * color.blue + 0.02f * blue;
            fog.alpha = 0;
            fn_8000DCC0()->m_camera->m_fogDensity = density;
            fn_8000DCC0();
            lbl_805F15B4->m_renderStateSet(15, *(u32 *)&fog);
            lbl_805F15B4->m_renderStateSet(16, 1);
            UnkStage_8003A840 *stage = fn_8003A840();
            stage->m_fogOn = 1;
            UnkFogColor_8057D6A0 state;
            state.blue = fog.blue;
            state.green = fog.green;
            state.red = fog.red;
            lbl_8057D6A0.m_fogColor = state;
        } else {
            UnkStage_8003A840 *stage = fn_8003A840();
            stage->m_fogOn = 0;
        }
    }
}

FogTable *FogTable::Get() {
    if (s_instance == 0) {
        static FogTable s_table;
        s_instance = &s_table;
    }
    return s_instance;
}

void TFogSet::GetActiveAreas(u8 *active) {
    UnkBlockSet_8003AAB0 *blocks = lbl_8051CE88[m_player];
    if (blocks != 0) {
        u32 on[4];
        memset(on, 0, sizeof(on));
        for (int i = 0; i < 0xA8; i++) {
            if (fn_8003AA30(blocks, i, 1)) {
                on[i / 32] |= 1 << (i % 32);
            }
        }
        memset(active, 0, kFogAreaCount);
        int k;
        for (int j = 0; j < kFogAreaCount; j++) {
            for (k = 0; k < 4; k++) {
                if (on[k] & FogTable::Get()->m_areas[j].m_blocks[k]) {
                    active[j] = true;
                    break;
                }
            }
        }
    }
}

void TFogSet::TurnFogOff() {
    UnkStage_8003A840 *stage = fn_8003A840();
    stage->m_fogOn = 0;
    UnkFogColor_8057D6A0 state;
    state.blue = 0;
    state.green = 0;
    state.red = 0;
    lbl_8057D6A0.m_fogColor = state;
}
