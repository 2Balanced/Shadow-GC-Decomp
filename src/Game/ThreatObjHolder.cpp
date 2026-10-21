/* ThreatObjHolder (class name from RTTI): the ThreatObj gadget, a breakable threat model
 * (THREATOBJ.BIN) of the horror and circus stages (from its debris files). The gadget holds one ThreatObj
 * (class name from the RTTI of its shared_ptr control block), which waits until the nearest player
 * comes within "RANGE_RADIUS m", then plays the opening motion of its "MODEL_TYPE" and loops a
 * second one, now and then playing a sound. Its attack collision takes damage; when its health runs
 * out it breaks into its debris (the model type's .DFF), rewards the attacker and the holder
 * finishes. The model types come from one of two tables picked by the stage when the type is
 * loaded. File name inferred from the gadget class. Method names marked "(name inferred)" are not
 * original. Definitions are in reverse address order (deferred inlining emits functions
 * last-defined first). */
#include <boost/bind.hpp>
#include <boost/function.hpp>

#include "Game/CharInfo.h"
#include "Game/CharaColliAttack.h"
#include "Game/DebugDraw.h"
#include "Game/Gadget.h"
#include "Game/GadgetCharInfo.h"
#include "Game/GadgetModelInfo.h"
#include "Game/GadgetClassDesc.h"
#include "Game/FileQueue.h"
#include "Game/Random.h"
#include "boost/shared_ptr.hpp"
#include "boost/weak_ptr.hpp"
#include "size_assert.h"

using SonicteamUSA::System::CharaColli::AttackCallback;
using SonicteamUSA::System::CharaColli::AttackCallbackParam;
using SonicteamUSA::System::CharaColli::CharaColliAttack;

namespace boost {

template<class T> inline void shared_ptr<T>::reset() {
    this_type().swap(*this);
}

} // namespace boost

/* The motion work of a model (struct name from the RTTI of its shared_ptr control block). */
struct _stMtnPlayWork;

/* The parameters of a debris spawn (layout as in MeteorsHolder.cpp). */
struct UnkDebrisParam_8015F0B8 {
    UnkDebrisParam_8015F0B8(const boost::shared_ptr<CharInfo> &info, int index);

    /* 0x00 */ u8 field_0x0[0x34];
    /* 0x34 */ float scale;
    /* 0x38 */ u8 field_0x38[0x40 - 0x38];
    /* 0x40 */ float field_0x40;
    /* 0x44 */ s32 model;
    /* 0x48 */ u8 field_0x48[0x4A - 0x48];
    /* 0x4A */ u8 field_0x4A;
    /* 0x4B */ u8 field_0x4B;
    /* 0x4C */ s32 field_0x4C;
    /* 0x50 */ u8 field_0x50[0x54 - 0x50];
    /* 0x54 */ u8 field_0x54;
    /* 0x55 */ u8 field_0x55[0x5C - 0x55];
};
SIZE_ASSERT(UnkDebrisParam_8015F0B8, 0x5C);

/* A sound entry of a model's table (see Coaster.cpp). */
struct UnkSoundEntry_80208888 {
    /* 0x00 */ const char *m_name;
    /* 0x04 */ u32 field_0x4;
    /* 0x08 */ u32 m_soundId; /* passed to the sound calls as 16 bits */
};

/* The game state block in .bss (see Coaster.cpp; only the member used here). */
struct UnkGameState_8057D6A0 {
    /* 0x000 */ u8 field_0x0[0xAC];
    /* 0x0AC */ s32 m_stage; /* (name inferred) */
    /* 0x0B0 */ u8 field_0xB0[0x150 - 0xB0];
};
SIZE_ASSERT(UnkGameState_8057D6A0, 0x150);

class UnkModelTable_80208888;

