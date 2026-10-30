/* The stage's land (the file name, LandManager.cpp, is the game's: its free calls pass it). The
 * stage object fn_8003A840 returns (UnkStage_8003A840, class name unknown) owns 0x20 land blocks,
 * each loaded from one archive with a LandALoadMananger task (class name from RTTI, spelled so):
 * the archive's 15 layer models (one per LandDispManager_* name), the texture dictionaries they
 * share (counted in s_texDicts) and the block's collision. Fifteen LandDispManager tasks (class
 * name from RTTI), one per layer, draw the blocks the block set shows into the layer's world.
 * Names other than the classes' are inferred. Definitions are in reverse address order (deferred
 * inlining emits functions last-defined first).
 *
 * NonMatching (28/30): LandALoadMananger::Tick reloads m_fileIndex at the top of the texture loop
 * where GC/2.7 reuses the value the loop test loaded (one instruction); UnkLandLayer_8003A840's
 * constructor stores its two halfwords in source order where GC/2.7 schedules both constants
 * first; and .sdata2 has "_COLI" before "_TEX", which GC/2.7 pools the other way round (it pools in
 * emission order and LoadLayers, emitted first, uses "_TEX"). About 15 source shapes were tried for
 * each. The handles at 0x8057327C are set up by hand (s_handlesRecord) because their element
 * destructor is the shared copy __dt__18UnkHandle_80045A98Fv. */
#include "Game/HandleUser.h"
#include "Game/Render.h"
#include "Game/Task.h"
#define UNKHANDLE_DEFAULT_CTOR
#include "Game/UnkHandleTable.h"
#include <dolphin/types.h>
#include "size_assert.h"
#include <string.h>
#include <stdio.h>

struct UnkScreen_8000DCC0;
struct UnkScreenHolder_8000DCC0 {
    /* 0x0 */ UnkScreen_8000DCC0 *m_camera;
};
struct UnkWorld_800289B0 {
    /* 0x00 */ u8 field_0x0[0x8];
    /* 0x08 */ u32 m_flags;
    /* 0x0C */ u8 field_0xC[0x24 - 0xC];
    /* 0x24 */ int m_count; /* drawn only while positive (meaning inferred) */
};
struct UnkRenderer_800289B0;
struct UnkBlockSet_8003AAB0;
struct UnkAdbManager_80013BC4;
struct UnkTexDictManager_80014534;
struct UnkTaskRegistry_80014B90;
struct RwTexDictionary;

/* An archive of the game's .one files (class name unknown; constructor 0x8004CFB0). */
struct UnkOneFile_8004CFB0 {
    /* 0x00 */ u8 field_0x0[0x34];
    /* 0x34 */ struct UnkOneFileHeader *m_header;
    /* 0x38 */ u8 field_0x38[0x58 - 0x38];
};
struct UnkOneFileHeader {
    /* 0x00 */ u8 field_0x0[0x10];
    /* 0x10 */ int m_fileCount;
};

typedef struct RwGlobals {
    /* 0x00 */ u8 field_0x0[0x20];
    /* 0x20 */ int (*fpRenderStateSet)(int state, void *value);
} RwGlobals;
/* The engine instance (RwEngineInstance). */
extern RwGlobals *lbl_805F15B4;

/* The video settings (struct name unknown; the byte at 0x4D is the alpha reference). */
extern u8 lbl_805E3210[];
/* The block sets (see Game/BossRender.cpp). */
extern UnkBlockSet_8003AAB0 *lbl_8051CE88[4];
/* The render owner's cached pointer (see Game/Render.h). */
extern UnkRenderOwner_80046178 *lbl_805EDFC0;

