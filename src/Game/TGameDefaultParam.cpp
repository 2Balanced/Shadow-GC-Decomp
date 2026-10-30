/* TGameDefaultParam (class name from RTTI, in the bound callback's type name): the development
 * defaults the game reads from a text file at start-up. Each line is "NAME : values"; a line
 * "[STAGE]" picks the stage the following lines set ("[ALL]" none), '#' starts a comment. The
 * object keeps one record per stage of the stage table and a table of 27 option names with the
 * boost::function that parses each. Other units reach the object through fn_800CBAA4 (a static
 * object; Game/CenFile.cpp and Game/ResultTask.cpp read its stage records under the placeholder
 * UnkStageTable_800CBAA4). File name from the class. Names marked "(name inferred)" are not
 * original; the option handlers are named after their option. Definitions are in reverse address
 * order (deferred inlining emits functions last-defined first).
 *
 * NonMatching: every function and section matches except ParseLine (0x801DFFB4, 99.6%): in the loop
 * that cuts the stage name at ']' retail keeps the pointer in r3 and the character in r4, GC/2.7
 * swaps them. About 25 source shapes were tried. With the unit linked, those six instructions are
 * the only difference in main.dol. */
/* The library's runtime_error (see <stdexcept>): its size sets boost::function's operator() frame,
 * and its weak copies leave their message in this unit's .rodata. */
#define MSL_RUNTIME_ERROR_IMPL
#include <boost/bind.hpp>
#include <boost/function.hpp>

#include "Game/StageTable.h"
#include <dolphin/types.h>
#include "size_assert.h"
#include <string.h>

/* The game has one shared copy of std::swap for boost::function's any_pointer (0x80019164), which
 * function1::swap calls (see Game/CatchStick.cpp). */
namespace std {
void swap(boost::detail::function::any_pointer &a, boost::detail::function::any_pointer &b);
}

namespace Mission {
/* A mission's kind and targets (see Game/Mission.cpp). */
struct MissionInfo {
    /* 0x0 */ int m_type;
    /* 0x4 */ int m_param;
    /* 0x8 */ int m_target;
    /* 0xC */ int m_subTarget;
};

/* The mission kinds by mission number (see Game/Mission.cpp). */
class MissionTable {
  public:
    const MissionInfo *Find(int id) const;
    void Set(int id, const MissionInfo &info);

    /* 0x0 */ u8 field_0x0[0x10];
};

/* The mission manager (see Game/Mission.cpp). */
class MissionManager {
  public:
    /* 0x00 */ u8 field_0x0[0x30];
    /* 0x30 */ MissionTable m_table;
};
} // namespace Mission

/* The languages of the game state (struct name unknown): the text language and the speech
 * language (meanings inferred). */
struct UnkLanguage_8057D6F8 {
    /* 0x0 */ s32 m_text;
    /* 0x4 */ s32 m_voice;
};

/* The game state (struct name unknown; see Game/TGi.h), as far as this unit uses it. */
struct UnkGameState_8057D6A0 {
    /* 0x00 */ u8 field_0x0[0x58];
    /* 0x58 */ UnkLanguage_8057D6F8 m_language;
};

extern UnkGameState_8057D6A0 lbl_8057D6A0;
/* The text language other units read (see Game/AdvOption.cpp). */
extern s32 lbl_805EE180;
/* The default of the stages' second start (a constant of another unit; meaning unknown). */
extern const float lbl_805F1C08;

/* A stage's entry in the stage table (name inferred). */
inline const UnkStageInfo_804C4A18 *GetStageInfo(UnkObject_800B74B4 *object, int index) {
    return &lbl_804C4A18[index];
}

/* The file system and its loader (see Game/ActStageController.cpp, Game/AudioDataBank.cpp). */
class UnkFileSystem_800371D8;