extern "C" {
extern UnkGameState_8057D6A0 lbl_8057D6A0;

/* The nearest player (see Coaster.cpp). */
boost::weak_ptr<CharInfo> fn_800A8B74(UnkCharaList_80074194 *manager, int id);
extern int lbl_805EE588;

/* Builds the character info of a gadget type (see GadgetUtil.h). */
boost::shared_ptr<GadgetCharInfo> fn_8015CD90(int arg0, int arg1);
/* The sound manager and playing a sound at a character info (see Coaster.cpp). */
void *fn_80053164(void);
void fn_801D54F4(void *manager, u16 id, GadgetCharInfo *charInfo, int arg3, int arg4);
/* The model loader, and a model's sound table entry (see Coaster.cpp). */
void *fn_800122D4(void);
UnkModelTable_80208888 *fn_80012700(void *loader, int arg1, const char *fileName);
UnkSoundEntry_80208888 *fn_80208848(UnkModelTable_80208888 *table, int index);

/* Loads model fileName for a gadget (behaviour inferred; arg0's meaning is unknown). */
boost::shared_ptr<GadgetModelInfo> fn_8015F800(int arg0, const char *fileName);
/* GadgetModelInfo methods that are not declared in its header yet (see ArkCannon.cpp, BAGunShip.cpp
 * and Catapult.cpp). */
bool fn_80161FC8(GadgetModelInfo *model, int variant, const boost::weak_ptr<CharInfo> &info);
boost::weak_ptr<CharaColliAttack> fn_80161428(GadgetModelInfo *model, int variant);
boost::weak_ptr<_stMtnPlayWork> fn_801602B4(GadgetModelInfo *model, int index);
void fn_80160A84(GadgetModelInfo *model, int index, int motion);
void fn_80160480(GadgetModelInfo *model, int index);
void fn_8016271C(GadgetModelInfo *model);
void fn_80162790(GadgetModelInfo *model);
int fn_80416F28(_stMtnPlayWork *work, int arg1, int arg2, int arg3);

/* Debris (see MeteorsHolder.cpp). */
void fn_8015D93C(UnkDebrisParam_8015F0B8 *param);
/* The attacker of an attack, and rewarding it (behaviour inferred; see ChaoHolder.cpp). */
boost::shared_ptr<CharInfo> fn_8015D570(AttackCallbackParam &param);
void fn_8020642C(const boost::shared_ptr<CharInfo> &attacker, int reward);
/* True when value is (nearly) zero (behaviour inferred). */
bool fn_8000B044(float value);
/* Writes the parameter block back to the owner, and tells the owner the gadget is done (behaviour
 * inferred). */
void fn_800C9D68(UnkPlacement_800C9CD0 *owner, const void *params, int size);
void fn_800C9B34(UnkPlacement_800C9CD0 *owner);
}

/* fn_8040CD74 called as returning a - b by value (see ArkCannon.cpp). */
typedef Vec3f (*VectorSubtractFn)(const Vec3f *a, const Vec3f *b);

/* One model type: the motion that opens it, the motion it loops afterwards (-1: none), and its
 * debris (names inferred). */
struct ThreatObjModel {
    /* 0x0 */ s32 m_openMotion;
    /* 0x4 */ s32 m_loopMotion;
    /* 0x8 */ const char *m_debrisFile;
};

/* The model types of one stage (name inferred). */
struct ThreatObjModelSet {
    /* 0x0 */ u32 m_count;
    /* 0x4 */ const ThreatObjModel *m_models;
};

static const ThreatObjModel lbl_804C021C[] = {
    {1, 2, "PR0300HORRORTHREAT_BRK.DFF"},
};

static const ThreatObjModel lbl_804C0264[] = {
    {1, -1, "PR0302CIRCUSTHREAT01BRK.DFF"},
    {2, 1, "PR0302CIRCUSTHREAT02PBRK.DFF"},
};

static const ThreatObjModelSet lbl_804C027C[] = {
    {1, lbl_804C021C},
    {2, lbl_804C0264},
};

/* The larger of a and b (name inferred). Retail calls it out of line, so it is not declared inline
 * like std::max. */
template <class T> const T &Max(const T &a, const T &b) {
    return a < b ? b : a;
}

/* The sizes of the two sets, and the larger one, which bounds "MODEL_TYPE" (names inferred). */
const u32 kHorrorModelCount = sizeof(lbl_804C021C) / sizeof(lbl_804C021C[0]);
const u32 kCircusModelCount = sizeof(lbl_804C0264) / sizeof(lbl_804C0264[0]);
static const u32 lbl_805EEFDC = Max(kHorrorModelCount, kCircusModelCount);

