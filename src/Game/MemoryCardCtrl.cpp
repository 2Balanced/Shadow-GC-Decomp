/* TMemoryCardCtrl (class name from RTTI): the memory card controller task, and the card access
 * layer under it. MemoryCard runs the Dolphin CARD library's asynchronous calls as a small state
 * machine (mount, check, open, create, read, write, set status, delete, format, file search), one
 * command per frame; MemoryCardFile lays out the banner, icon and comment block in front of the
 * save data. TMemoryCardCtrl turns requests (create, load, save, search, format, delete, ...)
 * into sequences of MemoryCard requests, checksums the save data with MD5 and encrypts it with DES
 * (Game/MemoryCardCrypt.cpp). The names of the two card classes and all member names are
 * inferred; the functions other units call keep their fn_ names. Definitions are in reverse
 * address order (deferred inlining emits functions last-defined first).
 *
 * NonMatching: 90 of 93 functions match, and every data section. fn_80298E5C and fn_80298E88
 * differ by two swapped instructions each: retail loads ctrl->m_card above the store of the link
 * register, GC/2.7 does not (as in Game/VehiclePathMove.cpp and Game/CameraScreenSet.cpp). PollOpen
 * (0x8029AD94) has its case 2 as a separate switch range that retail sends to the shared failure
 * block; GC/2.7 keeps the duplicated block (16 instructions; see the comment there). */
#include "Game/MemoryCardCtrl.h"
#include "Game/MemoryCardCrypt.h"
#include "Game/Task.h"
#include <cstdio>
#include <cstring>
#include <dolphin/card.h>
#include <dolphin/types.h>
#include "size_assert.h"

/* The game state (struct name unknown; see Game/TGi.h). */
struct UnkGameState_8057D6A0 {
    /* 0x000 */ u8 field_0x0[0x40];
    /* 0x040 */ s32 m_region; /* 0 Japan, 1 America, 2 Europe (meaning inferred from the codes) */
    /* 0x044 */ u8 field_0x44[0x150 - 0x44];
};
SIZE_ASSERT(UnkGameState_8057D6A0, 0x150);

extern UnkGameState_8057D6A0 lbl_8057D6A0;

/* The save data the controller writes (struct name unknown): an MD5 of the rest, then the game's
 * data. Only the stamp is read here. */
struct UnkSaveData_800BB74C {
    /* 0x0000 */ u8 m_digest[0x10];
    /* 0x0010 */ u8 field_0x10[0x1774 - 0x10];
    /* 0x1774 */ u32 m_stamp; /* (name inferred) */
    /* 0x1778 */ u8 field_0x1778[0x1F5C - 0x1778];
};
SIZE_ASSERT(UnkSaveData_800BB74C, 0x1F5C);

extern "C" {
/* Dolphin SDK CARD functions (names from the SDK; the symbols are still placeholders). */
s32 fn_8038D238(s32 chan);                                          /* CARDGetResultCode */
s32 fn_8038D268(s32 chan, s32 *bytesNotUsed, s32 *filesNotUsed);    /* CARDFreeBlocks */
s32 fn_8038FE90(s32 chan, CARDCallback callback);                   /* CARDCheckAsync */
s32 fn_8038FF84(s32 chan, s32 *memSize, s32 *sectorSize);           /* CARDProbeEx */
s32 fn_8039068C(s32 chan, void *workArea, CARDCallback detach, CARDCallback attach); /* CARDMountAsync */
s32 fn_803908C8(s32 chan);                                          /* CARDUnmount */
s32 fn_80391110(s32 chan, CARDCallback callback);                   /* CARDFormatAsync */
s32 fn_803915CC(s32 chan, const char *fileName, CARDFileInfo *fileInfo); /* CARDOpen */
s32 fn_803916E8(CARDFileInfo *fileInfo);                            /* CARDClose */
s32 fn_80391874(s32 chan, const char *fileName, u32 size, CARDFileInfo *fileInfo,
                CARDCallback callback);                             /* CARDCreateAsync */
s32 fn_80391D7C(CARDFileInfo *fileInfo, void *buf, s32 length, s32 offset,
                CARDCallback callback);                             /* CARDReadAsync */
s32 fn_803920E0(CARDFileInfo *fileInfo, const void *buf, s32 length, s32 offset,
                CARDCallback callback);                             /* CARDWriteAsync */
s32 fn_80392298(s32 chan, const char *fileName, CARDCallback callback); /* CARDDeleteAsync */
s32 fn_803925A0(s32 chan, s32 fileNo, CARDStat *stat);              /* CARDGetStatus */
s32 fn_803926B4(s32 chan, s32 fileNo, CARDStat *stat, CARDCallback callback); /* CARDSetStatusAsync */
/* MSL string functions (names inferred). */
int fn_803A8DA8(const void *a, const void *b, unsigned long size);  /* memcmp */
char *fn_803AC6C0(const char *s, const char *sub);                  /* strstr */
char *fn_803ACABC(char *dst, const char *src, unsigned long size);  /* strncpy */
/* Game memory: allocate and free (see Game/HintController.cpp). */
void *fn_80049B60(u32 size, int heap, int align);
void fn_80049B34(void *data);
/* Loads a file (see Game/HintController.cpp). */
void *fn_800371D8(void);
void *fn_8004509C(void *fileSystem, const char *path, u32 *size);
/* The game's save data (behaviour inferred). */
UnkSaveData_800BB74C *fn_800BB74C(void);
/* A new value for the save data's stamp (behaviour inferred). */
u32 fn_80051374(void);
}

/* The work area and file state of one card slot (class name inferred). */
class MemoryCardWork {
  public:
    MemoryCardWork() {
        int i;
        m_rawWorkArea = new u8[CARD_WORKAREA_SIZE + 0x20];
        m_workArea = (u8 *)(((u32)m_rawWorkArea + 0x1F) & ~0x1F);
        for (i = 0; i < CARD_FILENAME_MAX; i++) {
            m_fileName[i] = 0;
        }
        m_fileNo = 0;
        m_matchCount = 0;
        m_searchIndex = 0;
    }
    ~MemoryCardWork() {
        if (m_rawWorkArea != 0) {
            delete[] m_rawWorkArea;
        }
    }

    /* 0x00 */ u8 *m_workArea;
    /* 0x04 */ u8 *m_rawWorkArea;
    /* 0x08 */ char m_fileName[CARD_FILENAME_MAX];
    /* 0x28 */ CARDFileInfo m_fileInfo;
    /* 0x3C */ s32 m_fileNo;
    /* 0x40 */ CARDStat m_stat;
    /* 0xAC */ s32 m_matchCount;
    /* 0xB0 */ s32 m_searchIndex;
    /* 0xB4 */ char m_company[2];
    /* 0xB6 */ char m_gameName[4];
};
SIZE_ASSERT(MemoryCardWork, 0xBC);

/* The banner, icons and comment of a save file and their layout in front of the data (class name
 * inferred). */