extern "C" {
void *__nw__FUl(u32 size);
void __dl__FPv(void *p);
void *__construct_array(void *block, void *ctor, void *dtor, u32 size, u32 count);
void __destroy_arr(void *block, void *dtor, u32 size, u32 count);
void *__register_global_object(void *object, void *destructor, void *record);
void *__dynamic_cast(void *object, int vtableOffset, void *toType, void *fromType, int isReference);
void *__ct__18UnkHandle_80045A98Fv(void *handle);
void *__dt__18UnkHandle_80045A98Fv(void *object, short flags);
/* The handle table (Game/UnkHandles_800459CC.cpp). */
HandleUser *fn_80045A98(const UnkHandle_80045A98 &handle);
bool fn_80045A64(HandleUser &user);

UnkScreenHolder_8000DCC0 *fn_8000DCC0(void);
UnkRenderer_800289B0 *fn_800289B0(void);
void fn_80047DFC(UnkRenderer_800289B0 *renderer, UnkWorld_800289B0 *world);
void *fn_8001C4F8(void);
Task *fn_801EB304(void *root, int layer);
UnkTaskRegistry_80014B90 *fn_80014B90(void);
bool fn_8025D1C4(UnkTaskRegistry_80014B90 *registry, void *task, int kind, const char *name);
bool fn_8025D144(UnkTaskRegistry_80014B90 *registry, void *task);
UnkAdbManager_80013BC4 *fn_80013BC4(void);
UnkHandle_80045A98 fn_8004FB78(UnkAdbManager_80013BC4 *manager, void *data, int arg2, const char *path,
                               int arg4, int arg5);
void fn_8004F99C(UnkAdbManager_80013BC4 *manager, UnkHandle_80045A98 handle);
void fn_80038120(void *data);
UnkTexDictManager_80014534 *fn_80014534(void);
void fn_80039618(UnkTexDictManager_80014534 *manager, void *dict, int arg2);
bool IsRequested__12TBlockManageFii(UnkBlockSet_8003AAB0 *blocks, int id, int state);
char *fn_803AC6C0(const char *s, const char *sub);
RwTexDictionary *fn_8048D0C4(void *dict);
/* The game's free functions that name the caller's file and line. */
bool fn_80043A40(void *object, const char *file, int line);
bool fn_80043A64(void *object, const char *file, int line);

/* The archive's functions. */
UnkOneFile_8004CFB0 *fn_8004CFB0(void *file, const char *name, int arg2);
void fn_8004CF68(UnkOneFile_8004CFB0 *file, int flags);
bool fn_8004CC6C(UnkOneFile_8004CFB0 *file, const char *path, int arg2, int arg3, int arg4, int arg5);
bool fn_8004CAF8(UnkOneFile_8004CFB0 *file);
int fn_8004C850(UnkOneFile_8004CFB0 *file, const char *name, int start);
const char *fn_8004CA28(UnkOneFile_8004CFB0 *file, int index);
int fn_8004C910(UnkOneFile_8004CFB0 *file, const char *name);
void *fn_8004BF40(UnkOneFile_8004CFB0 *file, int index, void (*callback)(void *, void *), void *arg);
void fn_8004B2F0(UnkOneFile_8004CFB0 *file, int index, void (*callback)(void *, void *), void *arg);

/* RenderWare and the GX state. */
UnkWorld_800289B0 *fn_80460138(void *box);
void fn_8045FFA0(UnkWorld_800289B0 *world);
void fn_8045FF5C(UnkWorld_800289B0 *world);
void fn_80462130(UnkWorld_800289B0 *world, UnkScreen_8000DCC0 *camera);
void fn_80462184(UnkWorld_800289B0 *world, UnkScreen_8000DCC0 *camera);
UnkScreen_8000DCC0 *fn_804854DC(UnkScreen_8000DCC0 *camera);
UnkScreen_8000DCC0 *fn_804854B4(UnkScreen_8000DCC0 *camera);
void fn_80497F24(int comp0, int ref0, int op, int comp1, int ref1);
void fn_80497778(int before);
}

/* One layer model of a block. */
struct UnkLandLayer_8003A840 {
    UnkLandLayer_8003A840() {}
    ~UnkLandLayer_8003A840() {}

    /* 0x0 */ void *m_data;
    /* 0x4 */ int m_texIndex;
    /* 0x8 */ UnkHandle_80045A98 m_handle;
};

/* A land block. */
struct UnkLandBlock_8003A840 {
    /* 0x00 */ int m_state; /* 0 free, 5 loaded */
    /* 0x04 */ UnkLandLayer_8003A840 m_layers[15];
    /* 0xB8 */ UnkWorld_800289B0 *m_collision;
    /* 0xBC */ int m_id;
};

/* A texture dictionary the blocks share. */
struct UnkTexDict_80573220 {
    /* 0x0 */ int m_refCount;
    /* 0x4 */ void *m_dict;
};

static UnkTexDict_80573220 s_texDicts[10];

/* The stage (class name unknown; fn_8003A840 returns it). */
class UnkStage_8003A840 {
  public:
    UnkStage_8003A840();
    ~UnkStage_8003A840();

