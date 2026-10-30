/* The motion player of a vehicle's model (UnkMotion_8022F058, declared in Game/UnkMotion_8022F058.h;
 * class name unknown): Vehicle::BirdBase::BirdModelBase and the walker's model use it. Its state, an
 * Impl (class name unknown), is built from a BirdMotionTable: each part is a clump cloned from the
 * model resource slot into the stage's world, hung on a frame of the first part's clump (or on the
 * model's own frame), with its motions; the model keeps the world matrices of some frames of the
 * first clump as nodes. The motions' events play sounds and rumble and go to an event callback, and
 * while someone rides the vehicle the rider is told which motions to play with it. Other units call
 * the functions by their addresses, so they keep those names. File name and member names are
 * inferred. Definitions are in reverse address order (deferred inlining emits functions
 * last-defined first).
 *
 * NonMatching: 57 of 59 functions and all data match; linked as Matching, the DOL differs only
 * inside the two others. LoadParts (0x8022F54C) and ProcessEvents (0x8022F7BC) differ only in
 * callee-saved register numbers: retail gives the part pointer (LoadParts) and this
 * (ProcessEvents) r30, GC/2.7 the lowest register of the set, under every form tried (as
 * CreateLockons in Game/BkWorm.cpp). */
/* boost::scoped_array needs it (see boost/checked_delete.hpp). */
#define BOOST_CHECKED_ARRAY_DELETE
/* Declares Player::SetExternalMotionCommand's constructor (Game/CharCommand.h). */
#define CHARCOMMAND_SET_EXTERNAL_MOTION
/* boost::function's operator() puts a bad_function_call on the stack (see the MSL's stdexcept). */
#define MSL_RUNTIME_ERROR_STRING

#include "Game/UnkMotion_8022F058.h"
#include "Game/CharCommand.h"
#include "Game/CharInfo.h"
#include "Game/DebugDispShape.h"
#include "Game/MathTypes.h"
#include "Game/TVehicleBase.h"
#include "Game/VehicleMotionEvent.h"
#include "Game/Vibration.h"
#include "boost/scoped_array.hpp"
#include "boost/scoped_ptr.hpp"
#include "boost/shared_ptr.hpp"
#include "boost/weak_ptr.hpp"
#include <dolphin/types.h>
#include "size_assert.h"

struct _stMtnPlayWork;

/* The frames of a motion (struct name unknown; as in Game/PlayerMotion.cpp). */
struct UnkMotionRange_804125F4 {
    /* 0x0 */ float m_start;
    /* 0x4 */ float m_end;
    /* 0x8 */ u8 field_0x8[0x10 - 0x8];
};

struct UnkMotionFrames_80415D40 {
    /* 0x0 */ float m_start;
    /* 0x4 */ float m_end;
    /* 0x8 */ float field_0x8;
    /* 0xC */ float m_frame;
};

/* An object that keeps a clump in step with a motion (class name unknown; as in
 * Game/WeaponModel.cpp). */
class UnkClumpUpdater_80015A7C {
  public:
    UnkClumpUpdater_80015A7C(RpClump *clump, _stMtnPlayWork *motion);
    ~UnkClumpUpdater_80015A7C();

    /* 0x00 */ RpClump *m_clump;
    /* 0x04 */ u8 field_0x4[0x1C - 0x4];
};

/* A clump's colour fade (struct name unknown; laid out as in Game/VehicleGun.cpp, whose copy of it
 * retail's matches: the five values are copied as an array). Its constructor is out of line
 * (0x80027D5C; see Game/ClumpFade.h). */
struct UnkClumpFade_80027D5C;
extern "C" UnkClumpFade_80027D5C *fn_80027D5C(UnkClumpFade_80027D5C *fade);
struct UnkClumpFade_80027D5C {
    UnkClumpFade_80027D5C() {
        fn_80027D5C(this);
    }

    /* 0x00 */ float m_color[4];
    /* 0x10 */ float m_values[5];
    /* 0x24 */ u8 m_mode;
    /* 0x28 */ float field_0x28;

    /* Sets the first two values to 1 (name and meaning inferred, as in Game/VehicleGun.cpp). */
    void ResetValues() {
        for (int i = 0; i < 2; i++) {
            m_values[i] = 1.0f;
        }
    }
};
SIZE_ASSERT(UnkClumpFade_80027D5C, 0x2C);