extern "C" {
/* MSL's strstr and strncpy (the symbols are still placeholders). */
char *fn_803AC6C0(const char *s, const char *sub);
char *fn_803ACABC(char *dst, const char *src, unsigned long size);

Mission::MissionManager *fn_8006D6A8(void);
UnkFileSystem_800371D8 *fn_800371D8(void);
void *fn_8004509C(UnkFileSystem_800371D8 *fileSystem, const char *path, u32 *size);
void fn_80049B34(void *data);

/* The sound manager (see Game/SoundManager.h) and what changing the speech language does with it
 * (behaviour unknown). */
void *fn_80053164(void);
void fn_801D4C5C(void *sound);
void fn_801D654C(void *sound);
void fn_801D656C(void *sound);
}

class TGameDefaultParam;

/* What an option's handler is called through. */
typedef boost::function1<void, TGameDefaultParam *&> TGameDefaultParamFunction;

/* The option's handler (class name unknown). */
class TGameDefaultParamHandler {
  public:
    TGameDefaultParamHandler &operator=(const TGameDefaultParamFunction &f) {
        m_function = f;
        return *this;
    }

    /* 0x0 */ TGameDefaultParamFunction m_function;
};

class TGameDefaultParam {
  public:
    /* A player's start in a stage (name inferred). */
    struct Start {
        Start() {
            m_rotation[0] = 0.0f;
            m_rotation[1] = 0.0f;
            m_rotation[2] = 0.0f;
        }

        /* 0x00 */ float m_position[3];
        /* 0x0C */ float m_rotation[3];
        /* 0x18 */ float m_speed[3];
        /* 0x24 */ bool m_demo;
    };

    /* A difficulty's rank thresholds and goal event position (name inferred). */
    struct Goal {
        /* 0x00 */ s32 m_ranks[4];
        /* 0x10 */ float m_position[3];
    };

    /* One stage's defaults (name inferred). */
    struct Stage {
        /* 0x00 */ Start m_starts[2];
        /* 0x50 */ Goal m_goals[3];
        /* 0xA4 */ float m_mipmapK;
        /* 0xA8 */ float m_goalEventPosX[3];
        /* 0xB4 */ float m_start2P[3];
    };

    /* An option (name inferred). */
    struct Option {
        /* 0x0 */ const char *m_name;
        /* 0x4 */ TGameDefaultParamHandler m_handler;
    };

    TGameDefaultParam();
    ~TGameDefaultParam();

    void Register(int index, const char *name, const TGameDefaultParamFunction &handler);
    void Initialize();
    bool Load(const char *path);
    char *ParseLine();
    char *ParseInt(int *out, char *p);
    bool GetInts(int *out, int count);

    void OptRom(TGameDefaultParam *param);
    void OptDebugWho(TGameDefaultParam *param);
    void OptPlayer(TGameDefaultParam *param);
    void OptPreDvdList(TGameDefaultParam *param);
    void OptHeapInfo(TGameDefaultParam *param);
    void OptLogFile(TGameDefaultParam *param);
    void OptLogReport(TGameDefaultParam *param);
    void OptStartSpd(TGameDefaultParam *param);
    void OptStartDemo(TGameDefaultParam *param);
    void OptLanguage(TGameDefaultParam *param);
    void OptSpeech(TGameDefaultParam *param);
    void SetRank(int difficulty, int option);
    void OptRankH(TGameDefaultParam *param);
    void OptRankN(TGameDefaultParam *param);
    void OptRankD(TGameDefaultParam *param);
    void SetGoalEventPos(int difficulty, int option);
    void OptGoalEventPosH(TGameDefaultParam *param);
    void OptGoalEventPosN(TGameDefaultParam *param);
    void OptGoalEventPosD(TGameDefaultParam *param);
    void OptGoalEventPosX(TGameDefaultParam *param);
    void OptStart2P(TGameDefaultParam *param);
    void SetMissionCount(int player, int option);
    void OptMissionCountH(TGameDefaultParam *param);
    void OptMissionCountN(TGameDefaultParam *param);
    void OptMissionCountD(TGameDefaultParam *param);
    void OptMissionCountHard(TGameDefaultParam *param);
    void OptCheckSize(TGameDefaultParam *param);
    void OptMipmapK(TGameDefaultParam *param);
    void OptRegion(TGameDefaultParam *param);
    void OptArea(TGameDefaultParam *param);

