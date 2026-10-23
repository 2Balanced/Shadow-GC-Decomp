/* EscapePlaneRail::EscapePlaneRail (class name from RTTI; its s_taskName is "EscapePlaneRail"): the
 * PLANE RAIL gadget, a rail model (ESCAPEPLANERAIL.BIN) that waits for its switch and then sinks at
 * "speed" until it is "dist" below where it was placed. Built like Game/Magma.cpp. File name
 * inferred from the class. Names marked "(name inferred)" are not original. Definitions are in
 * reverse address order (deferred inlining emits functions last-defined first). */
#include "Game/Gadget.h"
#include "Game/GadgetCharInfo.h"
#include "Game/GadgetModelInfo.h"
#include "Game/GadgetClassDesc.h"
#include "Game/GadgetUtil.h"
#include "Game/UnkSwitchBank_800CB69C.h"
#include "Game/FileQueue.h"
#include "boost/shared_ptr.hpp"
#include "size_assert.h"

extern "C" {
/* The switch id the owner was placed with (behaviour inferred; see Magma.cpp). */
void fn_800C9E50(UnkPlacement_800C9CD0 *owner, s32 *id);
/* Loads a gadget model file (behaviour inferred; see BAShield.cpp). */
boost::shared_ptr<GadgetModelInfo> fn_8015F800(int arg0, const char *fileName);
/* The character info's placement as a matrix (see BKTank.cpp). */
void fn_8015CEA0(GadgetCharInfo *charInfo, Mtx34f *matrix);
}

namespace EscapePlaneRail {

/* The parameters of the owner record (names from the parameter table). */
struct EscapePlaneRailParams {
    EscapePlaneRailParams() : dist(0.0f), speed(0.0f) {}

    /* 0x0 */ float dist;
    /* 0x4 */ float speed;
};

class EscapePlaneRail : public Gadget {
  public:
    EscapePlaneRail(Task *parent, UnkPlacement_800C9CD0 *owner);
    virtual ~EscapePlaneRail();
    virtual void Behave(float dt);
    virtual void ReloadPlacement(UnkPlacement_800C9CD0 *owner);

    /* Creates the gadget under task layer 11 (name inferred). */
    static void Instantiate(UnkPlacement_800C9CD0 *owner);

    /* Copies the owner's placement, parameters and switch id (name inferred). */
    void ReadPlacement(UnkPlacement_800C9CD0 *owner);
    /* Places the model at the character info (name inferred). */
    void UpdateModel();
    /* Sinks towards the end height; true once it is there (name inferred). */
    bool Move(float dt);

    /* The model file (name inferred). */
    static const char *s_modelFile;
    static const char *s_taskName;

    /* Member names are inferred. */
    /* 0x30 */ boost::shared_ptr<GadgetCharInfo> m_chara;
    /* 0x38 */ boost::shared_ptr<GadgetModelInfo> m_model;
    /* 0x40 */ EscapePlaneRailParams m_params;
    /* 0x48 */ s32 m_state; /* 0 waiting for the switch, 1 sinking, 2 done */
    /* 0x4C */ s32 m_switchId;
    /* 0x50 */ Vec3f m_start;
    /* 0x5C */ u32 field_0x5C;
};
SIZE_ASSERT(EscapePlaneRail, 0x60);

const char *EscapePlaneRail::s_modelFile = "ESCAPEPLANERAIL.BIN";
const char *EscapePlaneRail::s_taskName = "EscapePlaneRail";

EscapePlaneRail::EscapePlaneRail(Task *parent, UnkPlacement_800C9CD0 *owner) : Gadget(parent, owner) {
    m_name = s_taskName;
    m_chara = fn_8015CD90(0x81, 4);
    ReadPlacement(owner);
    m_model = fn_8015F800(1, s_modelFile);
    m_model->fn_80162A68(0, 0);
    UpdateModel();
}

EscapePlaneRail::~EscapePlaneRail() {}

void EscapePlaneRail::Behave(float dt) {
    if (m_model.get() != 0) {
        m_model->fn_80162FB0(dt);
    }
    switch (m_state) {
    case 0:
        if (fn_8016904C(fn_800CB69C(), m_switchId) == 1) {
            m_state = 1;
        }
        break;
    case 1:
        if (Move(dt)) {
            m_state = 2;
        }
        UpdateModel();
        break;
    case 2:
        break;
    }
}

void EscapePlaneRail::ReloadPlacement(UnkPlacement_800C9CD0 *owner) {
    ReadPlacement(owner);
    UpdateModel();
    Vec3f start;
    fn_800091EC(&start, &m_start);
    Vec3f end;
    fn_800091EC(&end, &start);
    end.y -= m_params.dist;
    u8 segment[0x18];
    fn_80020A20(segment, &start, &end);
    fn_802E0B58(segment, &lbl_805F06D8);
}

void EscapePlaneRail::ReadPlacement(UnkPlacement_800C9CD0 *owner) {
    fn_800C9F14(owner, &m_chara->m_position);
    fn_800C9ED4(owner, &m_chara->m_rotation);
    fn_800C9DBC(owner, &m_params, sizeof(m_params));
    fn_800C9E50(owner, &m_switchId);
    fn_800091D0(&m_start, &m_chara->m_position);
    m_state = 0;
}

void EscapePlaneRail::UpdateModel() {
    if (m_model.get() != 0) {
        Mtx34f matrix;
        fn_8015CEA0(m_chara.get(), &matrix);
        m_model->fn_801633DC(&matrix);
    }
}

bool EscapePlaneRail::Move(float dt) {
    float end = m_start.y - m_params.dist;
    m_chara->m_position.y -= m_params.speed * dt;
    if (m_chara->m_position.y < end) {
        m_chara->m_position.y = end;
        return true;
    }
    return false;
}

} // namespace EscapePlaneRail

extern "C" void fn_802ABAF0(void) {
    fn_8006B474(EscapePlaneRail::EscapePlaneRail::s_modelFile, 1);
}

extern "C" void fn_802ABAEC(void) {}

void EscapePlaneRail::EscapePlaneRail::Instantiate(UnkPlacement_800C9CD0 *owner) {
    Task *parent = fn_801EB304(fn_8001C4F8(), 11);
    new EscapePlaneRail(parent, owner);
}

static GadgetParamDesc lbl_805546A0[] = {
    {4, "dist", 0, 0, 0, 0.0f, 0.0f, 65536.0f, fn_800CBC20, 0},
    {4, "speed", 0, 0, 0, 0.0f, 0.0f, 65536.0f, fn_800CBC20, 0},
    {0},
};

GadgetClassDesc lbl_80554718 = {
    "PLANE RAIL", fn_802ABAF0, fn_802ABAEC, EscapePlaneRail::EscapePlaneRail::Instantiate, 0, 0, 0x13F3, 1, 0, 10,
    lbl_805546A0,
};
