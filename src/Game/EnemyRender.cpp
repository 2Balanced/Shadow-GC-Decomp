/* EnemyRender (class name from the RTTI of its shared_ptr control blocks; declared in
 * Game/EnemyRender.h): the model and motion of an enemy, behind a pointer to EnemyRender::Impl
 * (RTTI), which is the enemy's model (class unknown, constructor 0x801A3E9C) and its motion (class
 * unknown, constructor 0x801A4D28) at once. The enemy units call the functions below under their
 * addresses, so they keep C names. Member and function names are inferred. Definitions are in
 * reverse address order (deferred inlining emits functions last-defined first).
 *
 * NonMatching: 55 of 57 functions and all data match. fn_802CD5CC and fn_802CD5F0 differ by two
 * swapped instructions: retail loads render->m_impl above the store of the link register (lwz r3,
 * 0(r3) before stw r0, 0x14(r1)); GC/2.7 does not, for every source form tried (the same
 * difference as fn_80334480 in Game/VehiclePathMove.cpp). */
#include "Game/EnemyRender.h"
#include "boost/shared_ptr.hpp"

/* The game has one shared copy of std::swap for boost::function's any_pointer (0x80019164), which
 * function1::swap calls; an overload declared here stands in for it (see Game/CatchStick.cpp). */
namespace std {
void swap(boost::detail::function::any_pointer &a, boost::detail::function::any_pointer &b);
}

/* The model part of an enemy (see Game/ThrowBomb.cpp; 8 bytes). */
class UnkModel_801A3E9C {
  public:
    UnkModel_801A3E9C();
    ~UnkModel_801A3E9C();

    /* 0x0 */ u32 field_0x0;
    /* 0x4 */ s32 m_frameCount;
};

/* The motion's event callback (class name unknown): an EnemyMotionCallback whose assignment and
 * reset are out of line in this unit. */
class UnkEnemyMotionCallback : public EnemyMotionCallback {
  public:
    UnkEnemyMotionCallback &operator=(const EnemyMotionCallback &callback) {
        EnemyMotionCallback::operator=(callback);
        return *this;
    }
    void Reset() {
        clear();
    }
};

/* The motion part of an enemy (class name unknown; 0x2C bytes; code at 0x801A4584..0x801A4D28,
 * not yet matched): its event callback is at 0x20. */
class UnkMotion_801A4D28 {
  public:
    UnkMotion_801A4D28();  /* 0x801A4D28 */
    ~UnkMotion_801A4D28(); /* 0x801A4B90 */

    /* The motion's data (name inferred). */
    void *GetData() {
        return m_data;
    }

    /* 0x00 */ void *m_data;
    /* 0x04 */ u8 field_0x4[0x20 - 0x4];
    /* 0x20 */ UnkEnemyMotionCallback m_callback;
};

/* The game state (struct name unknown; see Game/BossCharaColli.cpp): the frame time at 0x24. */
struct UnkGameState_8057D6A0 {
    /* 0x000 */ u8 field_0x0[0x24];
    /* 0x024 */ float m_dt;
    /* 0x028 */ u8 field_0x28[0x150 - 0x28];
};

extern "C" {
extern UnkGameState_8057D6A0 lbl_8057D6A0;

/* The model (behaviour inferred; see Game/ThrowBomb.cpp). */
void fn_801A3CE4(UnkModel_801A3E9C *model, int slot, const EnemyMotionTable *table, void *arg3);
void fn_801A3B58(UnkModel_801A3E9C *model);
void fn_801A3AA0(UnkModel_801A3E9C *model);
void fn_801A3AFC(UnkModel_801A3E9C *model);
void fn_801A367C(UnkModel_801A3E9C *model, void *arg1, void *arg2);
void fn_801A39E0(UnkModel_801A3E9C *model, float scale);
void fn_801A386C(UnkModel_801A3E9C *model, void *arg1);
void fn_801A38E8(UnkModel_801A3E9C *model);

/* The motion (behaviour inferred): its position (get, move by and get from a bone), a bone's
 * matrix, the motion to play, its update and the release of its model. */
void fn_801A46C0(UnkMotion_801A4D28 *motion, Vec3f *position);
void fn_801A469C(UnkMotion_801A4D28 *motion, Vec3f *position);
void fn_801A45E0(UnkMotion_801A4D28 *motion, const Vec3f *delta);
Mtx34f *fn_801A46E4(UnkMotion_801A4D28 *motion, int bone);
void fn_801A4898(UnkMotion_801A4D28 *motion, int id, int arg2, float blend);
void fn_801A4584(UnkMotion_801A4D28 *motion, void *arg1);
void fn_801A4604(UnkMotion_801A4D28 *motion, void *arg1);
void fn_801A4640(UnkMotion_801A4D28 *motion, const Vec3f *position, const Vec3f *rotation);
void fn_801A4848(UnkMotion_801A4D28 *motion, float dt);
void fn_801A4934(UnkMotion_801A4D28 *motion);
void fn_801A4B30(UnkMotion_801A4D28 *motion, void *model, void *motionData, int arg3);
void fn_801A4AF0(UnkMotion_801A4D28 *motion);

/* The translation of a matrix (declared with its result pointer: see fn_802CD508). */
void fn_8040FB08(Vec3f *result, const Mtx34f *matrix);
}

