/* folderStoryRoute (class name from the RTTI of its shared_ptr control block in Game/TGiPause.cpp):
 * the story route chart of the pause screen. It plays the scenes of the scene list Root/StoryRoute
 * and marks on them the stages and endings the save data has reached, the lines of the chart the
 * story went along or a cleared mission leads along, and the player's position. The file name and
 * the names marked "(name inferred)" are not original. Definitions are in reverse address order
 * (deferred inlining emits functions last-defined first). The unit also emits the save data's
 * accessor (fn_800BB74C), a flag test other units call (fn_800BBDD8), and StoryCheck's destructor
 * with the instances of its std::map.
 *
 * NonMatching: 19 of 20 functions and all the data match; see the constructor. */
#define TGI_NO_NUMBER_COLORS
#define TGI_HANDLE_ASSIGN
#define MSL_LESS_CAST
#define MSL_PAIR_COPY_CALL
#define MSL_STRING_COPY
#define MSL_STRING_LESS
#define MSL_STRING_COMPARE_INLINE
#define MSL_TREE_END_NODE_NAME __tree_end_node_StoryCheck
#define MSL_PAIR_DTOR_CALL
#include <dolphin/types.h>
#include <map>
#include <stdio.h>
#include <string>
#define SAVEDATA_ANALYZER_LINES
#include "Game/CsdRCPtr.h"
#include "Game/CsdSceneCtrl.h"
#include "Game/SavedataAnalyzer.h"
#include "Game/StageTable.h"
#include "Game/TGi.h"
#include "Game/TGiPause.h"
#include "size_assert.h"

using Chao::CSD::CNode;
using Chao::CSD::CScene;
using Chao::CSD::RCPtr;

namespace Chao {
namespace CSD {
/* A node of a scene (as far as this unit uses it). */
class CNode {
  public:
    void SetPosition(float x, float y);
};
} // namespace CSD
} // namespace Chao

/* Sixteen flags (class name unknown; see Game/UnkSaveData_800BB74C.cpp). */
struct UnkFlags_801E5810 {
    /* 0x0 */ u16 m_bits;
};

/* The save data as this unit reads it (struct names unknown; see Game/UnkSaveData_800BB74C.cpp). */
struct UnkMissionSave_800BB74C {
    /* 0x00 */ u8 m_cleared;
    /* 0x01 */ u8 field_0x1[0x18 - 0x1];
};

struct UnkStageSave_800BB74C {
    /* 0x00 */ s32 m_plays;
    /* 0x04 */ UnkMissionSave_800BB74C m_missions[3];
    /* 0x4C */ u8 field_0x4C[0x60 - 0x4C];
};

struct UnkPlayHistory_800BB74C {
    /* 0x0000 */ s32 m_count; /* the last entry of m_route */
    /* 0x0004 */ s32 m_route[24]; /* the stages played, as indices in the stage table */
    /* 0x0064 */ u8 field_0x64[0x78 - 0x64];
    /* 0x0078 */ UnkStageSave_800BB74C m_stages[59];
    /* 0x1698 */ u8 field_0x1698[0x16E0 - 0x1698];
    /* 0x16E0 */ UnkFlags_801E5810 m_endings; /* the endings seen (meaning inferred) */
};

struct UnkSaveData_800BB74C {
    /* 0x0000 */ u8 field_0x0[0x30];
    /* 0x0030 */ UnkPlayHistory_800BB74C m_history;
    /* 0x1714 */ u8 field_0x1714[0x1F5C - 0x1714];
};
SIZE_ASSERT(UnkSaveData_800BB74C, 0x1F5C);

/* The current save data and the copy last saved (Game/UnkSaveData_800BB74C.cpp). */
struct UnkSaveHolder_801E559C {
    UnkSaveHolder_801E559C();

    /* 0x0000 */ UnkSaveData_800BB74C m_current;
    /* 0x1F5C */ UnkSaveData_800BB74C m_saved;
};

/* A node's position (struct name unknown; see Game/folderPause.cpp). */
struct UnkCsdPosition_8036AE98 {
    float x;
    float y;
};

/* The holder's instance pointer (another unit's .sbss). */
extern UnkSaveHolder_801E559C *lbl_805EE014;

/* The instance pointer of the object fn_801E68A4 makes on first use (Game/UnkSystemManager_801E2128.cpp;
 * class unknown). Retail's .sbss holds it here, before this unit's guard, so the unit defines it. */