/* The parameters the owner supplies (names from the parameter table). */
struct ThreatObjParams {
    /* 0x0 */ u32 modelType;
    /* 0x4 */ float rangeRadius; /* metres in the table, world units once loaded */
};

class ThreatObjHolder;

class ThreatObj {
  public:
    ThreatObj(ThreatObjHolder *holder, UnkPlacement_800C9CD0 *owner);
    ~ThreatObj();

    /* Per-frame update (name inferred). */
    void Tick(float dt);
    /* Moves back to the owner's placement and draws the range (name inferred). */
    void ReloadPlacement(UnkPlacement_800C9CD0 *owner);
    /* Copies the owner's position and rotation (name inferred). */
    void Reset(UnkPlacement_800C9CD0 *owner);
    /* Places the model (name inferred). */
    void RebuildTransform();
    /* True when the nearest player is within the range (name inferred). */
    bool IsPlayerInRange();
    /* The attack callback of the model's collision (name inferred). */
    void OnAttack(AttackCallbackParam &param);

    /* Member names are inferred. */
    /* 0x00 */ ThreatObjHolder *m_holder;
    /* 0x04 */ const ThreatObjModel *m_type;
    /* 0x08 */ boost::shared_ptr<GadgetCharInfo> m_chara;
    /* 0x10 */ boost::shared_ptr<GadgetModelInfo> m_model;
    /* 0x18 */ s32 m_state; /* 0 waiting, 1 open, 2 broken */
    /* 0x1C */ float m_health;
    /* 0x20 */ s32 m_soundId; /* -1: none */
    /* 0x24 */ float m_soundTimer;
};
SIZE_ASSERT(ThreatObj, 0x28);

/* Deletes a threat object (name inferred; see checked_delete below). */
inline void DeleteThreatObj(ThreatObj *object) {
    delete object;
}

/* Retail keeps boost::checked_delete<ThreatObj> out of line and ~ThreatObj at its own place (see
 * docs/research-notes.md and Rocket.cpp). */
namespace boost {
template<> inline void checked_delete<ThreatObj>(ThreatObj *x) {
    DeleteThreatObj(x);
}
} // namespace boost

class ThreatObjHolder : public Gadget {
  public:
    ThreatObjHolder(Task *parent, UnkPlacement_800C9CD0 *owner);
    virtual ~ThreatObjHolder();
    virtual void Behave(float dt);
    virtual void ReloadPlacement(UnkPlacement_800C9CD0 *owner);

    /* Creates the gadget under task layer 11 (name inferred). */
    static void Instantiate(UnkPlacement_800C9CD0 *owner);

    /* Copies the owner's parameters, clamping the model type (name inferred). */
    void LoadParams(UnkPlacement_800C9CD0 *owner);
    /* Creates a new threat object at the owner's placement (name inferred). */
    void Spawn(UnkPlacement_800C9CD0 *owner);

    /* The parameters (name inferred). */
    const ThreatObjParams &GetParams() const {
        return m_params;
    }

    static const char *s_taskName;
    /* The model types of the loaded stage (name inferred). */
    static const ThreatObjModelSet *s_modelSet;

    /* Member names are inferred. */
    /* 0x30 */ boost::shared_ptr<ThreatObj> m_object;
    /* 0x38 */ ThreatObjParams m_params;
};
SIZE_ASSERT(ThreatObjHolder, 0x40);

ThreatObj::ThreatObj(ThreatObjHolder *holder, UnkPlacement_800C9CD0 *owner)
    : m_holder(holder), m_state(0), m_health(2.0f), m_soundId(-1) {
    m_type = &ThreatObjHolder::s_modelSet->m_models[m_holder->GetParams().modelType];
    m_chara = fn_8015CD90(0x44, 4);
    m_model = fn_8015F800(1, "THREATOBJ.BIN");
    m_model->fn_80162A68(m_holder->GetParams().modelType, 0);
    fn_80162790(m_model.get());
    UnkSoundEntry_80208888 *entry = fn_80208848(fn_80012700(fn_800122D4(), 1, "THREATOBJ.BIN"), 2);
    if (entry != 0) {
        m_soundId = entry->m_soundId;
        m_soundTimer = fn_8040EC80(2.5f, 3.0f);
    }
    Reset(owner);
    RebuildTransform();
}

