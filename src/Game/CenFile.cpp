/* UnkCenFile_80329514 (class name unknown): a .CEN file, the keys (a frame and a position) of an
 * event's centre point over time, read by the event unit at 0x8016952C, and the position at a
 * frame. File name and the names marked "(name inferred)" are not original; the functions other
 * units call keep their fn_ names. Definitions are in reverse address order (deferred inlining
 * emits functions last-defined first). */
#include <cstring>

#include <dolphin/types.h>
#include "size_assert.h"

/* The RenderWare vector, declared locally (layout as in rw/rwcore.h). */
extern "C" {
typedef struct RwV3d {
    float x;
    float y;
    float z;
} RwV3d;
}

/* The game state (struct name unknown; see Game/CityBombSmall.cpp). */
struct UnkGameState_8057D6A0 {
    /* 0x000 */ u8 field_0x0[0x44];
    /* 0x044 */ s32 m_screenWidth;  /* (name inferred) */
    /* 0x048 */ s32 m_screenHeight; /* (name inferred) */
    /* 0x04C */ u8 field_0x4C[0xAC - 0x4C];
    /* 0x0AC */ s32 m_stage; /* (name inferred) */
    /* 0x0B0 */ u8 field_0xB0;
    /* 0x0B1 */ u8 field_0xB1; /* (meaning unknown) */
    /* 0x0B2 */ u8 field_0xB2; /* (meaning unknown) */
    /* 0x0B3 */ u8 field_0xB3[0xC4 - 0xB3];
    /* 0x0C4 */ s32 field_0xC4; /* (meaning unknown) */
    /* 0x0C8 */ u8 field_0xC8[0x125 - 0xC8];
    /* 0x125 */ u8 field_0x125; /* (meaning unknown; see Game/CityBombBig.cpp) */
    /* 0x126 */ u8 field_0x126[0x150 - 0x126];
};
SIZE_ASSERT(UnkGameState_8057D6A0, 0x150);

extern UnkGameState_8057D6A0 lbl_8057D6A0;

/* One stage's entry in the stage table fn_800CBAA4 returns (struct name unknown; the offset of the
 * entries in the table is a guess). */
struct UnkStageInfo_800CBAA4 {
    /* 0x00 */ u8 field_0x0[0x60];
    /* 0x60 */ struct {
        RwV3d m_position;
        u8 field_0xC[0x1C - 0xC];
    } m_starts[2]; /* (name inferred) */
    /* 0x98 */ u8 field_0x98[0xA8 - 0x98];
    /* 0xA8 */ RwV3d field_0xA8; /* (meaning unknown) */
    /* 0xB4 */ RwV3d field_0xB4; /* (meaning unknown) */
};
SIZE_ASSERT(UnkStageInfo_800CBAA4, 0xC0);

struct UnkStageTable_800CBAA4 {
    /* 0x00 */ u8 field_0x0[0x10];
    /* 0x10 */ UnkStageInfo_800CBAA4 m_stages[1];
};

/* The points fn_80170D8C's object holds (struct name unknown). */
struct UnkPoints_80170D8C {
    /* 0x0 */ u8 field_0x0[0x8];
    /* 0x8 */ RwV3d m_points[1];
};

struct UnkObject_80170D8C {
    /* 0x0 */ u8 field_0x0[0x8];
    /* 0x8 */ UnkPoints_80170D8C *m_points;
};

struct UnkObject_80074470 {
    /* 0x0 */ u8 field_0x0[0x8];
    /* 0x8 */ s32 m_mode; /* (meaning unknown) */
};

/* A .CEN file (class name unknown): up to 12 keys of a frame and a position. */
struct UnkCenKey_80329514 {
    /* 0x0 */ s32 m_frame;
    /* 0x4 */ RwV3d m_position;
};

class UnkCenFile_80329514 {
  public:
    UnkCenFile_80329514();
    ~UnkCenFile_80329514();

    /* 0x00 */ char m_name[0x40];
    /* 0x40 */ s32 m_state; /* 1 loading, 2 loaded (name inferred) */
    /* 0x44 */ s32 m_count;
    /* 0x48 */ UnkCenKey_80329514 m_keys[12];
};
SIZE_ASSERT(UnkCenFile_80329514, 0x108);

extern "C" {
UnkObject_80170D8C *fn_80170D8C(void);
UnkStageTable_800CBAA4 *fn_800CBAA4(void);
UnkObject_80074470 *fn_80074470(void);

/* Files of an archive: the name of one, and loading one with a callback (behaviour inferred). */
const char *fn_8004CA28(void *archive, int index);
void fn_8004C358(void *archive, int index, void (*callback)(char *, UnkCenFile_80329514 *),
                 UnkCenFile_80329514 *user);
/* Frees a buffer (behaviour inferred). */
void fn_80049B34(void *p);
/* strnicmp (behaviour inferred). */
int fn_803AC8DC(const char *a, const char *b, u32 n);

void fn_80328F30(char *text, UnkCenFile_80329514 *file);
}

/* The .CEN files' extension (name inferred; the event unit at 0x8016952C reads it too). */
extern const char lbl_805F9CA8[];
const char lbl_805F9CA8[] = ".CEN";

