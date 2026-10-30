/* LightColli and LightColliMasterTask (class names from RTTI): the LIGHT_COLI gadget, an area (a
 * box, sphere or cylinder collider) that changes the light number of the characters inside it and
 * can light an effect on them, and the task that applies the colliders' settings every frame. The
 * colliders, the effects lit and the light numbers sent are kept in the shared state fn_801E6500
 * returns (Game/LightColliState.h). A collider of type AREA (0) or 3 collects the characters inside
 * it every frame; SWITCH_ON (1) adds a character for good and takes it from the SWITCH_ON
 * colliders it was in, SWITCH_OFF (2) only takes it out. The master task then gives each character
 * the light number of the colliders it is in (type 3 restores the character's default) and lights
 * or stops its effect. File name from the gadget's class. Names marked "(name inferred)" are not
 * original. Definitions are in reverse address order (deferred inlining emits functions
 * last-defined first). The unit's code reaches its .data through a pooled base address, so the
 * linker keeps that section whole: Game/TNumEdit.h and Game/CharaColliBaseCharInfo.h supply the
 * RTTI base lists it starts with. The maps and the set are the stand-ins of <map> and <set>; the
 * order of their instances at the end of the unit holds because the set's tree is the same template
 * as the maps' (see <set>).
 *
 * NonMatching (199 of 204 functions match, in retail's order; .data, .rodata, .sbss, .sdata2 and
 * .ctors match, .sdata up to the RTTI record of checked_deleter<CharCommandUnit>, which retail does
 * not keep):
 * - the three find_or_insert instances (map::operator[], 0x801F0188, 0x801F03BC, 0x801F06C8):
 *   GC/2.7 computes the address of the last node's value once, before comp(), and returns it on
 *   the path that finds the key; retail computes it twice, after comp() and on that path. Every
 *   other instruction matches. Forms tried: the value through an iterator (operator* or a named
 *   iterator), the tests turned around, a const node pointer, a reference cast.
 * - Behave (0x801EF300): retail keeps this in r30 and the contacts in r29, GC/2.7 the other way
 *   round; every declaration order of the locals and an inner scope give the same.
 * - fn_801EEB2C, the label function: retail keeps the pooled .data address in r4 and the guards'
 *   string in r5, GC/2.7 the other way round (declaration orders of the statics tried).
 * These look like the register allocation and expression reuse differences of
 * docs/research-notes.md. */
#define MSL_NEW_INLINE_BAD_ALLOC
#define MSL_LESS_CAST
#define MSL_MAP_SUBSCRIPT
#define MSL_PAIR_COPY_CALL
#define MSL_TREE_END_NODE_PER_TREE
#define MSL_TREE_ITERATOR_ASSIGN_CALL
#define BOOST_WP_LESS

#include <boost/bind.hpp>
#include <boost/function.hpp>

#include "Game/TNumEdit.h"
/* After TNumEdit.h: the order of the casts sets the order of the RTTI base lists in .data. */
#include "Game/CharaColliBaseCharInfo.h"
#include "Effect/System/EffectElement.h"
#include "Game/CharCommand.h"
#include "Game/CharCommandUnit.h"
#include "Game/CharInfo.h"
#include "Game/CharaColli.h"
#include "Game/DebugDisp.h"
#include "Game/Gadget.h"
#include "Game/GadgetCharCollision.h"
#include "Game/GadgetCharInfo.h"
#include "Game/GadgetClassDesc.h"
#include "Game/GadgetCollision.h"
#include "Game/LightColliState.h"
#include "Game/MathTypes.h"
#include "Game/Task.h"
#include "boost/shared_ptr.hpp"
#include "boost/weak_ptr.hpp"
#include "size_assert.h"
#include <map>
#include <set>

/* The game has one shared copy of std::swap for boost::function's any_pointer (0x80019164), which
 * function1::swap calls (see Game/CatchStick.cpp). */
namespace std {
void swap(boost::detail::function::any_pointer &a, boost::detail::function::any_pointer &b);
}

using SonicteamUSA::System::CharaColli::CharaColliBase;
using Effect::System::EffectElement;

/* A node of a collider's contact list and an iterator over the list (see Game/BlockChange.cpp). */
struct UnkColliContactNode_800587A0;

struct UnkColliContactIter_8001EDFC {
    /* Marks an iterator the out-of-line constructor fn_8001EDFC initializes. */
    struct Unset {};

    UnkColliContactIter_8001EDFC() : m_node(0) {}
    UnkColliContactIter_8001EDFC(Unset) {}

