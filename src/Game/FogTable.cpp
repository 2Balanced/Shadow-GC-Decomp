/* The stage's fog table (see Game/FogTable.h): loading stgNNNN/stgNNNN.fog through the file system
 * and swapping it to the game's byte order. The file and all names are inferred; this is its own
 * unit, as its float constant 1.0 is pooled apart from TFogSet's and s_instance sits in .sbss apart
 * from the guard of Get's static table. Definitions are in reverse address
 * order (deferred inlining emits functions last-defined first). */
#include "Game/FogTable.h"
#include <cstdio>

/* The game state (struct name unknown; see Game/CityBombSmall.cpp). */
struct UnkGameState_8057D6A0 {
    /* 0x000 */ u8 field_0x0[0xA8];
    /* 0x0A8 */ s32 m_stageFile; /* numbers the stage's files (see Game/GadgetStageData.cpp) */
    /* 0x0AC */ u8 field_0xAC[0x150 - 0xAC];
};
SIZE_ASSERT(UnkGameState_8057D6A0, 0x150);

extern UnkGameState_8057D6A0 lbl_8057D6A0;

class UnkFileSystem_800371D8;

extern "C" {
/* Byte-swaps a value of size bytes (behaviour inferred). */
void fn_80043D0C(void *value, int size);
/* The file system, a load request to it, and the release of loaded data (behaviour inferred). */
UnkFileSystem_800371D8 *fn_800371D8(void);
bool fn_80045044(UnkFileSystem_800371D8 *fileSystem, FogRequest *request, int arg2);
void fn_80049B34(void *data);
}

FogTable *FogTable::s_instance;

FogTable::FogTable() : m_state(STATE_NONE) {}

bool FogTable::Load() {
    if (m_state == STATE_LOADING) {
        return false;
    }
    m_disabled = 0;
    m_state = STATE_NONE;
    if (m_areas != 0) {
        fn_80049B34(m_areas);
        m_areas = 0;
    }
    MakeFileName();
    FogRequest request;
    request.m_path = m_fileName;
    request.m_data = 0;
    request.m_size = 0;
    request.m_user = this;
    request.m_callback = OnLoaded;
    if (fn_80045044(fn_800371D8(), &request, 1)) {
        m_state = STATE_LOADING;
    } else {
        m_state = STATE_NONE;
    }
    return m_state == STATE_LOADING;
}

void FogTable::Release() {
    m_state = STATE_NONE;
    if (m_areas != 0) {
        fn_80049B34(m_areas);
        m_areas = 0;
    }
}

void FogTable::MakeFileName() {
    sprintf(m_fileName, "stg%04d/stg%04d.fog", lbl_8057D6A0.m_stageFile, lbl_8057D6A0.m_stageFile);
}

void FogTable::OnLoaded(FogRequest *request) {
    FogTable *table = (FogTable *)request->m_user;
    table->m_state = STATE_LOADED;
    table->m_areas = (FogArea *)request->m_data;
    for (int i = 0; i < kFogAreaCount; i++) {
        fn_80043D0C(&table->m_areas[i].m_blocks, sizeof(table->m_areas[i].m_blocks));
        fn_80043D0C(&table->m_areas[i].m_start, sizeof(float));
        fn_80043D0C(&table->m_areas[i].m_end, sizeof(float));
        fn_80043D0C(&table->m_areas[i].m_type, sizeof(s32));
        fn_80043D0C(&table->m_areas[i].m_density, sizeof(float));
        fn_80043D0C(&table->m_areas[i].m_far, sizeof(float));
    }
    for (int i = 0; i < kFogAreaCount; i++) {
        if (table->m_areas[i].m_far < 1.0f) {
            table->m_areas[i].m_far = table->m_areas[i].m_end;
        }
    }
}