    /* The record of the stage being set, or none (name inferred). */
    Stage *GetStage() {
        return m_stage == -1 ? 0 : &m_stages[m_stage];
    }

    /* The line's ':' (name inferred). */
    char *FindValue() {
        return fn_803AC6C0(m_cursor, ":");
    }

    /* Copies the text after the line's ':' (name inferred). */
    bool GetString(char *out) {
        for (char *p = m_cursor; *p != '\0'; p++) {
            if (*p == ':') {
                p++;
                do {
                    if (*p == '\0') {
                        return false;
                    }
                    if (*p == '\n') {
                        return false;
                    }
                } while (*++p == ' ');
                fn_803ACABC(out, p, 0x20);
                return true;
            }
        }
        return false;
    }

    /* 0x0000 */ s32 m_rom;
    /* 0x0004 */ s32 m_region;
    /* 0x0008 */ s32 m_area;
    /* 0x000C */ bool m_preDvdList;
    /* 0x000D */ bool m_heapInfo;
    /* 0x000E */ bool m_logFile;
    /* 0x000F */ bool m_checkSize;
    /* 0x0010 */ Stage m_stages[59];
    /* 0x2C50 */ s32 m_stage;
    /* 0x2C54 */ Option m_options[27];
    /* 0x2E04 */ char *m_cursor;
};
SIZE_ASSERT(TGameDefaultParam, 0x2E08);

/* Calls a handler that is set (name inferred). */
inline void CallHandler(TGameDefaultParamHandler &handler, TGameDefaultParam *&param) {
    if (!handler.m_function.empty()) {
        handler.m_function(param);
    }
}

/* Sets the speech language; unless quiet, the sound manager is told around the change (name and
 * meaning inferred). */
extern "C" void fn_801DF6A0(UnkLanguage_8057D6F8 *language, int voice, bool quiet);