    UnkColliContactNode_800587A0 *m_node;
};

/* The light manager the master task resets the characters' lights through (class name unknown). */
class UnkLightManager_800289B0;

extern "C" {
/* The collider manager (behaviour inferred). */
void *fn_80021B08(void);
/* Creates a box, a sphere and a cylinder collider for charInfo (behaviour inferred; the meanings of
 * the integer arguments are unknown; see Game/Door.cpp, Game/BirdBase.cpp, Game/ArkLaser.cpp). */
boost::shared_ptr<CharaColliBase> fn_800C7C34(void *manager, const boost::weak_ptr<CharInfo> &charInfo,
                                              const Vec3f *size, int flags, int arg4, int arg5,
                                              const Vec3f *offset, const Vec3f *rotation, int arg8);
boost::shared_ptr<CharaColliBase> fn_800C7DEC(void *manager, const boost::weak_ptr<CharInfo> &charInfo,
                                              int flags, int arg3, int arg4, float radius,
                                              const Vec3f *offset);
boost::shared_ptr<CharaColliBase> fn_800C7A84(void *manager, const boost::weak_ptr<CharInfo> &charInfo,
                                              int flags, int arg3, int arg4, const Vec3f *offset,
                                              const Vec3f *rotation, float height, float radius,
                                              int arg9);
/* A vector constant read by the collision setup (meaning unknown). */
extern Vec3f lbl_805E07B8;
/* Wraps collider in a new handle, and re-registers the handle's collider (behaviour inferred). */
boost::shared_ptr<GadgetCharCollision> fn_8015B8C4(boost::shared_ptr<CharaColliBase> collider);
void fn_8015BE64(GadgetCharCollision *handle);
/* The character info a collider belongs to (behaviour inferred). */
boost::weak_ptr<CharInfo> fn_800C4568(void *manager, const boost::weak_ptr<CharaColliBase> &collider);
/* The contact list: its first and end nodes, the iterator constructor and post-increment, and the
 * collider of a node (behaviour from their bodies; see Game/BlockChange.cpp). */
UnkColliContactNode_800587A0 *fn_8001EE04(UnkColliContacts_8001EE04 *contacts);
UnkColliContactNode_800587A0 *fn_8001EDEC(UnkColliContacts_8001EE04 *contacts);
void fn_8001EDFC(UnkColliContactIter_8001EDFC *it, UnkColliContactNode_800587A0 *node);
UnkColliContactIter_8001EDFC fn_80027374(UnkColliContactIter_8001EDFC *it, int);
boost::weak_ptr<CharaColliBase> fn_800587A0(UnkColliContactNode_800587A0 *node);
/* The owner's state: whether it was removed or is out of range (behaviour inferred). */
bool fn_800C9A64(UnkPlacement_800C9CD0 *owner);
bool fn_800C996C(UnkPlacement_800C9CD0 *owner);
/* Launches effect id at position plus offset (behaviour inferred). */
boost::weak_ptr<EffectElement> fn_80168C48(const Vec3f *position, const Vec3f *offset, u32 id);
/* The light manager, and the setting of a light (behaviour inferred). */
UnkLightManager_800289B0 *fn_800289B0(void);
void fn_80047DD4(UnkLightManager_800289B0 *manager, int arg1, s8 light);
}

/* The parameters (names from the parameter table). */
struct LightColliParams {
    /* 0x00 */ s32 type;        /* 0 AREA, 1 SWITCH_ON, 2 SWITCH_OFF */
    /* 0x04 */ s32 shape;       /* 0 BOX, 1 SPHERE, 2 CYLINDER */
    /* 0x08 */ s32 lightFlag;
    /* 0x0C */ s32 lightNumber;
    /* 0x10 */ s32 effectFlag;
    /* 0x14 */ s32 effectNumber;
    /* 0x18 */ float scaleX;
    /* 0x1C */ float scaleY;
    /* 0x20 */ float scaleZ;
};
SIZE_ASSERT(LightColliParams, 0x24);

class LightColli : public Gadget {
  public:
    LightColli(Task *parent, UnkPlacement_800C9CD0 *owner);
    virtual ~LightColli();
    virtual void Behave(float dt);
    virtual void fn_800CBC1C();
    virtual void ReloadPlacement(UnkPlacement_800C9CD0 *owner);
    virtual bool HasEnded();

    /* Creates the gadget under task layer 11 (name inferred). */
    static void Instantiate(UnkPlacement_800C9CD0 *owner);