void *lbl_805EE624;

extern "C" {
/* The scene list at path of project, and a scene of a scene list by name (see
 * Game/folderPause.cpp). */
UnkCsdHandle_80042DF0 fn_8036D2FC(Chao::CSD::CProject *project, const char *path, void *factory);
RCPtr<CScene> fn_8036F470(void *object, const char *name);
/* Whether a scene's queued motions have all been played (see Game/CsdSceneSet.h). */
bool fn_8003DBB0(TPjsSceneTour *scene);
/* Nodes: the position, moving, hiding and a corner's colour (behaviour inferred). */
UnkCsdPosition_8036AE98 fn_8036AE98(CNode *node);
void fn_8036AE70(CNode *node, const UnkCsdPosition_8036AE98 *position);
void fn_8036AD78(CNode *node, int hidden);
void fn_8036AC60(CNode *node, int corner, const u32 *color);
UnkSaveData_800BB74C *fn_800BB74C(void);
bool fn_800BBDD8(const UnkFlags_801E5810 *flags, int bit);
}

/* Handle comparison (name inferred; as in Game/folderPause.cpp). */
inline bool operator!=(const UnkCsdHandle_80042DF0 &a, const UnkCsdHandle_80042DF0 &b) {
    void *pa = a.m_object != 0 ? *(void **)a.m_object : 0;
    void *pb = b.m_object != 0 ? *(void **)b.m_object : 0;
    if (pa != pb) {
        return true;
    }
    return false;
}

/* Node comparison (name inferred). */
inline bool operator!=(const RCPtr<CNode> &a, const RCPtr<CNode> &b) {
    CNode *pa = a.m_object != 0 ? a.m_object->m_object : 0;
    CNode *pb = b.m_object != 0 ? b.m_object->m_object : 0;
    if (pa != pb) {
        return true;
    }
    return false;
}

/* A stage of the chart, or an ending when stage is -1 (struct name and member names inferred). */
struct UnkStoryStage_800BB7A4 {
    s32 m_stage;  /* index in the stage table */
    s32 m_ending; /* -1 for a stage */
    const char *m_node;
};

static const UnkStoryStage_800BB7A4 s_stages[32] = {
    {5, -1, "0100"},  {6, -1, "0200"},  {7, -1, "0201"},  {8, -1, "0202"},  {9, -1, "0300"},
    {10, -1, "0301"}, {11, -1, "0302"}, {12, -1, "0400"}, {13, -1, "0401"}, {14, -1, "0402"},
    {15, -1, "0403"}, {16, -1, "0404"}, {17, -1, "0500"}, {18, -1, "0501"}, {19, -1, "0502"},
    {20, -1, "0503"}, {21, -1, "0504"}, {22, -1, "0600"}, {23, -1, "0601"}, {24, -1, "0602"},
    {25, -1, "0603"}, {26, -1, "0604"}, {-1, 0, "1"},     {-1, 1, "2"},     {-1, 2, "3"},
    {-1, 3, "4"},     {-1, 4, "5"},     {-1, 5, "6"},     {-1, 6, "7"},     {-1, 7, "8"},
    {-1, 8, "9"},     {-1, 9, "10"},
};

/* A line of the chart: from a stage's mission to the next stage, or to an ending (struct name and
 * member names inferred). */
struct UnkStoryLine_800BB3B4 {
    s32 m_stage;   /* index in the stage table */
    s32 m_mission;
    s32 m_ending;  /* -1 for a line between stages */
    const char *m_node;
};