TGameDefaultParam::TGameDefaultParam() {
    Register(0, "ROM ", TGameDefaultParamFunction(boost::bind(&TGameDefaultParam::OptRom, this, _1)));
    Register(1, "DEBUGWHO ", TGameDefaultParamFunction(boost::bind(&TGameDefaultParam::OptDebugWho, this, _1)));
    Register(2, "PLAYER ", TGameDefaultParamFunction(boost::bind(&TGameDefaultParam::OptPlayer, this, _1)));
    Register(3, "PREDVDLIST ", TGameDefaultParamFunction(boost::bind(&TGameDefaultParam::OptPreDvdList, this, _1)));
    Register(4, "HEAPINFO ", TGameDefaultParamFunction(boost::bind(&TGameDefaultParam::OptHeapInfo, this, _1)));
    Register(5, "LOGFILE ", TGameDefaultParamFunction(boost::bind(&TGameDefaultParam::OptLogFile, this, _1)));
    Register(6, "LOGREPORT ", TGameDefaultParamFunction(boost::bind(&TGameDefaultParam::OptLogReport, this, _1)));
    Register(7, "STARTSPD ", TGameDefaultParamFunction(boost::bind(&TGameDefaultParam::OptStartSpd, this, _1)));
    Register(8, "STARTDEMO ", TGameDefaultParamFunction(boost::bind(&TGameDefaultParam::OptStartDemo, this, _1)));
    Register(9, "LANGUAGE ", TGameDefaultParamFunction(boost::bind(&TGameDefaultParam::OptLanguage, this, _1)));
    Register(10, "SPEECH ", TGameDefaultParamFunction(boost::bind(&TGameDefaultParam::OptSpeech, this, _1)));
    Register(11, "RANK_H ", TGameDefaultParamFunction(boost::bind(&TGameDefaultParam::OptRankH, this, _1)));
    Register(12, "RANK_N ", TGameDefaultParamFunction(boost::bind(&TGameDefaultParam::OptRankN, this, _1)));
    Register(13, "RANK_D ", TGameDefaultParamFunction(boost::bind(&TGameDefaultParam::OptRankD, this, _1)));
    Register(14, "GOALEVENTPOS_H ",
             TGameDefaultParamFunction(boost::bind(&TGameDefaultParam::OptGoalEventPosH, this, _1)));
    Register(15, "GOALEVENTPOS_N ",
             TGameDefaultParamFunction(boost::bind(&TGameDefaultParam::OptGoalEventPosN, this, _1)));
    Register(16, "GOALEVENTPOS_D ",
             TGameDefaultParamFunction(boost::bind(&TGameDefaultParam::OptGoalEventPosD, this, _1)));
    Register(17, "GOALEVENTPOS_X ",
             TGameDefaultParamFunction(boost::bind(&TGameDefaultParam::OptGoalEventPosX, this, _1)));
    Register(18, "START2P ", TGameDefaultParamFunction(boost::bind(&TGameDefaultParam::OptStart2P, this, _1)));
    Register(19, "MISSIONCOUNT_H ",
             TGameDefaultParamFunction(boost::bind(&TGameDefaultParam::OptMissionCountH, this, _1)));
    Register(20, "MISSIONCOUNT_N ",
             TGameDefaultParamFunction(boost::bind(&TGameDefaultParam::OptMissionCountN, this, _1)));
    Register(21, "MISSIONCOUNT_D ",
             TGameDefaultParamFunction(boost::bind(&TGameDefaultParam::OptMissionCountD, this, _1)));
    Register(22, "CHECKSIZE ", TGameDefaultParamFunction(boost::bind(&TGameDefaultParam::OptCheckSize, this, _1)));
    Register(23, "MIPMAPK ", TGameDefaultParamFunction(boost::bind(&TGameDefaultParam::OptMipmapK, this, _1)));
    Register(24, "REGION ", TGameDefaultParamFunction(boost::bind(&TGameDefaultParam::OptRegion, this, _1)));
    Register(25, "AREA ", TGameDefaultParamFunction(boost::bind(&TGameDefaultParam::OptArea, this, _1)));
    Register(26, "MISSIONCOUNT_HARD ",
             TGameDefaultParamFunction(boost::bind(&TGameDefaultParam::OptMissionCountHard, this, _1)));
}

TGameDefaultParam::~TGameDefaultParam() {}

void TGameDefaultParam::Register(int index, const char *name, const TGameDefaultParamFunction &handler) {
    m_options[index].m_name = name;
    m_options[index].m_handler = handler;
}

void TGameDefaultParam::Initialize() {
    memset(this, 0, 0x10);
    memset(m_stages, 0, sizeof(m_stages));
    for (int i = 0; i < 59; i++) {
        m_stages[i].m_mipmapK = lbl_805F1C08;
    }
}

bool TGameDefaultParam::Load(const char *path) {
    u32 size;
    char *data = static_cast<char *>(fn_8004509C(fn_800371D8(), path, &size));
    if (data == 0) {
        return false;
    }
    for (m_cursor = data; m_cursor != 0;) {
        m_cursor = ParseLine();
        if (m_cursor >= data + size) {
            break;
        }
    }
    fn_80049B34(data);
    switch (m_rom) {
    case 0:
    case 1:
    case 2:
    case 3:
    case 4:
        break;
    case 5:
        lbl_8057D6A0.m_language.m_text = lbl_805EE180 = 0;
        fn_801DF6A0(&lbl_8057D6A0.m_language, 0, false);
        break;
    case 6:
    case 7:
    case 8:
        break;
    }
    return true;
}