    /* Creates the collider of the shape (name inferred). */
    void CreateCollision();
    /* Copies the owner's position, rotation and parameters, and places the matrix there (name
     * inferred). */
    void ReadPlacement(UnkPlacement_800C9CD0 *owner);
    /* The command unit's callback; it ignores every command (name inferred). */
    void OnCommand(CharCommand &command);

    static const char *s_taskName;

    /* Member names are inferred. */
    /* 0x30 */ boost::shared_ptr<GadgetCharInfo> m_chara;
    /* 0x38 */ LightColliParams m_params;
    /* 0x5C */ std::set<boost::weak_ptr<CharInfo> > m_inside; /* the characters collected */
    /* 0x68 */ boost::weak_ptr<EffectElement> m_effect;
    /* 0x70 */ Mtx34f m_matrix;
    /* 0xA0 */ boost::shared_ptr<GadgetCharCollision> m_collision;
};
SIZE_ASSERT(LightColli, 0xA8);

/* LightColliMasterTask (class name from RTTI): applies the colliders' settings every frame. */
class LightColliMasterTask : public Task {
  public:
    LightColliMasterTask(Task *parent) : Task(parent) {
        m_name = s_taskName;
    }
    virtual ~LightColliMasterTask();
    virtual void Tick(float dt);

    static const char *s_taskName;
};
SIZE_ASSERT(LightColliMasterTask, 0x28);

/* The colour of the colliders in the debug view. */
static UnkColor_8040D2D8 s_color(0xE0, 0xC0, 0xFF, 0xFF);

const char *LightColli::s_taskName = "LightColli";

LightColli::LightColli(Task *parent, UnkPlacement_800C9CD0 *owner) : Gadget(parent, owner) {
    m_name = s_taskName;
    fn_800C9DBC(owner, &m_params, sizeof(m_params));
    m_chara = fn_8015CB88(5, 4, "LightColli");
    m_chara->m_commandUnit = boost::shared_ptr<CharCommandUnit>(new CharCommandUnit());
    fn_800729B0(&m_chara->m_commandUnit->m_callback,
                &(const CharCommandCallback &)CharCommandCallback(
                    boost::bind(&LightColli::OnCommand, this, _1)));
    CreateCollision();
    ReadPlacement(m_owner);
    fn_801E6500()->m_colliders.push_back(this);
    m_effect.reset();
}

LightColli::~LightColli() {
    fn_801E6500()->m_colliders.remove(this);
    if (!m_effect.expired()) {
        boost::shared_ptr<EffectElement> effect(m_effect.lock());
        if (effect) {
            effect->Stop();
        }
        m_effect.reset();
    }
}

void LightColli::Behave(float dt) {
    UnkLightColliState_801E6500 *state;
    UnkColliContacts_8001EE04 &contacts = *reinterpret_cast<UnkColliContacts_8001EE04 *>(
        reinterpret_cast<u8 *>(m_collision->m_collider.get()) + 0xC);
    UnkColliContactIter_8001EDFC it;
    boost::weak_ptr<CharaColliBase> collider;
    boost::weak_ptr<CharInfo> owner;
    state = fn_801E6500();

    if (m_params.type == 0 || m_params.type == 3) {
        m_inside.clear();
    }
    {
        UnkColliContactIter_8001EDFC first((UnkColliContactIter_8001EDFC::Unset()));
        fn_8001EDFC(&first, fn_8001EE04(&contacts));
        it = first;
    }
    for (; it.m_node != fn_8001EDEC(&contacts); fn_80027374(&it, 0)) {
        collider = fn_800587A0(it.m_node);
        if (boost::shared_ptr<CharaColliBase> hit = collider.lock()) {
            owner = fn_800C4568(fn_80021B08(), hit);
            if (boost::shared_ptr<CharInfo> chara = owner.lock()) {
                if (chara->field_0x5C == 1 && chara->GetKind() == 1) {
                    switch (m_params.type) {
                    case 0:
                    case 3:
                        m_inside.insert(chara);
                        state->SetDefaultLight(chara, -1);
                        break;
                    case 1:
                        state->Leave(chara);
                        m_inside.insert(chara);
                        state->SetDefaultLight(chara, -1);
                        break;
                    case 2:
                        state->Leave(chara);
                        break;
                    }
                }
            }
        }
    }
}

