/* AdvReal::Synopsis::Phase (class name from RTTI): the front-end phase that recaps the story so far
 * before a story is continued. Its state lives in a Synopsis::Impl (class name inferred) that plays
 * the CSD project "csdFiles/Synopsis": it builds a list of message lines from the stages of the last
 * story played (the save data's history), scrolls them up the screen while their voices play,
 * switches the background pattern to the stage each line is about, and then starts the story. The
 * file name and all names marked "(name inferred)" are not original. Definitions are in reverse
 * address order (deferred inlining emits functions last-defined first). */
#include <boost/scoped_ptr.hpp>
#include <string.h>

#include "Game/AdvBase.h"
#include "Game/AdvPhaseManager.h"
#include "Game/CsdRCPtr.h"
#include "Game/CsdSceneCtrl.h"
#include "Game/CsdSceneSet.h"
#include "Game/StageSequence.h"
#include <dolphin/types.h>
#include "size_assert.h"

using Chao::CSD::CNode;
using Chao::CSD::CScene;
using Chao::CSD::CTexList;
using Chao::CSD::RCPtr;

/* The loaded CSD project (class name unknown; declared as in TGi.h). */
class UnkCsdResource_8003E808;

/* The value a project returns from fn_8036D598 (class name unknown; declared as in TGi.h). */
class UnkCsdHandle_80042DF0 {
  public:
    UnkCsdHandle_80042DF0(const UnkCsdHandle_80042DF0 &other);
    ~UnkCsdHandle_80042DF0();

    /* The object the handle holds (as in TGi.h). */
    void *Get() const {
        return m_object != 0 ? *(void **)m_object : 0;
    }

    /* 0x0 */ void *m_object;
};

/* An empty project handle (meaning inferred; see Game/ResultTask.cpp). */
extern UnkCsdHandle_80042DF0 lbl_805EE114;
/* An empty texture list (meaning inferred; see Game/AdvOption.cpp). */
extern RCPtr<CTexList> lbl_805EE110;

/* The save data (struct name unknown; see Game/AdvStoryLibrary.cpp). Only the members read here are
 * named. */
struct UnkSaveData_800BB74C {
    /* 0x00 */ u8 field_0x0[0x30];
    /* 0x30 */ s32 m_historyCount; /* the stages of the last story played (meaning inferred) */
    /* 0x34 */ s32 m_history[25];
    /* 0x98 */ s32 m_finalRoute; /* (meaning inferred) */
};

/* The game state (struct name unknown; see Game/AdvMemoryCheck.cpp). */
struct UnkGameState_8057D6A0 {
    /* 0x000 */ u8 field_0x0[0x18];
    /* 0x018 */ float m_dt; /* (name inferred) */
    /* 0x01C */ u8 field_0x1C[0x5C - 0x1C];
    /* 0x05C */ s32 m_voiceLanguage; /* (meaning inferred) */
    /* 0x060 */ u8 field_0x60[0xCC - 0x60];
    /* 0x0CC */ u8 m_record[0x148 - 0xCC]; /* (see Game/ScoreData.cpp) */
    /* 0x148 */ u8 field_0x148[3];
    /* 0x14B */ u8 field_0x14B[0x150 - 0x14B];
};
SIZE_ASSERT(UnkGameState_8057D6A0, 0x150);

extern UnkGameState_8057D6A0 lbl_8057D6A0;

/* The stage sequence's choices (struct name unknown; see Game/StageSequencePlay2POnce.cpp). */
struct UnkStageInfo_80584720 {
    /* 0x000 */ u8 field_0x0[0x4];
    /* 0x004 */ s32 m_storyMode; /* (meaning inferred) */
    /* 0x008 */ u8 field_0x8[0x230 - 0x8];
};

extern UnkStageInfo_80584720 lbl_80584720;

class UnkManager_801E5D48;

extern "C" {
/* The CSD project cache (Game/AdvCsdCache.cpp; behaviour inferred). */
void *fn_801E5F0C(void);
UnkCsdResource_8003E808 *fn_802B5504(void *cache, const char *file);
bool fn_802B5434(void *cache, UnkCsdResource_8003E808 *project);
/* Projects, scenes and nodes (behaviour inferred; see Game/AdvStoryLibrary.cpp). */
UnkCsdHandle_80042DF0 fn_8036D598(UnkCsdResource_8003E808 *project, int arg1);
RCPtr<CTexList> fn_8036C628(UnkCsdResource_8003E808 *project);
RCPtr<CNode> fn_803666B4(CScene *scene, const char *name);
void fn_8036AD14(CNode *node, int pattern);

/* The text manager: the alpha of the next text, and drawing message id at a position (behaviour
 * inferred; see Game/AdvMessage.cpp). */
UnkManager_801E5D48 *fn_801E5D48(void);
void fn_802BEC5C(UnkManager_801E5D48 *manager, float alpha, int color);
bool fn_802BEEC8(UnkManager_801E5D48 *manager, s32 id, s32 branch, float x, float y, bool right);

/* The voice manager and muting it (behaviour inferred; see Game/AdvMovie.cpp). */
void *fn_801E5CA8(void);
void fn_802C839C(void *manager, int arg1);

/* The save data (see Game/AdvStoryLibrary.cpp). */
UnkSaveData_800BB74C *fn_800BB74C(void);

/* Starting a story (behaviour inferred; see Game/AdvStoryLibrary.cpp). */
void *fn_801E2128(void);
void fn_801E5858(void *object, int arg1);
void fn_802062FC(void *record);
void fn_800CBBFC(void *dest, int value, int count);

/* The sound manager, playing a voice and playing a sound (see Game/AdvBattleMode.cpp). */
void *fn_80053164(void);
void fn_801D51A0(void *manager, int id, int arg2, int arg3);
void fn_801D560C(void *manager, int soundId, int arg2, int arg3);
/* The screen fade (see Game/AdvOption.cpp). */
void fn_80173834(int arg0, float time);
void fn_80173878(int arg0, int arg1, float time);
int fn_80044E10(void);

/* Pad input (Game/AdvPadInput.cpp). */
bool fn_802B7D5C(int button, int pad);
}

