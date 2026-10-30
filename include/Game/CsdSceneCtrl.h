#ifndef GAME_CSDSCENECTRL_H
#define GAME_CSDSCENECTRL_H

/* TPjsSceneTour (class name from the RTTI of its shared_ptr's control block,
 * boost::detail::sp_counted_base_impl<TPjsSceneTour *, boost::checked_deleter<TPjsSceneTour>>): a
 * CSD scene with a queue of motions to play (0x14 bytes, constructed by fn_8003DD20, destroyed by
 * fn_8003D34C). Game/ResultTask.cpp keeps two of them. The members are the out-of-line functions
 * around 0x8003D000 (Game/TPjsCsd.cpp); their names and meanings are inferred. */

#include "Game/CsdRCPtr.h"
#ifdef TPJS_SCENE_TOUR_IMPL
#include <string>
#include <vector>
#endif
#include <dolphin/types.h>
#include "size_assert.h"

class TPjsSceneTour {
  public:
    TPjsSceneTour();
#ifdef TPJS_SCENE_TOUR_IMPL
    /* (In Game/TPjsCsd.cpp the destructor and GetScene are inline, as the game emits them.) */
    ~TPjsSceneTour() {}

    Chao::CSD::RCPtr<Chao::CSD::CScene> GetScene() const {
        return m_scene;
    }
#else
    ~TPjsSceneTour();

    /* The scene (fn_8003CBEC) and setting it (fn_800BAC10). */
    Chao::CSD::RCPtr<Chao::CSD::CScene> GetScene() const;
#endif
    void SetScene(const Chao::CSD::RCPtr<Chao::CSD::CScene> &scene);
    /* Plays the queued motions; returns whether they have ended (fn_8003D914). */
    bool Update(bool arg);
    /* Rewinds the scene to its first frame (fn_8003D9B0). */
    void Reset();
    /* Queues motion (fn_8003DA58). */
    void Play(const char *motion, int arg);

#ifdef TPJS_SCENE_TOUR_IMPL
    /* (Where TPJS_SCENE_TOUR_IMPL is defined, Game/TPjsCsd.cpp, the unit with the class's code: the
     * members' real types.) */
    TPjsSceneTour(const Chao::CSD::RCPtr<Chao::CSD::CScene> &scene);

    /* A queued motion: its name and how it plays (1 once, 2 looped). */
    struct Motion {
        /* 0x0 */ std::string m_name;
        /* 0xC */ s32 m_mode;
    };
    typedef std::vector<Motion> MotionQueue;

    /* 0x00 */ MotionQueue m_queue;
    /* 0x0C */ Chao::CSD::RCPtr<Chao::CSD::CScene> m_scene;
    /* 0x10 */ bool m_ended;
#else
    /* 0x00 */ u8 m_queue[0xC];
    /* 0x0C */ Chao::CSD::RCPtr<Chao::CSD::CScene> m_scene;
    /* 0x10 */ u8 field_0x10;
#endif
};
SIZE_ASSERT(TPjsSceneTour, 0x14);

#endif