class MemoryCardFile {
  public:
    MemoryCardFile(void *banner, void *bannerTlut, void *icon, void *iconTlut, s32 iconCount,
                   s32 animType, s32 iconSpeed, const char *comment, const char *subComment);
    ~MemoryCardFile();
    u32 GetSize();
    u32 Build(u8 *dst);
    s32 GetIconAnim();
    s32 GetIconSpeed();
    s32 GetIconFormat();
    s32 GetBannerFormat();

    /* 0x00 */ u32 m_size;
    /* 0x04 */ char *m_comment;
    /* 0x08 */ void *m_banner;
    /* 0x0C */ void *m_bannerTlut;
    /* 0x10 */ void *m_icon;
    /* 0x14 */ void *m_iconTlut;
    /* 0x18 */ u32 m_commentOffset;
    /* 0x1C */ u32 m_bannerOffset;
    /* 0x20 */ u32 m_iconOffset;
    /* 0x24 */ s32 m_iconCount;
    /* 0x28 */ s32 m_animType;
    /* 0x2C */ s32 m_iconSpeed;
};
SIZE_ASSERT(MemoryCardFile, 0x30);

/* One card slot and the CARD call in progress on it (class name inferred). */
class MemoryCard {
  public:
    enum {
        CMD_IDLE,
        CMD_READY,
        CMD_ERROR,
        CMD_MOUNT,
        CMD_CHECK,
        CMD_OPEN,
        CMD_GET_STATUS,
        CMD_FORMAT,
        CMD_CREATE,
        CMD_DELETE,
        CMD_READ,
        CMD_WRITE,
        CMD_FREE_BLOCKS,
        CMD_SET_STATUS,
        CMD_SEARCH,
    };

    MemoryCard(s32 channel);
    ~MemoryCard();
    static MemoryCard *Create(s32 channel);
    s32 GetStatus();
    s32 IsBusy();
    s32 GetResult();
    s32 GetMatchCount();
    bool HasRoom(s32 size);
    bool BeginRequest(s32 request);
    bool RequestFormat();
    bool RequestCreate(const char *name, s32 size, void *data, MemoryCardFile *file);
    bool RequestDelete(const char *name);
    bool RequestRead(const char *name, s32 size, void *data);
    bool RequestWrite(const char *name, s32 size, void *data, MemoryCardFile *file);
    bool RequestSearch(const char *name);
    bool RequestFreeBlocks();
    bool Start(s32 command);
    void Poll(s32 command);
    s32 Update();
    void SetCommand(s32 command);
    void Finish();
    void Fail();
    void SetError(s32 result);
    bool CheckCard();
    void PollIdle();
    void PollReady();
    void PollError();
    bool StartMount();
    void PollMount();
    bool StartCheck();
    void PollCheck();
    bool StartOpen();
    void PollOpen();
    bool StartGetStatus();
    void PollGetStatus();
    bool StartFormat();
    void PollFormat();
    bool StartCreate();
    void PollCreate();
    bool StartDelete();
    void PollDelete();
    bool StartRead();
    void PollRead();
    bool StartWrite();
    void PollWrite();
    bool StartFreeBlocks();
    void PollFreeBlocks();
    bool StartSetStatus();
    void PollSetStatus();
    bool StartSearch();
    void PollSearch();
    void SetGameCode(const char *company, const char *gameName);

    /* 0x00 */ MemoryCardWork *m_work;
    /* 0x04 */ s32 m_channel;
    /* 0x08 */ s32 m_status;
    /* 0x0C */ s32 m_command;
    /* 0x10 */ s32 m_started;
    /* 0x14 */ s32 m_busy;
    /* 0x18 */ s32 m_request;
    /* 0x1C */ s32 m_result;
    /* 0x20 */ u32 m_flags; /* 1 mounted, 2 file open, 4 status read */
    /* 0x24 */ s32 m_size;
    /* 0x28 */ void *m_data;
    /* 0x2C */ MemoryCardFile *m_file;
    /* 0x30 */ s32 m_freeBytes;
    /* 0x34 */ s32 m_freeFiles;
};
SIZE_ASSERT(MemoryCard, 0x38);

static const char *s_iconNames[3] = {
    "mc_icon01.tp",
    "mc_icon02.tp",
    "mc_icon03.tp",
};

static const char *s_taskName = "MemoryCardManager";

/* The maker code and the game codes of the three regions' discs. */
static const char s_makerCode[] = "8P";
static const char s_gameCodeJapan[4] = {'G', 'U', 'P', 'J'};
static const char s_gameCodeAmerica[4] = {'G', 'U', 'P', 'E'};
static const char s_gameCodeEurope[4] = {'G', 'U', 'P', 'P'};

/* Full-width digits (unused here). */
static const char *const s_digits[10] = {
    "\x82\x4F", "\x82\x50", "\x82\x51", "\x82\x52", "\x82\x53",
    "\x82\x54", "\x82\x55", "\x82\x56", "\x82\x57", "\x82\x58",
};

static void *s_bannerTexture;
static u8 s_cardActive;
static UnkSaveData_800BB74C *s_saveData;
static void *s_iconTextures[3];

MemoryCardFile *MemoryCardFile_Create(void *banner, void *bannerTlut, void *icon, void *iconTlut,
                                      s32 iconCount, s32 animType, s32 iconSpeed,
                                      const char *comment, const char *subComment) {
    return new MemoryCardFile(banner, bannerTlut, icon, iconTlut, iconCount, animType, iconSpeed,
                              comment, subComment);
}

MemoryCardFile::MemoryCardFile(void *banner, void *bannerTlut, void *icon, void *iconTlut,
                               s32 iconCount, s32 animType, s32 iconSpeed, const char *comment,
                               const char *subComment) {
    m_banner = banner;
    m_bannerTlut = bannerTlut;
    m_icon = icon;
    m_iconTlut = iconTlut;
    m_iconCount = iconCount;
    m_animType = animType;
    m_iconSpeed = iconSpeed;
    m_comment = new char[0x40];
    memset(m_comment, 0, 0x40);
    char *text = m_comment;
    fn_803ACABC(text, comment, 0x20);
    fn_803ACABC(text + 0x20, subComment, 0x20);
    m_commentOffset = 0;
    m_size = 0x40;
    m_bannerOffset = m_size;
    if (m_banner != 0) {
        if (m_bannerTlut == 0) {
            m_size += 0x1800;
        } else {
            m_size += 0xE00;
        }
    }
    m_iconOffset = m_size;
    if (m_iconCount <= 0 || m_icon == 0) {
        m_iconCount = 1;
        m_icon = 0;
        m_iconTlut = 0;
    }
    if (m_iconCount > CARD_ICON_MAX) {
        m_iconCount = CARD_ICON_MAX;
    }
    if (m_iconTlut == 0) {
        m_size += m_iconCount * 0x800;
    } else {
        m_size += iconCount * 0x400 + 0x200;
    }
}

MemoryCardFile::~MemoryCardFile() {
    if (m_comment != 0) {
        delete[] m_comment;
    }
}

u32 MemoryCardFile::GetSize() {
    return m_size;
}

