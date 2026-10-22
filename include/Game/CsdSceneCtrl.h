#ifndef GAME_CSDSCENECTRL_H
#define GAME_CSDSCENECTRL_H

/* A CSD scene with a queue of motions to play (class name unknown; 0x14 bytes, constructed by
 * fn_8003DD20, destroyed by fn_8003D34C). Game/ResultTask.cpp keeps two of them. The members are
 * the out-of-line functions around 0x8003D000; their names and meanings are inferred. */

#include "Game/CsdRCPtr.h"
#include <dolphin/types.h>
#include "size_assert.h"

class UnkCsdSceneCtrl_8003DD20 {
  public:
    UnkCsdSceneCtrl_8003DD20();
    ~UnkCsdSceneCtrl_8003DD20();

    /* The scene (fn_8003CBEC) and setting it (fn_800BAC10). */
    Chao::CSD::RCPtr<Chao::CSD::CScene> GetScene() const;
    void SetScene(const Chao::CSD::RCPtr<Chao::CSD::CScene> &scene);
    /* Plays the queued motions; returns whether they have ended (fn_8003D914). */
    bool Update(bool arg);
    /* Rewinds the scene to its first frame (fn_8003D9B0). */
    void Reset();
    /* Queues motion (fn_8003DA58). */
    void Play(const char *motion, int arg);

    /* 0x00 */ u8 m_queue[0xC];
    /* 0x0C */ Chao::CSD::RCPtr<Chao::CSD::CScene> m_scene;
    /* 0x10 */ u8 field_0x10;
};
SIZE_ASSERT(UnkCsdSceneCtrl_8003DD20, 0x14);

#endif