/* The stage (class name unknown; fn_8003A840 returns it): the world of each of its areas (as in
 * Game/BossCtrlUnit.cpp). */
class UnkStage_8003A840 {
  public:
    void *GetWorld(int area);
};

/* What CharInfo's word at 0x60 points to (struct name unknown; see Game/DiablonPartsBody.cpp). */
struct UnkCharTime {
    /* 0x0 */ float m_frameTime;
    /* 0x4 */ float m_speed; /* (name inferred) */
};

/* The game state (struct name unknown; as far as this unit reads it): the frame time at 0x24 (see
 * Game/WeaponModel.cpp). */
struct UnkGameState_8057D6A0 {
    /* 0x000 */ u8 field_0x0[0x24];
    /* 0x024 */ float m_dt;
    /* 0x028 */ u8 field_0x28[0x150 - 0x28];
};
SIZE_ASSERT(UnkGameState_8057D6A0, 0x150);

extern UnkGameState_8057D6A0 lbl_8057D6A0;

extern "C" {
/* A 32-bit flag word (see Game/BKTankController.h): construct cleared, clear all, set, clear and
 * test a bit. */
void fn_80014E04(u32 *flags);
void fn_80014E10(u32 *flags);
void fn_80014E1C(u32 *flags, int bit);
void fn_8006127C(u32 *flags, int bit);
bool fn_80014E34(u32 *flags, int bit);
/* The resource manager, a model of a slot, the first motion number of a slot and a motion's data
 * (see Game/WeaponModel.cpp). */
void *fn_800122D4(void);
RpClump *fn_8001277C(void *manager, s32 slot, int model);
int fn_80012608(void *manager, s32 slot, int type);
void *fn_80012684(void *manager, s32 slot, int anim);
/* The stage, the area a clump's frame lies in (see Game/BossCtrlUnit.cpp). */
UnkStage_8003A840 *fn_8003A840(void);
void *fn_80169364(RpClump *clump);
int fn_80169350(void *frame);
/* RpClumpClone, the clump's set-up, its fade, showing and hiding it, advancing its material
 * animations (behaviour inferred), and RpClumpDestroy (see Game/WeaponModel.cpp). */
RpClump *fn_80457090(RpClump *clump);
void fn_80038C84(RpClump *clump);
void fn_80038198(RpClump *clump, UnkClumpFade_80027D5C *fade);
void fn_80038F94(RpClump *clump);
void fn_80038FC0(RpClump *clump);
void fn_80039B94(RpClump *clump, float dt, float arg2);
void fn_80457468(RpClump *clump);
/* The world a clump is in, and taking it out of and adding it to a world. */
void *fn_8046256C(RpClump *clump);
void fn_804623EC(void *world, RpClump *clump);
void fn_804622FC(void *world, RpClump *clump);
/* The frame of clump with hierarchy id id, the root of a frame, the world matrix of a frame,
 * RwFrameCreate, RwFrameDestroy and RwFrameGetLTM (see Game/WeaponModel.cpp). */
RwFrame *fn_80038D24(RpClump *clump, s32 id);
RwFrame *fn_804872D8(RwFrame *frame);
Mtx34f fn_802C8068(RwFrame *frame);
RwFrame *fn_80486D00(void);
int fn_80486E2C(RwFrame *frame);
RwMatrix *RwFrameGetLTM(RwFrame *frame);
/* The matrix of a RenderWare matrix, stored through out (Game/WeaponModel.cpp declares it as
 * returning the matrix, which passes out the same way; this unit keeps it in a named local), and a
 * matrix rotated by angles in order (see Game/ScatteringWeapon.cpp). */
void fn_80016414(Mtx34f *out, const RwMatrix *matrix);
Mtx34f fn_8040FFBC(const Mtx34f *matrix, const Vec3f *angles, int order);
/* The motion player (see Game/WeaponModel.cpp and Game/PlayerMotion.cpp). */
_stMtnPlayWork *fn_8041B908(void *anim, int arg1);
void fn_8041B884(_stMtnPlayWork *work);
void fn_804172D8(_stMtnPlayWork *work, const Mtx34f *matrix);
void fn_80416DC8(_stMtnPlayWork *work, int track, float speed);
void fn_80415EA4(_stMtnPlayWork *work);
void fn_800159AC(UnkClumpUpdater_80015A7C *updater);
void fn_80417568(_stMtnPlayWork *work, u16 motion);
int fn_80416F28(_stMtnPlayWork *work, int track, int flag, int arg3);
int fn_804125F4(UnkMotionRange_804125F4 *range, u16 motion);
void fn_80416E20(_stMtnPlayWork *work, int track, float frame);
int fn_804165D8(_stMtnPlayWork *work, int track, int arg2, int arg3);
void fn_80416148(_stMtnPlayWork *work, int track, float time);
void fn_80415D40(_stMtnPlayWork *work, int track, UnkMotionFrames_80415D40 *frames);
int fn_80415854(_stMtnPlayWork *work, int track, u16 *event, s32 **data, int arg4);
/* Sounds (see Game/BirdBase.cpp and Game/AudioLoopManagerTask.cpp). */
void *fn_80053164(void);
void fn_801D54F4(void *manager, u16 id, const void *source, int arg3, int arg4);
void fn_801D53C4(void *manager, u16 id, void *source, void *owner, int arg4, int arg5, int arg6);
/* Plays a rumble pattern on a player's pad (Game/VibrationRequest.cpp). */
boost::weak_ptr<Vibration::RequestHandle> fn_8034DF48(int pattern, int player);
}