/* A stage of the story (struct name inferred): its message lines by the route taken out of it. */
struct StageEntry {
    /* 0x00 */ u8 m_final;   /* the last stages of a story (meaning inferred) */
    /* 0x01 */ s8 m_route;   /* the route that leads into it (meaning inferred) */
    /* 0x02 */ s8 m_pattern; /* the background pattern (meaning inferred) */
    /* 0x04 */ s16 m_messages[8];
};

/* A message's voice and its display time per voice language (struct name inferred). */
struct MessageEntry {
    /* 0x0 */ s16 m_voice;
    /* 0x4 */ float m_time[2];
};

/* The messages that end each story (indexed by the story's ending, from 6; meaning inferred). */
static s16 s_endingMessages[39][3] = {
    {0x18E5, 0x18E6, 0x18E7},
    {0x18E5, 0x18E6, 0x18E7},
    {0x18E5, 0x18E6, 0x18E7},
    {0x18E8, 0x18E9, 0x18EA},
    {0x18E8, 0x18E9, 0x18EA},
    {0x18E8, 0x18E9, 0x18EA},
    {0x18EB, 0x18EC, 0},
    {0x18EB, 0x18EC, 0},
    {0x18EB, 0x18EC, 0},
    {0x18EB, 0x18EC, 0},
    {0x18EB, 0x18EC, 0},
    {0x18ED, 0x18EE, 0},
    {0x18ED, 0x18EE, 0},
    {0x18ED, 0x18EE, 0},
    {0x18ED, 0x18EE, 0},
    {0x18ED, 0x18EE, 0},
    {0x18EF, 0x18F0, 0},
    {0x18EF, 0x18F0, 0},
    {0x18EF, 0x18F0, 0},
    {0x18EF, 0x18F0, 0},
    {0x18EF, 0x18F0, 0},
    {0, 0, 0},
    {0x18E5, 0x18E6, 0x18E7},
    {0x18E8, 0x18E9, 0x18EA},
    {0x18EB, 0x18EC, 0},
    {0x18EB, 0x18EC, 0},
    {0x18EB, 0x18EC, 0},
    {0x18ED, 0x18EE, 0},
    {0x18ED, 0x18EE, 0},
    {0x18F1, 0, 0},
    {0x18F1, 0, 0},
    {0x18F1, 0, 0},
    {0x18F1, 0, 0},
    {0x18F1, 0, 0},
    {0x18F1, 0, 0},
    {0x18F1, 0, 0},
    {0x18F1, 0, 0},
    {0x18F1, 0, 0},
    {0x18F1, 0, 0},
};