u32 MemoryCardFile::Build(u8 *dst) {
    memcpy(dst, m_comment, 0x40);
    dst += 0x40;
    if (m_banner != 0) {
        if (m_bannerTlut == 0) {
            memcpy(dst, m_banner, 0x1800);
            dst += 0x1800;
        } else {
            memcpy(dst, m_banner, 0xC00);
            memcpy(dst + 0xC00, m_bannerTlut, 0x200);
            dst += 0xE00;
        }
    }
    if (m_icon == 0) {
        u16 *pixel = (u16 *)dst;
        u16 color = 0xFFFF;
        int y;
        int x;
        for (y = 0; y < 32; y++) {
            for (x = 0; x < 32; x++) {
                *pixel++ = color--;
            }
        }
    } else if (m_iconTlut == 0) {
        memcpy(dst, m_icon, m_iconCount << 11);
    } else {
        memcpy(dst, m_icon, m_iconCount << 10);
        memcpy(dst + 1, m_iconTlut, m_iconCount << 9);
    }
    return m_size;
}

s32 MemoryCardFile::GetBannerFormat() {
    if (m_banner == 0) {
        return 0;
    }
    return m_bannerTlut == 0 ? 2 : 1;
}

s32 MemoryCardFile::GetIconFormat() {
    return m_iconTlut == 0 ? 2 : 1;
}

s32 MemoryCardFile::GetIconSpeed() {
    switch (m_iconSpeed) {
    default:
        return 3;
    case 1:
        return 2;
    case 0:
        return 1;
    }
}

s32 MemoryCardFile::GetIconAnim() {
    switch (m_animType) {
    default:
        return 0;
    case 1:
        return 4;
    }
}

MemoryCard *MemoryCard::Create(s32 channel) {
    if (channel < 0 || channel >= 2) {
        return 0;
    }
    return new MemoryCard(channel);
}

MemoryCard::MemoryCard(s32 channel) {
    m_work = new MemoryCardWork;
    if (channel < 0) {
        channel = 0;
    } else if (channel >= 2) {
        channel = 1;
    }
    m_channel = channel;
    m_status = 0;
    m_command = CMD_IDLE;
    m_started = 0;
    m_busy = 0;
    m_request = 0;
    m_result = 3;
    m_flags = 0;
    m_size = 0;
    m_data = 0;
    m_file = 0;
    m_freeBytes = 0;
    m_freeFiles = 0;
}

MemoryCard::~MemoryCard() {
    if (m_flags & 1) {
        fn_803908C8(m_channel);
    }
    if (m_flags & 2) {
        fn_803916E8(&m_work->m_fileInfo);
    }
    m_flags = 0;
    if (m_work != 0) {
        delete m_work;
    }
}

s32 MemoryCard::GetStatus() {
    return m_status;
}

s32 MemoryCard::IsBusy() {
    return m_busy;
}

s32 MemoryCard::GetResult() {
    return m_result;
}

s32 MemoryCard::GetMatchCount() {
    return m_work->m_matchCount;
}

bool MemoryCard::HasRoom(s32 size) {
    bool room = false;
    if (m_freeBytes >= size && m_freeFiles > 0) {
        room = true;
    }
    return room;
}

bool MemoryCard::BeginRequest(s32 request) {
    if (m_busy == 0 && Start(CMD_MOUNT)) {
        m_busy = 1;
        m_request = request;
        m_result = 1;
        return true;
    }
    return false;
}

bool MemoryCard::RequestFreeBlocks() {
    if (BeginRequest(6)) {
        m_started = 1;
        m_command = CMD_MOUNT;
        return true;
    }
    return false;
}

bool MemoryCard::RequestCreate(const char *name, s32 size, void *data, MemoryCardFile *file) {
    if (BeginRequest(2)) {
        fn_803ACABC(m_work->m_fileName, name, CARD_FILENAME_MAX);
        m_size = size;
        m_data = data;
        m_file = file;
        m_started = 1;
        m_command = CMD_MOUNT;
        return true;
    }
    return false;
}

bool MemoryCard::RequestDelete(const char *name) {
    if (BeginRequest(3)) {
        fn_803ACABC(m_work->m_fileName, name, CARD_FILENAME_MAX);
        m_started = 1;
        m_command = CMD_MOUNT;
        return true;
    }
    return false;
}

bool MemoryCard::RequestRead(const char *name, s32 size, void *data) {
    if (BeginRequest(4)) {
        fn_803ACABC(m_work->m_fileName, name, CARD_FILENAME_MAX);
        m_size = size;
        m_data = data;
        m_started = 1;
        m_command = CMD_MOUNT;
        return true;
    }
    return false;
}

bool MemoryCard::RequestWrite(const char *name, s32 size, void *data, MemoryCardFile *file) {
    if (BeginRequest(5)) {
        fn_803ACABC(m_work->m_fileName, name, CARD_FILENAME_MAX);
        m_size = size;
        m_data = data;
        m_started = 1;
        m_command = CMD_MOUNT;
        return true;
    }
    return false;
}

bool MemoryCard::RequestSearch(const char *name) {
    if (BeginRequest(7)) {
        fn_803ACABC(m_work->m_fileName, name, CARD_FILENAME_MAX);
        m_work->m_matchCount = 0;
        m_work->m_searchIndex = 0;
        m_started = 1;
        m_command = CMD_MOUNT;
        return true;
    }
    return false;
}

bool MemoryCard::RequestFormat() {
    if (BeginRequest(1)) {
        m_started = 1;
        m_command = CMD_MOUNT;
        return true;
    }
    return false;
}

bool MemoryCard::Start(s32 command) {
    bool started = true;
    switch (command) {
    case CMD_IDLE:
    case CMD_READY:
    case CMD_ERROR:
        break;
    case CMD_MOUNT:
        started = StartMount();
        break;
    case CMD_CHECK:
        started = StartCheck();
        break;
    case CMD_OPEN:
        started = StartOpen();
        break;
    case CMD_GET_STATUS:
        started = StartGetStatus();
        break;
    case CMD_FORMAT:
        started = StartFormat();
        break;
    case CMD_CREATE:
        started = StartCreate();
        break;
    case CMD_DELETE:
        started = StartDelete();
        break;
    case CMD_READ:
        started = StartRead();
        break;
    case CMD_WRITE:
        started = StartWrite();
        break;
    case CMD_FREE_BLOCKS:
        started = StartFreeBlocks();
        break;
    case CMD_SET_STATUS:
        started = StartSetStatus();
        break;
    case CMD_SEARCH:
        started = StartSearch();
        break;
    }
    return started;
}

void MemoryCard::Poll(s32 command) {
    switch (command) {
    case CMD_IDLE:
        PollIdle();
        break;
    case CMD_READY:
        PollReady();
        break;
    case CMD_ERROR:
        PollError();
        break;
    case CMD_MOUNT:
        PollMount();
        break;
    case CMD_CHECK:
        PollCheck();
        break;
    case CMD_OPEN:
        PollOpen();
        break;
    case CMD_GET_STATUS:
        PollGetStatus();
        break;
    case CMD_FORMAT:
        PollFormat();
        break;
    case CMD_CREATE:
        PollCreate();
        break;
    case CMD_DELETE:
        PollDelete();
        break;
    case CMD_READ:
        PollRead();
        break;
    case CMD_WRITE:
        PollWrite();
        break;
    case CMD_FREE_BLOCKS:
        PollFreeBlocks();
        break;
    case CMD_SET_STATUS:
        PollSetStatus();
        break;
    case CMD_SEARCH:
        PollSearch();
        break;
    }
}