static const UnkStoryLine_800BB3B4 s_lines[55] = {
    {-1, -1, 0, "hsline"},       {-1, -1, 1, "hsline2"},      {-1, -1, 2, "hsline3"},
    {-1, -1, 3, "hsline4"},      {-1, -1, 4, "hsline5"},      {-1, -1, 5, "hsline6"},
    {-1, -1, 6, "hsline7"},      {-1, -1, 7, "hsline8"},      {-1, -1, 8, "hsline9"},
    {-1, -1, 9, "hsline10"},     {5, 0, -1, "line100-200"},   {5, 1, -1, "line100-201"},
    {5, 2, -1, "line100-202"},   {6, 0, -1, "line200-300"},   {6, 2, -1, "line200-301"},
    {7, 0, -1, "line201-300"},   {7, 1, -1, "line201-301"},   {7, 2, -1, "line201-302"},
    {8, 0, -1, "line202-301"},   {8, 2, -1, "line202-302"},   {9, 0, -1, "line300-400"},
    {9, 1, -1, "line300-401"},   {9, 2, -1, "line300-402"},   {10, 0, -1, "line301-401"},
    {10, 1, -1, "line301-402"},  {10, 2, -1, "line301-403"},  {11, 0, -1, "line302-402"},
    {11, 1, -1, "line302-403"},  {11, 2, -1, "line302-404"},  {12, 0, -1, "line400-500"},
    {12, 2, -1, "line400-501"},  {13, 0, -1, "line401-500"},  {13, 1, -1, "line401-501"},
    {13, 2, -1, "line401-502"},  {14, 0, -1, "line402-501"},  {14, 1, -1, "line402-502"},
    {14, 2, -1, "line402-503"},  {15, 0, -1, "line403-502"},  {15, 1, -1, "line403-503"},
    {15, 2, -1, "line403-504"},  {16, 0, -1, "line404-503"},  {16, 2, -1, "line404-504"},
    {17, 0, -1, "line500-600"},  {17, 1, -1, "line500-601"},  {18, 0, -1, "line501-600"},
    {18, 1, -1, "line501-601"},  {18, 2, -1, "line501-602"},  {19, 0, -1, "line502-601"},
    {19, 1, -1, "line502-602"},  {19, 2, -1, "line502-603"},  {20, 0, -1, "line503-602"},
    {20, 1, -1, "line503-603"},  {20, 2, -1, "line503-604"},  {21, 1, -1, "line504-603"},
    {21, 2, -1, "line504-604"},
};

/* The nodes of the three player icons and of their shadows (names inferred). */
static const char *s_iconNodes[3][2] = {
    {"dark", "dark_shadow"},
    {"normal", "normal_shadow"},
    {"hero", "hero_shadow"},
};

/* Not original: an unused table that pools the unit's .rodata strings in retail's order (the
 * linker drops the table; the literals below reuse these copies). The .rodata is kept whole, so it
 * also holds the RTTI names of the CSD headers' inline classes. */
static const char *s_rodataPool[] = {
    "SRIconsOpen",
    "SRIconsClose",
    "Not inititalized m_eDealloctor\n",
    "csdRCObject.h",
    "line%3d-%3d",
    "Root/StoryRoute",
    "StoryRoute",
    "SRIconsBG",
    "Chao::CSD::RCPtr<Chao::CSD::CSceneList>::RCObject<Chao::CSD::CSceneList>::CFunctorBase",
    "Chao::CSD::CBase",
    "Chao::CSD::RCPtr<Chao::CSD::CSceneList>::RCObject<Chao::CSD::CSceneList>",
    "Chao::CSD::RCPtr<Chao::CSD::CScene>::RCObject<Chao::CSD::CScene>::CFunctorBase",
    "Chao::CSD::RCPtr<Chao::CSD::CScene>::RCObject<Chao::CSD::CScene>",
    "Chao::CSD::RCPtr<Chao::CSD::CNode>::RCObject<Chao::CSD::CNode>::CFunctorBase",
    "Chao::CSD::RCPtr<Chao::CSD::CNode>::RCObject<Chao::CSD::CNode>",
};

/* The colours of the chart (meanings inferred). */
static const u32 s_reachedColor = 0x5499DBC0;
static const u32 s_unreachedColor = 0x96969680;
static const u32 s_storyColor = 0xFFFFFFC0;
static const u32 s_unplayedColor = 0x828296FF;

/* The stage table's entry at index (name inferred; the table is reached through its owner, which
 * the lookup ignores). */
inline const UnkStageInfo_804C4A18 *StageInfo(int index) {
    fn_800B74B4();
    return &lbl_804C4A18[index];
}

typedef std::map<std::string, SavedataAnalyzer::UnkStoryLine_801ABDB8> LineMap;
typedef std::__tree<LineMap::value_type, LineMap::value_compare, std::less<std::string> > LineTree;

/* Not original: the value allocator's destroy for the chart's lines. Retail inlines the pair's
 * destructor there but calls it out of line from the tree's destroy (where the allocator's destroy
 * is itself inlined, one level up); GC/2.7 inlines at one level only, so the pair's destructor is
 * kept out of line (MSL_PAIR_DTOR_CALL) and this specialization spells out the inlined copy. */