/* Moves clump into world (as in Game/WeaponModel.cpp). */
inline void SetClumpWorld(RpClump *clump, void *world) {
    void *current = fn_8046256C(clump);
    if (current != world) {
        if (current != 0) {
            fn_804623EC(current, clump);
        }
        fn_804622FC(world, clump);
    }
}

/* fn_8040B034 (Game/MathTypes.h) called as returning the translation matrix it builds (as in
 * Game/VehicleAirBolt.cpp). */
typedef Mtx34f (*TranslationFn)(const Vec3f *offset);

/* Copies value into matrix (as in Game/WeaponModel.cpp). */
inline void AssignMatrix(Mtx34f &matrix, const Mtx34f &value) {
    fn_8040C148(&matrix, &value);
}

/* A flag word (see Game/BKTankController.h; struct name unknown, as in Game/WeaponModel.cpp). */
struct UnkFlags_80014E04 {
    UnkFlags_80014E04() {
        fn_80014E04(&m_bits);
    }
    void ClearAll() {
        fn_80014E10(&m_bits);
    }
    void Set(int bit) {
        fn_80014E1C(&m_bits, bit);
    }
    void Clear(int bit) {
        fn_8006127C(&m_bits, bit);
    }
    bool Test(int bit) {
        return fn_80014E34(&m_bits, bit);
    }

    u32 m_bits;
};

/* The motion event callback of the motion (class name unknown; as UnkMotionEventCallback_8007B470
 * in Game/PlayerMotionEvent.h holds a player's). */
class UnkVehicleMotionEventCallback_8022ED3C {
  public:
    UnkVehicleMotionEventCallback_8022ED3C &operator=(const Vehicle::MotionEventCallback &f) {
        m_function = f;
        return *this;
    }
    /* Unsets the function (name inferred). */
    UnkVehicleMotionEventCallback_8022ED3C &Clear() {
        m_function.clear();
        return *this;
    }
    /* Whether a function is set, and calling it if one is (names inferred). */
    bool HasFunction() const {
        return !m_function.empty();
    }
    bool IsSet() const {
        return HasFunction();
    }
    void Call(Vehicle::MotionEventArgs &args) {
        if (!m_function.empty()) {
            m_function(args);
        }
    }

    /* 0x0 */ Vehicle::MotionEventCallback m_function;
};

/* The sound and rumble pattern of a motion's sound events (struct name unknown; -1: none). */
struct UnkMotionSound_8022F7BC {
    /* 0x0 */ s32 m_sound;
    /* 0x4 */ s32 m_rumble;
};

static const UnkMotionSound_8022F7BC s_sounds[] = {
    {0x204F, -1}, {0x2050, 1}, {0x204C, -1}, {0x2051, -1}, {0x2056, -1}, {0x2057, -1},
};

/* The motion's state. Flag bit 0: hidden. */
class UnkMotion_8022F058::Impl {
  public:
    /* A frame of the first part's clump and its last world matrix. */
    struct Node {
        Node() {}
        ~Node() {}

        /* 0x00 */ RwFrame *m_frame;
        /* 0x04 */ Mtx34f m_matrix;
    };

    /* A motion of a part: its number, and the motion played when it ends. */
    struct Motion {
        /* 0x0 */ u32 m_id;
        /* 0x4 */ s32 m_next;
    };