/* fn_8040CD74 called as returning a - b by value (it stores the result through its first
 * argument). */
typedef Vec3f (*VectorSubtractFn)(const Vec3f *a, const Vec3f *b);

class EnemyRender {
  public:
    class Impl;

    EnemyRender();
    ~EnemyRender();

    /* 0x0 */ boost::shared_ptr<Impl> m_impl;
};
SIZE_ASSERT(EnemyRender, 0x8);

class EnemyRender::Impl {
  public:
    Impl() : m_scale(0.0f), m_speed(1.0f) {}
    ~Impl() {
        fn_801A4AF0(&m_motion);
        fn_801A3B58(&m_model);
    }

    void LoadModel(int slot, const EnemyMotionTable &table, void *arg);
    void Update();
    void SetPosition(const Vec3f *position);

    UnkMotion_801A4D28 *GetMotion();
    UnkMotion_801A4D28 *Motion() {
        return &m_motion;
    }
    UnkModel_801A3E9C *GetModel() {
        return &m_model;
    }
    float GetScale() const {
        return m_scale;
    }
    void SetScale(float scale) {
        m_scale = scale;
    }
    void Draw(void *arg) {
        fn_801A39E0(&m_model, m_scale);
        fn_801A386C(&m_model, arg);
    }
    void Draw() {
        fn_801A39E0(&m_model, m_scale);
        fn_801A38E8(&m_model);
    }
    void Show(void *arg1, void *arg2) {
        fn_801A367C(&m_model, arg1, arg2);
    }
    void SetPlacement(const Vec3f *position, const Vec3f *rotation) {
        fn_801A4640(&m_motion, position, rotation);
    }
    void SetMotionArg1(void *arg) {
        fn_801A4584(&m_motion, arg);
    }
    void SetMotionArg2(void *arg) {
        fn_801A4604(&m_motion, arg);
    }
    void LoadMotion(void *model, void *motion) {
        if (model != 0) {
            fn_801A4B30(&m_motion, model, motion, 1);
        }
    }

    /* 0x00 */ UnkModel_801A3E9C m_model;
    /* 0x08 */ UnkMotion_801A4D28 m_motion;
    /* 0x34 */ float m_scale;
    /* 0x38 */ float m_speed;
};
SIZE_ASSERT(EnemyRender::Impl, 0x3C);

/* Deletes a render (name inferred; see checked_delete below). */
inline void DeleteRender(EnemyRender *render) {
    delete render;
}

/* Retail keeps boost::checked_delete<EnemyRender> out of line (0x802CDAB0) and ~EnemyRender at its
 * own place (0x802CDBC8); this specialization calls an inline helper, which gives retail's code and
 * order (as in Game/Rocket.cpp; see docs/research-notes.md). */
namespace boost {
template <> inline void checked_delete<EnemyRender>(EnemyRender *x) {
    DeleteRender(x);
}
} // namespace boost

void EnemyRender::Impl::LoadModel(int slot, const EnemyMotionTable &table, void *arg) {
    fn_801A3CE4(&m_model, slot, &table, arg);
    /* Compiles to nothing; retail keeps this in r31 across the call as such a statement makes
     * GC/2.7 do. */
    SetScale(m_scale);
}

void EnemyRender::Impl::Update() {
    fn_801A4848(&m_motion, lbl_8057D6A0.m_dt * m_speed);
    fn_801A4934(&m_motion);
}

void EnemyRender::Impl::SetPosition(const Vec3f *position) {
    Vec3f current;
    fn_801A46C0(&m_motion, &current);
    Vec3f delta;
    fn_800091EC(&delta, &(const Vec3f &)((VectorSubtractFn)fn_8040CD74)(position, &current));
    fn_801A45E0(&m_motion, &delta);
}

UnkMotion_801A4D28 *EnemyRender::Impl::GetMotion() {
    return &m_motion;
}

EnemyRender::EnemyRender() : m_impl(new Impl) {}

EnemyRender::~EnemyRender() {}

