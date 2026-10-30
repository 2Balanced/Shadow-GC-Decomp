/* TBlockManage (class name from RTTI): a player's stage blocks. The stage's block file is a list of
 * boxes, each naming the block (one of 0xA8) it belongs to. Each frame the task works out which
 * blocks the watched position (or the camera) is in, keeps two buffers of the wanted blocks, and
 * has the stage (fn_8003A840) load the wanted blocks' land and free the others. Gadgets ask for a
 * TBlockManage::forBlockCheck (the blocks a point lies in, class name from RTTI) to wait until
 * their blocks are loaded. HandleUser's constructor is defined here too. Names other than the
 * classes' are inferred. The functions other units call keep their address names. Definitions are
 * in reverse address order (deferred inlining emits functions last-defined first).
 *
 * NonMatching (38/41; every section and the layout match, and with the unit linked only these
 * three functions differ): HandleUser's constructor, where GC/2.7 loads both halfword constants
 * before storing the vtable pointer (the scheduling difference Game/LandManager.cpp documents for
 * its layer constructor); UpdateLoads (two pairs of callee-saved registers swapped) and fn_8003A380
 * (three registers rotated), register allocation only. About 15 source shapes were tried for each. */
#define TBLOCKMANAGE_DEFINE_CLASS
#include "Game/TBlockManage.h"
#define UNKHANDLE_DEFAULT_CTOR
#define UNKHANDLE_DTOR
#include "Game/UnkHandleTable.h"
#include "Game/HandleUser.h"
#include "Game/MathTypes.h"
#include "Game/Task.h"
#include "boost/shared_ptr.hpp"
#include "boost/weak_ptr.hpp"
#include <dolphin/types.h>
#include "size_assert.h"
#include <stdio.h>
#include <string.h>

/* One box of the block file: the block it belongs to and its bounds. */
struct UnkBlockBox_8003AB68 {
    /* 0x00 */ s32 m_id;
    /* 0x04 */ Vec3f m_min;
    /* 0x10 */ Vec3f m_max;
    /* 0x1C */ s32 field_0x1C;
};
SIZE_ASSERT(UnkBlockBox_8003AB68, 0x20);

/* The block file (big-endian on disc; swapped in place when loaded). */
struct UnkBlockData_8003AB68 {
    /* 0x00 */ char m_name[0x40]; /* the stage's name: its land files are named after it */
    /* 0x40 */ s32 m_count;
    /* 0x44 */ UnkBlockBox_8003AB68 m_boxes[1];
};

/* The blocks a point lies in. */
class TBlockManage::forBlockCheck {
  public:
    /* 0x00 */ s32 m_count;
    /* 0x04 */ s16 m_ids[0x18];
};
SIZE_ASSERT(TBlockManage::forBlockCheck, 0x34);

/* A stage block's land (see Game/LandManager.cpp). */
struct UnkLandBlock_8003A840 {
    /* 0x00 */ s32 m_state; /* 5: loaded */
};

/* The stage (class name unknown; Game/LandManager.cpp). */
class UnkStage_8003A840 {
  public:
    UnkStage_8003A840();
    ~UnkStage_8003A840();

    u8 field_0x0[0x1818];
};

SIZE_ASSERT(TBlockManage, 0x980);