    /* A part: a clump hung on a frame, with its motions. */
    struct Part {
        Part() : m_attach(0), field_0x4(0), m_clump(0), m_updater(0), m_motion(0), m_motionCount(0) {}
        ~Part() {}

        /* 0x00 */ RwFrame *m_attach; /* the frame the part hangs on */
        /* 0x04 */ RwFrame *field_0x4;
        /* 0x08 */ RpClump *m_clump;
        /* 0x0C */ UnkClumpUpdater_80015A7C *m_updater;
        /* 0x10 */ _stMtnPlayWork *m_motion;
        /* 0x14 */ s32 m_motionCount;
        /* 0x18 */ s32 m_next;
        /* 0x1C */ boost::scoped_array<Motion> m_motions;
    };

    Impl(Vehicle::TVehicleBase *owner, s32 slot, const BirdMotionTable *table);
    ~Impl();

    void SetPlacement(const Vec3f *position, const Vec3f *rotation);
    void Update(float dt);
    void SetMotion(int motion, float blend);
    const Mtx34f *GetFrame(int index);
    float GetMotionTime() const;
    void Hide();
    void Show();
    bool HasEnded();
    void SetFade(const UnkClumpFade_80027D5C *source);
    /* Tells the rider which motions to play with the vehicle's. */
    void UpdateRider();
    /* Passes the events of work's motion on. */
    void ProcessEvents(_stMtnPlayWork *work);
    void LoadParts(s32 slot, const BirdMotionTable *table);
    void DestroyParts();
    void SetupPart(s32 slot, Part *part, const BirdMotionSet *set);
    void CreateNodes(const BirdMotionTable *table);
    void UpdateNodes();

    /* 0x00 */ s32 m_partCount;
    /* 0x04 */ s32 m_nodeCount;
    /* 0x08 */ boost::scoped_array<Part> m_parts;
    /* 0x0C */ boost::scoped_array<Node> m_nodes;
    /* 0x10 */ UnkFlags_80014E04 m_flags;
    /* 0x14 */ s32 m_state;
    /* 0x18 */ float m_blend;
    /* 0x1C */ const BirdMotionFrames *m_riderFrames;
    /* 0x20 */ s32 m_riderState;
    /* 0x24 */ s32 m_riderMotionBase;
    /* 0x28 */ UnkVehicleMotionEventCallback_8022ED3C m_callback;
    /* 0x34 */ RwFrame *m_frame;
    /* 0x38 */ Vehicle::TVehicleBase *m_owner;
    /* 0x3C */ const UnkClumpFade_80027D5C *m_fadeSource;
    /* 0x40 */ UnkClumpFade_80027D5C m_fade;
};
SIZE_ASSERT(UnkMotion_8022F058::Impl, 0x6C);

typedef UnkMotion_8022F058::Impl Impl;

/* Deletes the Impl (see checked_delete below). */
inline void DeleteImpl(Impl *impl) {
    delete impl;
}

/* Retail keeps boost::checked_delete of the Impl out of line and its destructor at its own place;
 * this specialization gives that order under GC/2.7 (see docs/research-notes.md and
 * Game/EnergyCore.cpp). */
namespace boost {
template <>
inline void checked_delete<UnkMotion_8022F058::Impl>(UnkMotion_8022F058::Impl *x) {
    DeleteImpl(x);
}
} // namespace boost

UnkMotion_8022F058::Impl::Impl(Vehicle::TVehicleBase *owner, s32 slot, const BirdMotionTable *table)
    : m_owner(owner) {
    m_flags.ClearAll();
    m_state = -1;
    m_riderState = -1;
    m_frame = fn_80486D00();
    LoadParts(slot, table);
    CreateNodes(table);
    m_fadeSource = 0;
    for (int i = 0; i < m_partCount; i++) {
        RpClump *clump = m_parts[i].m_clump;
        if (clump != 0) {
            fn_80038198(clump, &m_fade);
        }
    }
    m_riderFrames = table->m_frames;
    if (m_riderFrames != 0) {
        m_riderMotionBase = fn_80012608(fn_800122D4(), slot, table->m_file) - table->field_0x18;
    }
}

UnkMotion_8022F058::Impl::~Impl() {
    DestroyParts();
    fn_80486E2C(m_frame);
}