    UnkLandBlock_8003A840 *GetFreeBlock() {
        for (int i = 0; i < 0x20; i++) {
            if (m_blocks[i].m_state == 0) {
                return &m_blocks[i];
            }
        }
        return 0;
    }
    UnkTexDict_80573220 &GetTexDict(int index) {
        return s_texDicts[index];
    }
    void SetTexDictionary(int index, void *dict) {
        s_texDicts[index].m_dict = dict;
    }
    void *GetTexDictionary(int index) {
        if (index > 0 && index < 10) {
            return s_texDicts[index].m_dict;
        }
        return 0;
    }
    int GetTexIndex(const char *name);
    Task *GetLayerTask(int layer);
    void FreeLight() {
        if (m_light != 0) {
            fn_80043A40(m_light, "LandManager.cpp", 0x311);
            m_light = 0;
        }
    }
    void Nothing() {}
    void Unlock() {
        m_locked = false;
    }
    void SetUseCollision(u8 use) {
        m_useCollision = use;
    }
    void SetFogEnable(u8 enable) {
        m_fogEnable = enable;
    }
    void CreateWorlds();
    void DestroyWorlds();

    /* 0x0000 */ UnkLandBlock_8003A840 m_blocks[0x20];
    /* 0x1800 */ u8 field_0x1800[4];
    /* 0x1804 */ void *m_light;
    /* 0x1808 */ u8 m_useCollision;
    /* 0x1809 */ u8 m_fogEnable;
    /* 0x180A */ u8 field_0x180A;
    /* 0x180C */ float m_mipmapK;
    /* 0x1810 */ bool m_locked;
};

extern "C" {
UnkStage_8003A840 *fn_8003A840(void);
void fn_80047310(UnkStage_8003A840 *stage);
}

/* One layer: its task's name, its world and the render state it is drawn with. */
struct UnkWorldEntry_8051D208 {
    /* 0x00 */ const char *m_suffix;
    /* 0x04 */ const char *m_taskName;
    /* 0x08 */ u32 m_renderFlags;
    /* 0x0C */ int m_renderLevel;
    /* 0x10 */ UnkWorld_800289B0 *m_world;
    /* 0x14 */ class LandDispManager *m_task;
};

UnkWorldEntry_8051D208 lbl_8051D208[15] = {
    {"_N_", "LandDispManager_N", 0x28, 1, 0, 0},
    {"_D_", "LandDispManager_D", 0x28, 1, 0, 0},
    {"_OS_", "LandDispManager_OS", 0x2C, 1, 0, 0},
    {"_OSL_", "LandDispManager_OSL", 0x2D, 1, 0, 0},
    {"_O_", "LandDispManager_O", 0x2C, 1, 0, 0},
    {"_OW_", "LandDispManager_OW", 0x3C, 1, 0, 0},
    {"_OL_", "LandDispManager_OL", 0x2D, 1, 0, 0},
    {"_P_", "LandDispManager_P", 0x80C, 1, 0, 0},
    {"_AF_", "LandDispManager_AF", 0x5C, 5, 0, 0},
    {"_A_", "LandDispManager_A", 0x5C, 5, 0, 0},
    {"_AL_", "LandDispManager_AL", 0x4D, 5, 0, 0},
    {"_K_", "LandDispManager_K", 0x9C, 7, 0, 0},
    {"_M_", "LandDispManager_M", 0x11C, 7, 0, 0},
    {"_G_", "LandDispManager_G", 0x98, 9, 0, 0},
    {"_GL_", "LandDispManager_GL", 0x99, 9, 0, 0},
};

/* The default mipmap K of the stages (Game/TGameDefaultParam.cpp, Game/BossRootTask.cpp). */
extern const float lbl_805F1C08;
const float lbl_805F1C08 = -7.0f;

/* Draws one layer of the blocks. */
class LandDispManager : public Task, public HandleUser {
  public:
    LandDispManager(Task *parent, int layer);
    virtual ~LandDispManager();
    virtual void Tick(float dt);

    /* 0x30 */ int m_layer;
    /* 0x34 */ u32 field_0x34;
};
SIZE_ASSERT(LandDispManager, 0x38);

/* Loads one block. */
class LandALoadMananger : public Task {
  public:
    LandALoadMananger(Task *parent, const char *name, int id, UnkLandBlock_8003A840 *block);
    virtual ~LandALoadMananger();
    virtual void Tick(float dt);
    bool LoadLayers(UnkLandBlock_8003A840 *block, UnkOneFile_8004CFB0 *file, int id);