extern "C" {
/* Game heap: allocate and free. */
void *fn_80049B90(u32 size, int heap);
void fn_80049B34(void *block);
/* Swaps the byte order of a value of size bytes. */
void fn_80043D0C(void *value, int size);
/* The camera manager and a camera's position. */
void *fn_80009548(void);
const Vec3f *fn_80010244(void *manager, int index);
/* The file system and reading a whole file. */
void *fn_800371D8(void);
void *fn_8004509C(void *fileSystem, const char *path, u32 *size);
/* The handle table: release a HandleUser's handle, give it one. */
bool fn_800459CC(HandleUser &user);
bool fn_80045A64(HandleUser &user);
/* The stage: load a block's land, free it, free every block and load the stage's textures. */
UnkLandBlock_8003A840 *fn_800462C4(UnkStage_8003A840 *stage, const char *name, int id);
void fn_80047380(UnkStage_8003A840 *stage, UnkLandBlock_8003A840 *block);
void fn_80047310(UnkStage_8003A840 *stage);
bool fn_80046B8C(UnkStage_8003A840 *stage, const char *path, const char *name);

void fn_8003AE90(void);
void fn_8003ACA8(TBlockManage *self, const Vec3f *position);
bool fn_8003AB68(TBlockManage *self, UnkBlockData_8003AB68 *data);
void fn_8003AAB0(TBlockManage *self, int id, s32 request);
UnkStage_8003A840 *fn_8003A840(void);
UnkLandBlock_8003A840 *fn_8003A540(TBlockManage *self, int id);
void fn_8003A52C(TBlockManage *self);
bool fn_8003A4FC(TBlockManage *self, int id);
s32 fn_8003A380(TBlockManage *self, s16 **ids, const Vec3f *position);
}

/* The stage's cached instance (set by fn_8003A840). */
extern UnkStage_8003A840 *lbl_805EDFBC;

/* The managers, one per player, and their handles. */
TBlockManage *lbl_8051CE88[4] = {0, 0, 0, 0};
UnkHandle_80045A98 lbl_80571484[4];

const char *TBlockManage::s_taskName = "TBlockManage";

/* Forgets the managers' handles. */
extern "C" void fn_8003AE90(void) {
    memset(lbl_80571484, 0, sizeof(lbl_80571484));
}

HandleUser::HandleUser() {
    field_0x4 = 0;
    field_0x6 = -1;
}

TBlockManage::TBlockManage(Task *parent, int player, const char *path, const Vec3f *position)
    : Task(parent), field_0x40(0) {
    m_name = s_taskName;
    fn_80045A64(*this);
    m_data = 0;
    m_player = player;
    m_position = position;
    Open(path);
    UnkHandle_80045A98 *handle = (UnkHandle_80045A98 *)GetHandle();
    lbl_80571484[player].m_serial = handle->m_serial;
    lbl_80571484[player].m_index = handle->m_index;
    lbl_8051CE88[player] = this;
    memset(m_requests, 0, sizeof(m_requests));
    m_changed = 0;
}

TBlockManage::~TBlockManage() {
    Release();
    fn_800459CC(*this);
    lbl_8051CE88[m_player] = 0;
}

/* Sets the position whose blocks are wanted (0: the camera's). */
extern "C" void fn_8003ACA8(TBlockManage *self, const Vec3f *position) {
    self->m_position = position;
}

bool TBlockManage::Open(const char *path) {
    Release();
    if (path != 0) {
        return fn_8003AB68(this, (UnkBlockData_8003AB68 *)fn_8004509C(fn_800371D8(), path, 0));
    }
    return true;
}

/* Takes a block file read into memory and loads the stage's textures. */
extern "C" bool fn_8003AB68(TBlockManage *self, UnkBlockData_8003AB68 *data) {
    if (data == 0) {
        return false;
    }
    self->m_data = data;
    fn_80043D0C(&self->m_data->m_count, 4);
    int i;
    UnkBlockBox_8003AB68 *box = self->m_data->m_boxes;
    for (i = 0; i < self->m_data->m_count; i++) {
        fn_80043D0C(&box->m_id, 4);
        fn_80043D0C(&box->m_min, 0xC);
        fn_80043D0C(&box->m_max, 0xC);
        fn_80043D0C(&box->field_0x1C, 4);
        box++;
    }
    if (self->m_data->m_count >= TBlockManage::kBlockCount) {
        self->m_data->m_count = TBlockManage::kBlockCount;
    }
    char path[0x40];
    sprintf(path, "%s/%s_texCM.one", self->m_data->m_name, self->m_data->m_name);
    fn_80046B8C(fn_8003A840(), path, 0);
    return true;
}

void TBlockManage::Release() {
    if (m_data != 0) {
        fn_80049B34(m_data);
        m_data = 0;
    }
    fn_80047310(fn_8003A840());
    memset(&m_current, 0, 0x978 - 0x44);
    m_ready = 0;
}