ThreatObj::~ThreatObj() {}

void ThreatObj::Tick(float dt) {
    switch (m_state) {
    case 0:
        if (IsPlayerInRange()) {
            fn_8016271C(m_model.get());
            m_model->fn_80160C28(m_holder->GetParams().modelType);
            m_model->fn_8015FF60();
            fn_80160A84(m_model.get(), m_holder->GetParams().modelType, m_type->m_openMotion);
            fn_80161FC8(m_model.get(), m_holder->GetParams().modelType, m_chara);
            {
                boost::shared_ptr<CharaColliAttack> collision =
                    fn_80161428(m_model.get(), m_holder->GetParams().modelType).lock();
                if (collision.get() != 0) {
                    collision->field_0x54 = 0;
                    collision->field_0x55 = 1;
                    collision->m_attackCallback =
                        AttackCallback(boost::bind(&ThreatObj::OnAttack, this, _1));
                }
            }
            m_state = 1;
            UnkSoundEntry_80208888 *entry =
                fn_80208848(fn_80012700(fn_800122D4(), 1, "THREATOBJ.BIN"), 0);
            if (entry != 0) {
                fn_801D54F4(fn_80053164(), entry->m_soundId, m_chara.get(), 0, 1);
            }
        }
        break;
    case 1: {
        boost::shared_ptr<_stMtnPlayWork> work =
            fn_801602B4(m_model.get(), m_holder->GetParams().modelType).lock();
        if (work.get() != 0 && fn_80416F28(work.get(), 0, 3, 1) != 0) {
            if (m_type->m_loopMotion != -1) {
                fn_80160A84(m_model.get(), m_holder->GetParams().modelType, m_type->m_loopMotion);
            } else {
                fn_80160480(m_model.get(), m_holder->GetParams().modelType);
            }
        }
        if (m_soundId != -1) {
            m_soundTimer -= dt;
            if (m_soundTimer < 0.0f) {
                fn_801D54F4(fn_80053164(), m_soundId, m_chara.get(), 0, 1);
                m_soundTimer = fn_8040EC80(2.5f, 3.0f);
            }
        }
        break;
    }
    }
    RebuildTransform();
    m_model->fn_80162FB0(dt);
}

void ThreatObj::ReloadPlacement(UnkPlacement_800C9CD0 *owner) {
    Reset(owner);
    RebuildTransform();
    m_model->fn_80162EF4();
    fn_802E0E84(&m_chara->m_position, &lbl_805F06D8, m_holder->GetParams().rangeRadius);
}

void ThreatObj::Reset(UnkPlacement_800C9CD0 *owner) {
    fn_800C9F14(owner, &m_chara->m_position);
    fn_800C9ED4(owner, &m_chara->m_rotation);
}

void ThreatObj::RebuildTransform() {
    Mtx34f matrix;

    fn_8040C148(&matrix, &lbl_805E0720);
    fn_8040B59C(&matrix, &m_chara->m_position);
    fn_8040B848(&matrix, m_chara->m_rotation.y);
    fn_8040B884(&matrix, m_chara->m_rotation.x);
    fn_8040B80C(&matrix, m_chara->m_rotation.z);
    m_model->fn_801633DC(&matrix);
}

bool ThreatObj::IsPlayerInRange() {
    boost::shared_ptr<CharInfo> player = fn_800A8B74(fn_80074194(), lbl_805EE588).lock();
    if (player.get() != 0) {
        Vec3f offset;
        fn_800091EC(&offset, &(const Vec3f &)((VectorSubtractFn)fn_8040CD74)(
                                 &player->m_position, &m_chara->m_position));
        if (fn_8040CF50(&offset) < m_holder->GetParams().rangeRadius) {
            return true;
        }
    }
    return false;
}