    static const char *s_taskName;

    /* 0x28 */ char m_stageName[0x40];
    /* 0x68 */ char m_path[0x40];
    /* 0xA8 */ UnkLandBlock_8003A840 *m_block;
    /* 0xAC */ UnkOneFile_8004CFB0 *m_file;
    /* 0xB0 */ UnkOneFile_8004CFB0 *m_texFile;
    /* 0xB4 */ int m_state;
    /* 0xB8 */ int m_fileIndex;
    /* 0xBC */ int m_texIndex;
    /* 0xC0 */ int m_id;
    /* 0xC4 */ u8 m_pending[15];
    /* 0xD3 */ u8 field_0xD3[0xD8 - 0xD3];
};
SIZE_ASSERT(LandALoadMananger, 0xD8);

const char *LandALoadMananger::s_taskName = "LandALoadMananger";

/* What a layer's load callback gets. */
struct UnkLayerLoad_80046410 {
    /* 0x0 */ LandALoadMananger *m_task;
    /* 0x4 */ UnkLandBlock_8003A840 *m_block;
    /* 0x8 */ int m_layer;
};

/* The layers' tasks' handles. They are set up by hand: their destructor is a shared copy. */
struct UnkHandlesRecord_80573270 {
    UnkHandlesRecord_80573270();

    u32 m_words[3];
};
static UnkHandlesRecord_80573270 s_handlesRecord;
static u32 s_handles[15];

extern "C" inline void fn_80047740(void) {
    __destroy_arr(s_handles, __dt__18UnkHandle_80045A98Fv, 4, 15);
}

inline UnkHandlesRecord_80573270::UnkHandlesRecord_80573270() {
    __construct_array(s_handles, __ct__18UnkHandle_80045A98Fv, __dt__18UnkHandle_80045A98Fv, 4, 15);
    __register_global_object(0, fn_80047740, this);
}

UnkStage_8003A840::UnkStage_8003A840() : m_light(0), m_mipmapK(-7.0f), m_locked(0) {
    memset(this, 0, 0x1800);
    memset(s_texDicts, 0, sizeof(s_texDicts));
    CreateWorlds();
    m_useCollision = 0;
    field_0x180A = 1;
}

inline void UnkStage_8003A840::DestroyWorlds() {
    for (int i = 0; i < 15; i++) {
        if (lbl_8051D208[i].m_world != 0) {
            fn_8045FFA0(lbl_8051D208[i].m_world);
            lbl_8051D208[i].m_world = 0;
        }
    }
}

UnkStage_8003A840::~UnkStage_8003A840() {
    DestroyWorlds();
    fn_80047310(this);
}

/* Frees a block. */
extern "C" void fn_80047380(UnkStage_8003A840 *stage, UnkLandBlock_8003A840 *block) {
    if (block->m_state == 5) {
        for (int i = 0; i < 15; i++) {
            UnkLandLayer_8003A840 *layer = &block->m_layers[i];
            if (fn_80045A98(layer->m_handle) != 0) {
                fn_8004F99C(fn_80013BC4(), layer->m_handle);
            }
            if (layer->m_data != 0) {
                fn_80043A64(layer->m_data, "LandManager.cpp", 0x12C);
                layer->m_data = 0;
            }
            if (layer->m_texIndex != 0) {
                UnkTexDict_80573220 *dict = &s_texDicts[layer->m_texIndex];
                if (dict->m_refCount != 0) {
                    dict->m_refCount--;
                    if (dict->m_refCount == 0) {
                        void *&texDict = dict->m_dict;
                        if (texDict != 0) {
                            fn_80043A40(texDict, "LandManager.cpp", 0x323);
                        }
                        texDict = 0;
                    }
                }
            }
        }
        if (block->m_collision != 0) {
            fn_80043A64(block->m_collision, "LandManager.cpp", 0x139);
            block->m_collision = 0;
        }
        block->m_state = 0;
    }
}

/* Frees every block and the light. */
extern "C" void fn_80047310(UnkStage_8003A840 *stage) {
    for (int i = 0; i < 0x20; i++) {
        fn_80047380(stage, &stage->m_blocks[i]);
    }
    stage->FreeLight();
}

extern "C" void fn_80046D58(UnkStage_8003A840 *stage, u32 flags, UnkWorld_800289B0 *world);