/* The state of the unit at 0x8016952C that the .CEN position is computed for (names inferred). */
extern s32 lbl_805EF2A0; /* the event */
extern s32 lbl_805EF2A4; /* the point, or -1 */
extern s32 lbl_805EF2B8; /* the current key */
extern RwV3d lbl_80580218; /* the base position */
extern RwV3d lbl_80580224; /* the key's position */
extern RwV3d lbl_80580230; /* their sum */

UnkCenFile_80329514::UnkCenFile_80329514() {
    m_state = 0;
    m_count = 0;
}

UnkCenFile_80329514::~UnkCenFile_80329514() {}

/* Whether name ends in a ".CEN" extension (name inferred). The event unit at 0x8016952C has the
 * same check (fn_80170E5C). */
inline bool IsCenFile(const char *name) {
    const char *ext = strchr(name, '.');
    if (ext != 0 && fn_803AC8DC(ext, lbl_805F9CA8, 4) == 0) {
        return true;
    }
    return false;
}

/* Starts loading file index of archive if it is a .CEN file (name inferred). */
extern "C" bool fn_80329420(UnkCenFile_80329514 *file, void *archive, int index) {
    if (IsCenFile(fn_8004CA28(archive, index))) {
        strcpy(file->m_name, fn_8004CA28(archive, index));
        fn_8004C358(archive, index, fn_80328F30, file);
        file->m_state = 1;
        return true;
    }
    return false;
}

/* Finds the key of frame and sets the positions; false when frame is outside the keys (name
 * inferred). */
extern "C" bool fn_803290D0(UnkCenFile_80329514 *file, float frame) {
    bool found = false;
    s32 saved = lbl_805EF2B8;
    if (file->m_count == 0) {
        memset(&lbl_80580224, 0, sizeof(RwV3d));
        found = true;
        lbl_805EF2B8 = 0;
    } else {
        for (lbl_805EF2B8 = 0; lbl_805EF2B8 < file->m_count - 1; lbl_805EF2B8++) {
            if (frame >= file->m_keys[lbl_805EF2B8].m_frame && frame < file->m_keys[lbl_805EF2B8 + 1].m_frame) {
                found = true;
                break;
            }
        }
        if (!found) {
            lbl_805EF2B8 = saved;
            return false;
        }
        lbl_80580224 = file->m_keys[lbl_805EF2B8].m_position;
    }
    UnkPoints_80170D8C *points = fn_80170D8C()->m_points;
    switch (lbl_805EF2A0) {
    case 950:
    case 970:
    case 972:
    case 974:
    case 976:
    case 978:
        lbl_80580218 = fn_800CBAA4()->m_stages[lbl_8057D6A0.m_stage].field_0xB4;
        break;
    default:
        if (fn_80074470()->m_mode == 6 || fn_80074470()->m_mode == 14) {
            if (lbl_8057D6A0.field_0x125) {
                lbl_80580218 = fn_800CBAA4()->m_stages[lbl_8057D6A0.m_stage].field_0xA8;
            } else if (lbl_8057D6A0.field_0xB2) {
                lbl_80580218 = fn_800CBAA4()->m_stages[lbl_8057D6A0.m_stage].m_starts[1].m_position;
            } else {
                int start = lbl_8057D6A0.field_0xB1 ? 1 : lbl_8057D6A0.field_0xC4;
                lbl_80580218 = fn_800CBAA4()->m_stages[lbl_8057D6A0.m_stage].m_starts[start].m_position;
            }
        } else if (points != 0) {
            if (lbl_805EF2A4 < 0) {
                lbl_80580218 = points->m_points[0];
            } else {
                lbl_80580218 = points->m_points[lbl_805EF2A4];
            }
        }
        break;
    }
    lbl_80580230.x = lbl_80580218.x + lbl_80580224.x;
    lbl_80580230.y = lbl_80580218.y + lbl_80580224.y;
    lbl_80580230.z = lbl_80580218.z + lbl_80580224.z;
    return found;
}

/* Reads the keys from a loaded .CEN file's text: numbers in groups of four (frame, x, y, z) up to
 * an 'E' (name inferred). */
extern "C" void fn_80328F30(char *text, UnkCenFile_80329514 *file) {
    char *p;
    bool negative = false;
    bool digits = false;
    int field = 0;
    int value = 0;
    file->m_count = 0;
    for (p = text; *p != 'E'; p++) {
        char c = *p;
        if (c == '-') {
            negative = true;
        } else if (c >= '0' && c <= '9') {
            value = value * 10 + c - '0';
            digits = true;
        } else if (digits) {
            if (negative) {
                value *= -1;
            }
            switch (field) {
            case 0:
                file->m_keys[file->m_count].m_frame = value;
                field++;
                break;
            case 1:
                file->m_keys[file->m_count].m_position.x = value;
                field++;
                break;
            case 2:
                file->m_keys[file->m_count].m_position.y = value;
                field++;
                break;
            case 3:
                file->m_keys[file->m_count].m_position.z = value;
                field = 0;
                file->m_count++;
                break;
            }
            negative = false;
            value = 0;
            digits = false;
        }
    }
    fn_80049B34(text);
    file->m_state = 2;
}