void UnkMotion_8022F058::Impl::SetPlacement(const Vec3f *position, const Vec3f *rotation) {
    RwFrame *frame = m_frame;
    Mtx34f matrix;
    AssignMatrix(matrix, fn_8040FFBC(&(const Mtx34f &)((TranslationFn)fn_8040B034)(position), rotation, 2));
    frame->modelling = fn_800591F0(&matrix);
    fn_8048722C(m_frame);
    for (int i = 0; i < m_partCount; i++) {
        Part *part = &m_parts[i];
        if (part->m_clump != 0) {
            if (part->m_motion != 0) {
                RwMatrix *ltm = RwFrameGetLTM(part->m_attach);
                Mtx34f world;
                fn_80016414(&world, ltm);
                fn_804172D8(part->m_motion, &world);
            } else {
                RwFrame *clumpFrame = fn_804872D8(part->m_clump->frame);
                clumpFrame->modelling = *RwFrameGetLTM(part->m_attach);
                fn_8048722C(clumpFrame);
            }
        }
    }
}

void UnkMotion_8022F058::Impl::Update(float dt) {
    boost::shared_ptr<CharInfo> rider = m_owner->GetRider().lock();
    float speed;
    if (rider.get() != 0) {
        speed = reinterpret_cast<UnkCharTime *&>(rider->field_0x60)->m_speed;
    } else {
        speed = lbl_8057D6A0.m_dt;
    }
    for (int i = 0; i < m_partCount; i++) {
        Part *part = &m_parts[i];
        if (part->m_motion != 0) {
            fn_80416DC8(part->m_motion, 0, speed);
            fn_80415EA4(part->m_motion);
            if (part->m_updater != 0) {
                fn_800159AC(part->m_updater);
            }
            if (fn_80416F28(part->m_motion, 0, 3, 1) != 0) {
                SetMotion(part->m_next, 0.0f);
            }
            ProcessEvents(part->m_motion);
        } else if (part->m_clump != 0) {
            RwFrame *frame = fn_804872D8(part->m_clump->frame);
            frame->modelling = *RwFrameGetLTM(part->m_attach);
            fn_8048722C(frame);
        }
        if (part->m_clump != 0) {
            fn_80039B94(part->m_clump, dt, -1.0f);
        }
    }
    UpdateNodes();
    UpdateRider();
    if (m_fadeSource != 0) {
        m_fade = *m_fadeSource;
    }
}

void UnkMotion_8022F058::Impl::SetMotion(int motion, float blend) {
    m_blend = blend;
    if (m_state != motion) {
        m_state = motion;
        for (int i = 0; i < m_partCount; i++) {
            Part *part = &m_parts[i];
            if (part->m_motion != 0) {
                if (m_blend > 0.0f) {
                    fn_804165D8(part->m_motion, 0, -1, -1);
                    fn_80416148(part->m_motion, 0, m_blend);
                }
                fn_80417568(part->m_motion, part->m_motions[m_state].m_id);
                fn_80416F28(part->m_motion, 0, 0xB, 0);
                part->m_next = part->m_motions[m_state].m_next;
            }
        }
    } else {
        for (int i = 0; i < m_partCount; i++) {
            Part *part = &m_parts[i];
            if (part->m_motion != 0) {
                UnkMotionRange_804125F4 range;
                if (fn_804125F4(&range, part->m_motions[m_state].m_id)) {
                    fn_80416E20(part->m_motion, 0, range.m_start);
                }
            }
        }
    }
}

const Mtx34f *UnkMotion_8022F058::Impl::GetFrame(int index) {
    return &m_nodes[index].m_matrix;
}

float UnkMotion_8022F058::Impl::GetMotionTime() const {
    _stMtnPlayWork *work = m_parts[0].m_motion;
    if (work != 0) {
        UnkMotionFrames_80415D40 frames;
        fn_80415D40(work, 0, &frames);
        return frames.m_frame;
    }
    return 0.0f;
}

void UnkMotion_8022F058::Impl::Hide() {
    if (!m_flags.Test(0)) {
        for (int i = 0; i < m_partCount; i++) {
            RpClump *clump = m_parts[i].m_clump;
            if (clump != 0) {
                fn_80038FC0(clump);
            }
        }
        m_flags.Set(0);
    }
}

void UnkMotion_8022F058::Impl::Show() {
    if (m_flags.Test(0)) {
        for (int i = 0; i < m_partCount; i++) {
            RpClump *clump = m_parts[i].m_clump;
            if (clump != 0) {
                fn_80038F94(clump);
            }
        }
        m_flags.Clear(0);
    }
}