void ThreatObj::OnAttack(AttackCallbackParam &param) {
    if (m_state == 1) {
        m_health -= param.m_damage;
        if (m_health < 0.0f || fn_8000B044(m_health)) {
            {
                UnkDebrisParam_8015F0B8 debris(m_chara, 0);
                debris.model = (s32)fn_80012700(fn_800122D4(), 1, m_type->m_debrisFile);
                debris.field_0x40 = m_chara->m_position.y;
                debris.scale = 1.0f;
                debris.field_0x4A = 1;
                debris.field_0x54 = 1;
                fn_8015D93C(&debris);
            }
            {
                boost::shared_ptr<CharInfo> attacker = fn_8015D570(param);
                if (attacker.get() != 0) {
                    fn_8020642C(attacker, 0x23);
                }
            }
            param.m_result = 3;
            UnkSoundEntry_80208888 *entry =
                fn_80208848(fn_80012700(fn_800122D4(), 1, "THREATOBJ.BIN"), 1);
            if (entry != 0) {
                fn_801D54F4(fn_80053164(), entry->m_soundId, m_chara.get(), 0, 1);
            }
            m_state = 2;
        } else {
            param.m_result = 1;
        }
    } else {
        param.m_result = 8;
    }
}

const char *ThreatObjHolder::s_taskName = "ThreatObjHolder";
const ThreatObjModelSet *ThreatObjHolder::s_modelSet;

ThreatObjHolder::ThreatObjHolder(Task *parent, UnkPlacement_800C9CD0 *owner) : Gadget(parent, owner) {
    m_name = s_taskName;
    LoadParams(owner);
}

ThreatObjHolder::~ThreatObjHolder() {}

extern "C" void fn_8013F274(void) {
    fn_8006B474("THREATOBJ.BIN", 1);
    ThreatObjHolder::s_modelSet = &lbl_804C027C[lbl_8057D6A0.m_stage != 9 ? 1 : 0];
}

extern "C" void fn_8013F268(void) {
    ThreatObjHolder::s_modelSet = 0;
}

void ThreatObjHolder::Instantiate(UnkPlacement_800C9CD0 *owner) {
    ThreatObjHolder *holder = new ThreatObjHolder(fn_801EB304(fn_8001C4F8(), 11), owner);
    holder->Spawn(owner);
}

void ThreatObjHolder::Behave(float dt) {
    if (m_object.get() != 0) {
        if (m_object->m_state == 2) {
            m_object.reset();
            fn_800C9B34(m_owner);
            m_flags |= 1;
        } else {
            m_object->Tick(dt);
        }
    }
}

void ThreatObjHolder::ReloadPlacement(UnkPlacement_800C9CD0 *owner) {
    ThreatObjParams previous = m_params;
    LoadParams(owner);
    if (previous.modelType != m_params.modelType) {
        Spawn(owner);
    }
    m_object->ReloadPlacement(owner);
}

void ThreatObjHolder::LoadParams(UnkPlacement_800C9CD0 *owner) {
    fn_800C9DBC(owner, &m_params, sizeof(m_params));
    if (s_modelSet != 0 && m_params.modelType >= s_modelSet->m_count) {
        m_params.modelType = s_modelSet->m_count - 1;
        fn_800C9D68(owner, &m_params, sizeof(m_params));
    }
    m_params.rangeRadius = 10.0f * m_params.rangeRadius;
}

void ThreatObjHolder::Spawn(UnkPlacement_800C9CD0 *owner) {
    m_object = boost::shared_ptr<ThreatObj>(new ThreatObj(this, owner));
}

static GadgetParamDesc lbl_8052938C[] = {
    {2, "MODEL_TYPE", 0, 0, lbl_805EEFDC - 1, 0.0f, 0.0f, 0.0f, fn_800CBC20, 0},
    {4, "RANGE_RADIUS m", 0, 0, 0, 5.0f, 0.0f, 65536.0f, fn_800CBC20, 0},
    {0},
};

GadgetClassDesc lbl_80529404 = {
    "ThreatObj", fn_8013F274, fn_8013F268, ThreatObjHolder::Instantiate, 0, 0, 0xBBC, 1, 0, 10, lbl_8052938C,
};