s32 MemoryCard::Update() {
    if (m_started == 0) {
        if (Start(m_command)) {
            m_started = 1;
            Poll(m_command);
        }
    } else {
        Poll(m_command);
    }
    return m_busy;
}

void MemoryCard::SetCommand(s32 command) {
    if (Start(command)) {
        m_started = 1;
        m_command = command;
    } else {
        m_started = 0;
        m_command = command;
    }
}

void MemoryCard::Finish() {
    m_busy = 0;
    m_request = 0;
    m_result = 0;
    if (m_flags & 1) {
        fn_803908C8(m_channel);
    }
    if (m_flags & 2) {
        fn_803916E8(&m_work->m_fileInfo);
    }
    m_flags = 0;
    SetCommand(CMD_READY);
}

void MemoryCard::Fail() {
    m_busy = 0;
    m_request = 0;
    if (m_flags & 1) {
        fn_803908C8(m_channel);
    }
    if (m_flags & 2) {
        fn_803916E8(&m_work->m_fileInfo);
    }
    m_flags = 0;
    SetCommand(CMD_ERROR);
}

void MemoryCard::SetError(s32 result) {
    s32 error;
    switch (result) {
    case CARD_RESULT_NOPERM:
    case CARD_RESULT_FATAL_ERROR:
        error = 2;
        break;
    case CARD_RESULT_WRONGDEVICE:
        error = 4;
        break;
    case CARD_RESULT_NOCARD:
        error = 3;
        break;
    case CARD_RESULT_IOERROR:
    case CARD_RESULT_BROKEN:
        error = 5;
        break;
    case CARD_RESULT_ENCODING:
        error = 12;
        break;
    case CARD_RESULT_NOFILE:
        error = 9;
        break;
    default:
        error = 2;
        break;
    }
    m_result = error;
    Fail();
}

/* Waits for the probe and turns its result into m_status: 0 ready, 2 error, 3 no card, 4 wrong
 * device. */
#define PROBE_CARD(result)                                                   \
    {                                                                        \
        s32 sectorSize;                                                      \
        s32 memSize;                                                         \
        do {                                                                 \
            result = fn_8038FF84(m_channel, &memSize, &sectorSize);          \
        } while (result == CARD_RESULT_BUSY);                                \
    }

#define SET_STATUS(result)                     \
    {                                          \
        s32 status;                            \
        switch (result) {                      \
        case CARD_RESULT_FATAL_ERROR:          \
        default:                               \
            status = 2;                        \
            break;                             \
        case CARD_RESULT_NOCARD:               \
        case CARD_RESULT_BUSY:                 \
            status = 3;                        \
            break;                             \
        case CARD_RESULT_WRONGDEVICE:          \
            status = 4;                        \
            break;                             \
        case CARD_RESULT_READY:                \
            status = 0;                        \
            break;                             \
        }                                      \
        m_status = status;                     \
    }

bool MemoryCard::CheckCard() {
    s32 result;
    PROBE_CARD(result);
    SET_STATUS(result);
    if (result != CARD_RESULT_READY) {
        m_result = m_status;
        Fail();
        return true;
    }
    return false;
}

void MemoryCard::PollIdle() {
    s32 result;
    PROBE_CARD(result);
    SET_STATUS(result);
    if (result == CARD_RESULT_READY) {
        SetCommand(CMD_READY);
    } else if (result == CARD_RESULT_BUSY) {
        SetCommand(CMD_ERROR);
    }
}

void MemoryCard::PollReady() {
    s32 result;
    PROBE_CARD(result);
    SET_STATUS(result);
    if (result != CARD_RESULT_READY) {
        SetCommand(CMD_ERROR);
    }
}

void MemoryCard::PollError() {
    s32 result;
    PROBE_CARD(result);
    SET_STATUS(result);
    if (result == CARD_RESULT_READY) {
        SetCommand(CMD_READY);
    }
}

bool MemoryCard::StartMount() {
    s32 result;
    PROBE_CARD(result);
    if (result != CARD_RESULT_READY) {
        return false;
    }
    return fn_8039068C(m_channel, m_work->m_workArea, 0, 0) != CARD_RESULT_BUSY;
}

void MemoryCard::PollMount() {
    s32 result;
    if (CheckCard()) {
        return;
    }
    result = fn_8038D238(m_channel);
    if (result == CARD_RESULT_BUSY) {
        return;
    }
    switch (result) {
    case CARD_RESULT_READY:
    case CARD_RESULT_BROKEN:
    case CARD_RESULT_ENCODING:
        m_flags |= 1;
        break;
    default:
        m_flags &= ~1;
        break;
    }
    if (result == CARD_RESULT_READY || result == CARD_RESULT_BROKEN) {
        SetCommand(CMD_CHECK);
    } else if (m_request == 1 && result == CARD_RESULT_ENCODING) {
        SetCommand(CMD_FORMAT);
    } else {
        SetError(result);
    }
}

bool MemoryCard::StartCheck() {
    s32 result;
    PROBE_CARD(result);
    if (result != CARD_RESULT_READY) {
        return false;
    }
    return fn_8038FE90(m_channel, 0) != CARD_RESULT_BUSY;
}

void MemoryCard::PollCheck() {
    s32 result;
    if (CheckCard()) {
        return;
    }
    result = fn_8038D238(m_channel);
    if (result == CARD_RESULT_BUSY) {
        return;
    }
    if (result == CARD_RESULT_READY) {
        switch (m_request) {
        case 0:
            m_result = 2;
            Fail();
            break;
        case 1:
            SetCommand(CMD_FORMAT);
            break;
        case 6:
            SetCommand(CMD_FREE_BLOCKS);
            break;
        case 4:
        case 5:
            SetCommand(CMD_OPEN);
            break;
        case 2:
            SetCommand(CMD_CREATE);
            break;
        case 3:
            SetCommand(CMD_DELETE);
            break;
        case 7:
            SetCommand(CMD_SEARCH);
            break;
        }
    } else if (m_request == 1) {
        SetCommand(CMD_FORMAT);
    } else {
        m_result = 6;
        Fail();
    }
}

bool MemoryCard::StartOpen() {
    s32 result;
    PROBE_CARD(result);
    if (result != CARD_RESULT_READY) {
        return false;
    }
    result = fn_803915CC(m_channel, m_work->m_fileName, &m_work->m_fileInfo);
    if (result == CARD_RESULT_READY) {
        m_flags |= 2;
    } else {
        m_flags &= ~2;
    }
    return result != CARD_RESULT_BUSY;
}