/* Draws a world into the camera. */
inline void DrawWorld(UnkStage_8003A840 *stage, UnkWorld_800289B0 *world, UnkScreen_8000DCC0 *camera, int layer) {
    fn_80462130(world, camera);
    fn_804854DC(camera);
    fn_80046D58(stage, lbl_8051D208[layer].m_renderFlags, world);
    fn_8045FF5C(world);
    fn_804854B4(camera);
    fn_80462184(world, camera);
}

/* Draws a layer's own world. */
inline void DrawLayerWorld(UnkStage_8003A840 *stage, UnkScreen_8000DCC0 *camera, int layer) {
    UnkWorld_800289B0 *world = lbl_8051D208[layer].m_world;
    if (world != 0 && world->m_count > 0) {
        fn_80462130(world, camera);
        fn_804854DC(camera);
        fn_80046D58(stage, lbl_8051D208[layer].m_renderFlags, world);
        fn_8045FF5C(world);
        fn_804854B4(camera);
        fn_80462184(world, camera);
    }
}

/* Draws one layer: the blocks the block set shows, then the layer's world. */
static void DrawLayer(UnkStage_8003A840 *stage, UnkScreen_8000DCC0 *camera, int layer) {
    UnkBlockSet_8003AAB0 *blocks;
    if (!(layer & 0x200) && (blocks = lbl_8051CE88[0]) != 0) {
        for (int i = 0; i < 0x20; i++) {
            UnkLandBlock_8003A840 *block = &stage->m_blocks[i];
            UnkWorld_800289B0 *world;
            if (stage->m_useCollision == 0) {
                world = (UnkWorld_800289B0 *)block->m_layers[layer].m_data;
            } else {
                world = block->m_collision;
            }
            if (block->m_state == 5 && world != 0 && IsRequested__12TBlockManageFii(blocks, block->m_id, 1)) {
                DrawWorld(stage, world, camera, layer);
            }
        }
        if (stage->m_useCollision == 0) {
            DrawLayerWorld(stage, camera, layer);
        }
    }
}

/* Sets the render state of a layer. */
extern "C" void fn_80046D58(UnkStage_8003A840 *stage, u32 flags, UnkWorld_800289B0 *world) {
    if (world != 0) {
        u32 worldFlags = world->m_flags;
        if (flags & 1) {
            world->m_flags = worldFlags | 0x30;
            fn_80047DFC(fn_800289B0(), world);
        } else {
            world->m_flags = worldFlags & ~0x30;
            fn_80047DFC(fn_800289B0(), 0);
        }
    }
    if (flags & 0x40) {
        lbl_805F15B4->fpRenderStateSet(10, (void *)5);
        lbl_805F15B4->fpRenderStateSet(11, (void *)6);
        lbl_805F15B4->fpRenderStateSet(6, (void *)1);
        lbl_805F15B4->fpRenderStateSet(8, (void *)0);
        fn_80497F24(7, 0, 0, 7, 0);
        fn_80497778(1);
    } else if (flags & 0x80) {
        lbl_805F15B4->fpRenderStateSet(10, (void *)5);
        lbl_805F15B4->fpRenderStateSet(11, (void *)2);
        lbl_805F15B4->fpRenderStateSet(6, (void *)1);
        lbl_805F15B4->fpRenderStateSet(8, (void *)0);
        fn_80497F24(7, 0, 0, 7, 0);
        fn_80497778(1);
    } else if (flags & 0x100) {
        lbl_805F15B4->fpRenderStateSet(10, (void *)10);
        lbl_805F15B4->fpRenderStateSet(11, (void *)1);
        fn_80497F24(7, 0, 0, 7, 0);
        fn_80497778(1);
    } else if (flags & 0x800) {
        lbl_805F15B4->fpRenderStateSet(10, (void *)5);
        lbl_805F15B4->fpRenderStateSet(11, (void *)6);
        lbl_805F15B4->fpRenderStateSet(8, (void *)1);
        lbl_805F15B4->fpRenderStateSet(6, (void *)1);
        fn_80497F24(4, 200, 0, 7, lbl_805E3210[0x4D]);
        fn_80497778(0);
    } else if (flags & 0x400) {
        lbl_805F15B4->fpRenderStateSet(10, (void *)5);
        lbl_805F15B4->fpRenderStateSet(11, (void *)6);
        lbl_805F15B4->fpRenderStateSet(6, (void *)0);
        lbl_805F15B4->fpRenderStateSet(8, (void *)0);
        fn_80497F24(7, 0, 0, 7, 0);
        fn_80497778(1);
    } else {
        lbl_805F15B4->fpRenderStateSet(10, (void *)5);
        lbl_805F15B4->fpRenderStateSet(11, (void *)6);
        lbl_805F15B4->fpRenderStateSet(6, (void *)1);
        lbl_805F15B4->fpRenderStateSet(8, (void *)1);
        fn_80497F24(4, 1, 0, 7, lbl_805E3210[0x4D]);
        fn_80497778(0);
    }
    if (flags & 8) {
        lbl_805F15B4->fpRenderStateSet(9, (void *)2);
    } else {
        lbl_805F15B4->fpRenderStateSet(9, (void *)0);
    }
    if (flags & 0x10) {
        lbl_805F15B4->fpRenderStateSet(20, (void *)1);
    } else {
        lbl_805F15B4->fpRenderStateSet(20, (void *)2);
    }
    if (flags & 4) {
        lbl_805F15B4->fpRenderStateSet(14, (void *)stage->m_fogEnable);
    } else {
        lbl_805F15B4->fpRenderStateSet(14, (void *)0);
    }
}

