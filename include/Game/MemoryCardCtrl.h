#ifndef GAME_MEMORYCARDCTRL_H
#define GAME_MEMORYCARDCTRL_H

#include "Game/Task.h"
#include <dolphin/types.h>
#include "size_assert.h"

class MemoryCard;
class MemoryCardFile;

/* TMemoryCardCtrl (class name from RTTI): the task that drives the memory card in slot A (its
 * methods lie at 0x80298D88-0x8029A078; see Game/MemoryCardCtrl.cpp). A caller starts one request
 * at a time (save, load, search, format, ...), polls until m_request drops back to 0 and reads
 * m_result. The size is from the allocation in fn_80299F14. Member names are inferred. */
class TMemoryCardCtrl : public Task {
  public:
    TMemoryCardCtrl(Task *parent);
    virtual ~TMemoryCardCtrl();
    virtual void Tick(float dt);

    void RebuildFile();
    void SetRequest(int request, int slot);
    void StepVerify(int step);
    void StepDelete(int step);
    void StepSave(int step);
    void StepLoadChecked(int step);
    void StepLoad(int step);
    void StepFormat(int step);
    void StepReadSlot(int step);
    void StepSearch(int step);
    void StepCreate(int step);

    /* 0x028 */ MemoryCard *m_card;
    /* 0x02C */ MemoryCardFile *m_file;
    /* 0x030 */ char m_fileName[0x80];
    /* 0x0B0 */ u8 *m_image;     /* the whole file as written to the card */
    /* 0x0B4 */ u8 *m_slotImages; /* optional: one file image per slot, read by request 6 */
    /* 0x0B8 */ u8 *m_saveData;  /* the encrypted save data inside m_image */
    /* 0x0BC */ s32 m_request;   /* 0 when idle */
    /* 0x0C0 */ s32 m_step;
    /* 0x0C4 */ s32 m_fileCount;
    /* 0x0C8 */ s32 m_slot;
    /* 0x0CC */ s32 m_result;
    /* 0x0D0 */ u32 m_headerSize;
    /* 0x0D4 */ u32 m_imageSize;
    /* 0x0D8 */ s32 m_slotStates[100];
    /* 0x268 */ u32 m_subStep;
    /* 0x26C */ const char *m_fileNameFormat;
    /* 0x270 */ const char *m_searchName;
    /* 0x274 */ u8 *m_slotImage;
    /* 0x278 */ u32 m_savedStamp;
    /* 0x27C */ u8 m_checkStamp;
};
SIZE_ASSERT(TMemoryCardCtrl, 0x280);

extern "C" {
/* Creates the controller below parent. */
TMemoryCardCtrl *fn_80299F14(Task *parent);
/* Releases and loads the controller's banner and icon textures and the save data pointer. */
void fn_80299F5C(void);
void fn_80299FE0(void);
}

#endif