char *TGameDefaultParam::ParseLine() {
    TGameDefaultParam *self = this;
    int length = 0;
    char *line = m_cursor;
    char c;
    while ((c = line[length]) != '\0' && c != '\n' && c != '\r') {
        line[length] = c;
        length++;
    }
    line[length] = '\0';
    char *next = &line[length + 1];
    if (self->m_cursor[0] == '[') {
        self->m_stage = -1;
        if (strcmp(self->m_cursor, "[ALL]") != 0) {
            for (char *p = self->m_cursor; *p != '\0'; p++) {
                if (*p == ']') {
                    *p = '\0';
                    break;
                }
            }
            for (int i = 0; i < 59; i++) {
                const UnkStageInfo_804C4A18 *info = GetStageInfo(fn_800B74B4(), i);
                if (info != 0 && strcmp(self->m_cursor + 1, info->m_name) == 0) {
                    self->m_stage = i;
                    return next;
                }
            }
        }
    } else {
        for (int i = 0; i < 27; i++) {
            if (self->m_cursor[0] != '#' && fn_803AC6C0(self->m_cursor, self->m_options[i].m_name) != 0) {
                CallHandler(self->m_options[i].m_handler, self);
                return next;
            }
        }
        if (self->m_cursor[0] != '#') {
            fn_803AC6C0(self->m_cursor, ":");
        }
    }
    return next;
}

char *TGameDefaultParam::ParseInt(int *out, char *p) {
    while (!((*p >= '0' && *p <= '9') || *p == '-')) {
        if (*p == '\n') {
            return 0;
        }
        if (*p == '\0') {
            return 0;
        }
        p++;
    }
    bool negative = false;
    if (*p == '-') {
        negative = true;
        p++;
    }
    if (p[0] == '0' && p[1] == 'x') {
        p += 2;
        *out = 0;
        while (*p >= '0' && *p <= '9') {
            *out = *out * 16 + (*p - '0');
            p++;
        }
        if (negative) {
            *out *= -1;
        }
        return p;
    }
    *out = 0;
    while (*p >= '0' && *p <= '9') {
        *out = *out * 10 + (*p - '0');
        p++;
    }
    if (negative) {
        *out *= -1;
    }
    return p;
}

bool TGameDefaultParam::GetInts(int *out, int count) {
    char *p = FindValue();
    if (p == 0) {
        return false;
    }
    for (int i = 0; i < count; i++) {
        p = ParseInt(&out[i], p);
        if (p == 0) {
            return false;
        }
    }
    return true;
}

void TGameDefaultParam::OptRom(TGameDefaultParam *param) {
    char value[0x20];
    if (GetString(value)) {
        if (fn_803AC6C0(value, "DEVELOP")) {
            m_rom = 0;
        } else if (fn_803AC6C0(value, "TEST")) {
            m_rom = 1;
        } else if (fn_803AC6C0(value, "1ST")) {
            m_rom = 2;
        } else if (fn_803AC6C0(value, "E3Media")) {
            m_rom = 3;
        } else if (fn_803AC6C0(value, "E3Title")) {
            m_rom = 4;
        } else if (fn_803AC6C0(value, "TGS")) {
            m_rom = 5;
        } else if (fn_803AC6C0(value, "ALPHA")) {
            m_rom = 6;
        } else if (fn_803AC6C0(value, "BETA")) {
            m_rom = 7;
        } else if (fn_803AC6C0(value, "FINAL")) {
            m_rom = 8;
        }
    }
}

void TGameDefaultParam::OptDebugWho(TGameDefaultParam *param) {}

void TGameDefaultParam::OptPlayer(TGameDefaultParam *param) {
    Stage *stage = GetStage();
    if (stage != 0) {
        int values[7];
        if (GetInts(values, 7)) {
            int index = values[0];
            stage->m_starts[index].m_position[0] = values[1];
            stage->m_starts[index].m_position[1] = values[2];
            stage->m_starts[index].m_position[2] = values[3];
            stage->m_starts[index].m_rotation[0] = values[4];
            stage->m_starts[index].m_rotation[1] = values[5];
            stage->m_starts[index].m_rotation[2] = values[6];
        }
    }
}

void TGameDefaultParam::OptPreDvdList(TGameDefaultParam *param) {
    int value;
    if (GetInts(&value, 1)) {
        if (value == 0) {
            m_preDvdList = false;
        } else if (value == 1) {
            m_preDvdList = true;
        }
    }
}

