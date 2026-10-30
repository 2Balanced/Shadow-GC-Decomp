/* ElecPanel (class name from RTTI): the ELEC_PANEL gadget of the electric circuit area, a floor
 * panel (PANEL.BIN) showing one of four colours. A character stepping onto it, or an attack, makes
 * it show the next colour and marks its circuit as changed; Game/ElecPanelManagerTask.cpp turns the
 * circuit's switch on once all the circuit's panels show one colour. A switch panel's colour is
 * kept by the owner in an ElecPanelData; a move panel slides along its move vector once the
 * circuit's switch is on. Once the switch is on, the panel's material brightens. File name from the
 * class. Names marked "(name inferred)" are not original. Definitions are in reverse address order
 * (deferred inlining emits functions last-defined first). */
#define MSL_MULTIMAP_TREE
#define MSL_NEW_INLINE_BAD_ALLOC
#define MSL_LESS_CAST
#define MSL_PAIR_COPY_CALL
#define MSL_SET_TREE_ASSIGN
#define MSL_TREE_END_NODE_PER_TREE
#define MSL_TREE_ITERATOR_ASSIGN_CALL
#define BOOST_WP_LESS
#include <boost/bind.hpp>
#include <boost/function.hpp>

#include "Game/TNumEdit.h"
/* After TNumEdit.h: the order of the casts sets the order of the RTTI base lists in .data. */
#include "Game/CharaColliBaseCharInfo.h"
#include "Game/CharInfo.h"
#include "Game/CharaColli.h"
#include "Game/CharaColliAttack.h"
#include "Game/DebugDraw.h"
#include "Game/ElecPanel.h"
#include "Game/FileQueue.h"
#include "Game/Gadget.h"
#include "Game/GadgetCharCollision.h"
#include "Game/GadgetCharInfo.h"
#include "Game/GadgetClassDesc.h"
#include "Game/GadgetCollision.h"
#include "Game/GadgetModelInfo.h"
#include "Game/GadgetUtil.h"
#include "Game/MathTypes.h"
#include "Game/Player.h"
#include "Game/SetAdapterState.h"
#include "Game/UnkSwitchBank_800CB69C.h"
#include "boost/shared_ptr.hpp"
#include "boost/weak_ptr.hpp"
#include "size_assert.h"
#include <set>

using SonicteamUSA::System::CharaColli::AttackCallback;
using SonicteamUSA::System::CharaColli::AttackCallbackParam;
using SonicteamUSA::System::CharaColli::CharaColliAttack;
using SonicteamUSA::System::CharaColli::CharaColliBase;

/* A node of a collider's contact list and an iterator over the list (see Game/BlockChange.cpp). */
struct UnkColliContactNode_800587A0;

struct UnkColliContactIter_8001EDFC {
    UnkColliContactIter_8001EDFC() : m_node(0) {}

    UnkColliContactNode_800587A0 *m_node;
};

/* What a player stands on (as declared in Game/PlayerBehaviorEdgeHang.cpp). */
class UnkColliObject_80028CA4;

struct UnkColliMoveContact_803334A4 {
    UnkColliMoveContact_803334A4();
    ~UnkColliMoveContact_803334A4();

    bool IsValid() const;

    /* 0x00 */ u8 m_isCollider;
    /* 0x04 */ boost::weak_ptr<CharInfo> m_chara;
    /* 0x0C */ boost::weak_ptr<CharaColliBase> m_collider;
    /* 0x14 */ UnkColliObject_80028CA4 *m_object;
};
SIZE_ASSERT(UnkColliMoveContact_803334A4, 0x18);

/* The material state of a part (see Game/ElecAccessPanel.cpp). */
struct UnkMaterialColor_801621F0 {
    /* 0x0 */ u8 field_0x0[0x8];
    /* 0x8 */ u8 r;
    /* 0x9 */ u8 g;
    /* 0xA */ u8 b;
    /* 0xB */ u8 a;
};