/* The stages of the story (indexed by stage id, from 5; meaning inferred). */
static StageEntry s_stageEntries[39] = {
    {0, 0, 0, {0x18F6, 0x18F2, 0x18FA, -1, -1, -1, -1, -1}},
    {0, 0, 1, {0x1926, 0x192A, -1, -1, -1, -1, -1, -1}},
    {0, 1, 2, {0x1912, 0x190E, 0x1916, -1, -1, -1, -1, -1}},
    {0, 2, 3, {-1, 0x18FE, 0x1902, -1, -1, 0x18FE, -1, 0x1902}},
    {0, 0, 4, {0x193E, 0x193A, 0x1942, -1, -1, 0x193E, 0x193A, 0x1942}},
    {0, 1, 5, {-1, 0x1956, 0x1952, 0x195A, -1, -1, -1, -1}},
    {0, 2, 6, {-1, -1, 0x194A, 0x1946, 0x194E, -1, -1, -1}},
    {0, 0, 7, {0x1906, 0x190A, -1, -1, -1, -1, -1, -1}},
    {0, 1, 8, {0x1976, 0x1972, 0x197A, -1, -1, 0x1976, 0x1972, 0x197A}},
    {0, 2, 9, {-1, 0x191E, 0x191A, 0x1922, -1, -1, -1, -1}},
    {0, 3, 10, {-1, -1, 0x1932, 0x192E, 0x1936, 0x1932, 0x192E, 0x1936}},
    {0, 4, 11, {-1, -1, -1, 0x195E, 0x1962, 0x195E, -1, 0x1962}},
    {0, 0, 12, {0x1982, 0x197E, -1, -1, -1, 0x1982, 0x197E, -1}},
    {0, 1, 13, {0x19A4, 0x19A0, 0x19A8, -1, -1, -1, -1, -1}},
    {0, 2, 14, {-1, 0x196A, 0x1966, 0x196E, -1, 0x196A, 0x1966, 0x196E}},
    {0, 3, 15, {-1, -1, 0x198A, 0x1986, 0x198E, -1, -1, -1}},
    {0, 4, 16, {-1, -1, -1, 0x1992, 0x1996, -1, -1, -1}},
    {0, 0, 17, {-1, -1, -1, -1, -1, 0x19AC, -1, 0x19AF}},
    {0, 1, 18, {-1, -1, -1, -1, -1, 0x19B8, -1, 0x19BB}},
    {0, 2, 19, {-1, -1, -1, -1, -1, 0x19B2, -1, 0x19B5}},
    {0, 3, 20, {-1, -1, -1, -1, -1, 0x199A, -1, 0x199D}},
    {0, 4, 21, {-1, -1, -1, -1, -1, 0x19BE, -1, 0x19C1}},
    {0, 0, -1, {-1, -1, -1, -1, -1, -1, -1, -1}},
    {1, 0, 3, {0x19C4, -1, -1, -1, -1, -1, -1, -1}},
    {1, 0, 4, {0x19CC, -1, -1, -1, -1, -1, -1, -1}},
    {1, 0, 8, {0x19D8, -1, -1, -1, -1, -1, -1, -1}},
    {1, 0, 10, {0x19D0, -1, -1, -1, -1, -1, -1, -1}},
    {1, 0, 11, {0x19C8, -1, -1, -1, -1, -1, -1, -1}},
    {1, 0, 12, {0x19DC, -1, -1, -1, -1, -1, -1, -1}},
    {1, 0, 14, {0x19D4, -1, -1, -1, -1, -1, -1, -1}},
    {1, 0, -1, {-1, -1, -1, -1, -1, -1, -1, -1}},
    {1, 0, -1, {-1, -1, -1, -1, -1, -1, -1, -1}},
    {1, 0, -1, {-1, -1, -1, -1, -1, -1, -1, -1}},
    {1, 0, -1, {-1, -1, -1, -1, -1, -1, -1, -1}},
    {1, 0, -1, {-1, -1, -1, -1, -1, -1, -1, -1}},
    {1, 0, -1, {-1, -1, -1, -1, -1, -1, -1, -1}},
    {1, 0, -1, {-1, -1, -1, -1, -1, -1, -1, -1}},
    {1, 0, -1, {-1, -1, -1, -1, -1, -1, -1, -1}},
    {1, 0, -1, {-1, -1, -1, -1, -1, -1, -1, -1}},
};

