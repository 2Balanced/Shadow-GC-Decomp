/* ElecFootingRotate (class name from RTTI): the ELEC_FOOTINGROTATE gadget, "OBJ_NUM" platforms
 * (FOOTINGROTATE.BIN) spaced evenly on a circle of "RADIUS m" around the gadget that turn about it
 * at "CIRCLE_SPD deg/sec". Each platform is an ElecFootingRotateObj (class name from the RTTI of
 * its shared_ptr control block) with a character info and a model of its own, kept in a std::list.
 * File name inferred from the class. Names marked "(name inferred)" are not original. Definitions
 * are in reverse address order (deferred inlining emits functions last-defined first). */
#include <list>

#include "Game/CharInfo.h"
#include "Game/FileQueue.h"
#include "Game/Gadget.h"
#include "Game/GadgetCharInfo.h"
#include "Game/GadgetClassDesc.h"
#include "Game/GadgetModelInfo.h"
#include "Game/MathConstants.h"
#include "Game/MathTypes.h"
#include "boost/detail/sp_counted_base_impl.hpp"
#include "boost/shared_ptr.hpp"
#include "boost/weak_ptr.hpp"
#include <dolphin/types.h>
#include "size_assert.h"

typedef Vec3f (*VectorOpFn)(const Vec3f *a, const Vec3f *b);

extern "C" {
/* Builds the character info of a gadget type (see Game/GadgetUtil.h). */
boost::shared_ptr<GadgetCharInfo> fn_8015CD90(int arg0, int arg1);
/* Loads a gadget model file (see Game/ElecSmallCube.cpp). */
boost::shared_ptr<GadgetModelInfo> fn_8015F800(int arg0, const char *fileName);
/* vector turned by angle about Y (see Game/BossSonicDesireMove.cpp). */
Vec3f fn_8040D0AC(const Vec3f &vector, float angle);
/* a + b (see Game/BAMine.cpp). */
Vec3f fn_8040CDD0(const Vec3f *a, const Vec3f *b);
}

/* Copies value into vector through the game's out-of-line assignment (as in Game/GadgetUtil.h,
 * which this unit does not include: it declares pi without const). */
inline void AssignVector(Vec3f &vector, const Vec3f &value) {
    fn_800091D0(&vector, &value);
}

/* Converts degrees to radians (name inferred; as in Game/DefenseSystem.cpp). */
inline float DegToRad(float degrees) {
    return degrees * (lbl_805FAD38 / 180.0f);
}

class ElecFootingRotate;

/* The gadget's parameters (names from the parameter table). */
struct ElecFootingRotateParams {
    /* 0x0 */ u32 m_objNum;
    /* 0x4 */ float m_radius;      /* read in m, kept in units of 10 cm (inferred) */
    /* 0x8 */ float m_circleSpeed; /* read in degrees a second, kept in radians */
};
SIZE_ASSERT(ElecFootingRotateParams, 0xC);

/* One platform (class name from RTTI; member names inferred). */
class ElecFootingRotateObj {
  public:
    ElecFootingRotateObj(UnkPlacement_800C9CD0 *owner, ElecFootingRotate *parent, u32 index);
    ~ElecFootingRotateObj();

    /* Names inferred. */
    void Update(float dt);
    void Reload();
    void UpdatePosition();
    void UpdateMatrix();

    /* 0x00 */ u32 m_index;
    /* 0x04 */ ElecFootingRotate *m_parent;
    /* 0x08 */ float m_offset; /* the platform's angle on the circle */
    /* 0x0C */ UnkPlacement_800C9CD0 *m_owner;
    /* 0x10 */ boost::shared_ptr<GadgetCharInfo> m_chara;
    /* 0x18 */ boost::shared_ptr<GadgetModelInfo> m_model;
};
SIZE_ASSERT(ElecFootingRotateObj, 0x20);

/* Deletes a platform (name inferred; see checked_delete below). */
inline void DeleteObj(ElecFootingRotateObj *obj) {
    delete obj;
}

/* Retail keeps boost::checked_delete<ElecFootingRotateObj> out of line (0x802DCD70) and the
 * platform's destructor at its own place; this specialization calls an inline helper, which gives
 * retail's code and order (as in Game/DiablonExecuter.cpp; see docs/research-notes.md). */
namespace boost {
template <> inline void checked_delete<ElecFootingRotateObj>(ElecFootingRotateObj *x) {
    DeleteObj(x);
}
} // namespace boost

class ElecFootingRotate : public Gadget {
  public:
    ElecFootingRotate(Task *parent, UnkPlacement_800C9CD0 *owner);
    virtual ~ElecFootingRotate();
    virtual void Behave(float dt);
    virtual void ReloadPlacement(UnkPlacement_800C9CD0 *owner);

    /* Creates the gadget under task layer 11 (name inferred). */
    static void Instantiate(UnkPlacement_800C9CD0 *owner);

    /* Copies the owner's position, rotation and parameters (name inferred). */
    void ReadPlacement(UnkPlacement_800C9CD0 *owner);
    /* Makes the platforms anew (name inferred). */
    void CreateObjs();

    static const char *s_taskName;

    /* 0x30 */ Vec3f m_position;
    /* 0x3C */ float m_angle; /* the circle's turn */
    /* 0x40 */ ElecFootingRotateParams m_params;
    /* 0x4C */ std::list<boost::shared_ptr<ElecFootingRotateObj> > m_objs;
};
SIZE_ASSERT(ElecFootingRotate, 0x58);

