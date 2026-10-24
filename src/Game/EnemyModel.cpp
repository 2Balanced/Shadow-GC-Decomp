/* The model of an enemy (UnkModel_801A3E9C, class name unknown; Game/EnemyRender.cpp's
 * EnemyRender::Impl holds one): an array of parts, each a clump loaded from a resource slot into
 * the world of its stage area, with the frame it hangs from, an updater that keeps it in step with
 * a motion, and its colour fade. The first part is the body; the others hang from frames of the
 * body or follow the motion. Game/EnemyRender.cpp, Game/BkLarva.h and Game/ThrowBomb.cpp declare
 * the class with only its size; its functions keep their address names (the callers use them).
 * File name inferred; names marked "(name inferred)" are not original. Definitions are in reverse
 * address order (deferred inlining emits functions last-defined first).
 *
 * NonMatching: 30 of 32 functions and all data match. Two differ only in register allocation:
 * fn_801A3CE4, where retail gives the parameters r27..r30 in their order (GC/2.7 gives model r29
 * and slot r23, for every source form tried), and Part::SetAlpha, where retail loads m_area into r3
 * and m_baseArea into r0 for the comparison in its second branch (GC/2.7 swaps the two). */
/* boost::scoped_array needs it (see boost/checked_delete.hpp). */
#define BOOST_CHECKED_ARRAY_DELETE

#include "Game/DebugDispShape.h"
#include "Game/EnemyRender.h"
#include "Game/MathTypes.h"
#include "boost/scoped_array.hpp"
#include <dolphin/types.h>
#include "size_assert.h"

struct _stMtnPlayWork;

/* A clump's colour fade (struct name unknown; as in Game/BossCtrlUnit.cpp): its constructor is out
 * of line (0x80027D5C). The word at 0x28 is a float (the part's colour copy loads it as one). */
struct UnkClumpFade_80027D5C;
extern "C" UnkClumpFade_80027D5C *fn_80027D5C(UnkClumpFade_80027D5C *fade);
struct UnkClumpFade_80027D5C {
    UnkClumpFade_80027D5C() {
        fn_80027D5C(this);
    }

    /* 0x00 */ float m_color[4];
    /* 0x10 */ float field_0x10[5];
    /* 0x24 */ u8 m_mode;
    /* 0x28 */ float field_0x28;
};
SIZE_ASSERT(UnkClumpFade_80027D5C, 0x2C);

/* An object that keeps a clump in step with a motion (class name unknown; as in
 * Game/DiablonPartsModelBase.cpp). */
class UnkClumpUpdater_80015A7C {
  public:
    UnkClumpUpdater_80015A7C(RpClump *clump, _stMtnPlayWork *motion);
    ~UnkClumpUpdater_80015A7C();

    /* 0x00 */ u8 field_0x0[0x1C];
};

/* The world of each stage area (as in Game/BossCtrlUnit.cpp). */
struct UnkWorldEntry_8051D208 {
    /* 0x00 */ u8 field_0x0[0x10];
    /* 0x10 */ void *m_world;
    /* 0x14 */ u8 field_0x14[0x18 - 0x14];
};
extern UnkWorldEntry_8051D208 lbl_8051D208[];

extern "C" {
/* The resource loader and a model of a slot (see Game/BossCtrlUnit.cpp). */
void *fn_800122D4(void);
void *fn_8001277C(void *loader, s32 slot, int model);
/* RpClumpClone, RpClumpDestroy, the world a clump is in, taking it out of and adding it to a
 * world, the area a frame lies in and the stage (see Game/BossCtrlUnit.cpp). */
RpClump *fn_80457090(void *data);
void fn_80457468(RpClump *clump);
void *fn_8046256C(RpClump *clump);
void fn_804623EC(void *world, RpClump *clump);
void fn_804622FC(void *world, RpClump *clump);
void *fn_80169364(RpClump *clump);
int fn_80169350(void *frame);
void fn_8003A840(void);
/* The clump's fade set-up (see Game/BossCtrlUnit.cpp), and two calls on a clump (meanings
 * unknown; see Game/BkLarva.cpp). */
void fn_80038198(RpClump *clump, UnkClumpFade_80027D5C *fade);
void fn_80038FC0(RpClump *clump);
void fn_80038F94(RpClump *clump);
/* The frame with hierarchy id id under frame (behaviour inferred), RwFrameAddChild,
 * RwFrameRemoveChild, RwMatrixUpdate and RwFrameTransform. */
RwFrame *fn_80038E84(RwFrame *frame, s32 id);
RwFrame *fn_804872E0(RwFrame *parent, RwFrame *child);
RwFrame *fn_80487448(RwFrame *frame);
RwMatrix *fn_8047EBA0(RwMatrix *matrix);
RwFrame *fn_804876E8(RwFrame *frame, const RwMatrix *matrix, int combine);
/* The updater's update, with and without an argument (behaviour inferred). */
void fn_80015954(UnkClumpUpdater_80015A7C *updater, void *arg);
void fn_800159AC(UnkClumpUpdater_80015A7C *updater);
}