/* The voice and display time of each message, from message 0x18E5 on (meaning inferred). */
static MessageEntry s_messageEntries[251] = {
    {0x273, {7.707f, 6.582f}},
    {0x274, {5.681f, 5.117f}},
    {0x275, {9.368f, 8.279f}},
    {0x276, {3.478f, 3.796f}},
    {0x277, {6.271f, 4.768f}},
    {0x278, {5.425f, 6.193f}},
    {0x279, {5.492f, 4.56f}},
    {0x27A, {4.648f, 6.012f}},
    {0x27B, {3.54f, 2.61f}},
    {0x27C, {7.529f, 6.407f}},
    {0x27D, {5.128f, 6.374f}},
    {0x27E, {6.864f, 5.39f}},
    {0x27F, {4.769f, 3.708f}},
    {0xE3F, {2.931f, 3.731f}},
    {0xE40, {4.417f, 4.036f}},
    {0xE41, {5.621f, 4.683f}},
    {0xE42, {3.707f, 4.107f}},
    {0xE43, {2.637f, 3.689f}},
    {0xE44, {4.566f, 5.053f}},
    {0xE45, {3.78f, 3.187f}},
    {0xE46, {4.487f, 6.429f}},
    {0xE47, {6.798f, 5.539f}},
    {0xE48, {4.339f, 4.509f}},
    {0xE49, {4.692f, 5.16f}},
    {0xE4A, {6.397f, 4.858f}},
    {0xE88, {5.812f, 4.321f}},
    {0xE89, {7.148f, 5.156f}},
    {0xE8A, {6.543f, 5.347f}},
    {0xE8B, {3.316f, 2.662f}},
    {0xE8C, {5.986f, 5.082f}},
    {0xE8D, {4.386f, 4.651f}},
    {0xE8E, {6.322f, 5.053f}},
    {0xE8F, {4.634f, 3.887f}},
    {0xEE1, {4.35f, 2.968f}},
    {0xEE2, {9.91f, 7.019f}},
    {0xEE3, {10.44f, 6.912f}},
    {0xEE4, {5.705f, 4.923f}},
    {0xEE5, {3.59f, 2.643f}},
    {0xEE6, {6.805f, 5.804f}},
    {0xEE7, {9.142f, 6.012f}},
    {0xEE8, {6.062f, 6.713f}},
    {0xD19, {4.504f, 3.414f}},
    {0xD1A, {5.755f, 6.439f}},
    {0xD1B, {5.436f, 6.063f}},
    {0xD1C, {4.785f, 4.078f}},
    {0xD1D, {4.504f, 3.414f}},
    {0xD1E, {7.505f, 6.413f}},
    {0xD1F, {6.786f, 5.668f}},
    {0xD20, {7.0f, 5.312f}},
    {0xD21, {4.504f, 3.414f}},
    {0xD22, {7.692f, 6.685f}},
    {0xD23, {4.886f, 4.858f}},
    {0xD24, {8.076f, 6.037f}},
    {0xD5E, {5.436f, 5.105f}},
    {0xD5F, {5.58f, 3.964f}},
    {0xD60, {4.096f, 4.369f}},
    {0xD61, {5.327f, 5.208f}},
    {0xD62, {5.436f, 5.105f}},
    {0xD63, {4.762f, 3.938f}},
    {0xD64, {4.852f, 3.511f}},
    {0xD65, {5.041f, 3.887f}},
    {0xD66, {5.436f, 5.105f}},
    {0xD67, {6.117f, 6.335f}},
    {0xD68, {3.44f, 2.921f}},
    {0xD69, {5.644f, 5.224f}},
    {0xF30, {5.088f, 5.13f}},
    {0xF31, {7.867f, 5.247f}},
    {0xF32, {4.753f, 3.867f}},
    {0xF33, {5.565f, 4.849f}},
    {0xF34, {5.416f, 4.852f}},
    {0xF35, {5.165f, 5.079f}},
    {0xF36, {6.799f, 5.117f}},
    {0xF37, {5.054f, 5.117f}},
    {0xFB6, {5.455f, 4.198f}},
    {0xFB7, {4.5f, 3.984f}},
    {0xFB8, {3.434f, 3.692f}},
    {0xFB9, {6.81f, 5.558f}},
    {0xFBA, {6.697f, 4.586f}},
    {0xFBB, {7.737f, 6.109f}},
    {0xFBC, {5.408f, 5.467f}},
    {0xFBD, {5.613f, 4.23f}},
    {0xFBE, {5.826f, 3.589f}},
    {0xFBF, {3.914f, 3.835f}},
    {0xFC0, {5.618f, 4.858f}},
    {0xFC1, {6.331f, 5.124f}},
    {0x10F1, {5.039f, 3.718f}},
    {0x10F2, {5.233f, 5.221f}},
    {0x10F3, {4.654f, 4.904f}},
    {0x10F4, {4.225f, 3.874f}},
    {0x10F5, {4.0f, 4.709f}},
    {0x10F6, {5.716f, 5.869f}},
    {0x10F7, {3.862f, 3.99f}},
    {0x10F8, {5.909f, 5.973f}},
    {0x10F9, {5.952f, 6.284f}},
    {0x10FA, {6.77f, 6.543f}},
    {0x10FB, {5.462f, 5.26f}},
    {0x10FC, {5.277f, 4.288f}},
    {0x113F, {6.58f, 6.128f}},
    {0x1140, {7.165f, 6.154f}},
    {0x1141, {4.425f, 4.755f}},
    {0x1142, {4.893f, 5.143f}},
    {0x1143, {6.254f, 5.169f}},
    {0x1144, {8.243f, 5.739f}},
    {0x1145, {4.706f, 4.107f}},
    {0x1146, {7.242f, 5.662f}},
    {0x1147, {5.807f, 5.286f}},
    {0x1148, {6.748f, 5.817f}},
    {0x1149, {3.975f, 4.609f}},
    {0x114A, {4.808f, 5.205f}},
    {0x1007, {5.23f, 3.167f}},
    {0x1008, {6.041f, 4.042f}},
    {0x1009, {6.144f, 4.029f}},
    {0x100A, {4.576f, 3.53f}},
    {0x100B, {5.293f, 2.973f}},
    {0x100C, {5.821f, 3.793f}},
    {0x100D, {5.224f, 3.964f}},
    {0x100E, {6.25f, 5.234f}},
    {0x100F, {7.864f, 4.56f}},
    {0x1010, {5.199f, 4.703f}},
    {0x1011, {4.668f, 4.204f}},
    {0x1012, {8.189f, 5.856f}},
    {0x1048, {4.057f, 4.113f}},
    {0x1049, {6.907f, 5.208f}},
    {0x104A, {6.356f, 6.504f}},
    {0x104B, {8.599f, 5.804f}},
    {0x104C, {5.366f, 5.526f}},
    {0x104D, {8.61f, 5.921f}},
    {0x104E, {4.469f, 4.055f}},
    {0x104F, {7.636f, 5.312f}},
    {0x1096, {4.138f, 4.081f}},
    {0x1097, {5.162f, 4.058f}},
    {0x1098, {5.114f, 4.415f}},
    {0x1099, {6.129f, 5.454f}},
    {0x109A, {7.637f, 5.934f}},
    {0x109B, {5.267f, 5.053f}},
    {0x109C, {4.284f, 4.599f}},
    {0x109D, {4.69f, 3.977f}},
    {0x109E, {7.637f, 5.934f}},
    {0x109F, {4.91f, 4.45f}},
    {0x10A0, {4.759f, 5.286f}},
    {0x10A1, {4.628f, 3.958f}},
    {0x997, {5.096f, 5.772f}},
    {0x998, {8.058f, 5.947f}},
    {0x999, {6.073f, 4.146f}},
    {0x99A, {6.666f, 6.219f}},
    {0x99B, {5.096f, 5.772f}},
    {0x99C, {6.396f, 4.21f}},
    {0x99D, {8.438f, 6.996f}},
    {0x99E, {7.6f, 6.789f}},
    {0x99F, {5.887f, 6.854f}},
    {0x9A0, {5.268f, 5.027f}},
    {0x9A1, {7.949f, 7.268f}},
    {0x9A2, {7.66f, 6.867f}},
    {0x9CF, {7.318f, 6.167f}},
    {0x9D0, {5.331f, 4.742f}},
    {0x9D1, {4.686f, 5.73f}},
    {0x9D2, {8.0f, 5.338f}},
    {0x9D3, {7.318f, 6.167f}},
    {0x9D4, {5.904f, 6.964f}},
    {0x9D5, {6.965f, 6.867f}},
    {0x9D6, {7.841f, 7.644f}},
    {0xA17, {7.819f, 6.633f}},
    {0xA18, {6.661f, 5.921f}},
    {0xA19, {9.059f, 7.877f}},
    {0xA1A, {4.809f, 4.405f}},
    {0xA1B, {4.508f, 4.509f}},
    {0xA1C, {8.196f, 6.62f}},
    {0xA1D, {9.077f, 8.162f}},
    {0xA1E, {6.756f, 5.39f}},
    {0xA1F, {5.022f, 3.55f}},
    {0xA20, {6.407f, 6.465f}},
    {0xA21, {7.406f, 7.061f}},
    {0xA22, {5.686f, 5.623f}},
    {0xA57, {8.408f, 6.491f}},
    {0xA58, {8.774f, 6.471f}},
    {0xA59, {8.417f, 7.54f}},
    {0xA5A, {8.771f, 5.75f}},
    {0xA5B, {8.689f, 6.765f}},
    {0xA5C, {7.983f, 5.426f}},
    {0xA5D, {4.934f, 6.003f}},
    {0xA5E, {8.566f, 5.429f}},
    {0xAA1, {4.862f, 5.595f}},
    {0xAA2, {6.825f, 5.891f}},
    {0xAA3, {5.009f, 5.334f}},
    {0xAA4, {4.855f, 4.904f}},
    {0xAA5, {7.188f, 5.933f}},
    {0xAA6, {5.7f, 5.137f}},
    {0xAFB, {5.318f, 5.017f}},
    {0xAFC, {4.858f, 3.593f}},
    {0xAFD, {5.603f, 4.453f}},
    {0xAFE, {8.221f, 6.229f}},
    {0xAFF, {4.804f, 5.073f}},
    {0xB00, {7.864f, 7.032f}},
    {0xB01, {4.551f, 5.679f}},
    {0xB02, {7.232f, 5.82f}},
    {0xB03, {5.765f, 5.75f}},
    {0xB04, {6.946f, 5.218f}},
    {0xB05, {6.741f, 6.37f}},
    {0xB06, {8.643f, 6.757f}},
    {0xB3D, {4.097f, 3.815f}},
    {0xB3E, {7.047f, 6.267f}},
    {0xB3F, {7.3f, 5.616f}},
    {0xB40, {7.235f, 6.06f}},
    {0xB41, {5.269f, 3.755f}},
    {0xB42, {7.128f, 5.221f}},
    {0xAAD, {5.177f, 6.003f}},
    {0xAAE, {8.233f, 6.839f}},
    {0xAAF, {6.815f, 6.292f}},
    {0xAB0, {7.65f, 5.552f}},
    {0xAB1, {7.868f, 4.594f}},
    {0xAB2, {5.594f, 5.214f}},
    {0xD88, {5.784f, 4.411f}},
    {0xD89, {9.114f, 6.314f}},
    {0xD8A, {6.409f, 5.609f}},
    {0xD8B, {5.359f, 5.722f}},
    {0xD8C, {7.271f, 6.567f}},
    {0xD8D, {4.931f, 4.059f}},
    {0xDCA, {5.657f, 4.256f}},
    {0xDCB, {9.241f, 6.779f}},
    {0xDCC, {4.745f, 4.798f}},
    {0xDCD, {7.552f, 6.018f}},
    {0xDCE, {7.528f, 5.436f}},
    {0xDCF, {5.135f, 5.045f}},
    {0xB8A, {5.265f, 4.636f}},
    {0xB8B, {5.716f, 4.453f}},
    {0xB8C, {5.07f, 3.671f}},
    {0xB8D, {6.119f, 4.756f}},
    {0xB9D, {5.731f, 4.77f}},
    {0xB9E, {7.041f, 6.229f}},
    {0xB9F, {5.808f, 5.838f}},
    {0xBA0, {4.802f, 4.608f}},
    {0xB7A, {3.47f, 4.805f}},
    {0xB7B, {3.834f, 3.537f}},
    {0xB7C, {5.518f, 5.39f}},
    {0xB7D, {5.164f, 5.299f}},
    {0xBC0, {3.47f, 4.805f}},
    {0xBC1, {8.387f, 5.83f}},
    {0xBC2, {7.781f, 5.594f}},
    {0xBC3, {5.675f, 4.405f}},
    {0xBD4, {5.311f, 2.821f}},
    {0xBD5, {7.105f, 6.122f}},
    {0xBD6, {8.96f, 7.719f}},
    {0xBD7, {5.603f, 4.824f}},
    {0xBE9, {4.893f, 4.876f}},
    {0xBEA, {9.072f, 7.106f}},
    {0xBEB, {5.037f, 4.642f}},
    {0xBEC, {5.155f, 4.798f}},
    {0xBFF, {4.945f, 3.703f}},
    {0xC00, {5.769f, 5.098f}},
    {0xC01, {8.37f, 7.551f}},
    {0xC02, {7.281f, 5.848f}},
};