/* The bounding box of a layer's world. */
struct UnkBBox_80460138 {
    /* 0x00 */ float m_supX, m_supY, m_supZ;
    /* 0x0C */ float m_infX, m_infY, m_infZ;
};

void UnkStage_8003A840::CreateWorlds() {
    for (int i = 0; i < 15; i++) {
        UnkBBox_80460138 box;
        box.m_infZ = -10000000.0f;
        box.m_infY = -10000000.0f;
        box.m_infX = -10000000.0f;
        box.m_supZ = 10000000.0f;
        box.m_supY = 10000000.0f;
        box.m_supX = 10000000.0f;
        lbl_8051D208[i].m_world = fn_80460138(&box);
    }
}

/* Loads the stage's light: the texture dictionary called name in the archive at path, or else the
 * first one. */
extern "C" bool fn_80046B8C(UnkStage_8003A840 *stage, const char *path, const char *name) {
    int i;
    void *light;
    if (stage->m_light != 0) {
        stage->FreeLight();
    }
    UnkOneFile_8004CFB0 *file = new UnkOneFile_8004CFB0;
    if (file != 0) {
        file = fn_8004CFB0(file, path, 0);
    }
    if (file == 0) {
        light = 0;
    } else {
        light = 0;
        if (name != 0) {
            light = fn_8004BF40(file, fn_8004C910(file, name), 0, 0);
        } else {
            for (i = 2; i < 15; i++) {
                const char *fileName = fn_8004CA28(file, i);
                if (fileName == 0) {
                    break;
                }
                if (fn_803AC6C0(fileName, ".TXD") != 0) {
                    light = fn_8004BF40(file, i, 0, 0);
                    break;
                }
            }
        }
        fn_8004CF68(file, 1);
    }
    stage->m_light = light;
    fn_80039618(fn_80014534(), stage->m_light, 6);
    return stage->m_light != 0;
}

LandALoadMananger::LandALoadMananger(Task *parent, const char *name, int id, UnkLandBlock_8003A840 *block)
    : Task(parent) {
    m_name = s_taskName;
    sprintf(m_path, "%s/%s_%02d.one", name, name, id);
    strcpy(m_stageName, name);
    m_block = block;
    m_state = 1;
    m_block->m_state = 1;
    m_file = 0;
    m_texFile = 0;
    m_fileIndex = 2;
    m_texIndex = 0;
    m_id = id;
    fn_8025D1C4(fn_80014B90(), this, 0, "LandALoadManager");
}

LandALoadMananger::~LandALoadMananger() {
    if (m_file != 0) {
        fn_8004CF68(m_file, 1);
    }
    if (m_texFile != 0) {
        fn_8004CF68(m_texFile, 1);
    }
    fn_8025D144(fn_80014B90(), this);
}

static void OnTexDictLoaded(void *dict, void *arg);
static void OnCollisionLoaded(void *data, void *arg);