void MemoryCard::PollOpen() {
    s32 result;
    if (CheckCard()) {
        return;
    }
    if (m_flags & 2) {
        result = fn_8038D238(m_channel);
    } else {
        result = fn_803915CC(m_channel, m_work->m_fileName, &m_work->m_fileInfo);
    }
    if (result == CARD_RESULT_BUSY) {
        return;
    }
    if (result == CARD_RESULT_READY) {
        m_flags |= 2;
        m_work->m_fileNo = m_work->m_fileInfo.fileNo;
        /* Retail compares the request as in PollGetStatus, with case 2 sent to the failure block
         * above it; GC/2.7 keeps a copy of the block for case 2 instead of merging the two. */
        switch (m_request) {
        case 3:
        case 6:
        default:
            m_result = 2;
            Fail();
            break;
        case 2:
            m_result = 2;
            Fail();
            break;
        case 4:
        case 5:
            SetCommand(CMD_GET_STATUS);
            break;
        }
    } else {
        SetError(result);
    }
}

bool MemoryCard::StartGetStatus() {
    s32 result;
    PROBE_CARD(result);
    if (result != CARD_RESULT_READY) {
        return false;
    }
    result = fn_803925A0(m_channel, m_work->m_fileNo, &m_work->m_stat);
    if (result == CARD_RESULT_READY) {
        m_flags |= 4;
    } else {
        m_flags &= ~4;
    }
    return result != CARD_RESULT_BUSY;
}

void MemoryCard::PollGetStatus() {
    s32 result;
    if (CheckCard()) {
        return;
    }
    if (m_flags & 4) {
        result = fn_8038D238(m_channel);
    } else {
        result = fn_803925A0(m_channel, m_work->m_fileNo, &m_work->m_stat);
    }
    if (result == CARD_RESULT_BUSY) {
        return;
    }
    if (result == CARD_RESULT_READY) {
        m_flags |= 4;
        switch (m_request) {
        case 3:
        case 6:
        default:
            m_result = 2;
            Fail();
            break;
        case 2:
            SetCommand(CMD_WRITE);
            break;
        case 4:
        case 5:
            if (m_work->m_stat.iconAddr == 0xFFFFFFFF) {
                m_result = 8;
                Fail();
            } else if (m_request == 4) {
                SetCommand(CMD_READ);
            } else {
                SetCommand(CMD_WRITE);
            }
            break;
        }
    } else {
        SetError(result);
    }
}

bool MemoryCard::StartFormat() {
    s32 result;
    PROBE_CARD(result);
    if (result != CARD_RESULT_READY) {
        return false;
    }
    return fn_80391110(m_channel, 0) != CARD_RESULT_BUSY;
}

void MemoryCard::PollFormat() {
    s32 result;
    if (CheckCard()) {
        return;
    }
    result = fn_8038D238(m_channel);
    if (result == CARD_RESULT_BUSY) {
        return;
    }
    if (result == CARD_RESULT_READY) {
        if (m_request == 1) {
            Finish();
        } else {
            m_result = 2;
            Fail();
        }
    } else {
        SetError(result);
    }
}

bool MemoryCard::StartCreate() {
    s32 result;
    PROBE_CARD(result);
    if (result != CARD_RESULT_READY) {
        return false;
    }
    return fn_80391874(m_channel, m_work->m_fileName, m_size, &m_work->m_fileInfo, 0) !=
           CARD_RESULT_BUSY;
}

void MemoryCard::PollCreate() {
    s32 result;
    if (CheckCard()) {
        return;
    }
    result = fn_8038D238(m_channel);
    if (result == CARD_RESULT_BUSY) {
        return;
    }
    if (result == CARD_RESULT_READY) {
        SetCommand(CMD_GET_STATUS);
        m_flags |= 2;
        m_work->m_fileNo = m_work->m_fileInfo.fileNo;
        return;
    }
    switch (result) {
    case CARD_RESULT_EXIST:
        m_result = 10;
        Fail();
        break;
    case CARD_RESULT_NOENT:
    case CARD_RESULT_INSSPACE:
        m_result = 7;
        Fail();
        break;
    case CARD_RESULT_NAMETOOLONG:
        m_result = 2;
        Fail();
        break;
    default:
        SetError(result);
        break;
    }
}

bool MemoryCard::StartDelete() {
    s32 result;
    PROBE_CARD(result);
    if (result != CARD_RESULT_READY) {
        return false;
    }
    return fn_80392298(m_channel, m_work->m_fileName, 0) != CARD_RESULT_BUSY;
}

void MemoryCard::PollDelete() {
    s32 result;
    if (CheckCard()) {
        return;
    }
    result = fn_8038D238(m_channel);
    if (result == CARD_RESULT_BUSY) {
        return;
    }
    if (result == CARD_RESULT_READY) {
        if (m_request == 3) {
            Finish();
        } else {
            m_result = 2;
            Fail();
        }
    } else {
        SetError(result);
    }
}

bool MemoryCard::StartRead() {
    s32 result;
    PROBE_CARD(result);
    if (result != CARD_RESULT_READY) {
        return false;
    }
    return fn_80391D7C(&m_work->m_fileInfo, m_data, m_size, 0, 0) != CARD_RESULT_BUSY;
}

void MemoryCard::PollRead() {
    s32 result;
    if (CheckCard()) {
        return;
    }
    result = fn_8038D238(m_channel);
    if (result == CARD_RESULT_BUSY) {
        return;
    }
    if (result == CARD_RESULT_READY) {
        if (m_request == 4) {
            Finish();
        } else {
            m_result = 2;
            Fail();
        }
    } else {
        SetError(result);
    }
}

bool MemoryCard::StartWrite() {
    s32 result;
    PROBE_CARD(result);
    if (result != CARD_RESULT_READY) {
        return false;
    }
    return fn_803920E0(&m_work->m_fileInfo, m_data, m_size, 0, 0) != CARD_RESULT_BUSY;
}

void MemoryCard::PollWrite() {
    s32 result;
    if (CheckCard()) {
        return;
    }
    result = fn_8038D238(m_channel);
    if (result == CARD_RESULT_BUSY) {
        return;
    }
    if (result == CARD_RESULT_READY) {
        if (m_request == 2) {
            MemoryCardWork *work = m_work;
            s32 speed;
            int i;
            work->m_stat.commentAddr = m_file->m_commentOffset;
            work->m_stat.bannerFormat =
                (work->m_stat.bannerFormat & ~3) | m_file->GetBannerFormat();
            work->m_stat.iconAddr = m_file->m_bannerOffset;
            speed = m_file->GetIconSpeed();
            for (i = 0; i < m_file->m_iconCount; i++) {
                work->m_stat.iconFormat = (work->m_stat.iconFormat & ~(3 << (i * 2))) |
                                          (m_file->GetIconFormat() << (i * 2));
                work->m_stat.iconSpeed = (work->m_stat.iconSpeed & ~(3 << (i * 2))) | (speed << (i * 2));
            }
            work->m_stat.iconSpeed &= ~(3 << (m_file->m_iconCount * 2));
            work->m_stat.bannerFormat = (work->m_stat.bannerFormat & ~4) | m_file->GetIconAnim();
            SetCommand(CMD_SET_STATUS);
        } else if (m_request == 5) {
            Finish();
        } else {
            m_result = 2;
            Fail();
        }
    } else {
        SetError(result);
    }
}