/* The CSD project of the phase. */
static const char *s_csdFile = "csdFiles/Synopsis";

/* The scenes of the project. */
static const char *const s_sceneNames[3] = {"Synopsis", "Synopsis2", "Button"};

namespace AdvReal {
namespace Synopsis {

/* A line of the recap (struct name inferred). */
struct Line {
    /* 0x00 */ float m_x;
    /* 0x04 */ float m_y;
    /* 0x08 */ float m_gap; /* the space to keep below the previous line */
    /* 0x0C */ s32 m_message;
    /* 0x10 */ s32 m_pattern;
    /* 0x14 */ float m_alpha;
    /* 0x18 */ s32 m_voice;
    /* 0x1C */ float m_time;
    /* 0x20 */ Line *m_next;
    /* 0x24 */ Line *m_prev;
};
SIZE_ASSERT(Line, 0x28);

/* The phase's state (class name inferred). */
class Impl {
  public:
    Impl();
    ~Impl() {
        Line *line = m_head;
        if (line != 0) {
            while (line != 0) {
                Line *next = line->m_next;
                delete line;
                line = next;
            }
        }
    }

    bool Tick();
    void Start();
    void Update();
    bool Draw();
    bool Load();
    void StartStory();
    void UpdatePattern();
    void BuildLines();
    void UpdateLines();
    void DrawLines();