/* Moves clump into world (as in Game/DiablonPartsModelBase.cpp). */
inline void SetClumpWorld(RpClump *clump, void *world) {
    void *current = fn_8046256C(clump);
    if (current != world) {
        if (current != 0) {
            fn_804623EC(current, clump);
        }
        fn_804622FC(world, clump);
    }
}

/* Deletes p and clears it (name inferred). */
template <class T> inline void SafeDelete(T *&p) {
    if (p != 0) {
        delete p;
        p = 0;
    }
}

/* The world of a stage area (name inferred). */
inline void *AreaWorld(int area) {
    fn_8003A840();
    return lbl_8051D208[area].m_world;
}

class UnkModel_801A3E9C {
  public:
    /* A part of the model (struct name inferred). */
    struct Part {
        Part();  /* 0x801A43D8 */
        ~Part(); /* 0x801A4390 */

        void Load(int slot, int model);
        void SetMatrix(const Mtx34f &matrix);
        void Release();
        void SetAlpha(float alpha);
        void SetColor(float r, float g, float b, float a);
        void SetMotion(_stMtnPlayWork *motion);
        void SetFade(const UnkClumpFade_80027D5C &fade);

        /* The frame with hierarchy id id under the part's frame. */
        RwFrame *FindFrame(int id) {
            if (m_frame != 0) {
                return fn_80038E84(m_frame, id);
            }
            return 0;
        }
        /* Hangs the part's frame from parent, and takes it off its parent. */
        void Attach(RwFrame *parent) {
            if (parent != 0 && m_frame != 0) {
                fn_804872E0(parent, m_frame);
            }
        }
        void Detach() {
            if (m_frame != 0 && *reinterpret_cast<void **>(&m_frame->object[0x4]) != 0) {
                fn_80487448(m_frame);
            }
        }
        /* Brings the clump up to date with its motion or its frame. */
        void Update(void *arg) {
            if (m_frame != 0) {
                if (m_updater != 0) {
                    fn_80015954(m_updater, arg);
                } else {
                    fn_8048722C(m_frame);
                }
            }
        }
        void Update() {
            if (m_frame != 0) {
                if (m_updater != 0) {
                    fn_800159AC(m_updater);
                } else {
                    fn_8048722C(m_frame);
                }
            }
        }
        /* Takes the clump out of its world. */
        void LeaveWorld() {
            void *world = fn_8046256C(m_clump);
            if (world != 0) {
                fn_804623EC(world, m_clump);
            }
        }

        /* 0x00 */ RpClump *m_clump;
        /* 0x04 */ RwFrame *m_frame;
        /* 0x08 */ UnkClumpUpdater_80015A7C *m_updater;
        /* 0x0C */ float m_alpha;
        /* 0x10 */ s32 m_baseArea; /* the area the clump was loaded into */
        /* 0x14 */ s32 m_area;     /* the area it is drawn in (10 while it is see-through) */
        /* 0x18 */ UnkClumpFade_80027D5C m_fade;
    };

    UnkModel_801A3E9C();
    ~UnkModel_801A3E9C();

    Part *GetPart(int index) {
        return &m_parts[index];
    }

    /* 0x0 */ boost::scoped_array<Part> m_parts;
    /* 0x4 */ s32 m_frameCount; /* the number of parts */
};
SIZE_ASSERT(UnkModel_801A3E9C, 0x8);
SIZE_ASSERT(UnkModel_801A3E9C::Part, 0x44);

extern "C" {
/* Defined below. */
void fn_801A3CE4(UnkModel_801A3E9C *model, int slot, const EnemyMotionTable *table, _stMtnPlayWork *motion);
void fn_801A3B58(UnkModel_801A3E9C *model);
}

UnkModel_801A3E9C::Part::Part()
    : m_clump(0), m_frame(0), m_updater(0), m_alpha(0.0f), m_baseArea(6), m_area(6) {}

UnkModel_801A3E9C::Part::~Part() {
    Release();
}

void UnkModel_801A3E9C::Part::Load(int slot, int model) {
    if (m_clump == 0) {
        void *data = fn_8001277C(fn_800122D4(), slot, model);
        if (data != 0) {
            m_clump = fn_80457090(data);
            if (m_clump != 0) {
                m_frame = m_clump->frame;
                int area = fn_80169350(fn_80169364(m_clump));
                if (area == 9) {
                    area = 10;
                }
                void *world = AreaWorld(area);
                if (world != 0) {
                    m_baseArea = area;
                    m_area = area;
                }
                SetClumpWorld(m_clump, world);
                m_fade.m_mode = 8;
                fn_80038198(m_clump, &m_fade);
            }
        }
    }
}