void TGameDefaultParam::OptHeapInfo(TGameDefaultParam *param) {
    int value;
    if (GetInts(&value, 1)) {
        if (value == 0) {
            m_heapInfo = false;
        } else if (value == 1) {
            m_heapInfo = true;
        }
    }
}

void TGameDefaultParam::OptLogFile(TGameDefaultParam *param) {
    int value;
    if (GetInts(&value, 1)) {
        if (value == 0) {
            m_logFile = false;
        } else if (value == 1) {
            m_logFile = true;
        }
    }
}

void TGameDefaultParam::OptLogReport(TGameDefaultParam *param) {}

void TGameDefaultParam::OptStartSpd(TGameDefaultParam *param) {
    Stage *stage = GetStage();
    if (stage != 0) {
        int values[4];
        if (GetInts(values, 4)) {
            int index = values[0];
            stage->m_starts[index].m_speed[0] = values[1];
            stage->m_starts[index].m_speed[1] = values[2];
            stage->m_starts[index].m_speed[2] = values[3];
        }
    }
}

void TGameDefaultParam::OptStartDemo(TGameDefaultParam *param) {
    Stage *stage = GetStage();
    if (stage != 0) {
        int values[2];
        if (GetInts(values, 2)) {
            stage->m_starts[values[0]].m_demo = values[1] != 0;
        }
    }
}

void TGameDefaultParam::OptLanguage(TGameDefaultParam *param) {
    char value[0x20];
    if (GetString(value)) {
        if (fn_803AC6C0(value, "JAPANESE")) {
            lbl_8057D6A0.m_language.m_text = lbl_805EE180 = 0;
        } else if (fn_803AC6C0(value, "ENGLISH")) {
            lbl_8057D6A0.m_language.m_text = lbl_805EE180 = 1;
        } else if (fn_803AC6C0(value, "SPANISH")) {
            lbl_8057D6A0.m_language.m_text = lbl_805EE180 = 3;
        } else if (fn_803AC6C0(value, "FRENCH")) {
            lbl_8057D6A0.m_language.m_text = lbl_805EE180 = 2;
        } else if (fn_803AC6C0(value, "GERMAN")) {
            lbl_8057D6A0.m_language.m_text = lbl_805EE180 = 4;
        } else if (fn_803AC6C0(value, "ITALIAN")) {
            lbl_8057D6A0.m_language.m_text = lbl_805EE180 = 5;
        }
    }
}

extern "C" void fn_801DF6A0(UnkLanguage_8057D6F8 *language, int voice, bool quiet) {
    if (!quiet) {
        fn_801D4C5C(fn_80053164());
        fn_801D654C(fn_80053164());
        language->m_voice = voice;
        fn_801D656C(fn_80053164());
    } else {
        language->m_voice = voice;
    }
}

void TGameDefaultParam::OptSpeech(TGameDefaultParam *param) {
    char value[0x20];
    if (GetString(value)) {
        if (fn_803AC6C0(value, "JAPANESE")) {
            fn_801DF6A0(&lbl_8057D6A0.m_language, 0, true);
        } else if (fn_803AC6C0(value, "ENGLISH")) {
            fn_801DF6A0(&lbl_8057D6A0.m_language, 1, true);
        }
    }
}

void TGameDefaultParam::SetRank(int difficulty, int option) {
    Stage *stage = GetStage();
    if (stage != 0) {
        int values[4];
        if (GetInts(values, 4)) {
            for (int i = 0; i < 4; i++) {
                stage->m_goals[difficulty].m_ranks[i] = values[i];
            }
        }
    }
}

void TGameDefaultParam::OptRankH(TGameDefaultParam *param) {
    SetRank(2, 15);
}

void TGameDefaultParam::OptRankN(TGameDefaultParam *param) {
    SetRank(1, 15);
}

void TGameDefaultParam::OptRankD(TGameDefaultParam *param) {
    SetRank(0, 16);
}

void TGameDefaultParam::SetGoalEventPos(int difficulty, int option) {
    Stage *stage = GetStage();
    if (stage != 0) {
        int values[3];
        if (GetInts(values, 3)) {
            stage->m_goals[difficulty].m_position[0] = values[0];
            stage->m_goals[difficulty].m_position[1] = values[1];
            stage->m_goals[difficulty].m_position[2] = values[2];
        }
    }
}