void LandALoadMananger::Tick(float dt) {
    switch (m_state) {
    case 1:
        m_state = 6;
    case 6: {
        m_block->m_state = 2;
        UnkOneFile_8004CFB0 *file = new UnkOneFile_8004CFB0;
        if (file != 0) {
            file = fn_8004CFB0(file, 0, 0);
        }
        m_file = file;
        if (m_file != 0) {
            if (fn_8004CC6C(m_file, m_path, 0, 0, 0, 0)) {
                m_state = 7;
            } else {
                m_block->m_state = 5;
                m_state = 12;
            }
        } else {
            m_state = 12;
        }
        break;
    }
    case 7:
        if (fn_8004CAF8(m_file)) {
            m_state = 2;
        }
        break;
    case 2: {
        UnkOneFileHeader *header = m_file->m_header;
        while (m_fileIndex < header->m_fileCount + 2) {
            m_texIndex = fn_8003A840()->GetTexIndex(fn_8004CA28(m_file, m_fileIndex));
            m_fileIndex++;
            int texIndex = m_texIndex;
            if (texIndex != 0) {
                if (fn_8003A840()->GetTexDict(texIndex).m_refCount == 0) {
                    UnkOneFile_8004CFB0 *file = new UnkOneFile_8004CFB0;
                    if (file != 0) {
                        file = fn_8004CFB0(file, 0, 0);
                    }
                    m_texFile = file;
                    if (m_texFile != 0) {
                        char path[0x40];
                        sprintf(path, "%s/%s_TEX%02d.one", m_stageName, m_texIndex);
                        if (fn_8004CC6C(m_texFile, path, 0, 0, 0, 0)) {
                            m_state = 3;
                            return;
                        }
                        fn_8004CF68(m_texFile, 1);
                        m_texFile = 0;
                    }
                } else {
                    fn_8003A840()->GetTexDict(m_texIndex).m_refCount++;
                }
            }
        }
        m_state = 8;
        break;
    }
    case 3:
        if (fn_8004CAF8(m_texFile)) {
            if (fn_8003A840()->GetTexDictionary(m_texIndex) == 0) {
                fn_8004BF40(m_texFile, 2, OnTexDictLoaded, this);
            }
            m_state = 4;
        }
        break;
    case 5:
        fn_8004CF68(m_texFile, 1);
        m_texFile = 0;
        m_state = 2;
        break;
    case 8:
        if (LoadLayers(m_block, m_file, m_id)) {
            m_state = 9;
        } else {
            m_state = 10;
        }
        break;
    case 10: {
        UnkOneFile_8004CFB0 *file = m_file;
        bool loading;
        int index = fn_8004C850(file, "_COLI", 2);
        if (index > 0) {
            fn_8004B2F0(file, index, OnCollisionLoaded, this);
            loading = true;
        } else {
            m_block->m_state = 5;
            loading = false;
        }
        if (loading) {
            m_state = 11;
        } else {
            m_state = 12;
        }
        break;
    }
    case 12:
        m_state = 0;
        m_flags |= 1;
        break;
    }
}

inline int UnkStage_8003A840::GetTexIndex(const char *name) {
    char *p = fn_803AC6C0(name, "_TEX");
    if (p != 0) {
        p += strlen("_TEX");
        return (p[0] - '0') * 10 + (p[1] - '0');
    }
    return 0;
}

static void OnLayerLoaded(void *data, void *arg);

bool LandALoadMananger::LoadLayers(UnkLandBlock_8003A840 *block, UnkOneFile_8004CFB0 *file, int id) {
    bool loading = false;
    memset(m_pending, 0, 15);
    for (int i = 0; i < 15; i++) {
        int index = fn_8004C850(file, lbl_8051D208[i].m_suffix, 2);
        if (index > 0) {
            UnkLandLayer_8003A840 *layer = &block->m_layers[i];
            layer->m_texIndex = fn_8003A840()->GetTexIndex(fn_8004CA28(file, index));
            int texIndex = layer->m_texIndex;
            if (texIndex != 0) {
                fn_8048D0C4(fn_8003A840()->GetTexDictionary(texIndex));
            } else {
                fn_8048D0C4(fn_8003A840()->m_light);
            }
            UnkLayerLoad_80046410 *load = new UnkLayerLoad_80046410;
            load->m_task = this;
            load->m_block = block;
            load->m_layer = i;
            m_pending[load->m_layer] = 1;
            fn_8004B2F0(file, index, OnLayerLoaded, load);
            loading = true;
        }
    }
    block->m_id = id;
    return loading;
}

