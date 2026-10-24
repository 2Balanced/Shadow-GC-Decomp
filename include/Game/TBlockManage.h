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

/* TBlockManage (class name from RTTI; vtable at 0x8051CEAC): a player's stage blocks, a Task that
 * is also a HandleUser (at 0x28). Only the fields Game/StageInitalizeTask.cpp reads are named. */
class TBlockManage : public Task, public HandleUser {
  public:
    class forBlockCheck;

    virtual ~TBlockManage();
    virtual void Tick(float dt);

    /* 0x030 */ u8 field_0x30[0x40 - 0x30];
    /* 0x040 */ u32 field_0x40;
    /* 0x044 */ u8 field_0x44[0x978 - 0x44];
    /* 0x978 */ u8 m_loading; /* (name inferred) */
    /* 0x979 */ u8 m_ready;   /* (name inferred) */
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