bool MemoryCard::StartFreeBlocks() {
    s32 result;
    PROBE_CARD(result);
    return result == CARD_RESULT_READY;
}

void MemoryCard::PollFreeBlocks() {
    s32 result;
    s32 bytes;
    s32 files;
    if (CheckCard()) {
        return;
    }
    result = fn_8038D268(m_channel, &bytes, &files);
    if (result == CARD_RESULT_BUSY) {
        return;
    }
    if (result == CARD_RESULT_READY) {
        m_freeBytes = bytes;
        m_freeFiles = files;
        if (m_request == 6) {
            Finish();
        } else {
            m_result = 2;
            Fail();
        }
    } else {
        SetError(result);
    }
}

bool MemoryCard::StartSetStatus() {
    s32 result;
    PROBE_CARD(result);
    if (result != CARD_RESULT_READY) {
        return false;
    }
    return fn_803926B4(m_channel, m_work->m_fileNo, &m_work->m_stat, 0) != CARD_RESULT_BUSY;
}

void MemoryCard::PollSetStatus() {
    s32 result;
    if (CheckCard()) {
        return;
    }
    result = fn_8038D238(m_channel);
    if (result == CARD_RESULT_BUSY) {
        return;
    }
    if (result == CARD_RESULT_READY) {
        if (m_request == 2) {
            Finish();
        } else {
            m_result = 2;
            Fail();
        }
    } else {
        SetError(result);
    }
}

bool MemoryCard::StartSearch() {
    s32 result;
    PROBE_CARD(result);
    return result == CARD_RESULT_READY;
}

void MemoryCard::PollSearch() {
    s32 result;
    if (CheckCard()) {
        return;
    }
    while (m_work->m_searchIndex != 127) {
        result = fn_803925A0(m_channel, m_work->m_searchIndex, &m_work->m_stat);
        if (result == CARD_RESULT_BUSY) {
            return;
        }
        if (result != CARD_RESULT_NOPERM &&
            fn_803A8DA8(m_work->m_company, m_work->m_stat.company, 2) == 0 &&
            fn_803A8DA8(m_work->m_gameName, m_work->m_stat.gameName, 4) == 0) {
            switch (result) {
            case CARD_RESULT_READY:
                if (fn_803AC6C0(m_work->m_stat.fileName, m_work->m_fileName) != 0) {
                    m_work->m_matchCount++;
                }
                break;
            case CARD_RESULT_NOFILE:
                break;
            default:
                m_work->m_matchCount = 0;
                SetError(result);
                return;
            }
        }
        m_work->m_searchIndex++;
    }
    if (m_work->m_searchIndex == 127) {
        Finish();
    }
}

void MemoryCard::SetGameCode(const char *company, const char *gameName) {
    memcpy(m_work->m_company, company, 2);
    memcpy(m_work->m_gameName, gameName, 4);
}

/* Encrypts (encrypt != 0) or decrypts the save data in place. */
void CryptSaveData(void *data, int encrypt) {
    const u64 key = 0xEEB06CAEA4784990ULL;
    if (data != 0) {
        char in[8];
        char out[8];
        CardDes des;
        u32 size = sizeof(UnkSaveData_800BB74C) - 4;
        u8 *block = (u8 *)data;
        des.SetKey((const char *)&key);
        for (; size != 0; size -= 8) {
            memset(in, 0, 8);
            memcpy(in, block, 8);
            des.Crypt(in, out, encrypt);
            memcpy(block, out, 8);
            block += 8;
        }
        memset(in, 0, 8);
        memset(out, 0, 8);
    }
}

/* Stores the digest of the save data in front of it. */
void SignSaveData(UnkSaveData_800BB74C *data) {
    CardMd5 md5;
    md5.Init();
    md5.Update(data->field_0x10, sizeof(UnkSaveData_800BB74C) - 0x10);
    md5.Final(data->m_digest);
}

/* Whether the digest in front of the save data matches it. */
bool CheckSaveData(UnkSaveData_800BB74C *data) {
    u8 digest[16];
    CardMd5 md5;
    bool ok;
    md5.Init();
    md5.Update(data->field_0x10, sizeof(UnkSaveData_800BB74C) - 0x10);
    md5.Final(digest);
    ok = fn_803A8DA8(digest, data->m_digest, 16) == 0;
    memset(digest, 0, 16);
    return ok;
}

extern "C" void fn_80299FE0(void) {
    int i;
    if (s_bannerTexture == 0) {
        s_bannerTexture = fn_8004509C(fn_800371D8(), "mc_bunner_e.tp", 0);
    }
    for (i = 0; i != 3; i++) {
        if (s_iconTextures[i] == 0) {
            s_iconTextures[i] = fn_8004509C(fn_800371D8(), s_iconNames[i], 0);
        }
    }
    s_saveData = fn_800BB74C();
}

extern "C" void fn_80299F5C(void) {
    int i;
    s_saveData = 0;
    if (s_bannerTexture != 0) {
        fn_80049B34(s_bannerTexture);
        s_bannerTexture = 0;
    }
    for (i = 0; i != 3; i++) {
        if (s_iconTextures[i] != 0) {
            fn_80049B34(s_iconTextures[i]);
            s_iconTextures[i] = 0;
        }
    }
    s_cardActive = 0;
}

extern "C" TMemoryCardCtrl *fn_80299F14(Task *parent) {
    return new TMemoryCardCtrl(parent);
}

TMemoryCardCtrl::TMemoryCardCtrl(Task *parent) : Task(parent) {
    int i;
    m_name = s_taskName;
    m_fileName[0] = 0;
    m_request = 0;
    m_step = 0;
    m_subStep = 0;
    m_fileCount = 0;
    for (i = 0; i < 100; i++) {
        m_slotStates[i] = 2;
    }
    m_result = 0;
    m_card = MemoryCard::Create(0);
    m_file = MemoryCardFile_Create(s_bannerTexture, 0, s_iconTextures[0], 0, 1, 0, 1, "GAMENAME",
                                   "FILEINFO");
    if (m_card != 0) {
        switch (lbl_8057D6A0.m_region) {
        case 1:
            m_card->SetGameCode(s_makerCode, s_gameCodeAmerica);
            break;
        case 2:
            m_card->SetGameCode(s_makerCode, s_gameCodeEurope);
            break;
        case 0:
        default:
            m_card->SetGameCode(s_makerCode, s_gameCodeJapan);
            break;
        }
    }
    m_fileNameFormat = "SHADOWTHEHEDGEHOG";
    m_searchName = "SHADOWTHEHEDGEHOG";
    m_headerSize = (m_file->GetSize() + 0x1F) & ~0x1F;
    m_imageSize = (m_headerSize + sizeof(UnkSaveData_800BB74C) + 0x1FFF) & ~0x1FFF;
    m_image = (u8 *)fn_80049B60(m_imageSize, 1, 2);
    m_saveData = m_image + m_headerSize;
    m_slotImages = 0;
    m_slotImage = 0;
    m_savedStamp = 0;
    m_checkStamp = 1;
    m_flags |= 0x100;
}