    /* Appends a new line at the bottom of the screen (name inferred). */
    Line *AddLine() {
        Line *line = new Line;
        if (m_head == 0) {
            m_tail = line;
            m_head = line;
            line->m_next = 0;
            line->m_prev = 0;
        } else {
            Line *last = m_head;
            while (last->m_next != 0) {
                last = last->m_next;
            }
            last->m_next = line;
            line->m_prev = last;
            line->m_next = 0;
            m_tail = line;
        }
        line->m_alpha = 0.0f;
        line->m_x = 100.0f;
        line->m_y = 380.0f;
        line->m_gap = 80.0f;
        return line;
    }

    /* Member names are inferred. */
    /* 0x00 */ s32 m_state; /* 0 loading, 1 running, 2 done */
    /* 0x04 */ s32 m_step;
    /* 0x08 */ UnkCsdResource_8003E808 *m_project;
    /* 0x0C */ UnkCsdSceneSet_8003D5D8 m_scenes;
    /* 0x18 */ u8 m_busy;
    /* 0x1C */ Line *m_head;
    /* 0x20 */ Line *m_tail;
    /* 0x24 */ s32 m_pattern;
    /* 0x28 */ s32 m_nextPattern;
    /* 0x2C */ s32 m_scene; /* which of the two scenes shows the pattern; -1 none */
    /* 0x30 */ float m_speed;
    /* 0x34 */ float m_voiceWait;
};
SIZE_ASSERT(Impl, 0x38);

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

Impl::Impl() : m_state(0), m_head(0), m_pattern(-1), m_nextPattern(-1), m_scene(-1) {
    memset(&m_tail, 0, sizeof(m_tail));
    fn_80173834(1, 0.0f);
    fn_802C839C(fn_801E5CA8(), 2);
}

bool Impl::Tick() {
    switch (m_state) {
    case 0:
        Start();
        break;
    case 1:
        Update();
        break;
    }
    if (m_state == 2) {
        StartStory();
        fn_802B7528(fn_801E5E00(), 0);
        return false;
    }
    m_busy = fn_8003CDF4(&m_scenes);
    return true;
}

void Impl::Start() {
    if (Load()) {
        fn_8003CE5C(&m_scenes);
        m_state = 1;
        m_step = 0;
        fn_802C839C(fn_801E5CA8(), 2);
    } else {
        m_state = 2;
        m_step = 0;
    }
}

void Impl::Update() {
    switch (m_step) {
    case 0:
        fn_8003CEAC(&m_scenes, s_sceneNames[2])->Play("Open", 1);
        fn_8003C8D8(&m_scenes, s_sceneNames[2], true);
        fn_8003CEAC(&m_scenes, s_sceneNames[0])->Play("Open", 1);
        fn_8003C8D8(&m_scenes, s_sceneNames[0], true);
        m_scene = 0;
        m_pattern = m_nextPattern = 0x16;
        {
            RCPtr<CNode> node = fn_803666B4(fn_8003CEAC(&m_scenes, s_sceneNames[0])->GetScene().Get(), "bg");
            fn_8036AD14(node.Get(), m_pattern);
        }
        m_step = 1;
        break;
    case 1:
        if (!m_busy) {
            BuildLines();
            m_step = 2;
        }
        break;
    case 2:
        if (fn_802B7D5C(0, 4)) {
            m_step = 3;
            fn_801D560C(fn_80053164(), 0xE004, 0, 0);
            fn_80173878(1, 1, 1.0f);
        }
        if (m_tail->m_voice == 0 && m_tail->m_alpha <= 0.0f) {
            m_step = 3;
            fn_8003CEAC(&m_scenes, s_sceneNames[0])->Play("Close", 1);
            fn_80173878(1, 1, 1.0f);
        }
        break;
    case 3:
        if (fn_80044E10() == 1) {
            StartStory();
            fn_802B7528(fn_801E5E00(), 0);
        }
        break;
    }
    UpdateLines();
    UpdatePattern();
}

bool Impl::Draw() {
    if (!fn_802B5434(fn_801E5F0C(), m_project)) {
        return false;
    }
    if (SameTexList(fn_8036C628(m_project), lbl_805EE110)) {
        return false;
    }
    fn_8003C92C(&m_scenes, fn_8036C628(m_project));
    DrawLines();
    return true;
}

bool Impl::Load() {
    m_project = fn_802B5504(fn_801E5F0C(), s_csdFile);
    if (m_project != 0) {
        UnkCsdHandle_80042DF0 project = fn_8036D598(m_project, 0);
        void *object = project.Get();
        void *empty = lbl_805EE114.Get();
        if (object != empty) {
            if (fn_8003CEF8(&m_scenes, project, s_sceneNames, 3)) {
                return true;
            }
        }
    }
    return false;
}

void Impl::StartStory() {
    fn_801E5858(fn_801E2128(), 5);
    StageSequence::Manager::SetNext(1);
    lbl_80584720.m_storyMode = 1;
    fn_802062FC(lbl_8057D6A0.m_record);
    fn_800CBBFC(lbl_8057D6A0.field_0x148, 1, 3);
}

void Impl::UpdatePattern() {
    if (m_pattern != m_nextPattern) {
        RCPtr<CNode> node;
        switch (m_scene) {
        case -1:
            fn_8003CEAC(&m_scenes, s_sceneNames[0])->Play("Open", 1);
            fn_8003C8D8(&m_scenes, s_sceneNames[0], true);
            node = fn_803666B4(fn_8003CEAC(&m_scenes, s_sceneNames[0])->GetScene().Get(), "bg");
            m_scene = 0;
            break;
        case 0:
            fn_8003CEAC(&m_scenes, s_sceneNames[0])->Play("Close", 1);
            fn_8003C8D8(&m_scenes, s_sceneNames[0], true);
            fn_8003CEAC(&m_scenes, s_sceneNames[1])->Play("Open", 1);
            fn_8003C8D8(&m_scenes, s_sceneNames[1], true);
            node = fn_803666B4(fn_8003CEAC(&m_scenes, s_sceneNames[1])->GetScene().Get(), "bg");
            m_scene = 1;
            break;
        case 1:
            fn_8003CEAC(&m_scenes, s_sceneNames[0])->Play("Open", 1);
            fn_8003C8D8(&m_scenes, s_sceneNames[0], true);
            node = fn_803666B4(fn_8003CEAC(&m_scenes, s_sceneNames[0])->GetScene().Get(), "bg");
            fn_8003CEAC(&m_scenes, s_sceneNames[1])->Play("Close", 1);
            fn_8003C8D8(&m_scenes, s_sceneNames[1], true);
            m_scene = 0;
            break;
        }
        m_pattern = m_nextPattern;
        fn_8036AD14(node.Get(), m_pattern);
    }
}

void Impl::BuildLines() {
    UnkSaveData_800BB74C *save = fn_800BB74C();
    int language = 0;
    if (lbl_8057D6A0.m_voiceLanguage != 0) {
        language = 1;
    }
    int ending = save->m_history[save->m_historyCount];
    for (int k = 0; k < 3; k++) {
        if (s_endingMessages[ending - 6][k] != 0) {
            AddLine();
            m_tail->m_message = s_endingMessages[ending - 6][k];
            m_tail->m_pattern = 0x16;
            m_tail->m_voice = s_messageEntries[m_tail->m_message - 0x18E5].m_voice;
            m_tail->m_time = s_messageEntries[m_tail->m_message - 0x18E5].m_time[language];
        }
        if (k == 0) {
            m_tail->m_gap = 0.0f;
        }
    }
    int i = 0;
    while (i < save->m_historyCount) {
        if (i < save->m_historyCount - 2) {
            i++;
        } else {
            StageEntry *stage = &s_stageEntries[save->m_history[i] - 5];
            int next = save->m_history[i + 1] - 5;
            int message;
            if (stage->m_final) {
                message = stage->m_messages[0];
            } else if (s_stageEntries[next].m_final) {
                if (i == save->m_historyCount - 1) {
                    message = stage->m_messages[save->m_finalRoute + 2];
                } else {
                    int after = save->m_history[i + 2] - 5;
                    message = stage->m_messages[s_stageEntries[after].m_route];
                }
            } else {
                message = stage->m_messages[s_stageEntries[next].m_route];
            }
            if (i == save->m_historyCount - 1) {
                s8 *pattern = &stage->m_pattern;
                for (int j = 0; j < 3; j++) {
                    AddLine();
                    if (j == 0) {
                        m_tail->m_gap *= 1.5f;
                    }
                    m_tail->m_message = message + j;
                    m_tail->m_pattern = *pattern;
                    m_tail->m_voice = s_messageEntries[m_tail->m_message - 0x18E5].m_voice;
                    m_tail->m_time = s_messageEntries[m_tail->m_message - 0x18E5].m_time[language];
                }
            } else {
                AddLine();
                m_tail->m_message = message + 3;
                m_tail->m_pattern = stage->m_pattern;
                m_tail->m_gap *= 1.5f;
                m_tail->m_voice = s_messageEntries[m_tail->m_message - 0x18E5].m_voice;
                m_tail->m_time = s_messageEntries[m_tail->m_message - 0x18E5].m_time[language];
            }
            i++;
        }
    }
    float distance = 280.0f;
    float time = 0.0f;
    for (Line *line = m_head; line != 0; line = line->m_next) {
        distance += line->m_gap;
        time += 0.5f + line->m_time;
    }
    m_speed = distance / time;
    m_voiceWait = 0.0f;
}

void Impl::UpdateLines() {
    float dt = lbl_8057D6A0.m_dt;
    Line *line = m_head;
    while (line != 0) {
        Line *next = line->m_next;
        if (line->m_prev == 0 || line->m_y - line->m_prev->m_y > line->m_gap) {
            line->m_y -= dt * m_speed;
            if (line->m_y > 100.0f) {
                line->m_alpha += dt;
                if (line->m_alpha > 1.0f) {
                    line->m_alpha = 1.0f;
                }
                if (line->m_y < 300.0f) {
                    m_nextPattern = line->m_pattern;
                }
            } else {
                line->m_alpha -= dt;
                if (line->m_alpha < 0.0f) {
                    line->m_alpha = 0.0f;
                }
            }
        }
        line = next;
    }
    m_voiceWait -= dt;
    if (m_voiceWait < 0.0f) {
        for (Line *line = m_head; line != 0; line = line->m_next) {
            if (line->m_voice > 0) {
                fn_801D51A0(fn_80053164(), line->m_voice, -1, -1);
                m_voiceWait = 0.5f + line->m_time;
                line->m_voice = 0;
                return;
            }
        }
    }
}

void Impl::DrawLines() {
    for (Line *line = m_head; line != 0; line = line->m_next) {
        float alpha = line->m_alpha;
        if (alpha > 0.0f) {
            fn_802BEC5C(fn_801E5D48(), alpha, 0);
            fn_802BEEC8(fn_801E5D48(), line->m_message, 1, line->m_x, line->m_y, false);
        }
    }
}

Phase::Phase() : m_impl(new Impl) {}

Phase::~Phase() {}

bool Phase::Tick() {
    return m_impl->Tick();
}

bool Phase::Draw() {
    return m_impl->Draw();
}

} // namespace Synopsis
} // namespace AdvReal