template <> void LineTree::value_allocator::destroy(LineMap::value_type *p) {
    if (p != 0) {
        p->first.~basic_string();
    }
    std::__tree_nothing();
}

/* Whether flag bit is set (other units call it by its address). */
extern "C" bool fn_800BBDD8(const UnkFlags_801E5810 *flags, int bit) {
    return (flags->m_bits & (1 << bit)) != 0;
}

/* NonMatching (2 instructions): retail loads the manager's player and cursor into two registers
 * before storing either; GC/2.7 gives both r0 and stores the player before loading the cursor (the
 * same detail as folderPause's constructor). Locals in either order, stores in either order, struct
 * copies, an inline setter and pointer copies did not reproduce it. */
folderStoryRoute::folderStoryRoute(Chao::CSD::CProject *project) : m_check(0) {
    TPauseManager *manager = fn_800BADE8();
    m_managerState = manager->m_state;
    m_player = manager->m_player;
    m_cursor = manager->m_cursor;
    InitScenes(project);
    m_route.Reset();
    m_stages.Reset();
    m_icons.Reset();
    m_state = 0;
}

SavedataAnalyzer::StoryCheck::~StoryCheck() {}

folderStoryRoute::~folderStoryRoute() {}

void folderStoryRoute::InitScenes(Chao::CSD::CProject *project) {
    m_sceneList = fn_8036D2FC(project, "Root/StoryRoute", 0);
    if (m_sceneList != UnkCsdHandle_80042DF0(0)) {
        m_route.SetScene(fn_8036F470(m_sceneList.Get(), "StoryRoute"));
        m_stages.SetScene(fn_8036F470(m_sceneList.Get(), "SRIconsBG"));
        m_icons.SetScene(fn_8036F470(m_sceneList.Get(), "SRIcons"));
    }
}

void folderStoryRoute::UpdateStages() {
    UnkSaveData_800BB74C *save = fn_800BB74C();
    for (int i = 0; i < 32; i++) {
        const UnkStoryStage_800BB7A4 *entry = &s_stages[i];
        RCPtr<CNode> node = fn_803666B4(m_stages.GetScene().Get(), entry->m_node);
        if (node != RCPtr<CNode>(0)) {
            if (entry->m_ending == -1) {
                int count = save->m_history.m_count;
                int j;
                for (j = 0; j <= count; j++) {
                    if (save->m_history.m_route[j] == entry->m_stage) {
                        break;
                    }
                }
                if (j <= count) {
                    fn_8036AD14(node.Get(), 1);
                } else {
                    fn_8036AD14(node.Get(), 0);
                    UnkStageSave_800BB74C *stage = &save->m_history.m_stages[entry->m_stage];
                    if (!stage->m_missions[2].m_cleared && !stage->m_missions[1].m_cleared &&
                        !stage->m_missions[0].m_cleared) {
                        for (int corner = 0; corner < 4; corner++) {
                            fn_8036AC60(node.Get(), corner, &s_unplayedColor);
                        }
                    }
                }
            } else {
                fn_8036AD14(node.Get(), 0);
                if (!fn_800BBDD8(&save->m_history.m_endings, entry->m_ending)) {
                    for (int corner = 0; corner < 4; corner++) {
                        fn_8036AC60(node.Get(), corner, &s_unplayedColor);
                    }
                }
            }
        }
    }
}

/* The save data in use (other units call it by its address). */
extern "C" UnkSaveData_800BB74C *fn_800BB74C(void) {
    if (lbl_805EE014 == 0) {
        static UnkSaveHolder_801E559C s_holder;
        lbl_805EE014 = &s_holder;
    }
    return &lbl_805EE014->m_current;
}

