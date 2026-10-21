/* GadgetUtility (namespace name from the RTTI of Watcher's shared_ptr control block): helpers the
 * gadgets share. Watcher remembers the players a gadget hit recently; the other functions start
 * controller vibrations, test the players' distance to a point, read the game mode, and walk a
 * gadget model's parts to attach a particle controller, find a frame, or read a frame's matrix. The
 * file name follows the namespace; all names other than Watcher and boost's are inferred, and the
 * free functions keep their address names, as other units call them. Definitions are in reverse
 * address order (deferred inlining emits functions last-defined first). */
#define GADGET_UTILITY_WATCHER_MEMBERS
#include <boost/scoped_ptr.hpp>

#include "Game/CharInfo.h"
#include "Game/GadgetCollision.h"
#include "Game/GadgetParticle.h"
#include "Game/MathTypes.h"
#include "Game/Vibration.h"
#include "boost/shared_ptr.hpp"
#include "boost/weak_ptr.hpp"
#include "size_assert.h"

class GadgetCharInfo;
class GadgetModelInfo;
class UnkCameraManager_80009548;

/* The RenderWare types used here, declared locally (layouts as in rwcore.h, reduced to the members
 * used). */
extern "C" {
typedef struct RwV3d {
    float x;
    float y;
    float z;
} RwV3d;

typedef struct RwMatrix {
    /* 0x00 */ RwV3d right;
    /* 0x0C */ u32 flags;
    /* 0x10 */ RwV3d up;
    /* 0x1C */ u32 pad1;
    /* 0x20 */ RwV3d at;
    /* 0x2C */ u32 pad2;
    /* 0x30 */ RwV3d pos;
    /* 0x3C */ u32 pad3;
} RwMatrix;

typedef struct RwFrame RwFrame;

RwMatrix *RwFrameGetLTM(RwFrame *frame);
}

/* The game state (struct name unknown; see Game/CityBombSmall.cpp). */
struct UnkGameState_8057D6A0 {
    /* 0x000 */ u8 field_0x0[0xB2];
    /* 0x0B2 */ u8 m_twoPlayers; /* (name inferred: picks whether both players are tested) */
    /* 0x0B3 */ u8 field_0xB3[0x150 - 0xB3];
};
SIZE_ASSERT(UnkGameState_8057D6A0, 0x150);

extern UnkGameState_8057D6A0 lbl_8057D6A0;

/* The object fn_80074470 returns (class unknown). */
struct UnkGameMode_80074470 {
    /* 0x0 */ u8 field_0x0[0x8];
    /* 0x8 */ s32 m_mode;
};

/* The model object a part points at (class unknown; see Game/EggBalloon.cpp). */
struct UnkModelObject_8015FA78 {
    /* 0x0 */ u32 field_0x0;
    /* 0x4 */ void *m_clump; /* searched by fn_80038E84 */
};

/* What fn_8015FA78 passes for each part of a model (layout from fn_8015FA78; names inferred). */
struct UnkModelPart_8015FA78 {
    /* 0x0 */ UnkModelObject_8015FA78 *m_object;
    /* 0x4 */ void *field_0x4;
    /* 0x8 */ s32 m_index;
};
SIZE_ASSERT(UnkModelPart_8015FA78, 0xC);

/* What fn_80169E8C is passed: the id of the frame to find, and the frame found. */
struct UnkFrameSearch_80169E8C {
    /* 0x0 */ s32 m_id;
    /* 0x4 */ RwFrame *m_frame;
};

/* What fn_80169EF0 passes to fn_80169F5C (names inferred). */
struct UnkMatrixSearch_80169EF0 {
    /* 0x0 */ Mtx34f *m_matrix;
    /* 0x4 */ s32 m_index;
    /* 0x8 */ s32 m_id;
    /* 0xC */ bool m_found;
};
SIZE_ASSERT(UnkMatrixSearch_80169EF0, 0x10);

typedef UnkModelPart_8015FA78 *(*UnkModelPartCallback_8015FA78)(UnkModelPart_8015FA78 *part,
                                                                void *data);