extern "C" {
/* GadgetModelInfo helpers (see Game/ElecAccessPanel.cpp): reset the model's motion (behaviour
 * inferred), select the material named name, and find the material named name. */
void fn_8016232C(GadgetModelInfo *model);
void fn_8016239C(GadgetModelInfo *model, const char *name);
UnkMaterialColor_801621F0 *fn_801621F0(GadgetModelInfo *model, const char *name);
/* The attack collision of a model variant (see Game/CandleStick.cpp), and setting its callback. */
boost::weak_ptr<CharaColliAttack> fn_80161428(GadgetModelInfo *model, int variant);
void fn_80096B64(CharaColliAttack *attack, int arg1, const AttackCallback &callback);
/* The collider manager and a box collider (see Game/CommandCollision.cpp). */
void *fn_80021B08(void);
boost::shared_ptr<CharaColliBase> fn_800C7C34(void *manager, const boost::weak_ptr<CharInfo> &charInfo,
                                              const Vec3f *size, int flags, int arg4, int arg5,
                                              const Vec3f *offset, const Vec3f *rotation, int arg8);
boost::shared_ptr<GadgetCharCollision> fn_8015B8C4(boost::shared_ptr<CharaColliBase> collider);
void fn_8015BE9C(GadgetCharCollision *handle, float dt);
/* The contact list (see Game/BlockChange.cpp). */
UnkColliContactNode_800587A0 *fn_8001EE04(UnkColliContacts_8001EE04 *contacts);
UnkColliContactNode_800587A0 *fn_8001EDEC(UnkColliContacts_8001EE04 *contacts);
/* The iterator at node (its out-of-line constructor, as in Game/BossCharaColli.cpp). */
UnkColliContactIter_8001EDFC fn_8001EDFC(UnkColliContactNode_800587A0 *node);
UnkColliContactIter_8001EDFC fn_80027374(UnkColliContactIter_8001EDFC *it, int);
boost::weak_ptr<CharaColliBase> fn_800587A0(UnkColliContactNode_800587A0 *node);
boost::weak_ptr<CharInfo> fn_800C4568(void *manager, const boost::weak_ptr<CharaColliBase> &collider);
/* What the player stands on (see Game/PlayerBehaviorWallHang.cpp). */
const UnkColliMoveContact_803334A4 *fn_80078DA0(Player::PlayerBase *player);
/* Sounds (see Game/CandleStick.cpp and Game/ElecAccessPanel.cpp). */
void fn_801D54F4(void *manager, int id, GadgetCharInfo *charInfo, int arg3, int arg4);
void fn_801D53C4(void *manager, int id, GadgetCharInfo *charInfo, void *source, int arg4, int arg5,
                 int arg6);
/* Moves the character info by velocity, accelerating it (behaviour inferred). */
void fn_804103E8(GadgetCharInfo *chara, float arg1, Vec3f *velocity, const Vec3f *acceleration, float dt);
/* Vector helpers: the unit vector, scaling, addition (behaviour inferred). */
Vec3f fn_8040CEE8(const Vec3f *v);
Vec3f fn_8040CDA8(const Vec3f *v, float scale);
Vec3f fn_8040CDD0(const Vec3f *a, const Vec3f *b);
void fn_8040CFF8(Vec3f *vector, const Vec3f *value);
}

/* A vector constant read by the collision setup (meaning unknown; see Game/CommandCollision.cpp). */
extern Vec3f lbl_805E07B8;

/* fn_8040CD74 returns lhs - rhs (as in Game/AttackObject.cpp). */
typedef Vec3f (*VectorSubtractFn)(const Vec3f *lhs, const Vec3f *rhs);

/* v * scale through fn_8040CDA8 (name inferred). */
inline Vec3f Scale(const Vec3f &v, float scale) {
    return fn_8040CDA8(&v, scale);
}

const char *ElecPanel::s_taskName = "ElecPanel";

inline ElecPanelData *ElecPanel::GetData() {
    return dynamic_cast<ElecPanelData *>(m_owner->m_adapterState);
}