extern "C" {

boost::shared_ptr<EnemyRender> fn_802CDB08(int slot, const EnemyMotionTable &table, void *model,
                                           void *motion);
boost::shared_ptr<EnemyRender> fn_802CD8E4(int slot, const EnemyMotionTable &table);
void fn_802CD870(EnemyRender *render, int slot, const EnemyMotionTable &table, void *model,
                 void *motion);
void fn_802CD848(EnemyRender *render, int slot, const EnemyMotionTable &table);

boost::shared_ptr<EnemyRender> fn_802CDB08(int slot, const EnemyMotionTable &table, void *model,
                                           void *motion) {
    boost::shared_ptr<EnemyRender> render = boost::shared_ptr<EnemyRender>(new EnemyRender);
    if (render.get() != 0) {
        fn_802CD870(render.get(), slot, table, model, motion);
        return render;
    }
    return boost::shared_ptr<EnemyRender>();
}

boost::shared_ptr<EnemyRender> fn_802CD8E4(int slot, const EnemyMotionTable &table) {
    boost::shared_ptr<EnemyRender> render = boost::shared_ptr<EnemyRender>(new EnemyRender);
    if (render.get() != 0) {
        fn_802CD848(render.get(), slot, table);
        return render;
    }
    return boost::shared_ptr<EnemyRender>();
}

void fn_802CD870(EnemyRender *render, int slot, const EnemyMotionTable &table, void *model,
                 void *motion) {
    render->m_impl->LoadMotion(model, motion);
    /* The game tests the address of the motion against null before it reads the motion's data. */
    EnemyRender::Impl *impl = render->m_impl.get();
    UnkMotion_801A4D28 *part = &impl->m_motion;
    if (part != 0) {
        impl->LoadModel(slot, table, part->m_data);
    }
}

void fn_802CD848(EnemyRender *render, int slot, const EnemyMotionTable &table) {
    render->m_impl->LoadModel(slot, table, 0);
}

void fn_802CD824(EnemyRender *render, float dt) {
    render->m_impl->Update();
}

void fn_802CD7E8(EnemyRender *render) {
    render->m_impl->Draw();
}

void fn_802CD7A4(EnemyRender *render, void *arg) {
    render->m_impl->Draw(arg);
}

void fn_802CD768(EnemyRender *render, void *arg1, void *arg2) {
    render->m_impl->Show(arg1, arg2);
}

void fn_802CD728(EnemyRender *render, const Vec3f *position, const Vec3f *rotation) {
    render->m_impl->SetPlacement(position, rotation);
}

void fn_802CD6F0(EnemyRender *render, void *arg) {
    render->m_impl->SetMotionArg2(arg);
}

void fn_802CD6B8(EnemyRender *render, void *arg) {
    render->m_impl->SetMotionArg1(arg);
}

void fn_802CD68C(EnemyRender *render, int id, float blend) {
    fn_801A4898(&render->m_impl->m_motion, id, 0, blend);
}

void fn_802CD668(EnemyRender *render, const Vec3f *position) {
    render->m_impl->SetPosition(position);
}

void fn_802CD638(EnemyRender *render, float scale) {
    render->m_impl->SetScale(scale);
}

float fn_802CD614(EnemyRender *render) {
    return render->m_impl->GetScale();
}

void fn_802CD5F0(EnemyRender *render) {
    fn_801A3AFC(&render->m_impl->m_model);
}

void fn_802CD5CC(EnemyRender *render) {
    fn_801A3AA0(&render->m_impl->m_model);
}

UnkModel_801A3E9C *fn_802CD5AC(EnemyRender *render) {
    return render->m_impl->GetModel();
}

UnkMotion_801A4D28 *fn_802CD588(EnemyRender *render) {
    return render->m_impl->Motion();
}

Mtx34f *fn_802CD560(EnemyRender *render, int bone) {
    return fn_801A46E4(&render->m_impl->m_motion, bone);
}

/* The next three return a Vec3f through result: the game copies the vector into it with its
 * out-of-line copy constructor, fn_800091EC. */
void fn_802CD508(Vec3f *result, EnemyRender *render, int bone) {
    Mtx34f *matrix = fn_802CD560(render, bone);
    if (matrix != 0) {
        fn_8040FB08(result, matrix);
    } else {
        fn_800091EC(result, &lbl_805E07A0);
    }
}

void fn_802CD4AC(Vec3f *result, EnemyRender *render) {
    Vec3f position;
    fn_800091EC(&position, &lbl_805E07A0);
    fn_801A469C(&render->m_impl->m_motion, &position);
    fn_800091EC(result, &position);
}

void fn_802CD450(Vec3f *result, EnemyRender *render) {
    Vec3f position;
    fn_800091EC(&position, &lbl_805E07A0);
    fn_801A46C0(&render->m_impl->m_motion, &position);
    fn_800091EC(result, &position);
}

void *fn_802CD420(EnemyRender *render) {
    return render->m_impl->GetMotion()->GetData();
}

void fn_802CD270(EnemyRender *render, const EnemyMotionCallback &callback) {
    render->m_impl->m_motion.m_callback = callback;
}

void fn_802CD228(EnemyRender *render) {
    render->m_impl->m_motion.m_callback.Reset();
}

void fn_802CD21C(EnemyRender *render, float speed) {
    render->m_impl->m_speed = speed;
}

} // extern "C"
