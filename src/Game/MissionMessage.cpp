/* Mission::Message and Mission::MessageManage (class names from RTTI): the base classes of the
 * mission hint messages (see Game/MissionMessage.h), and the tables of each mission number's hints
 * the managers read. File name from the namespace and the class; all other names are inferred.
 * Definitions are in reverse address order (deferred inlining emits functions last-defined
 * first). */
#include "Game/MissionMessage.h"
#include <typeinfo>

/* The game state (struct name unknown; see Game/TFogSet.cpp). */
struct UnkGameState_8057D6A0 {
    /* 0x000 */ u8 field_0x0[0x38];
    /* 0x038 */ s32 m_gameMode;
    /* 0x03C */ u8 field_0x3C[0x150 - 0x3C];
};
SIZE_ASSERT(UnkGameState_8057D6A0, 0x150);

extern UnkGameState_8057D6A0 lbl_8057D6A0;

class UnkHintManager_801FB180;

extern "C" {
s32 fn_801FAAC4(int player);
s32 fn_801FAF00(UnkHintManager_801FB180 *manager, s32 id, s32 branch, int arg3, int arg4);
}

UnkHintManager_801FB180 *GetHintManager();

namespace Mission {

/* Not called anywhere: the linker strips it from the game. Retail's .rodata starts with Message's
 * RTTI name; GC/2.7 creates the record after the source is read (behind the tables), so this
 * typeid creates it up front. */
const std::type_info &MessageType() {
    return typeid(Message);
}

/* The hints of each mission number, and the table used in game mode 5. */
/* The hint lists are 8-byte aligned in retail (alignment of the first one inferred from its
 * offset; the rest follow from their sizes). */
const MessageInfo s_mission1Hints[6] __attribute__((aligned(8))) = {

    {0x2C7, 1, 0, 1, 0.0f, -1},

    {0x2C8, 1, 1, 0, 0.25f, -1},

    {0x2C9, 1, 1, 0, 0.5f, -1},

    {0x2CA, 1, 1, 0, 0.75f, -1},

    {0x2CB, 1, 2, 1, 0.0f, -1},

    {0x2CC, 0, 0, 0, 0.0f, -1},

};



const MessageInfo s_mission12Hints[6] = {

    {0x2CD, 1, 0, 10, 0.0f, -1},

    {0x2CE, 1, 1, 0, 0.25f, -1},

    {0x2CF, 1, 1, 0, 0.5f, -1},

    {0x2D0, 1, 1, 0, 0.75f, -1},

    {0x2D1, 1, 2, 10, 0.0f, -1},

    {0x2D2, 0, 0, 0, 0.0f, -1},

};



const MessageInfo s_mission6Hints[2] = {

    {0x309, 0, 0, 0, 0.0f, -1},

    {0x30A, 0, 1, 0, 0.0f, -1},

};



const MessageInfo s_mission14Hints[27] = {

    {0x321, 1, 0, 1, 0.0f, -1},

    {0x322, 1, 0, 2, 0.0f, -1},

    {0x323, 1, 0, 3, 0.0f, -1},

    {0x324, 1, 0, 4, 0.0f, -1},

    {0x325, 1, 0, 5, 0.0f, -1},

    {0x326, 1, 0, 6, 0.0f, -1},

    {0x327, 1, 0, 7, 0.0f, -1},

    {0x328, 1, 0, 8, 0.0f, -1},

    {0x329, 1, 0, 9, 0.0f, -1},

    {0x32A, 1, 0, 10, 0.0f, -1},

    {0x32B, 1, 0, 11, 0.0f, -1},

    {0x32C, 1, 0, 12, 0.0f, -1},

    {0x32D, 1, 0, 13, 0.0f, -1},

    {0x32E, 1, 0, 14, 0.0f, -1},

    {0x32F, 1, 0, 15, 0.0f, -1},

    {0x330, 1, 0, 16, 0.0f, -1},

    {0x331, 1, 0, 17, 0.0f, -1},

    {0x332, 1, 0, 18, 0.0f, -1},

    {0x333, 1, 0, 19, 0.0f, -1},

    {0x334, 0, 0, 0, 0.0f, -1},

    {0x346, 2, 0, 0, 60.0f, -1},

    {0x347, 2, 2, 0, 300.0f, -1},

    {0x348, 2, 2, 0, 180.0f, -1},

    {0x349, 2, 2, 0, 60.0f, -1},

    {0x34A, 2, 2, 0, 30.0f, -1},

    {0x34B, 0, 1, 0, 0.0f, 2},

    {0x34B, 0, 1, 0, 0.0f, 1},

};



const MessageInfo s_mission13Hints[11] = {

    {0x336, 1, 0, 1, 0.0f, -1},

    {0x337, 1, 0, 2, 0.0f, -1},

    {0x338, 1, 0, 3, 0.0f, -1},

    {0x339, 1, 0, 4, 0.0f, -1},

    {0x33A, 0, 0, 0, 0.0f, -1},

    {0x346, 2, 0, 0, 60.0f, -1},

    {0x347, 2, 2, 0, 300.0f, -1},

    {0x348, 2, 2, 0, 180.0f, -1},

    {0x349, 2, 2, 0, 60.0f, -1},

    {0x34A, 2, 2, 0, 30.0f, -1},

    {0x34B, 0, 1, 0, 0.0f, 0},

};



const MessageInfo s_mission4Hints[5] = {

    {0x350, 1, 0, 1, 0.0f, -1},

    {0x351, 1, 0, 2, 0.0f, -1},

    {0x352, 1, 0, 3, 0.0f, -1},

    {0x353, 1, 0, 4, 0.0f, -1},

    {0x354, 0, 0, 0, 0.0f, -1},

};



const MessageInfo s_mission18Hints[9] = {

    {0x367, 1, 0, 1, 0.0f, 2},

    {0x367, 1, 0, 1, 0.0f, 0},

    {0x368, 1, 0, 2, 0.0f, 2},

    {0x368, 1, 0, 2, 0.0f, 0},

    {0x369, 1, 0, 3, 0.0f, 2},

    {0x369, 1, 0, 3, 0.0f, 0},

    {0x36A, 1, 0, 4, 0.0f, 2},

    {0x36A, 1, 0, 4, 0.0f, 0},

    {0x36B, 0, 0, 0, 0.0f, -1},

};



const MessageInfo s_mission17Hints[9] = {

    {0x36E, 1, 0, 1, 0.0f, 2},

    {0x36E, 1, 0, 1, 0.0f, 0},

    {0x36F, 1, 0, 2, 0.0f, 2},

    {0x36F, 1, 0, 2, 0.0f, 0},

    {0x370, 1, 0, 3, 0.0f, 2},

    {0x370, 1, 0, 3, 0.0f, 0},

    {0x371, 1, 0, 4, 0.0f, 2},

    {0x371, 1, 0, 4, 0.0f, -1},

    {0x372, 0, 0, 0, 0.0f, -1},

};



const MessageInfo s_mission3Hints[1] = {

    {0x389, 0, 0, 0, 0.0f, -1},

};



const MessageInfo s_mission20Hints[4] = {

    {0x3A9, 1, 0, 1, 0.0f, -1},

    {0x3AA, 1, 0, 2, 0.0f, -1},

    {0x3AB, 1, 0, 3, 0.0f, -1},

    {0x3AC, 0, 0, 0, 0.0f, -1},

};



const MessageInfo s_mission19Hints[5] = {

    {0x3AE, 1, 1, 0, 0.25f, -1},

    {0x3AF, 1, 1, 0, 0.5f, -1},

    {0x3B0, 1, 1, 0, 0.75f, -1},

    {0x3B1, 1, 2, 1, 0.0f, -1},

    {0x3B2, 0, 0, 0, 0.0f, -1},

};



const MessageInfo s_mission7Hints[5] = {

    {0x3D5, 1, 0, 1, 0.0f, -1},

    {0x3D6, 1, 0, 2, 0.0f, -1},

    {0x3D7, 1, 0, 3, 0.0f, -1},

    {0x3D8, 1, 0, 4, 0.0f, -1},

    {0x3D9, 0, 0, 0, 0.0f, -1},

};



const MessageInfo s_mission10Hints[5] = {

    {0x405, 1, 0, 1, 0.0f, -1},

    {0x406, 1, 0, 2, 0.0f, -1},

    {0x407, 1, 0, 3, 0.0f, -1},

    {0x408, 1, 0, 4, 0.0f, -1},

    {0x409, 0, 0, 0, 0.0f, -1},

};



const MessageInfo s_mission26Hints[2] = {

    {0x433, 0, 0, 0, 0.0f, -1},

    {0x435, 0, 1, 0, 0.0f, -1},

};



const MessageInfo s_mission16Hints[10] = {

    {0x44F, 1, 0, 1, 0.0f, -1},

    {0x450, 1, 0, 2, 0.0f, -1},

    {0x451, 1, 0, 3, 0.0f, -1},

    {0x452, 1, 0, 4, 0.0f, -1},

    {0x453, 1, 0, 5, 0.0f, -1},

    {0x454, 1, 0, 6, 0.0f, -1},

    {0x455, 1, 0, 7, 0.0f, -1},

    {0x456, 1, 0, 8, 0.0f, -1},

    {0x457, 1, 0, 9, 0.0f, -1},

    {0x458, 0, 0, 0, 0.0f, -1},

};



const MessageInfo s_mission22Hints[4] = {

    {0x46F, 1, 0, 1, 0.0f, -1},

    {0x470, 1, 0, 2, 0.0f, -1},

    {0x471, 1, 0, 3, 0.0f, -1},

    {0x472, 0, 0, 0, 0.0f, -1},

};



const MessageInfo s_mission28Hints[6] = {

    {0x496, 2, 1, 0, 0.800000011920929f, -1},

    {0x497, 2, 1, 0, 0.6000000238418579f, -1},

    {0x498, 2, 1, 0, 0.4000000059604645f, -1},

    {0x499, 2, 1, 0, 0.30000001192092896f, -1},

    {0x49A, 2, 2, 0, 20.0f, -1},

    {0x49B, 0, 1, 0, 0.0f, -1},

};



const MessageInfo s_mission27Hints[7] = {

    {0x49F, 1, 0, 1, 0.0f, 0},

    {0x49F, 1, 0, 1, 0.0f, 1},

    {0x4A0, 1, 0, 2, 0.0f, -1},

    {0x4A1, 1, 0, 3, 0.0f, -1},

    {0x4A2, 1, 0, 4, 0.0f, -1},

    {0x4A3, 1, 0, 5, 0.0f, -1},

    {0x4A4, 0, 0, 0, 0.0f, -1},

};



const MessageInfo s_mission34Hints[7] = {

    {0x4DF, 2, 1, 0, 0.800000011920929f, -1},

    {0x4E0, 2, 1, 0, 0.6000000238418579f, -1},

    {0x4E1, 2, 1, 0, 0.4000000059604645f, -1},

    {0x4E2, 2, 1, 0, 0.30000001192092896f, -1},

    {0x4E3, 2, 2, 0, 20.0f, -1},

    {0x4E4, 0, 1, 0, 0.0f, 2},

    {0x4E4, 0, 1, 0, 0.0f, 1},

};



const MessageInfo s_mission33Hints[6] = {

    {0x4DF, 2, 1, 0, 0.800000011920929f, -1},

    {0x4E0, 2, 1, 0, 0.6000000238418579f, -1},

    {0x4E1, 2, 1, 0, 0.4000000059604645f, -1},

    {0x4E2, 2, 1, 0, 0.30000001192092896f, -1},

    {0x4E3, 2, 2, 0, 20.0f, -1},

    {0x4E4, 0, 1, 0, 0.0f, 0},

};



const MessageInfo s_mission23Hints[2] = {

    {0x4F5, 0, 0, 0, 0.0f, -1},

    {0x4F6, 0, 1, 0, 0.0f, -1},

};



const MessageInfo s_mission30Hints[3] = {

    {0x50A, 1, 0, 1, 0.0f, -1},

    {0x50B, 1, 0, 2, 0.0f, -1},

    {0x50D, 0, 0, 0, 0.0f, -1},

};



const MessageInfo s_mission32Hints[5] = {

    {0x522, 1, 0, 1, 0.0f, -1},

    {0x523, 1, 0, 2, 0.0f, -1},

    {0x524, 1, 0, 3, 0.0f, -1},

    {0x525, 1, 0, 4, 0.0f, -1},

    {0x526, 0, 0, 0, 0.0f, -1},

};



const MessageTableEntry s_messageTable[41] = {

    {0, 0},

    {s_mission1Hints, 6},

    {s_mission1Hints, 6},

    {s_mission3Hints, 1},

    {s_mission4Hints, 5},

    {s_mission1Hints, 6},

    {s_mission6Hints, 2},

    {s_mission7Hints, 5},

    {0, 0},

    {s_mission1Hints, 6},

    {s_mission10Hints, 5},

    {s_mission1Hints, 6},

    {s_mission12Hints, 6},

    {s_mission13Hints, 11},

    {s_mission14Hints, 27},

    {s_mission1Hints, 6},

    {s_mission16Hints, 10},

    {s_mission17Hints, 9},

    {s_mission18Hints, 9},

    {s_mission19Hints, 5},

    {s_mission20Hints, 4},

    {s_mission1Hints, 6},

    {s_mission22Hints, 4},

    {s_mission23Hints, 2},

    {s_mission1Hints, 6},

    {s_mission1Hints, 6},

    {s_mission26Hints, 2},

    {s_mission27Hints, 7},

    {s_mission28Hints, 6},

    {s_mission1Hints, 6},

    {s_mission30Hints, 3},

    {s_mission1Hints, 6},

    {s_mission32Hints, 5},

    {s_mission33Hints, 6},

    {s_mission34Hints, 7},

    {0, 0},

    {0, 0},

    {0, 0},

    {0, 0},

    {0, 0},

    {0, 0},

};



const MessageInfo s_mission7HintsMode5[2] = {

    {0x3D8, 1, 0, 1, 0.0f, -1},

    {0x3D9, 0, 0, 0, 0.0f, -1},

};



const MessageTableEntry s_messageTableMode5[41] = {

    {0, 0},

    {s_mission1Hints, 6},

    {s_mission1Hints, 6},

    {s_mission3Hints, 1},

    {s_mission4Hints, 5},

    {s_mission1Hints, 6},

    {s_mission6Hints, 2},

    {s_mission7HintsMode5, 2},

    {0, 0},

    {s_mission1Hints, 6},

    {s_mission10Hints, 5},

    {s_mission1Hints, 6},

    {s_mission12Hints, 6},

    {s_mission13Hints, 11},

    {s_mission14Hints, 27},

    {s_mission1Hints, 6},

    {s_mission16Hints, 10},

    {s_mission17Hints, 9},

    {s_mission18Hints, 9},

    {s_mission19Hints, 5},

    {s_mission20Hints, 4},

    {s_mission1Hints, 6},

    {s_mission22Hints, 4},

    {s_mission23Hints, 2},

    {s_mission1Hints, 6},

    {s_mission1Hints, 6},

    {s_mission26Hints, 2},

    {s_mission27Hints, 7},

    {s_mission28Hints, 6},

    {s_mission1Hints, 6},

    {s_mission30Hints, 3},

    {s_mission1Hints, 6},

    {s_mission32Hints, 5},

    {s_mission33Hints, 6},

    {s_mission34Hints, 7},

    {0, 0},

    {0, 0},

    {0, 0},

    {0, 0},

    {0, 0},

    {0, 0},

};


/* A mission number's hints and how many there are. */
static const MessageInfo *GetMessageInfos(int id) {
    if (id >= 0 && id < 41) {
        if (lbl_8057D6A0.m_gameMode == 5) {
            return s_messageTableMode5[id].m_infos;
        }
        return s_messageTable[id].m_infos;
    }
    return 0;
}

static int GetMessageCount(int id) {
    if (id >= 0 && id < 41) {
        if (lbl_8057D6A0.m_gameMode == 5) {
            return s_messageTableMode5[id].m_count;
        }
        return s_messageTable[id].m_count;
    }
    return 0;
}

Message::Message(const MessageInfo *info) : m_info(info) {}

Message::~Message() {}

void Message::Show(int player) {
    s32 branch = fn_801FAAC4(player);
    fn_801FAF00(GetHintManager(), m_info->m_hintId, branch, 0, 0);
}

MessageManage::MessageManage(int id, int player) : m_player(player) {
    m_infos = GetMessageInfos(id);
    m_infoCount = GetMessageCount(id);
}

} // namespace Mission