bool UnkMotion_8022F058::Impl::HasEnded() {
    _stMtnPlayWork *work = m_parts[0].m_motion;
    if (work != 0 && fn_80416F28(work, 0, 3, 1) != 0) {
        return true;
    }
    return false;
}

void UnkMotion_8022F058::Impl::SetFade(const UnkClumpFade_80027D5C *source) {
    m_fadeSource = source;
    if (m_fadeSource == 0) {
        m_fade.ResetValues();
    }
}

void UnkMotion_8022F058::Impl::UpdateRider() {
    if (m_riderFrames != 0) {
        boost::shared_ptr<CharInfo> rider = m_owner->GetRider().lock();
        if (rider.get() != 0 && fn_80014E34(&m_owner->m_vehicleFlags, 0)) {
            if (m_riderState != m_state) {
                m_riderState = m_state;
                BirdMotionFrames frames = m_riderFrames[m_riderState];
                if (frames.m_frames[0] >= 0) {
                    frames.m_frames[0] += m_riderMotionBase;
                }
                if (frames.m_frames[1] >= 0) {
                    frames.m_frames[1] += m_riderMotionBase;
                }
                if (frames.m_frames[2] >= 0) {
                    frames.m_frames[2] += m_riderMotionBase;
                }
                rider->PushCommand(new Player::SetExternalMotionCommand(frames.m_frames, 4, m_blend));
            }
        } else {
            m_riderState = -1;
        }
    }
}

void UnkMotion_8022F058::Impl::ProcessEvents(_stMtnPlayWork *work) {
    u16 event;
    s32 *data;
    while (fn_80415854(work, 0, &event, &data, 0) != 0) {
        work = 0;
        switch (event) {
        case 0x5003: {
            s32 *value = data;
            int index = value[0];
            if (s_sounds[index].m_sound >= 0) {
                fn_801D54F4(fn_80053164(), s_sounds[index].m_sound, m_owner, 0, 1);
            }
            if (s_sounds[value[0]].m_rumble != -1 && !m_owner->GetRider().expired()) {
                fn_8034DF48(s_sounds[value[0]].m_rumble, m_owner->GetRider().lock()->field_0x54);
            }
            break;
        }
        case 0xD004: {
            int index = data[0];
            if (s_sounds[index].m_sound >= 0) {
                fn_801D53C4(fn_80053164(), s_sounds[index].m_sound, 0, this, 2, 0, 5);
            }
            break;
        }
        }
        if (m_callback.IsSet()) {
            Vehicle::MotionEventArgs args;
            args.m_event = event;
            args.m_value = data;
            m_callback.Call(args);
        }
    }
}

void UnkMotion_8022F058::Impl::LoadParts(s32 slot, const BirdMotionTable *table) {
    m_partCount = table->m_setCount;
    m_parts.reset(new Part[m_partCount]);
    void *manager = fn_800122D4();
    UnkStage_8003A840 *stage = fn_8003A840();
    for (int i = 0; i < m_partCount; i++) {
        const BirdMotionSet *set = &table->m_sets[i];
        Part *part = &m_parts[i];
        RpClump *model = fn_8001277C(manager, slot, set->m_file);
        if (model != 0) {
            part->m_clump = fn_80457090(model);
            fn_80038C84(part->m_clump);
            int area = fn_80169350(fn_80169364(part->m_clump));
            RpClump *clump = part->m_clump;
            SetClumpWorld(clump, stage->GetWorld(area));
            if (set->field_0x0 >= 0) {
                part->m_attach = fn_80038D24(m_parts[0].m_clump, set->field_0x0);
            }
            if (part->m_attach == 0) {
                part->m_attach = m_frame;
            }
        }
        SetupPart(slot, part, set);
    }
}

void UnkMotion_8022F058::Impl::DestroyParts() {
    for (int i = 0; i < m_partCount; i++) {
        Part *part = &m_parts[i];
        if (part->m_updater != 0) {
            delete part->m_updater;
        }
        if (part->m_motion != 0) {
            fn_8041B884(part->m_motion);
        }
        part->m_updater = 0;
        part->m_motion = 0;
        if (part->m_clump != 0) {
            void *world = fn_8046256C(part->m_clump);
            if (world != 0) {
                fn_804623EC(world, part->m_clump);
            }
            fn_80457468(part->m_clump);
        }
    }
}