TMemoryCardCtrl::~TMemoryCardCtrl() {
    if (m_image != 0) {
        fn_80049B34(m_image);
    }
    if (m_slotImages != 0) {
        fn_80049B34(m_slotImages);
    }
    if (m_file != 0) {
        delete m_file;
    }
    if (m_card != 0) {
        delete m_card;
    }
    s_cardActive = 0;
}

void TMemoryCardCtrl::Tick(float dt) {
    int i;
    if (m_card != 0) {
        m_card->Update();
        if (m_card->GetStatus() != 0) {
            for (i = 0; i < 100; i++) {
                m_slotStates[i] = 2;
            }
        }
    }
    switch (m_request) {
    case 1:
        StepCreate(m_step);
        break;
    case 5:
        StepSearch(m_step);
        break;
    case 2:
        StepLoad(m_step);
        break;
    case 3:
        StepLoadChecked(m_step);
        break;
    case 4:
        StepSave(m_step);
        break;
    case 6:
        StepReadSlot(m_step);
        break;
    case 7:
        StepFormat(m_step);
        break;
    case 8:
        StepDelete(m_step);
        break;
    case 9:
        StepVerify(m_step);
        break;
    }
}

/* Requests: 2 load, 1 create, 4 save, 5 search; 7 format and 8 delete (names inferred). */
extern "C" bool fn_80299B4C(TMemoryCardCtrl *ctrl) {
    if (ctrl->m_request == 0 && ctrl->m_card->IsBusy() == 0) {
        ctrl->SetRequest(2, 0);
        return true;
    }
    return false;
}

extern "C" bool fn_80299AEC(TMemoryCardCtrl *ctrl) {
    if (ctrl->m_request == 0 && ctrl->m_card->IsBusy() == 0) {
        ctrl->SetRequest(4, 0);
        return true;
    }
    return false;
}

extern "C" bool fn_80299A8C(TMemoryCardCtrl *ctrl) {
    if (ctrl->m_request == 0 && ctrl->m_card->IsBusy() == 0) {
        ctrl->SetRequest(1, 0);
        return true;
    }
    return false;
}

extern "C" bool fn_80299A2C(TMemoryCardCtrl *ctrl) {
    if (ctrl->m_request == 0 && ctrl->m_card->IsBusy() == 0) {
        ctrl->SetRequest(5, 0);
        return true;
    }
    return false;
}

extern "C" bool fn_802999CC(TMemoryCardCtrl *ctrl) {
    if (ctrl->m_request == 0 && ctrl->m_card->IsBusy() == 0) {
        ctrl->SetRequest(7, 0);
        return true;
    }
    return false;
}

extern "C" bool fn_8029996C(TMemoryCardCtrl *ctrl) {
    if (ctrl->m_request == 0 && ctrl->m_card->IsBusy() == 0) {
        ctrl->SetRequest(8, 0);
        return true;
    }
    return false;
}

/* The number of matching files the last search found. */
extern "C" s32 fn_80299950(TMemoryCardCtrl *ctrl) {
    if (ctrl->m_request == 0) {
        return ctrl->m_fileCount;
    }
    return 0;
}

/* Writes a new file. */
void TMemoryCardCtrl::StepCreate(int step) {
    switch (step) {
    case 0: {
        UnkSaveData_800BB74C *saveData = s_saveData;
        saveData->m_stamp = fn_80051374();
        memset(m_image, 0, m_imageSize);
        RebuildFile();
        m_file->Build(m_image);
        memcpy(m_saveData, s_saveData, sizeof(UnkSaveData_800BB74C));
        SignSaveData((UnkSaveData_800BB74C *)m_saveData);
        CryptSaveData((UnkSaveData_800BB74C *)m_saveData, 1);
        m_card->RequestCreate(m_fileName, m_imageSize, m_image, m_file);
        m_step = 1;
        break;
    }
    case 1:
        if (m_card->IsBusy() == 0) {
            m_result = m_card->GetResult();
            SetRequest(0, 0);
        }
        break;
    }
}

/* Counts the files whose name contains m_searchName. */
void TMemoryCardCtrl::StepSearch(int step) {
    switch (step) {
    case 0:
        if (m_card->IsBusy() == 0) {
            m_card->RequestFreeBlocks();
            m_step = 1;
        }
        break;
    case 1:
        if (m_card->IsBusy() == 0) {
            m_card->RequestSearch(m_searchName);
            m_step = 2;
        }
        break;
    case 2:
        if (m_card->IsBusy() == 0) {
            if (m_card->GetResult() == 0) {
                m_fileCount = m_card->GetMatchCount();
            } else {
                m_fileCount = 0;
            }
            m_result = m_card->GetResult();
            SetRequest(0, 0);
        }
        break;
    }
}

/* Reads the file of slot m_slot and records whether it is valid. */
void TMemoryCardCtrl::StepReadSlot(int step) {
    switch (step) {
    case 0:
        if (m_slotImages == 0) {
            m_slotImage = m_image;
        } else {
            m_slotImage = m_slotImages + m_imageSize * m_slot;
        }
        m_card->RequestRead(m_fileName, m_imageSize, m_slotImage);
        m_step = 1;
        break;
    case 1:
        if (m_card->IsBusy() == 0) {
            UnkSaveData_800BB74C *saveData = (UnkSaveData_800BB74C *)(m_slotImage + m_headerSize);
            switch (m_card->GetResult()) {
            case 0:
                CryptSaveData(saveData, 0);
                if (!CheckSaveData(saveData)) {
                    m_slotStates[m_slot] = 3;
                } else {
                    m_slotStates[m_slot] = 1;
                }
                m_result = 0;
                break;
            case 9:
                m_slotStates[m_slot] = 0;
                m_result = 0;
                break;
            default:
                m_slotStates[m_slot] = 3;
                m_result = m_card->GetResult();
                break;
            }
            SetRequest(0, 0);
        }
        break;
    }
}

/* Formats the card. */
void TMemoryCardCtrl::StepFormat(int step) {
    switch (step) {
    case 0:
        m_card->RequestFormat();
        m_step = 1;
        break;
    case 1:
        if (m_card->IsBusy() == 0) {
            m_result = m_card->GetResult();
            SetRequest(0, 0);
        }
        break;
    }
}

/* Loads the file into the game's save data. */
void TMemoryCardCtrl::StepLoad(int step) {
    switch (step) {
    case 0:
        m_card->RequestRead(m_fileName, m_imageSize, m_image);
        m_step = 1;
        break;
    case 1:
        if (m_card->IsBusy() == 0) {
            switch (m_card->GetResult()) {
            case 0: {
                UnkSaveData_800BB74C *saveData = (UnkSaveData_800BB74C *)m_saveData;
                CryptSaveData(saveData, 0);
                if (!CheckSaveData(saveData)) {
                    m_result = 8;
                } else {
                    memcpy(s_saveData, saveData, sizeof(UnkSaveData_800BB74C));
                    m_result = 0;
                }
                SetRequest(0, 0);
                break;
            }
            default:
                m_result = m_card->GetResult();
                SetRequest(0, 0);
                break;
            }
        }
        break;
    }
}

