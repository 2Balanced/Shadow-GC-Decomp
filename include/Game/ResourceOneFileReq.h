#ifndef GAME_RESOURCEONEFILEREQ_H
#define GAME_RESOURCEONEFILEREQ_H

#include <dolphin/types.h>
#include "size_assert.h"

/* ResourceOneFileReq (class name from RTTI; vtable at 0x8051FA3C, base Resource): a single-file
 * resource loaded on request, Game/ResourceOneFileReq.cpp. list is the request list the entries to
 * load are copied from (the global one at 0x80575EE4, which the request functions fn_8006B474 and
 * fn_8006B50C fill). The size comes from the allocation in GadgetCommand.cpp's loader. Units that
 * only construct one see the short declaration; Game/ResourceOneFileReq.cpp defines
 * RESOURCEONEFILEREQ_FULL for the whole class. */
#ifdef RESOURCEONEFILEREQ_FULL
#include <string>
#include <vector>

#include "Game/Resource.h"

/* A request: an entry of the file, given by index or (flag bit 0) by name, and its type (1: the
 * entry is a .BIN file whose references are requested too). Struct and member names inferred. */
struct ResourceRequest {
    ResourceRequest(int index, int type) {
        m_index = index;
        strcpy(m_name, "");
        m_flags = 0;
        m_type = type;
    }
    ResourceRequest(const char *name, int type) {
        m_index = 0;
        strcpy(m_name, name);
        m_flags = 1;
        m_type = type;
    }

    /* 0x00 */ int m_index;
    /* 0x04 */ u32 m_flags;
    /* 0x08 */ int m_type;
    /* 0x0C */ char m_name[0x20];
};
SIZE_ASSERT(ResourceRequest, 0x2C);

inline bool operator==(const ResourceRequest &a, const ResourceRequest &b) {
    return a.m_index == b.m_index && a.m_flags == b.m_flags && a.m_type == b.m_type &&
           strcmp(a.m_name, b.m_name) == 0;
}

/* A loaded entry of the file (struct and member names inferred). m_type says what m_data is: 1 a
 * bone file (.BON, .BOP), 2 a motion (.MTN, .MTP), 3 a clump (.DFF, .RP2), 4 a texture dictionary
 * (.TXD), 5 a UV animation (.UVA), 6 a .BIN file, 7 a .PTB/.CCL file, 8 a .DMA file, 9 a CSD project
 * (.SNCP, .GNCP, .XNCP), 10 an .ADB file. */
struct ResourceOneFileReqEntry {
    ResourceOneFileReqEntry() {
        strcpy(m_name, "");
        m_data = 0;
        m_extra = 0;
        m_file = 0;
        m_type = 0;
        m_hierarchy = 0;
        m_motion = 0;
        m_size = 0;
    }

    /* 0x00 */ char m_name[0x20];
    /* 0x20 */ void *m_data;
    /* 0x24 */ void *m_extra;
    /* 0x28 */ struct UnkPath_8040E61C *m_file;
    /* 0x2C */ u32 m_type;
    /* 0x30 */ void *m_hierarchy;
    /* 0x34 */ u32 m_motion;
    /* 0x38 */ u32 m_size;
};
SIZE_ASSERT(ResourceOneFileReqEntry, 0x3C);

class ResourceOneFileReq : public Resource {
  public:
    ResourceOneFileReq(const char *fileName, void *list);
    virtual ~ResourceOneFileReq();
    virtual void Load();
    virtual bool Release();
    virtual int GetState();
    virtual void *GetData(int index);
    virtual void *GetData(const char *name);
    virtual void *GetHierarchy(int index);
    virtual void *GetHierarchy(const char *name);
    virtual u32 GetMotion(int index);
    virtual u32 GetMotion(const char *name);
    virtual void *vf_0x30(int index);
    virtual void *vf_0x34(const char *name);
    virtual int GetIndex(const char *name);
    virtual u32 GetSize(int index);
    virtual u32 GetSize(const char *name);

    bool Unload();
    bool LoadBinFiles(struct UnkOneFile_8004CFB0 *file);
    bool LoadFiles(struct UnkOneFile_8004CFB0 *file);
    void RequestReferences(struct UnkBinData_8040E61C *data, struct UnkOneFile_8004CFB0 *file);
    void Request(int index);

    /* 0x04 */ int m_state;
    /* 0x08 */ std::string m_fileName;
    /* 0x14 */ ResourceOneFileReqEntry *m_entries;
    /* 0x18 */ int m_entryCount;
    /* 0x1C */ std::vector<ResourceRequest> m_requests;
};
#else
class ResourceOneFileReq {
  public:
    ResourceOneFileReq(const char *fileName, void *list);

    /* 0x00 */ u8 field_0x0[0x28];
};
#endif
SIZE_ASSERT(ResourceOneFileReq, 0x28);

#endif