void TBlockManage::Tick(float dt) {
    UpdateRequests();
    UpdateLoads();
    if ((s32)field_0x40 != 0) {
        field_0x40--;
    }
}

/* Asks for a block to be shown (1) or hidden (2) whatever the position, for the next frame. */
extern "C" void fn_8003AAB0(TBlockManage *self, int id, s32 request) {
    self->m_requests[id] = request;
}

/* Whether a block is wanted: in either buffer (which 0) or in the current one (which 1). Other
 * units call it too. */
inline bool TBlockManage::IsRequested(int id, int which) {
    switch (which) {
    case 0:
        for (int i = 0; i < 2; i++) {
            if (m_shown[i][id] != 0) {
                return true;
            }
        }
        break;
    case 1:
        if (m_shown[m_current][id] != 0) {
            return true;
        }
        break;
    }
    return false;
}

/* Never called: it makes the compiler emit IsRequested out of line here, where the game has it
 * (other units call it); the linker strips this function. */
bool (TBlockManage::*KeepIsRequested())(int, int) {
    return &TBlockManage::IsRequested;
}

void TBlockManage::UpdateRequests() {
    if (m_data == 0) {
        return;
    }
    s16 shown[kBlockCount];
    memset(shown, 0, sizeof(shown));
    int i = 0;
    UnkBlockBox_8003AB68 *boxes = m_data->m_boxes;
    int current = m_current;
    for (; i < m_data->m_count; i++) {
        UnkBlockBox_8003AB68 *box = &boxes[i];
        int id = box->m_id;
        switch (m_requests[id]) {
        case 1:
            shown[id] = 1;
            break;
        case 2:
            shown[id] = 0;
            break;
        default:
            if (IsInside(&box->m_min, &box->m_max, m_position)) {
                shown[id] = 1;
            }
            break;
        }
    }
    u8 changed = m_changed;
    m_changed = 0;
    for (int j = 0; j < kBlockCount; j++) {
        if (m_shown[current][j] != shown[j]) {
            changed = 1;
            break;
        }
    }
    if (changed) {
        int next = m_current + 1;
        if (next >= 2) {
            next = 0;
        }
        memcpy(m_shown[next], shown, sizeof(shown));
        m_current = next;
    }
    memset(m_requests, 0, sizeof(m_requests));
}

/* The stage (class name unknown). */
extern "C" UnkStage_8003A840 *fn_8003A840(void) {
    if (lbl_805EDFBC == 0) {
        static UnkStage_8003A840 s_stage;
        lbl_805EDFBC = &s_stage;
    }
    return lbl_805EDFBC;
}

void TBlockManage::UpdateLoads() {
    int id;
    s16 i;
    bool anyShown = false;
    s32 loaded = 0;
    m_loading = 0;
    for (i = 0; i < kBlockCount; i++) {
        id = i;
        if (IsRequested(id, 0)) {
            UnkLandBlock_8003A840 *block = m_blocks[id];
            if (block == 0) {
                m_blocks[id] = fn_800462C4(fn_8003A840(), m_data->m_name, id);
            } else if (block->m_state == 5) {
                m_loadedIds[loaded++] = i;
            }
        } else if (m_blocks[id] != 0 && m_blocks[id]->m_state == 5) {
            fn_80047380(fn_8003A840(), m_blocks[id]);
            m_blocks[id] = 0;
        }
        if (IsRequested(id, 1)) {
            anyShown = true;
            UnkLandBlock_8003A840 *block = m_blocks[id];
            if (block != 0 && block->m_state != 5) {
                m_loading = 1;
            }
        }
    }
    m_loadedIds[loaded] = -1;
    if (anyShown && !m_loading) {
        m_ready = 1;
    }
}