ElecPanel::ElecPanel(Task *parent, UnkPlacement_800C9CD0 *owner) : Gadget(parent, owner) {
    m_name = s_taskName;
    m_chara = fn_8015CD90(0x20, 4);
    ReadPlacement(owner);
    if (m_type == 0) {
        if (owner->m_adapterState == 0) {
            ElecPanelData *data = new ElecPanelData;
            fn_800C9930(owner, data);
            data->m_color = m_params.color;
            m_color = m_params.color;
        } else {
            m_color = GetData()->m_color;
        }
    } else {
        m_color = m_params.color;
    }
    m_model = new GadgetModelInfo(1, "PANEL.BIN");
    m_model->fn_80162A68(m_color, 0);
    fn_8016239C(m_model, "mat");
    m_fade = 0.0f;
    m_model->fn_80161F08("col", m_chara);
    boost::weak_ptr<CharaColliAttack> attackCollision = fn_80161428(m_model, 0);
    if (boost::shared_ptr<CharaColliAttack> attack = attackCollision.lock()) {
        fn_80096B64(attack.get(), 1, boost::bind(&ElecPanel::OnAttack, this, _1));
    }
    Vec3f size(7.5f, 1.0f, 7.5f);
    boost::shared_ptr<CharaColliBase> collider =
        fn_800C7C34(fn_80021B08(), m_chara, &size, 0x40000, 10, 0, &Vec3f(0.0f, -1.0f, 0.0f), &lbl_805E07B8, 0);
    m_collision = fn_8015B8C4(collider);
    fn_800F5980(m_collision.get(), boost::bind(&ElecPanel::OnColliDetect, this, _1));
    /* fn_800F5980 assigns the callback at the start of its first argument. */
    fn_800F5980(reinterpret_cast<GadgetCharCollision *>(&m_collision.get()->m_onLeave), boost::bind(&ElecPanel::OnColliLeave, this, _1));
    m_collision->TurnOn();
    RebuildTransform();
    fn_801E6114()->AddPanel(this);
    m_timer = 0.0f;
}

ElecPanel::~ElecPanel() {
    if (m_type == 1) {
        fn_801690D0(fn_800CB69C(), m_circuitId, 0);
    }
    if (m_model != 0) {
        delete m_model;
        m_model = 0;
    }
    fn_801E6114()->RemovePanel(this);
    m_inside.clear();
}

void ElecPanel::Behave(float dt) {
    if (m_model != 0) {
        m_model->fn_80162FB0(dt);
    }
    if (m_type == 1) {
        switch (m_moveState) {
        case 1: {
            Vec3f move(m_params.moveX, m_params.moveY, m_params.moveZ);
            m_distance = fn_8040CF2C(&move);
            fn_800091D0(&m_velocity, &(const Vec3f &)Vec3f(0.0f, 0.0f, 0.0f));
            fn_800091D0(&m_acceleration, &(const Vec3f &)Scale(fn_8040CEE8(&move), m_params.accel));
            m_moveState = 0;
            break;
        }
        case 0:
            if (fn_8016904C(fn_800CB69C(), m_circuitId) == 1) {
                m_moveState = 2;
                break;
            }
            if (0.0f == m_timer) {
                m_inside.clear();
            } else {
                m_timer -= dt;
                if (m_timer < 0.0f) {
                    m_timer = 0.0f;
                }
            }
            m_changed = 0;
            break;
        case 2: {
            Vec3f start;
            fn_800C9F14(m_owner, &start);
            fn_804103E8(m_chara.get(), 0.0f, &m_velocity, &m_acceleration, dt);
            fn_801D53C4(fn_80053164(), 0x7237, m_chara.get(), this, 3, 0, 5);
            if (fn_8040CF2C(&(const Vec3f &)((VectorSubtractFn)fn_8040CD74)(&m_chara->m_position, &start)) >= m_distance) {
                Vec3f move(m_params.moveX, m_params.moveY, m_params.moveZ);
                AssignVector(m_chara.px->m_position, fn_8040CDD0(&start, &move));
                m_moveState = 3;
            }
            break;
        }
        case 3:
            break;
        }
    } else {
        if (0.0f == m_timer) {
            m_inside.clear();
        } else {
            m_timer -= dt;
            if (m_timer < 0.0f) {
                m_timer = 0.0f;
            }
        }
        m_changed = 0;
    }
    RebuildTransform();
    u8 id = m_circuitId;
    if (id != 0 && fn_8016904C(fn_800CB69C(), id) == 1) {
        if (m_type == 0 && 0.0f == m_fade) {
            GetData()->m_color = m_color;
        }
        if (m_fade < 1.0f) {
            m_fade += dt;
            if (m_fade > 1.0f) {
                m_fade = 1.0f;
            }
            UnkMaterialColor_801621F0 *material = fn_801621F0(m_model, "mat");
            material->r = 255.0f * m_fade;
            material->g = 255.0f * m_fade;
            material->b = 255.0f * m_fade;
            material->a = 255.0f * m_fade;
        }
    }
    fn_8015BE9C(m_collision.get(), dt);
}