void TGameDefaultParam::OptGoalEventPosH(TGameDefaultParam *param) {
    SetGoalEventPos(2, 15);
}

void TGameDefaultParam::OptGoalEventPosN(TGameDefaultParam *param) {
    SetGoalEventPos(1, 15);
}

void TGameDefaultParam::OptGoalEventPosD(TGameDefaultParam *param) {
    SetGoalEventPos(0, 16);
}

void TGameDefaultParam::OptGoalEventPosX(TGameDefaultParam *param) {
    Stage *stage = GetStage();
    if (stage != 0) {
        int values[3];
        if (GetInts(values, 3)) {
            stage->m_goalEventPosX[0] = values[0];
            stage->m_goalEventPosX[1] = values[1];
            stage->m_goalEventPosX[2] = values[2];
        }
    }
}

void TGameDefaultParam::OptStart2P(TGameDefaultParam *param) {
    Stage *stage = GetStage();
    if (stage != 0) {
        int values[3];
        if (GetInts(values, 3)) {
            stage->m_start2P[0] = values[0];
            stage->m_start2P[1] = values[1];
            stage->m_start2P[2] = values[2];
        }
    }
}

void TGameDefaultParam::SetMissionCount(int player, int option) {
    if (GetStage() != 0) {
        int values[2];
        if (GetInts(values, 2)) {
            int id;
            const Mission::MissionInfo *info;
            if (option != 26) {
                id = fn_80176BE4(fn_800B74B4(), m_stage, player);
                info = fn_8006D6A8()->m_table.Find(id);
            } else {
                id = GetStageInfo(fn_800B74B4(), m_stage)->m_missionId;
                info = fn_8006D6A8()->m_table.Find(id);
            }
            if (info != 0) {
                Mission::MissionInfo changed;
                changed = *info;
                changed.m_target = values[0];
                changed.m_subTarget = values[1];
                fn_8006D6A8()->m_table.Set(id, changed);
            }
        }
    }
}

void TGameDefaultParam::OptMissionCountH(TGameDefaultParam *param) {
    SetMissionCount(2, 19);
}

void TGameDefaultParam::OptMissionCountN(TGameDefaultParam *param) {
    SetMissionCount(1, 20);
}

void TGameDefaultParam::OptMissionCountD(TGameDefaultParam *param) {
    SetMissionCount(0, 21);
}

void TGameDefaultParam::OptMissionCountHard(TGameDefaultParam *param) {
    SetMissionCount(1, 26);
}

void TGameDefaultParam::OptCheckSize(TGameDefaultParam *param) {
    if (m_stage == -1) {
        int value;
        if (GetInts(&value, 1)) {
            if (value == 0) {
                m_checkSize = false;
            } else if (value == 1) {
                m_checkSize = true;
            }
        }
    }
}

void TGameDefaultParam::OptMipmapK(TGameDefaultParam *param) {
    Stage *stage = GetStage();
    if (stage != 0) {
        int value;
        if (GetInts(&value, 1)) {
            stage->m_mipmapK = 0.1f * value;
        }
    }
}

void TGameDefaultParam::OptRegion(TGameDefaultParam *param) {
    char value[0x20];
    if (GetString(value)) {
        if (fn_803AC6C0(value, "NTSC")) {
            m_region = 0;
        } else if (fn_803AC6C0(value, "PAL60")) {
            m_region = 1;
        } else if (fn_803AC6C0(value, "PAL50")) {
            m_region = 2;
        }
    }
}

void TGameDefaultParam::OptArea(TGameDefaultParam *param) {
    char value[0x20];
    if (GetString(value)) {
        if (fn_803AC6C0(value, "JP")) {
            m_area = 0;
        } else if (fn_803AC6C0(value, "US")) {
            m_area = 1;
        } else if (fn_803AC6C0(value, "EU")) {
            m_area = 2;
        }
    }
}