/* Not original, and never called (the linker strips them). This unit's .data is kept whole (its
 * tables are addressed through one base), so retail keeps there, zero-filled, the weak vtables
 * the original instantiated from the CSD and boost headers: RCPtr<T>::RCObject<T>::CFunctorBase
 * and RCObject<T> for the texture list, the scene list (the project handle), the scene and the
 * node, then boost's sp_counted_base; their RTTI base lists (Chao::CSD::CBase) stay. These
 * functions create them in retail's order: the compiler emits the RCObject vtable a function
 * needs before the CFunctorBase vtable it constructs.
 *
 * The unit matches in every function and section but stays NonMatching: the linker zero-fills a
 * weak vtable here only when an earlier object defines it too. Retail's copies of the
 * CFunctorBase<CTexList>, <CSceneList> and <CNode> vtables came from earlier units that are not
 * decompiled yet (none survives in the DOL), so linked from this object they would keep their
 * contents and pull in the functors' weak functions. The RCObject vtables and destructors this
 * needs are named at their retail addresses (0x8051CF40, 0x8051CF58, 0x8051CFE4). */
namespace Chao {
namespace CSD {
class CSceneList;
} // namespace CSD
} // namespace Chao

using Chao::CSD::CSceneList;

void PoolSynopsisVtables0(void **functor) {
    *functor = new RCPtr<CTexList>::RCObject<CTexList>::CFunctorBase;
}

void PoolSynopsisVtables1(RCPtr<CTexList> &texList, void **functor) {
    texList = (CTexList *)0;
    *functor = new RCPtr<CSceneList>::RCObject<CSceneList>::CFunctorBase;
}

void PoolSynopsisVtables2(RCPtr<CSceneList> &sceneList, void **functor) {
    sceneList = (CSceneList *)0;
    *functor = new RCPtr<CScene>::RCObject<CScene>::CFunctorBase;
}

void PoolSynopsisVtables3(RCPtr<CScene> &scene, void **functor) {
    scene = (CScene *)0;
    *functor = new RCPtr<CNode>::RCObject<CNode>::CFunctorBase;
}

void PoolSynopsisVtables4(RCPtr<CNode> &node) {
    node = (CNode *)0;
    boost::shared_ptr<AdvReal::AdvBase> phase;
}