/* Loads the file if its stamp matches the game's. */
void TMemoryCardCtrl::StepLoadChecked(int step) {
    switch (step) {
    case 0:
        m_card->RequestRead(m_fileName, m_imageSize, m_image);
        m_step = 1;
        break;
    case 1:
        if (m_card->IsBusy() == 0) {
            switch (m_card->GetResult()) {
            case 0: {
                UnkSaveData_800BB74C *saveData = (UnkSaveData_800BB74C *)m_saveData;
                CryptSaveData(saveData, 0);
                if (!CheckSaveData(saveData)) {
                    m_result = 8;
                } else if (saveData->m_stamp != s_saveData->m_stamp) {
                    m_result = 11;
                    SetRequest(0, 0);
                } else {
                    memcpy(s_saveData, saveData, sizeof(UnkSaveData_800BB74C));
                    m_result = 0;
                }
                SetRequest(0, 0);
                break;
            }
            default:
                m_result = m_card->GetResult();
                SetRequest(0, 0);
                break;
            }
        }
        break;
    }
}

/* Checks the file on the card, then overwrites it. */
void TMemoryCardCtrl::StepSave(int step) {
    if (m_subStep == 0) {
        switch (step) {
        case 0:
            m_card->RequestRead(m_fileName, m_imageSize, m_image);
            m_step = 1;
            break;
        case 1:
            if (m_card->IsBusy() == 0) {
                switch (m_card->GetResult()) {
                case 0: {
                    UnkSaveData_800BB74C *saveData = (UnkSaveData_800BB74C *)m_saveData;
                    CryptSaveData(saveData, 0);
                    if (!CheckSaveData(saveData)) {
                        m_result = 8;
                        SetRequest(0, 0);
                    } else if (m_checkStamp != 0 && saveData->m_stamp != s_saveData->m_stamp) {
                        m_result = 11;
                        SetRequest(0, 0);
                    } else {
                        m_step = 0;
                        m_subStep = 1;
                    }
                    break;
                }
                default:
                    m_result = m_card->GetResult();
                    SetRequest(0, 0);
                    break;
                }
            }
            break;
        }
    }
    if (m_subStep == 1) {
        switch (m_step) {
        case 0: {
            m_savedStamp = s_saveData->m_stamp;
            UnkSaveData_800BB74C *saveData = s_saveData;
            saveData->m_stamp = fn_80051374();
            memset(m_image, 0, m_imageSize);
            RebuildFile();
            m_file->Build(m_image);
            memcpy(m_saveData, s_saveData, sizeof(UnkSaveData_800BB74C));
            SignSaveData((UnkSaveData_800BB74C *)m_saveData);
            CryptSaveData((UnkSaveData_800BB74C *)m_saveData, 1);
            m_card->RequestWrite(m_fileName, m_imageSize, m_image, m_file);
            m_step = 1;
            break;
        }
        case 1:
            if (m_card->IsBusy() == 0) {
                switch (m_card->GetResult()) {
                case 0: {
                    m_result = 0;
                    SetRequest(0, 0);
                    break;
                }
                default:
                    s_saveData->m_stamp = m_savedStamp;
                    m_result = m_card->GetResult();
                    SetRequest(0, 0);
                    break;
                }
            }
            break;
        }
    }
}

/* Deletes the file. */
void TMemoryCardCtrl::StepDelete(int step) {
    switch (step) {
    case 0:
        m_card->RequestDelete(m_fileName);
        m_step = 1;
        break;
    case 1:
        if (m_card->IsBusy() == 0) {
            m_result = m_card->GetResult();
            SetRequest(0, 0);
        }
        break;
    }
}

/* Reads the file and checks its digest and stamp. */
void TMemoryCardCtrl::StepVerify(int step) {
    switch (step) {
    case 0:
        m_card->RequestRead(m_fileName, m_imageSize, m_image);
        m_step = 1;
        break;
    case 1:
        if (m_card->IsBusy() == 0) {
            switch (m_card->GetResult()) {
            case 0: {
                UnkSaveData_800BB74C *saveData = (UnkSaveData_800BB74C *)m_saveData;
                CryptSaveData(saveData, 0);
                if (!CheckSaveData(saveData)) {
                    m_result = 8;
                } else if (m_checkStamp != 0 && saveData->m_stamp != s_saveData->m_stamp) {
                    m_result = 11;
                } else {
                    m_result = 0;
                }
                SetRequest(0, 0);
                break;
            }
            default:
                m_result = m_card->GetResult();
                SetRequest(0, 0);
                break;
            }
        }
        break;
    }
}

void TMemoryCardCtrl::SetRequest(int request, int slot) {
    int i;
    m_request = request;
    m_step = 0;
    m_subStep = 0;
    s_cardActive = 0;
    switch (request) {
    case 1:
        s_cardActive = 1;
        m_slot = 0;
        break;
    case 4:
        s_cardActive = 1;
        m_slot = 0;
        break;
    case 2:
    case 3:
    case 8:
    case 9:
        m_slot = 0;
        break;
    case 5:
        m_fileCount = 0;
        m_slot = 0;
        for (i = 0; i < 100; i++) {
            m_slotStates[i] = 2;
        }
        break;
    case 6:
        m_slot = slot;
        break;
    case 7:
        s_cardActive = 1;
        m_slot = 0;
        break;
    }
    sprintf(m_fileName, "%s", m_fileNameFormat);
}

/* Whether the request finished (behaviour inferred). */
extern "C" bool fn_80298EBC(TMemoryCardCtrl *ctrl) {
    return ctrl->m_request != 0;
}

extern "C" s32 fn_80298EB4(TMemoryCardCtrl *ctrl) {
    return ctrl->m_result;
}

/* Whether the card has room for the file. */
extern "C" bool fn_80298E88(TMemoryCardCtrl *ctrl) {
    return ctrl->m_card->HasRoom(ctrl->m_imageSize);
}

extern "C" void fn_80298E80(TMemoryCardCtrl *ctrl, u8 checkStamp) {
    ctrl->m_checkStamp = checkStamp;
}

extern "C" s32 fn_80298E5C(TMemoryCardCtrl *ctrl) {
    return ctrl->m_card->GetStatus();
}

/* Recreates the file description with the comment in the game's language. */
void TMemoryCardCtrl::RebuildFile() {
    char subComment[64];
    char comment[64];
    if (m_file != 0) {
        delete m_file;
    }
    if (lbl_8057D6A0.m_region == 0) {
        sprintf(comment, "\x83\x56\x83\x83\x83\x68\x83\x45\x81\x45\x83\x55\x81\x45\x83\x77\x83\x62"
                         "\x83\x57\x83\x7A\x83\x62\x83\x4F");
        sprintf(subComment, "\x83\x51\x81\x5B\x83\x80\x83\x66\x81\x5B\x83\x5E");
    } else {
        sprintf(comment, "SHADOW THE HEDGEHOG");
        sprintf(subComment, "GAME DATA");
    }
    m_file = MemoryCardFile_Create(s_bannerTexture, 0, s_iconTextures[0], 0, 1, 0, 1, comment,
                                   subComment);
}