extern "C" {
/* Controller vibrations (behaviour inferred; see Game/Vibration.h). */
boost::weak_ptr<Vibration::RequestHandle> fn_8034DD24(int type, float strength,
                                                      GadgetCharInfo *charInfo, int arg3);
boost::weak_ptr<Vibration::RequestHandle> fn_8034DD9C(int type, GadgetCharInfo *charInfo,
                                                      int arg2);
boost::weak_ptr<Vibration::RequestHandle> fn_8034DE08(int type, GadgetCharInfo *charInfo,
                                                      int arg2);
boost::weak_ptr<Vibration::RequestHandle> fn_8034DEE4(int pattern, int arg1);
boost::weak_ptr<Vibration::RequestHandle> fn_8034DF48(int pattern, int arg1);
UnkCameraManager_80009548 *fn_80009548(void);
/* The position of player index (behaviour inferred). */
const Vec3f *fn_80010244(UnkCameraManager_80009548 *manager, int index);
UnkGameMode_80074470 *fn_80074470(void);
/* The frame with id below clump (behaviour inferred). */
RwFrame *fn_80038E84(void *clump, s32 id);
/* Attaches the particle controller to a model object (behaviour inferred). */
void fn_80166FDC(GadgetParticle::GadgetParticleCtrl *ctrl, UnkModelObject_8015FA78 *object);
/* Calls callback with each part of the model until it returns 0 (behaviour inferred). */
void fn_8015FA78(GadgetModelInfo *model, UnkModelPartCallback_8015FA78 callback, void *data);
}

/* fn_8040CD74 called as returning lhs - rhs by value (it stores the result through its first
 * argument, as Game/MathTypes.h declares it). */
typedef Vec3f (*VectorSubtractFn)(const Vec3f *lhs, const Vec3f *rhs);

inline float LengthSquared(const Vec3f &v) {
    return fn_8040CF2C(&v);
}

namespace GadgetUtility {

/* Watcher's implementation (name inferred): two slots of a player number and the time left
 * before that player counts as new again. */
class Watcher::Impl {
  public:
    struct Slot {
        Slot() : m_timer(0.0f), m_player(2) {}

        /* 0x0 */ float m_timer;
        /* 0x4 */ s32 m_player; /* 2: none */
    };

    Impl();
    ~Impl() {}
    bool Check(const boost::weak_ptr<CharInfo> &target);
    void Update(float dt);

    /* 0x00 */ Slot m_slots[2];
    /* 0x10 */ float m_cooldown;
};
SIZE_ASSERT(Watcher::Impl, 0x14);

inline void DeleteWatcher(Watcher *watcher) {
    delete watcher;
}

} // namespace GadgetUtility

/* Retail keeps boost::checked_delete<Watcher> out of line (0x80169774). GC/2.7 only does so when
 * the body calls an inline function (see docs/research-notes.md, EnergyCore), so this
 * specialization calls an inline helper. (checked_delete<Watcher::Impl> calls the inline ~Impl.) */
namespace boost {
template<> inline void checked_delete<GadgetUtility::Watcher>(GadgetUtility::Watcher *x) {
    GadgetUtility::DeleteWatcher(x);
}
} // namespace boost

using GadgetUtility::Watcher;