void ElecPanel::ReloadPlacement(UnkPlacement_800C9CD0 *owner) {
    ReadPlacement(owner);
    if (m_model != 0) {
        m_model->fn_80162EF4();
        RebuildTransform();
        if (m_params.color != m_color) {
            m_color = m_params.color;
            fn_8016232C(m_model);
            m_model->fn_80162804();
            m_model->fn_80162A68(m_color, 0);
            fn_8016239C(m_model, "mat");
        }
        Vec3f start;
        fn_800091EC(&start, &m_chara->m_position);
        Vec3f end(m_params.moveX, m_params.moveY, m_params.moveZ);
        fn_8040CFF8(&end, &start);
        u8 segment[0x18];
        fn_802E0B58(fn_80020A20(segment, &start, &end), &lbl_805F06D8);
        m_model->fn_8016108C();
    }
    fn_801E6114()->RemoveOwner(owner);
}

void ElecPanel::ReadPlacement(UnkPlacement_800C9CD0 *owner) {
    fn_800C9F14(owner, &m_chara.get()->m_position);
    fn_800C9ED4(owner, &m_chara.get()->m_rotation);
    fn_800C9DBC(owner, &m_params, sizeof(m_params));
    fn_800C9E60(owner, &m_circuitId);
    m_type = m_params.type;
    m_moveState = 1;
}

void ElecPanel::RebuildTransform() {
    if (m_model != 0) {
        Mtx34f matrix;

        fn_8040C148(&matrix, &lbl_805E0720);
        fn_8040B59C(&matrix, &m_chara.get()->m_position);
        fn_8040B848(&matrix, m_chara.get()->m_rotation.y);
        fn_8040B884(&matrix, m_chara.get()->m_rotation.x);
        fn_8040B80C(&matrix, m_chara.get()->m_rotation.z);
        m_model->fn_801633DC(&matrix);
    }
}

void ElecPanel::OnColliDetect(ColliDetectArgs &args) {
    if (fn_8016904C(fn_800CB69C(), m_circuitId) == 1) {
        return;
    }
    std::set<boost::weak_ptr<CharInfo> >::iterator it;
    std::set<boost::weak_ptr<CharInfo> > inside;
    for (it = m_inside.begin(); it != m_inside.end(); it++) {
        boost::shared_ptr<CharInfo> chara(*it);
        if (chara.get() != 0 && chara->GetKind() == 1 && chara->field_0x5C == 1) {
            m_timer = 0.3333f;
        }
    }
    UnkColliContactIter_8001EDFC contact;
    for (contact = fn_8001EDFC(fn_8001EE04(args.m_contacts)); contact.m_node != fn_8001EDEC(args.m_contacts); fn_80027374(&contact, 0)) {
        boost::shared_ptr<CharInfo> chara(fn_800C4568(fn_80021B08(), fn_800587A0(contact.m_node)));
        if (chara.get() != 0 && chara->GetKind() == 1 && chara->field_0x5C == 1) {
            if (0.0f == m_timer) {
                it = m_inside.find(chara);
                if (it == m_inside.end() && !m_changed) {
                    NextColor();
                    m_changed = 1;
                    m_timer = 0.3333f;
                }
            }
            inside.insert(chara);
        }
    }
    m_inside = inside;
}

void ElecPanel::OnColliLeave(ColliDetectArgs &args) {
    if (fn_8016904C(fn_800CB69C(), m_circuitId) != 1) {
        m_timer = 0.3333f;
    }
}