static void OnLayerLoaded(void *data, void *arg) {
    UnkLayerLoad_80046410 *load = (UnkLayerLoad_80046410 *)arg;
    UnkLandLayer_8003A840 *layer = &load->m_block->m_layers[load->m_layer];
    layer->m_data = data;
    if (layer->m_data != 0) {
        layer->m_handle = fn_8004FB78(fn_80013BC4(), layer->m_data, 0, load->m_task->m_path, 0, 0);
        fn_80038120(layer->m_data);
    }
    load->m_task->m_pending[load->m_layer] = 0;
    for (int i = 0; i < 15; i++) {
        if (load->m_task->m_pending[i] != 0) {
            delete load;
            return;
        }
    }
    load->m_block->m_state = 4;
    load->m_task->m_state = 10;
    delete load;
}

static void OnCollisionLoaded(void *data, void *arg) {
    LandALoadMananger *task = (LandALoadMananger *)arg;
    task->m_block->m_collision = (UnkWorld_800289B0 *)data;
    task->m_block->m_state = 5;
    task->m_state = 12;
}

static void OnTexDictLoaded(void *dict, void *arg) {
    LandALoadMananger *task = (LandALoadMananger *)arg;
    fn_8003A840()->SetTexDictionary(task->m_texIndex, dict);
    fn_8003A840()->GetTexDict(task->m_texIndex).m_refCount++;
    task->m_state = 5;
}

/* Starts loading block id of the stage's land called name; returns the block. */
extern "C" UnkLandBlock_8003A840 *fn_800462C4(UnkStage_8003A840 *stage, const char *name, int id) {
    if (stage->m_locked) {
        return 0;
    }
    UnkLandBlock_8003A840 *block = stage->GetFreeBlock();
    if (block != 0) {
        memset(block, 0, sizeof(UnkLandBlock_8003A840));
        new LandALoadMananger(fn_801EB304(fn_8001C4F8(), 10), name, id, block);
    }
    return block;
}

LandDispManager::LandDispManager(Task *parent, int layer) : Task(parent) {
    m_name = lbl_8051D208[layer].m_taskName;
    fn_80045A64(*this);
    lbl_8051D208[layer].m_task = this;
    m_flags |= 0x100;
    m_layer = layer;
}

void LandDispManager::Tick(float dt) {
    if (m_layer == 0) {
        fn_8003A840()->Nothing();
    }
    DrawLayer(fn_8003A840(), fn_8000DCC0()->m_camera, m_layer);
}

/* The render owner (a function-local static). */
inline UnkRenderOwner_80046178 &UnkRenderOwner_80046178_Instance() {
    static UnkRenderOwner_80046178 s_owner;
    return s_owner;
}

extern "C" UnkRenderOwner_80046178 *fn_80046178(void) {
    if (lbl_805EDFC0 == 0) {
        lbl_805EDFC0 = &UnkRenderOwner_80046178_Instance();
    }
    return lbl_805EDFC0;
}

/* A render level's task. */
Task *GetRenderLevel(UnkRenderOwner_80046178 *owner, int level) {
    return ((Task **)owner)[level];
}

inline Task *UnkStage_8003A840::GetLayerTask(int layer) {
    return GetRenderLevel(fn_80046178(), lbl_8051D208[layer].m_renderLevel);
}

/* Creates the layers' tasks (from StageManager::Start). */
extern "C" void fn_80046098(void) {
    fn_8003A840()->Unlock();
    fn_8003A840()->SetUseCollision(0);
    for (int i = 0; i < 15; i++) {
        LandDispManager *task = new LandDispManager(fn_8003A840()->GetLayerTask(i), i);
        UnkHandle_80045A98 *handle = (UnkHandle_80045A98 *)task->GetHandle();
        ((UnkHandle_80045A98 *)s_handles)[i].m_serial = handle->m_serial;
        ((UnkHandle_80045A98 *)s_handles)[i].m_index = handle->m_index;
    }
    fn_8003A840()->SetFogEnable(0);
}

/* Ends the layers' tasks (from StageManager::Finish). */
extern "C" void fn_80046024(void) {
    for (int i = 0; i < 15; i++) {
        LandDispManager *task = dynamic_cast<LandDispManager *>(fn_80045A98(((UnkHandle_80045A98 *)s_handles)[i]));
        if (task != 0) {
            task->m_flags |= 1;
        }
    }
}

LandDispManager::~LandDispManager() {}