bool TBlockManage::IsInside(const Vec3f *min, const Vec3f *max, const Vec3f *position) {
    if (position == 0) {
        const Vec3f *camera = fn_80010244(fn_80009548(), 0);
        if (camera->x < min->x) {
            return false;
        }
        if (camera->x >= max->x) {
            return false;
        }
        if (camera->y < min->y) {
            return false;
        }
        if (camera->y >= max->y) {
            return false;
        }
        if (camera->z < min->z) {
            return false;
        }
        if (camera->z >= max->z) {
            return false;
        }
    } else {
        if (position->x < min->x) {
            return false;
        }
        if (position->x >= max->x) {
            return false;
        }
        if (position->y < min->y) {
            return false;
        }
        if (position->y >= max->y) {
            return false;
        }
        if (position->z < min->z) {
            return false;
        }
        if (position->z >= max->z) {
            return false;
        }
    }
    return true;
}

/* A block's land (0 if it is not loaded). */
extern "C" UnkLandBlock_8003A840 *fn_8003A540(TBlockManage *self, int id) {
    return self->m_blocks[id];
}

/* Has the wanted blocks worked out again. */
extern "C" void fn_8003A52C(TBlockManage *self) {
    self->m_changed = 1;
    self->m_ready = 0;
}

/* Whether a block's land is loaded. */
extern "C" bool fn_8003A4FC(TBlockManage *self, int id) {
    UnkLandBlock_8003A840 *block = self->m_blocks[id];
    if (block != 0 && block->m_state == 5) {
        return true;
    }
    return false;
}

/* Whether a block's land is being loaded. */
bool TBlockManage::IsLoading(int id) {
    UnkLandBlock_8003A840 *block = m_blocks[id];
    if (block != 0) {
        switch (block->m_state) {
        case 1:
        case 2:
        case 3:
        case 4:
            return true;
        }
    }
    return false;
}

/* The blocks a position lies in: returns their number and an array of their ids (0 if none),
 * which the caller frees. */
extern "C" s32 fn_8003A380(TBlockManage *self, s16 **ids, const Vec3f *position) {
    UnkBlockData_8003AB68 *data;
    s16 i;
    UnkBlockBox_8003AB68 *boxes = self->m_data->m_boxes;
    s16 count = 0;
    u8 found[TBlockManage::kBlockCount];
    memset(found, 0, sizeof(found));
    data = self->m_data;
    for (i = 0; i < data->m_count; i++) {
        UnkBlockBox_8003AB68 *box = &boxes[i];
        s32 id = box->m_id;
        if (self->IsInside(&box->m_min, &box->m_max, position) && !found[id]) {
            found[id] = 1;
            count++;
        }
    }
    if (count != 0) {
        *ids = (s16 *)fn_80049B90(count * sizeof(s16), 2);
    } else {
        *ids = 0;
        return 0;
    }
    s16 n = 0;
    for (s16 j = 0; j < TBlockManage::kBlockCount; j++) {
        if (found[j]) {
            (*ids)[n++] = j;
            if (n == count) {
                return count;
            }
        }
    }
    return count;
}

/* The blocks a position lies in (an empty pointer if there is no manager or none). */
extern "C" boost::shared_ptr<TBlockManage::forBlockCheck> fn_8003A154(const Vec3f *position) {
    TBlockManage *manage = lbl_8051CE88[0];
    boost::shared_ptr<TBlockManage::forBlockCheck> none;
    if (manage == 0) {
        return none;
    }
    TBlockManage::forBlockCheck *check = new TBlockManage::forBlockCheck;
    s16 *ids;
    check->m_count = fn_8003A380(manage, &ids, position);
    if (ids == 0) {
        return none;
    }
    memcpy(check->m_ids, ids, check->m_count * sizeof(s16));
    fn_80049B34(ids);
    return boost::shared_ptr<TBlockManage::forBlockCheck>(check);
}

/* Whether any of the blocks is being loaded. */
extern "C" bool fn_80039EDC(const boost::weak_ptr<TBlockManage::forBlockCheck> &block) {
    TBlockManage *manage;
    if ((manage = lbl_8051CE88[0]) == 0) {
        return false;
    }
    boost::shared_ptr<TBlockManage::forBlockCheck> check = block.lock();
    if (check.get() != 0) {
        for (int i = 0; i < check->m_count; i++) {
            if (manage->IsLoading(check->m_ids[i])) {
                return true;
            }
        }
    }
    return false;
}