void LightColli::ReloadPlacement(UnkPlacement_800C9CD0 *owner) {
    int shape = m_params.shape;
    ReadPlacement(owner);
    if (m_collision.get() != 0) {
        if (shape != m_params.shape) {
            CreateCollision();
        }
        m_collision->m_collider->SetSize(m_params.scaleX, m_params.scaleY, m_params.scaleZ, -2);
    }
}

void LightColli::fn_800CBC1C() {
    if (m_collision.get() != 0) {
        m_collision->m_collider->SetDebugColor(&s_color);
        fn_8015BE64(m_collision.get());
    }
}

void LightColli::ReadPlacement(UnkPlacement_800C9CD0 *owner) {
    fn_800C9F14(owner, &m_chara->m_position);
    fn_800C9ED4(owner, &m_chara->m_rotation);
    fn_800C9DBC(owner, &m_params, sizeof(m_params));
    fn_8040B6F8(&m_matrix);
    fn_8040B59C(&m_matrix, &m_chara->m_position);
    fn_8040B848(&m_matrix, m_chara->m_rotation.y);
    fn_8040B884(&m_matrix, m_chara->m_rotation.x);
    fn_8040B80C(&m_matrix, m_chara->m_rotation.z);
}

bool LightColli::HasEnded() {
    if (m_owner != 0) {
        if (fn_800C9A64(m_owner)) {
            m_flags |= 1;
            return true;
        }
        if (m_params.type == 1 && m_inside.size() != 0) {
            return false;
        }
        if (fn_800C996C(m_owner)) {
            m_flags |= 1;
            return true;
        }
    }
    return false;
}

void LightColli::OnCommand(CharCommand &command) {}

/* Creates the master task with the first collider's files (name inferred). */
extern "C" void fn_801EF034(void) {
    if (fn_801E6500()->m_task == 0) {
        fn_801E6500()->m_task = new LightColliMasterTask(fn_801EB304(fn_8001C4F8(), 15));
    }
}

/* Ends the master task (name inferred). */
extern "C" void fn_801EEFF4(void) {
    if (fn_801E6500()->m_task != 0) {
        fn_801E6500()->m_task->m_flags |= 1;
    }
}

void LightColli::Instantiate(UnkPlacement_800C9CD0 *owner) {
    new LightColli(fn_801EB304(fn_8001C4F8(), 11), owner);
}

void LightColli::CreateCollision() {
    void *manager = fn_80021B08();
    switch (m_params.shape) {
    case 0: {
        Vec3f size(m_params.scaleX, m_params.scaleY, m_params.scaleZ);
        boost::shared_ptr<CharaColliBase> collider(fn_800C7C34(
            manager, m_chara, &size, 0x40000, 10, 0, &lbl_805E07A0, &lbl_805E07B8, 0));
        m_collision = fn_8015B8C4(collider);
        break;
    }
    case 1: {
        boost::shared_ptr<CharaColliBase> collider(
            fn_800C7DEC(manager, m_chara, 0x40000, 10, 0, m_params.scaleX, &lbl_805E07A0));
        m_collision = fn_8015B8C4(collider);
        break;
    }
    case 2: {
        boost::shared_ptr<CharaColliBase> collider(
            fn_800C7A84(manager, m_chara, 0x40000, 10, 0, &lbl_805E07A0, &lbl_805E07B8,
                        m_params.scaleZ, m_params.scaleX, 0));
        m_collision = fn_8015B8C4(collider);
        break;
    }
    }
}

/* The label function of the parameter table: the names of the parameters' values, which depend on
 * the values chosen before (name inferred). */