void folderStoryRoute::UpdateLines() {
    UnkSaveData_800BB74C *save = fn_800BB74C();
    for (int i = 0; i < 55; i++) {
        const UnkStoryLine_800BB3B4 *entry = &s_lines[i];
        RCPtr<CNode> node = fn_803666B4(m_route.GetScene().Get(), entry->m_node);
        if (node != RCPtr<CNode>(0)) {
            if (entry->m_ending == -1) {
                if (save->m_history.m_stages[entry->m_stage].m_missions[entry->m_mission].m_cleared) {
                    for (int corner = 0; corner < 4; corner++) {
                        fn_8036AC60(node.Get(), corner, &s_reachedColor);
                    }
                } else {
                    for (int corner = 0; corner < 4; corner++) {
                        fn_8036AC60(node.Get(), corner, &s_unreachedColor);
                    }
                }
            } else if (fn_800BBDD8(&save->m_history.m_endings, entry->m_ending)) {
                for (int corner = 0; corner < 4; corner++) {
                    fn_8036AC60(node.Get(), corner, &s_reachedColor);
                }
            } else {
                for (int corner = 0; corner < 4; corner++) {
                    fn_8036AC60(node.Get(), corner, &s_unreachedColor);
                }
            }
        }
    }
    int i = 0;
    int index;
    int prev = -1;
    while (i <= save->m_history.m_count) {
        index = save->m_history.m_route[i];
        if (index > 27) {
            i++;
            continue;
        }
        fn_800B74B4();
        int id = lbl_804C4A18[index].m_id;
        if (prev != -1) {
            char name[16];
            sprintf(name, "line%3d-%3d", prev, id);
            RCPtr<CNode> node = fn_803666B4(m_route.GetScene().Get(), name);
            if (node != RCPtr<CNode>(0)) {
                for (int corner = 0; corner < 4; corner++) {
                    fn_8036AC60(node.Get(), corner, &s_storyColor);
                }
            }
        }
        prev = id;
        i++;
    }
}

void folderStoryRoute::UpdateIcons() {
    int stage = lbl_8057D6A0.m_stage;
    const UnkStageInfo_804C4A18 *info = StageInfo(stage);
    for (int i = 0; i < 3; i++) {
        const UnkStageInfo_804C4A18 *next = StageInfo(info->m_missions[i].field_0x0);
        if (lbl_8057D6A0.field_0xA8 >= 600) {
            RCPtr<CNode> node;
            node = fn_803666B4(m_icons.GetScene().Get(), s_iconNodes[i][0]);
            fn_8036AD78(node.Get(), 1);
            node = fn_803666B4(m_icons.GetScene().Get(), s_iconNodes[i][1]);
            fn_8036AD78(node.Get(), 1);
        } else {
            if (next->m_flags & 1) {
                next = StageInfo(next->m_missions[i].field_0x0);
            }
            if (next != 0) {
                char name[16];
                sprintf(name, "%04d", next->m_id);
                RCPtr<CNode> position = fn_803666B4(m_icons.GetScene().Get(), name);
                if (position != RCPtr<CNode>(0)) {
                    RCPtr<CNode> icon;
                    icon = fn_803666B4(m_icons.GetScene().Get(), s_iconNodes[i][0]);
                    UnkCsdPosition_8036AE98 xy = fn_8036AE98(position.Get());
                    fn_8036AE70(icon.Get(), &xy);
                    icon = fn_803666B4(m_icons.GetScene().Get(), s_iconNodes[i][1]);
                    UnkCsdPosition_8036AE98 xy2 = fn_8036AE98(position.Get());
                    UnkCsdPosition_8036AE98 xy3 = fn_8036AE98(position.Get());
                    icon.Get()->SetPosition(2.0f + xy2.x, 2.0f + xy3.y);
                }
            }
        }
    }
}

bool folderStoryRoute::Update() {
    switch (m_state) {
    case 0:
        if (lbl_805EF9F4->m_state == 4) {
            m_managerState = lbl_805EF9F4->m_state;
            fn_801ABDB8(&m_check);
            m_route.Play("Open", 1);
            m_stages.Play("SRIconsOpen", 1);
            m_icons.Play("SRIconsOpen", 1);
            UpdateStages();
            UpdateLines();
            UpdateIcons();
            m_state = 1;
        }
        break;
    case 1:
        if (fn_8003DBB0(&m_route)) {
            m_state = 2;
        }
        break;
    case 2:
        if (lbl_805EF9F4->m_state != 4) {
            m_managerState = lbl_805EF9F4->m_state;
            m_route.Play("Close", 1);
            m_stages.Play("SRIconsClose", 1);
            m_icons.Play("SRIconsClose", 1);
            m_state = 3;
        }
        break;
    case 3:
        if (fn_8003DBB0(&m_route)) {
            m_managerState = lbl_805EF9F4->m_state;
            m_state = 0;
        }
        break;
    }
    int busy = m_route.Update(false);
    busy += m_stages.Update(false);
    busy += m_icons.Update(false);
    return busy != 0;
}

#include <map_impl>