void UnkModel_801A3E9C::Part::SetMatrix(const Mtx34f &matrix) {
    if (m_frame != 0) {
        RwMatrix modelling;
        modelling.right.x = matrix.values[0][0];
        modelling.right.y = matrix.values[1][0];
        modelling.right.z = matrix.values[2][0];
        modelling.up.x = matrix.values[0][1];
        modelling.up.y = matrix.values[1][1];
        modelling.up.z = matrix.values[2][1];
        modelling.at.x = matrix.values[0][2];
        modelling.at.y = matrix.values[1][2];
        modelling.at.z = matrix.values[2][2];
        modelling.pos.x = matrix.values[0][3];
        modelling.pos.y = matrix.values[1][3];
        modelling.pos.z = matrix.values[2][3];
        modelling.pad3 = 0;
        modelling.pad2 = 0;
        modelling.pad1 = 0;
        modelling.flags = 0;
        fn_8047EBA0(&modelling);
        fn_804876E8(m_frame, &modelling, 0);
    }
}

void UnkModel_801A3E9C::Part::Release() {
    SafeDelete(m_updater);
    Detach();
    if (m_clump != 0) {
        LeaveWorld();
        fn_80457468(m_clump);
        m_clump = 0;
        m_frame = 0;
    }
}

/* Below an alpha of 1 the clump is drawn in area 10 (name inferred). */
void UnkModel_801A3E9C::Part::SetAlpha(float alpha) {
    if (m_clump != 0) {
        m_alpha = alpha;
        m_fade.m_color[3] = m_alpha;
        if (m_alpha < 1.0f) {
            if (m_area == 6) {
                m_area = 10;
                LeaveWorld();
                void *world = AreaWorld(m_area);
                SetClumpWorld(m_clump, world);
                Update();
            }
        } else if (m_area != m_baseArea) {
            m_area = m_baseArea;
            LeaveWorld();
            void *world = AreaWorld(m_area);
            SetClumpWorld(m_clump, world);
            Update();
        }
    }
}

void UnkModel_801A3E9C::Part::SetColor(float r, float g, float b, float a) {
    if (m_clump != 0) {
        m_fade.m_color[3] = a;
        m_fade.m_color[0] = r;
        m_fade.m_color[1] = g;
        m_fade.m_color[2] = b;
    }
}

void UnkModel_801A3E9C::Part::SetMotion(_stMtnPlayWork *motion) {
    if (m_clump != 0 && motion != 0) {
        if (m_updater != 0) {
            SafeDelete(m_updater);
        }
        m_updater = new UnkClumpUpdater_80015A7C(m_clump, motion);
    }
}

void UnkModel_801A3E9C::Part::SetFade(const UnkClumpFade_80027D5C &fade) {
    m_fade = fade;
}

UnkModel_801A3E9C::UnkModel_801A3E9C() : m_frameCount(0) {}

UnkModel_801A3E9C::~UnkModel_801A3E9C() {
    fn_801A3B58(this);
}

/* Loads a part for each entry of table from resource slot slot: the first is the body, which
 * follows motion; the others hang from the body's frame with the entry's hierarchy id, or follow
 * motion where the id is negative (name inferred). */
extern "C" void fn_801A3CE4(UnkModel_801A3E9C *model, int slot, const EnemyMotionTable *table,
                            _stMtnPlayWork *motion) {
    int i;
    int count = table->count;
    model->m_frameCount = count;
    model->m_parts.reset(new UnkModel_801A3E9C::Part[count]);
    const EnemyMotionEntry *entries = table->entries;
    for (i = 0; i < count; i++) {
        model->m_parts[i].Load(slot, entries[i].id);
    }
    if (motion != 0) {
        model->m_parts[0].SetMotion(motion);
    }
    int n = table->count;
    for (i = 1; i < n; i++) {
        int id = entries[i].value;
        if (id >= 0) {
            RwFrame *frame = model->m_parts[0].FindFrame(id);
            if (frame != 0) {
                model->GetPart(i)->Attach(frame);
            }
        } else if (motion != 0) {
            model->m_parts[i].SetMotion(motion);
        }
    }
}

/* Loads the one part of model model from resource slot slot (name inferred). */
extern "C" void fn_801A3CA4(UnkModel_801A3E9C *model, int slot, int id, _stMtnPlayWork *motion) {
    EnemyMotionEntry entry = {id, -1};
    EnemyMotionTable table;
    table.entries = &entry;
    table.count = 1;
    fn_801A3CE4(model, slot, &table, motion);
}