extern "C" const char *fn_801EEB2C(UnkLabelQuery_800CBC20 *query) {
    struct Query {
        u32 field_0x0;
        s32 index;
        s32 value;
    };
    static const char *types[] = {"AREA", "SWITCH_ON", "SWITCH_OFF"};
    static const char *shapes[] = {"BOX", "SPHERE", "CYLINDER"};
    static const char *lights[] = {
        "LightNo:PLAYER_0", "LightNo:PLAYER_1", "LightNo:PLAYER_2", "LightNo:PLAYER_3",
        "LightNo:OBJECT_0", "LightNo:OBJECT_1", "LightNo:OBJECT_2", "LightNo:OBJECT_3",
        "LightNo:ENEMY_0",  "LightNo:ENEMY_1",  "LightNo:ENEMY_2",  "LightNo:ENEMY_3",
        "LightNo:KAGE",     "LightNo:OTHER_0",  "LightNo:OTHER_1",  "LightNo:OTHER_2",
    };
    static const char *scaleX[] = {"Scale X", "Radius", "Radius"};
    static const char *scaleY[] = {"Scale Y", "DoNotUse", "DoNotUse"};
    static const char *scaleZ[] = {"Scale Z", "DoNotUse", "Length"};
    static const char *lightFlags[] = {"Light OFF", "Light ON"};
    static const char *effectFlags[] = {"Effect OFF", "Effect ON"};
    static int s_shape;
    static int s_type;
    static int s_lightFlag;
    static int s_effectFlag;

    if (s_shape < 0 || s_shape > 2) {
        s_shape = 0;
    }
    static const char *effectNumber = "EffectNumber";
    static const char *doNotUse = "DoNotUse";
    Query *q = reinterpret_cast<Query *>(query);
    switch (q->index) {
    case 0:
        s_type = q->value;
        return types[q->value];
    case 1:
        s_shape = q->value;
        return shapes[s_shape];
    case 2:
        s_lightFlag = q->value;
        return lightFlags[q->value];
    case 3: {
        bool unused = s_lightFlag == 0 || s_type == 2;
        if (unused) {
            return doNotUse;
        }
        return lights[q->value];
    }
    case 4:
        s_effectFlag = q->value;
        return effectFlags[q->value];
    case 5: {
        bool unused = s_effectFlag == 0 || s_type == 2;
        if (unused) {
            return doNotUse;
        }
        return effectNumber;
    }
    case 6:
        return scaleX[s_shape];
    case 7:
        return scaleY[s_shape];
    case 8:
        return scaleZ[s_shape];
    }
    return 0;
}

static GadgetParamDesc lbl_8053DED0[] = {
    {1, "Type", 0, 0, 2, 0.0f, 0.0f, 0.0f, fn_801EEB2C, 0},
    {1, "Shape", 0, 0, 2, 0.0f, 0.0f, 0.0f, fn_801EEB2C, 0},
    {1, "LightFlag", 1, 0, 1, 0.0f, 0.0f, 0.0f, fn_801EEB2C, 0},
    {1, "LightNumber", 0, 0, 15, 0.0f, 0.0f, 0.0f, fn_801EEB2C, 0},
    {1, "EffectFlag", 0, 0, 1, 0.0f, 0.0f, 0.0f, fn_801EEB2C, 0},
    {1, "EffectNumber", 0, 0, 0x10000, 0.0f, 0.0f, 0.0f, fn_801EEB2C, 0},
    {4, "ScaleX", 0, 0, 0, 10.0f, 0.0f, 65536.0f, fn_801EEB2C, 0},
    {4, "ScaleY", 0, 0, 0, 10.0f, 0.0f, 65536.0f, fn_801EEB2C, 0},
    {4, "ScaleZ", 0, 0, 0, 10.0f, 0.0f, 65536.0f, fn_801EEB2C, 0},
    {0},
};

GadgetClassDesc lbl_8053E060 = {
    "LIGHT_COLI", fn_801EF034, fn_801EEFF4, LightColli::Instantiate, 0, 0, 0x62, 1, 0, 10,
    lbl_8053DED0,
};

const char *LightColliMasterTask::s_taskName = "LightColliMasterTask";

UnkLightColliState_801E6500::UnkLightColliState_801E6500() {
    m_task = 0;
}

LightColliMasterTask::~LightColliMasterTask() {
    UnkLightColliState_801E6500 *state = fn_801E6500();
    UnkLightColliState_801E6500::LightMap::iterator it;
    for (it = state->m_lights.begin(); it != state->m_lights.end(); it++) {
        fn_80047DD4(fn_800289B0(), 0, state->GetDefaultLight((*it).first));
    }
    fn_801E6500()->m_task = 0;
    state->m_effects.clear();
    state->m_lights.clear();
    state->m_defaultLights.clear();
    state->m_colliders.clear();
}