void UnkMotion_8022F058::Impl::SetupPart(s32 slot, Part *part, const BirdMotionSet *set) {
    part->m_motionCount = set->m_entryCount;
    part->m_motions.reset(new Motion[part->m_motionCount]);
    if (part->m_motionCount > 0) {
        void *manager = fn_800122D4();
        for (int i = 0; i < part->m_motionCount; i++) {
            const BirdMotionEntry *entry = &set->m_entries[i];
            if (entry->m_file < 0) {
                part->m_motions[i].m_id = 0xFFFF;
            } else {
                part->m_motions[i].m_id = entry->m_motion + fn_80012608(manager, slot, entry->m_file);
            }
            part->m_motions[i].m_next = entry->m_slot;
        }
        if (set->field_0x8 >= 0) {
            part->m_motion = fn_8041B908(fn_80012684(manager, slot, set->field_0x8), 1);
            part->m_updater = new UnkClumpUpdater_80015A7C(part->m_clump, part->m_motion);
        }
        m_state = -1;
        SetMotion(0, 0.0f);
    }
}

void UnkMotion_8022F058::Impl::CreateNodes(const BirdMotionTable *table) {
    m_nodeCount = table->m_idCount;
    m_nodes.reset(new Node[m_nodeCount]);
    if (m_nodeCount > 0) {
        for (int i = 0; i < m_nodeCount; i++) {
            m_nodes[i].m_frame = fn_80038D24(m_parts[0].m_clump, table->m_ids[i]);
        }
    }
}

void UnkMotion_8022F058::Impl::UpdateNodes() {
    for (int i = 0; i < m_nodeCount; i++) {
        if (m_nodes[i].m_frame != 0) {
            AssignMatrix(m_nodes[i].m_matrix, fn_802C8068(m_nodes[i].m_frame));
        }
    }
}

UnkMotion_8022F058::UnkMotion_8022F058(CharInfo *owner, int kind, const BirdMotionTable *table)
    : m_impl(new Impl(static_cast<Vehicle::TVehicleBase *>(owner), kind, table)) {}

UnkMotion_8022F058::~UnkMotion_8022F058() {}

extern "C" {

/* Called by the walker's model (names inferred). */
void fn_8022ED14(UnkMotion_8022F058 *motion, const Vehicle::MotionEventCallback &callback);
void fn_8022ECBC(UnkMotion_8022F058 *motion);
UnkMotion_8022F058::Impl *fn_8022ECB4(UnkMotion_8022F058 *motion);
int fn_8022EF0C(UnkMotion_8022F058 *motion);

void fn_8022EF78(UnkMotion_8022F058 *motion, const Vec3f *position, const Vec3f *rotation) {
    motion->m_impl->SetPlacement(position, rotation);
}

void fn_8022EF54(UnkMotion_8022F058 *motion, float dt) {
    motion->m_impl->Update(dt);
}

void fn_8022EF30(UnkMotion_8022F058 *motion, int motionId, float blend) {
    motion->m_impl->SetMotion(motionId, blend);
}

/* The motion playing. */
int fn_8022EF0C(UnkMotion_8022F058 *motion) {
    return fn_8022ECB4(motion)->m_state;
}

void *fn_8022EEE8(UnkMotion_8022F058 *motion, int index) {
    return (void *)motion->m_impl->GetFrame(index);
}

float fn_8022EEC4(const UnkMotion_8022F058 *motion) {
    return motion->m_impl->GetMotionTime();
}

/* Sets and unsets the event callback. */
void fn_8022ED14(UnkMotion_8022F058 *motion, const Vehicle::MotionEventCallback &callback) {
    motion->m_impl->m_callback = callback;
}

void fn_8022ECBC(UnkMotion_8022F058 *motion) {
    motion->m_impl->m_callback.Clear();
}

UnkMotion_8022F058::Impl *fn_8022ECB4(UnkMotion_8022F058 *motion) {
    return motion->m_impl.get();
}

void fn_8022EC90(UnkMotion_8022F058 *motion) {
    motion->m_impl->Hide();
}

void fn_8022EC6C(UnkMotion_8022F058 *motion) {
    motion->m_impl->Show();
}

bool fn_8022EC48(UnkMotion_8022F058 *motion) {
    return motion->m_impl->HasEnded();
}

void fn_8022EC24(UnkMotion_8022F058 *motion, void *arg) {
    motion->m_impl->SetFade(static_cast<const UnkClumpFade_80027D5C *>(arg));
}

}
