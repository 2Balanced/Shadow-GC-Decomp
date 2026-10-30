#ifndef GAME_TBLOCKMANAGE_H
#define GAME_TBLOCKMANAGE_H

#include "Game/MathTypes.h"
#include "boost/shared_ptr.hpp"
#include "boost/weak_ptr.hpp"

/* TBlockManage::forBlockCheck (class names from the RTTI of its shared_ptr instances): the stage
 * block a point lies in. Only declared; gadgets hold it to wait until their block is ready. */
#ifdef TBLOCKMANAGE_DEFINE_CLASS
#include "Game/HandleUser.h"
#include "Game/Task.h"

struct UnkBlockData_8003AB68;
struct UnkLandBlock_8003A840;

/* TBlockManage (class name from RTTI; vtable at 0x8051CEAC): a player's stage blocks, a Task that
 * is also a HandleUser (at 0x28). Game/TBlockManage.cpp. It reads the stage's block file (a list
 * of boxes, each naming the block it belongs to), works out each frame which blocks the watched
 * position (or the camera) is in, and has the stage load and free their land. Names other than
 * the class's are inferred. */
class TBlockManage : public Task, public HandleUser {
  public:
    class forBlockCheck;

    enum { kBlockCount = 0xA8 };

    TBlockManage(Task *parent, int player, const char *path, const Vec3f *position);
    virtual ~TBlockManage();
    virtual void Tick(float dt);

    bool IsLoading(int id);
    bool IsInside(const Vec3f *min, const Vec3f *max, const Vec3f *position);
    void UpdateLoads();
    void UpdateRequests();
    bool IsRequested(int id, int which);
    void Release();
    bool Open(const char *path);

    static const char *s_taskName;

    /* 0x030 */ s32 m_player;
    /* 0x034 */ UnkBlockData_8003AB68 *m_data;
    /* 0x038 */ u8 m_changed;
    /* 0x03C */ const Vec3f *m_position; /* 0: the camera's */
    /* 0x040 */ u32 field_0x40;
    /* 0x044 */ s32 m_current;                   /* which of m_shown is current */
    /* 0x048 */ s16 m_shown[2][kBlockCount];      /* nonzero: the block is wanted */
    /* 0x2E8 */ s32 m_requests[kBlockCount];      /* 1 show, 2 hide, else by position */
    /* 0x588 */ UnkLandBlock_8003A840 *m_blocks[kBlockCount]; /* the stage's land blocks */
    /* 0x828 */ s16 m_loadedIds[kBlockCount];     /* the loaded wanted blocks, -1 ends */
    /* 0x978 */ u8 m_loading; /* (name inferred) */
    /* 0x979 */ u8 m_ready;   /* (name inferred) */
    /* 0x97A */ u8 field_0x97A[0x980 - 0x97A];
};
#else
class TBlockManage {
  public:
    class forBlockCheck;
};
#endif

extern "C" {
/* Returns the TBlockManage block containing position (behaviour inferred). */
boost::shared_ptr<TBlockManage::forBlockCheck> fn_8003A154(const Vec3f *position);
/* Reports a state of the block: SetVehicle waits for it, ContainerBehavior pauses while it is set
 * (meaning unknown). */
bool fn_80039EDC(const boost::weak_ptr<TBlockManage::forBlockCheck> &block);
}

#endif