const char *ElecFootingRotate::s_taskName = "ElecFootingRotate";

ElecFootingRotateObj::ElecFootingRotateObj(UnkPlacement_800C9CD0 *owner, ElecFootingRotate *parent,
                                           u32 index)
    : m_index(index), m_parent(parent), m_owner(owner) {
    m_offset = index * (360.0f * (lbl_805FAD38 / 180.0f) / m_parent->m_params.m_objNum);
    m_chara = fn_8015CD90(0x20, 4);
    m_chara->field_0x30 = 2;
    m_chara->m_rotation.x = m_chara->m_rotation.z = 0.0f;
    m_model = fn_8015F800(1, "FOOTINGROTATE.BIN");
    m_model->fn_80162A68(0, 0);
    m_model->fn_80161F08("col", boost::weak_ptr<CharInfo>(m_chara));
    UpdatePosition();
    UpdateMatrix();
}

ElecFootingRotateObj::~ElecFootingRotateObj() {}

void ElecFootingRotateObj::Update(float dt) {
    UpdatePosition();
    UpdateMatrix();
    m_model->fn_80162FB0(dt);
}

void ElecFootingRotateObj::Reload() {
    UpdatePosition();
    UpdateMatrix();
    m_model->fn_80162EF4();
    m_model->fn_8016108C();
}

void ElecFootingRotateObj::UpdatePosition() {
    float angle = m_parent->m_angle + m_offset;
    Vec3f offset;
    Vec3f previous;
    fn_800091EC(&offset, &(const Vec3f &)fn_8040D0AC(
                             Vec3f(0.0f, 0.0f, -m_parent->m_params.m_radius), angle));
    fn_800091EC(&previous, &m_chara->m_position);
    AssignVector(m_chara.px->m_position, fn_8040CDD0(&m_parent->m_position, &offset));
    m_chara->m_rotation.y = angle;
    AssignVector(m_chara.px->field_0x18,
                 ((VectorOpFn)fn_8040CD74)(&m_chara.px->m_position, &previous));
}

void ElecFootingRotateObj::UpdateMatrix() {
    if (m_model.get() != 0) {
        Mtx34f matrix;
        Mtx34f translation;
        SetTranslationMtx(&translation, m_chara->m_position);
        fn_8040C148(&matrix, &translation);
        fn_8040B848(&matrix, m_chara->m_rotation.y);
        m_model->fn_801633DC(&matrix);
    }
}

extern "C" void fn_802DD28C(void) {
    fn_8006B474("FOOTINGROTATE.BIN", 1);
}

extern "C" void fn_802DD288(void) {}

void ElecFootingRotate::Instantiate(UnkPlacement_800C9CD0 *owner) {
    new ElecFootingRotate(fn_801EB304(fn_8001C4F8(), 0xB), owner);
}

ElecFootingRotate::ElecFootingRotate(Task *parent, UnkPlacement_800C9CD0 *owner)
    : Gadget(parent, owner) {
    m_name = s_taskName;
    m_objs.clear();
    ReadPlacement(owner);
    CreateObjs();
}

ElecFootingRotate::~ElecFootingRotate() {}

void ElecFootingRotate::Behave(float dt) {
    m_angle += m_params.m_circleSpeed * dt;
    for (std::list<boost::shared_ptr<ElecFootingRotateObj> >::iterator it = m_objs.begin();
         it != m_objs.end(); it++) {
        it->get()->Update(dt);
    }
}

void ElecFootingRotate::ReloadPlacement(UnkPlacement_800C9CD0 *owner) {
    u32 objNum = m_params.m_objNum;
    ReadPlacement(owner);
    if (objNum != m_params.m_objNum) {
        CreateObjs();
    }
    for (std::list<boost::shared_ptr<ElecFootingRotateObj> >::iterator it = m_objs.begin();
         it != m_objs.end(); it++) {
        it->get()->Reload();
    }
}

void ElecFootingRotate::ReadPlacement(UnkPlacement_800C9CD0 *owner) {
    fn_800C9F14(owner, &m_position);
    Vec3f rotation(0.0f, 0.0f, 0.0f);
    fn_800C9ED4(owner, &rotation);
    m_angle = rotation.y;
    fn_800C9DBC(owner, &m_params, sizeof(m_params));
    m_params.m_radius = 10.0f * m_params.m_radius;
    m_params.m_circleSpeed = DegToRad(m_params.m_circleSpeed);
}

void ElecFootingRotate::CreateObjs() {
    m_objs.clear();
    for (u32 i = 0; i < m_params.m_objNum; i++) {
        m_objs.push_back(boost::shared_ptr<ElecFootingRotateObj>(
            new ElecFootingRotateObj(m_owner, this, i)));
    }
}

static GadgetParamDesc lbl_80558DC0[] = {
    {2, "OBJ_NUM", 4, 1, 0x20, 0.0f, 0.0f, 0.0f, fn_800CBC20, 0},
    {4, "RADIUS m", 0, 0, 0, 20.0f, 1.0f, 65536.0f, fn_800CBC20, 0},
    {4, "CIRCLE_SPD deg/sec", 0, 0, 0, 0.0f, -1080.0f, 1080.0f, fn_800CBC20, 0},
    {0},
};

GadgetClassDesc lbl_80558E60 = {
    "ELEC_FOOTINGROTATE", fn_802DD28C, fn_802DD288, ElecFootingRotate::Instantiate, 0, 0, 0x7EB, 1,
    0, 0x14, lbl_80558DC0,
};
