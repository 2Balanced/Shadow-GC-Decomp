#ifndef SHADOW_GAME_GADGETMODELINFO_H
#define SHADOW_GAME_GADGETMODELINFO_H

#include "Game/MathTypes.h"
#include "boost/weak_ptr.hpp"
#include <dolphin/types.h>
#include "size_assert.h"

class CharInfo;

/* GadgetModelInfo, named by the game's RTTI (its vtable, at 0x8052B500, holds only the
 * destructor). It owns the model a gadget loads from a .BIN file, starting with that model's
 * collision object. Its methods still carry their addresses; what callers suggest about them is
 * noted where it helps. */
class GadgetModelInfo {
  public:
    /* Load the model named `path`. The first argument's meaning is open. */
    GadgetModelInfo(int arg0, const char *path);
    /* Load model number `modelId` (0x80163F7C). */
    GadgetModelInfo(int arg0, int modelId);
    virtual ~GadgetModelInfo();

    void fn_8015FF60();
    void fn_801604FC();
    void fn_80160B18(int arg);
    /* Plays motion `motion`; false when the model has none of that name. */
    bool fn_80160BA0(const char *motion);
    void fn_80160C28(int arg);
    void fn_8016108C();
    void fn_801617D0(int index, int arg1, float x, float y, float z);
    void fn_80161880();
    /* Attaches `info` to the node called `node`. */
    bool fn_80161F08(const char *node, const boost::weak_ptr<CharInfo> &info);
    void fn_80162804();
    void fn_80162A68(int index, int arg1);
    void fn_80162EF4();
    /* Advances animation by dt. */
    void fn_80162FB0(float dt);
    /* Places the model with the given world matrix. */
    void fn_801633DC(const Mtx34f *world);

    /* 0x04 */ u8 field_0x4[0x74 - 0x4];
};
SIZE_ASSERT(GadgetModelInfo, 0x74);

#endif