extern "C" {

UnkModelPart_8015FA78 *fn_80169F5C(UnkModelPart_8015FA78 *part, void *data) {
    UnkMatrixSearch_80169EF0 *search = (UnkMatrixSearch_80169EF0 *)data;
    if (part->m_index == search->m_index) {
        if (part->m_object != 0 && part->m_object->m_clump != 0) {
            RwFrame *frame = fn_80038E84(part->m_object->m_clump, search->m_id);
            if (frame != 0) {
                RwMatrix *ltm = RwFrameGetLTM(frame);
                if (ltm != 0) {
                    Mtx34f *m = search->m_matrix;
                    m->values[0][0] = ltm->right.x;
                    m->values[0][1] = ltm->up.x;
                    m->values[0][2] = ltm->at.x;
                    m->values[0][3] = ltm->pos.x;
                    m->values[1][0] = ltm->right.y;
                    m->values[1][1] = ltm->up.y;
                    m->values[1][2] = ltm->at.y;
                    m->values[1][3] = ltm->pos.y;
                    m->values[2][0] = ltm->right.z;
                    m->values[2][1] = ltm->up.z;
                    m->values[2][2] = ltm->at.z;
                    m->values[2][3] = ltm->pos.z;
                    search->m_found = true;
                }
            }
        }
        return 0;
    }
    return part;
}

bool fn_80169EF0(Mtx34f *matrix, const boost::shared_ptr<GadgetModelInfo> &model, int index, int id) {
    UnkMatrixSearch_80169EF0 search = {matrix, index, id, false};
    fn_8015FA78(model.get(), fn_80169F5C, &search);
    return search.m_found;
}

UnkModelPart_8015FA78 *fn_80169E8C(UnkModelPart_8015FA78 *part, void *data) {
    UnkFrameSearch_80169E8C *search = (UnkFrameSearch_80169E8C *)data;
    if (search != 0 && part->m_object != 0) {
        search->m_frame = fn_80038E84(part->m_object->m_clump, search->m_id);
        if (search->m_frame != 0) {
            return 0;
        }
    }
    return part;
}

UnkModelPart_8015FA78 *fn_80169D00(UnkModelPart_8015FA78 *part, UnkParticleAttach_80169D00 *attach) {
    if (attach != 0) {
        if (boost::shared_ptr<GadgetParticle::GadgetParticleCtrl> ctrl = attach->ctrl.lock()) {
            fn_80166FDC(ctrl.get(), part->m_object);
        }
    }
    return part;
}

bool fn_80169CD0(void) {
    if (fn_80074470()->m_mode == 4) {
        return true;
    }
    return false;
}

bool fn_80169BE4(const Vec3f *position, float rangeSquared) {
    if (lbl_8057D6A0.m_twoPlayers) {
        bool far = true;
        if (LengthSquared(((VectorSubtractFn)fn_8040CD74)(fn_80010244(fn_80009548(), 0), position)) <
            rangeSquared) {
            far = false;
        }
        if (LengthSquared(((VectorSubtractFn)fn_8040CD74)(fn_80010244(fn_80009548(), 1), position)) <
            rangeSquared) {
            far = false;
        }
        return far;
    }
    return !(LengthSquared(((VectorSubtractFn)fn_8040CD74)(fn_80010244(fn_80009548(), 0), position)) <
             rangeSquared);
}

boost::weak_ptr<Vibration::RequestHandle> fn_80169BB0(const boost::weak_ptr<CharInfo> &target, int pattern) {
    return fn_8034DF48(pattern, 0);
}

boost::weak_ptr<Vibration::RequestHandle> fn_80169B80(int type, GadgetCharInfo *charInfo) {
    return fn_8034DE08(type, charInfo, -2);
}

boost::weak_ptr<Vibration::RequestHandle> fn_80169B4C(const boost::weak_ptr<CharInfo> &target, int pattern) {
    return fn_8034DEE4(pattern, 0);
}

boost::weak_ptr<Vibration::RequestHandle> fn_80169B1C(int type, GadgetCharInfo *charInfo) {
    return fn_8034DD9C(type, charInfo, -2);
}

boost::weak_ptr<Vibration::RequestHandle> fn_80169AEC(int type, float strength, GadgetCharInfo *charInfo) {
    return fn_8034DD24(type, strength, charInfo, -2);
}

} // extern "C"

namespace GadgetUtility {

Watcher::Impl::Impl() {
    for (int i = 0; i < 2; i++) {
        m_slots[i].m_timer = 0.0f;
        m_slots[i].m_player = 2;
    }
    m_cooldown = 1.0f;
}

void Watcher::Impl::Update(float dt) {
    for (int i = 0; i < 2; i++) {
        if (m_slots[i].m_timer > 0.0f) {
            m_slots[i].m_timer -= dt;
            if (m_slots[i].m_timer <= 0.0f) {
                m_slots[i].m_timer = 0.0f;
                m_slots[i].m_player = 2;
            }
        }
    }
}

bool Watcher::Impl::Check(const boost::weak_ptr<CharInfo> &target) {
    if (boost::shared_ptr<CharInfo> info = target.lock()) {
        s32 player = info->field_0x54;
        for (int i = 0; i < 2; i++) {
            if (m_slots[i].m_player == player) {
                return true;
            }
        }
        for (int i = 0; i < 2; i++) {
            Slot &slot = m_slots[i];
            if (slot.m_timer <= 0.0f) {
                slot.m_timer = m_cooldown;
                slot.m_player = player;
                return false;
            }
        }
    }
    return true;
}

Watcher::Watcher() : m_impl(new Impl) {}

Watcher::~Watcher() {}

void Watcher::Update(float dt) {
    m_impl->Update(dt);
}

bool Watcher::Check(const boost::weak_ptr<CharInfo> &target) {
    return m_impl->Check(target);
}

} // namespace GadgetUtility

namespace boost {

/* boost/shared_ptr.hpp only declares shared_ptr::reset, which the game has out of line; this unit
 * emits the copy for Watcher (boost 1.32's definition, as in Game/Coaster.cpp). */
template<class T> inline void shared_ptr<T>::reset() {
    this_type().swap(*this);
}

} // namespace boost

extern "C" {

boost::shared_ptr<Watcher> fn_801695B8(void) {
    boost::shared_ptr<Watcher> watcher(new Watcher);
    return watcher;
}

void fn_8016952C(boost::shared_ptr<Watcher> *watcher) {
    watcher->reset();
}

} // extern "C"