void ElecPanel::OnAttack(AttackCallbackParam &param) {
    param.m_result = 5;
    if (fn_8016904C(fn_800CB69C(), m_circuitId) == 1) {
        return;
    }
    std::set<boost::weak_ptr<CharInfo> >::iterator it;
    bool standing = false;
    boost::shared_ptr<CharInfo> player(fn_800A8A7C(fn_80074194(), &m_chara.get()->m_position));
    if (player.get() != 0) {
        boost::shared_ptr<Player::PlayerBase> base = boost::dynamic_pointer_cast<Player::PlayerBase>(player);
        if (base.get() != 0) {
            const UnkColliMoveContact_803334A4 *contact = fn_80078DA0(base.get());
            if (contact != 0 && contact->IsValid()) {
                boost::shared_ptr<CharInfo> ground = contact->m_chara.lock();
                if (ground == m_chara) {
                    standing = true;
                }
            }
        }
    }
    if (m_timer > 0.0f) {
        return;
    }
    boost::shared_ptr<CharaColliAttack> attacker(param.m_attacker);
    if (attacker.get() != 0 && (attacker->m_flags & 0xBC0) != 0) {
        boost::shared_ptr<CharInfo> chara(attacker->m_owner);
        if (chara.get() != 0 && chara->field_0x5C == 1) {
            it = m_inside.find(chara);
            if (it == m_inside.end()) {
                if (!m_changed && !standing) {
                    NextColor();
                    m_changed = 1;
                }
                m_inside.insert(chara);
            }
        }
    }
}

void ElecPanel::NextColor() {
    if (++m_color >= 4) {
        m_color = 0;
    }
    fn_8016232C(m_model);
    m_model->fn_80162804();
    m_model->fn_80162A68(m_color, 0);
    fn_8016239C(m_model, "mat");
    fn_801E6114()->SetChanged(m_circuitId);
    switch (m_color) {
    case 0:
        fn_801D54F4(fn_80053164(), 0x7232, m_chara.get(), 0, 1);
        break;
    case 1:
        fn_801D54F4(fn_80053164(), 0x7233, m_chara.get(), 0, 1);
        break;
    case 2:
        fn_801D54F4(fn_80053164(), 0x7234, m_chara.get(), 0, 1);
        break;
    case 3:
        fn_801D54F4(fn_80053164(), 0x7235, m_chara.get(), 0, 1);
        break;
    }
}

/* Loads the model and the tables, and creates the manager task (name inferred). */
extern "C" void fn_80259494(void) {
    fn_8006B474("PANEL.BIN", 1);
    fn_801E6114()->Load();
    if (fn_801E6114()->m_task == 0) {
        fn_801E6114()->m_task = new ElecPanelManagerTask(fn_801EB304(fn_8001C4F8(), 15));
    }
}

/* Deletes the manager task and empties the tables (name inferred). */
extern "C" void fn_8025944C(void) {
    delete fn_801E6114()->m_task;
    fn_801E6114()->Clear();
}

void ElecPanel::Instantiate(UnkPlacement_800C9CD0 *owner) {
    new ElecPanel(fn_801EB304(fn_8001C4F8(), 11), owner);
}

/* The label function of the parameter table: the names of the colours and types (name inferred). */
extern "C" const char *fn_802593B0(UnkLabelQuery_800CBC20 *query) {
    struct Query {
        u32 field_0x0;
        s32 index;
        s32 value;
    };
    static const char *colors[] = {"Color : Red", "Color : Yellow", "Color : Green", "Color : Blue"};
    static const char *types[] = {"Type : switch", "Type : move"};
    Query *q = reinterpret_cast<Query *>(query);

    if (q->index == 0) {
        return colors[q->value];
    }
    if (q->index == 1) {
        return types[q->value];
    }
    return 0;
}

static GadgetParamDesc lbl_8054A018[] = {
    {1, "Color", 0, 0, 3, 0.0f, 0.0f, 0.0f, fn_802593B0, 0},
    {1, "Type", 0, 0, 1, 0.0f, 0.0f, 0.0f, fn_802593B0, 0},
    {4, "Accell", 0, 0, 0, 0.0f, 0.0f, 65536.0f, fn_800CBC20, 0},
    {4, "MoveVectorX", 0, 0, 0, 0.0f, -65536.0f, 65536.0f, fn_800CBC20, 0},
    {4, "MoveVectorY", 0, 0, 0, 0.0f, -65536.0f, 65536.0f, fn_800CBC20, 0},
    {4, "MoveVectorZ", 0, 0, 0, 0.0f, -65536.0f, 65536.0f, fn_800CBC20, 0},
    {0},
};

GadgetClassDesc lbl_8054A130 = {
    "ELEC_PANEL", fn_80259494, fn_8025944C, ElecPanel::Instantiate, 0, 0, 0x7D2, 1, 0, 10, lbl_8054A018,
};

#include <set_impl>