void LightColliMasterTask::Tick(float dt) {
    UnkLightColliState_801E6500 *state = fn_801E6500();
    UnkLightColliList_801E6500::iterator it;
    UnkLightColliState_801E6500::LightMap lights;
    UnkLightColliState_801E6500::LightMap types;
    std::map<boost::weak_ptr<CharInfo>, LightColli *> effects;

    for (it = state->m_colliders.begin(); it != state->m_colliders.end(); it++) {
        std::set<boost::weak_ptr<CharInfo> >::iterator inside;
        for (inside = (*it)->m_inside.begin(); inside != (*it)->m_inside.end(); inside++) {
            if ((*it)->m_params.effectFlag != 0) {
                if (effects.count(*inside) == 0) {
                    effects[*inside] = *it;
                } else if ((*it)->m_params.type != 1 && effects[*inside]->m_params.type == 1) {
                    effects[*inside] = *it;
                }
            }
            if ((*it)->m_params.lightFlag != 0) {
                if (lights.count(*inside) == 0) {
                    if ((*it)->m_params.type == 3) {
                        lights[*inside] = state->GetDefaultLight(*inside);
                    } else {
                        lights[*inside] = (*it)->m_params.lightNumber;
                    }
                    types[*inside] = (*it)->m_params.type;
                } else if ((*it)->m_params.type != 1) {
                    if ((state->m_lights.count(*inside) != 0 &&
                         (*it)->m_params.lightNumber == state->m_lights[*inside]) ||
                        types[*inside] == 1) {
                        lights[*inside] = (*it)->m_params.lightNumber;
                        types[*inside] = (*it)->m_params.type;
                    }
                }
            }
        }
    }

    UnkLightColliState_801E6500::LightMap::iterator light;
    for (light = state->m_defaultLights.begin(); light != state->m_defaultLights.end(); light++) {
        boost::weak_ptr<CharInfo> chara((*light).first);
        int defaultLight = state->GetDefaultLight(chara);
        if (lights.count(chara) == 0 || defaultLight == lights[chara]) {
            if (state->GetLight(chara) != defaultLight) {
                if (!chara.expired()) {
                    chara.lock()->PushCommand(new SetLightCommand(defaultLight));
                }
                state->SetLight(chara, defaultLight);
            }
        } else {
            if (state->GetLight(chara) != lights[chara]) {
                if (!chara.expired()) {
                    chara.lock()->PushCommand(new SetLightCommand(lights[chara]));
                }
                state->SetLight(chara, lights[chara]);
            }
        }
        if (effects.count(chara) != 0) {
            LightColli *collider = effects[chara];
            if (collider->m_effect.expired()) {
                Vec3f offset(0.0f, 1.0f, 0.0f);
                collider->m_effect =
                    fn_80168C48(&collider->m_chara->m_position, &offset, collider->m_params.effectNumber);
                if (!collider->m_effect.expired()) {
                    boost::shared_ptr<EffectElement> effect(collider->m_effect.lock());
                    if (effect) {
                        state->m_effects[chara] = effect;
                        effect->SetMatrix(&collider->m_matrix);
                    }
                }
            }
        } else if (!state->m_effects[chara].expired()) {
            boost::shared_ptr<EffectElement> effect(state->m_effects[chara].lock());
            if (effect) {
                effect->Stop();
            }
            state->m_effects[chara].reset();
        }
    }
}

void UnkLightColliState_801E6500::Leave(boost::weak_ptr<CharInfo> chara) {
    UnkLightColliList_801E6500::iterator it;
    for (it = fn_801E6500()->m_colliders.begin(); it != fn_801E6500()->m_colliders.end(); it++) {
        if ((*it)->m_params.type == 1) {
            std::set<boost::weak_ptr<CharInfo> >::iterator found = (*it)->m_inside.find(chara);
            if (found != (*it)->m_inside.end()) {
                (*it)->m_inside.erase(found);
            }
        }
    }
}

void UnkLightColliState_801E6500::SetDefaultLight(boost::weak_ptr<CharInfo> chara, int light) {
    if (light == -1) {
        if (m_defaultLights.count(chara) == 0) {
            m_defaultLights[chara] = 0;
        }
    } else {
        m_defaultLights[chara] = light;
    }
}

int UnkLightColliState_801E6500::GetDefaultLight(boost::weak_ptr<CharInfo> chara) {
    if (m_defaultLights.count(chara) == 0) {
        return 0;
    }
    return m_defaultLights[chara];
}

void UnkLightColliState_801E6500::SetLight(boost::weak_ptr<CharInfo> chara, int light) {
    m_lights[chara] = light;
}

int UnkLightColliState_801E6500::GetLight(boost::weak_ptr<CharInfo> chara) {
    if (m_lights.count(chara) == 0) {
        return -1;
    }
    return m_lights[chara];
}

/* Not called anywhere: the linker strips it from the game. Retail's unit starts with the light
 * map's instances that a loop like the master task's destructor uses, emitted before GetLight. */
void UnusedLightLoop(UnkLightColliState_801E6500 *state) {
    UnkLightColliState_801E6500::LightMap::iterator it;
    for (it = state->m_lights.begin(); it != state->m_lights.end(); it++) {
    }
}

#include <map_impl>
#include <set_impl>