/* Releases the parts (name inferred). */
extern "C" void fn_801A3B58(UnkModel_801A3E9C *model) {
    for (int i = model->m_frameCount - 1; i >= 0; i--) {
        SafeDelete(model->GetPart(i)->m_updater);
    }
    for (int i = model->m_frameCount - 1; i >= 0; i--) {
        model->GetPart(i)->Detach();
    }
    model->m_frameCount = 0;
    model->m_parts.reset();
}

/* Two calls on the clump of each part (meanings unknown), and the first of them on the parts
 * after the body (names inferred). */
extern "C" void fn_801A3AFC(UnkModel_801A3E9C *model) {
    for (int i = 0; i < model->m_frameCount; i++) {
        RpClump *clump = model->GetPart(i)->m_clump;
        if (clump != 0) {
            fn_80038F94(clump);
        }
    }
}

extern "C" void fn_801A3AA0(UnkModel_801A3E9C *model) {
    for (int i = 0; i < model->m_frameCount; i++) {
        RpClump *clump = model->GetPart(i)->m_clump;
        if (clump != 0) {
            fn_80038FC0(clump);
        }
    }
}

extern "C" void fn_801A3A44(UnkModel_801A3E9C *model) {
    for (int i = 1; i < model->m_frameCount; i++) {
        RpClump *clump = model->GetPart(i)->m_clump;
        if (clump != 0) {
            fn_80038FC0(clump);
        }
    }
}

/* Sets the alpha, and the colour, of every part (names inferred). */
extern "C" void fn_801A39E0(UnkModel_801A3E9C *model, float alpha) {
    for (int i = 0; i < model->m_frameCount; i++) {
        model->m_parts[i].SetAlpha(alpha);
    }
}

extern "C" void fn_801A395C(UnkModel_801A3E9C *model, float r, float g, float b, float a) {
    for (int i = 0; i < model->m_frameCount; i++) {
        model->m_parts[i].SetColor(r, g, b, a);
    }
}

/* Brings every part up to date (names inferred). */
extern "C" void fn_801A38E8(UnkModel_801A3E9C *model) {
    for (int i = 0; i < model->m_frameCount; i++) {
        model->GetPart(i)->Update();
    }
}

extern "C" void fn_801A386C(UnkModel_801A3E9C *model, void *arg) {
    for (int i = 0; i < model->m_frameCount; i++) {
        model->GetPart(i)->Update(arg);
    }
}

/* Hangs the body from parent, and takes it off its parent (names inferred). */
extern "C" void fn_801A3814(UnkModel_801A3E9C *model, RwFrame *parent) {
    if (model->m_frameCount > 0) {
        model->GetPart(0)->Attach(parent);
    }
}

extern "C" void fn_801A37C8(UnkModel_801A3E9C *model) {
    if (model->m_frameCount > 0) {
        model->GetPart(0)->Detach();
    }
}

/* The body's clump and frame (names inferred). */
extern "C" RpClump *fn_801A378C(UnkModel_801A3E9C *model) {
    if (model->m_frameCount > 0) {
        return model->GetPart(0)->m_clump;
    }
    return 0;
}

extern "C" RwFrame *fn_801A3750(UnkModel_801A3E9C *model) {
    if (model->m_frameCount > 0) {
        return model->GetPart(0)->m_frame;
    }
    return 0;
}

/* The frame with hierarchy id id in the body (name inferred). */
extern "C" RwFrame *fn_801A3704(UnkModel_801A3E9C *model, int id) {
    if (model->m_frameCount > 0) {
        return model->m_parts[0].FindFrame(id);
    }
    return 0;
}

/* Places the body at position, turned by rotation (name inferred). */
extern "C" void fn_801A367C(UnkModel_801A3E9C *model, const Vec3f *position, const Vec3f *rotation) {
    if (model->m_frameCount > 0) {
        Mtx34f matrix;
        fn_8040C148(&matrix, &lbl_805E0720);
        fn_8040B59C(&matrix, position);
        fn_8040B848(&matrix, rotation->y);
        fn_8040B884(&matrix, rotation->x);
        fn_8040B80C(&matrix, rotation->z);
        model->m_parts[0].SetMatrix(matrix);
    }
}

/* The fade of part index (name inferred). */
extern "C" UnkClumpFade_80027D5C *fn_801A3634(UnkModel_801A3E9C *model, int index) {
    if (index < model->m_frameCount) {
        return &model->GetPart(index)->m_fade;
    }
    return 0;
}

/* Gives part index, or every part (index -1), the fade fade (name inferred). */
extern "C" void fn_801A35AC(UnkModel_801A3E9C *model, const UnkClumpFade_80027D5C *fade, int index) {
    if (index == 0xFFFFFFFF) {
        for (int i = 0; i < model->m_frameCount; i++) {
            model->m_parts[i].SetFade(*fade);
        }
    } else if (index < model->m_frameCount) {
        model->m_parts[index].SetFade(*fade);
    }
}
