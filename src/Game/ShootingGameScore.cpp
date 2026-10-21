/* ShootingGame::Score (class name from RTTI): the SHOOTING_PANEL gadget of the Circus Park shooting
 * game, the panel (SHOOTINGGAMESCORE.BIN) that shows the score of the shooting game entry "Id"
 * (the ShootingGame manager unit keeps the table). When the entry starts the panel plays its "hit"
 * motion; once that ends it switches to its "act" model, whose sixteen lamps light up for the
 * entry's score (all of them in a third colour when the score is negative). Hitting the panel while
 * the entry runs restarts it, and an attack with collision flag bit 4 clears it; when the entry ends
 * the panel fades out, reports the last hitter to the table and finishes. File name inferred from
 * the class and its model file. Method names marked "(name inferred)" are not original. Definitions
 * are in reverse address order (deferred inlining emits functions last-defined first). */
#include <boost/bind.hpp>
#include <boost/function.hpp>

#include "Game/CharInfo.h"
#include "Game/CharaColliAttack.h"
#include "Game/Gadget.h"
#include "Game/GadgetCharInfo.h"
#include "Game/GadgetModelInfo.h"
#include "Game/GadgetClassDesc.h"
#include "Game/ShootingGameTable.h"
#include "Game/FileQueue.h"
#include "boost/shared_ptr.hpp"
#include "boost/weak_ptr.hpp"
#include "size_assert.h"

using SonicteamUSA::System::CharaColli::AttackCallback;
using SonicteamUSA::System::CharaColli::AttackCallbackParam;
using SonicteamUSA::System::CharaColli::CharaColliAttack;

/* The motion work of a model (struct name from the RTTI of its shared_ptr control block). */
struct _stMtnPlayWork;

/* Four colour bytes (see ElecBarrier.cpp). */
struct UnkColor_8040D2D8 {
    /* 0x0 */ u8 r;
    /* 0x1 */ u8 g;
    /* 0x2 */ u8 b;
    /* 0x3 */ u8 a;
};
SIZE_ASSERT(UnkColor_8040D2D8, 0x4);

/* The material animation fn_80162298 returns (see ElecCoreProgram.cpp). */
struct UnkMaterialAnim_801621F0 {
    /* 0x0 */ u8 field_0x0[0x8];
    /* 0x8 */ UnkColor_8040D2D8 color;
};

using ShootingGame::BalloonPosition;
using ShootingGame::BalloonTable;

extern "C" {
/* The manager's table (see ShootingGameManager.cpp). */
extern BalloonTable *lbl_805EEFC0;
/* The table's accessors (see ShootingGameManager.cpp). */
s32 fn_8013D794(BalloonTable *table, int id);
void fn_8013D72C(BalloonTable *table, int id);
void fn_8013D5BC(BalloonTable *table, int id, boost::weak_ptr<CharInfo> character);
void fn_8013D568(BalloonTable *table, int id);
void fn_8013D524(BalloonTable *table, int id);
bool fn_8013D4D8(BalloonTable *table, int id);
void fn_8013D49C(BalloonTable *table, int id, BalloonPosition *position);
float fn_8013D474(BalloonTable *table, int id);

/* A 32-bit flag word: fn_80014E04 constructs it clear, fn_80014E1C sets bit and fn_80014E34 tests
 * bit (see ShootingGameManager.cpp). */
void fn_80014E04(u32 *flags);
void fn_80014E1C(u32 *flags, int bit);
bool fn_80014E34(u32 *flags, int bit);

/* Whether the owner went away, and whether it is out of the game's range (behaviour inferred; see
 * Coaster.cpp). */
bool fn_800C9A64(UnkPlacement_800C9CD0 *owner);
bool fn_800C996C(UnkPlacement_800C9CD0 *owner);
/* Marks the gadget finished (behaviour inferred). */
void fn_8015A794(Gadget *gadget);
/* Builds the character info's matrix (behaviour inferred). */
void fn_8015CEA0(GadgetCharInfo *charInfo, Mtx34f *matrix);
/* Loads model fileName for a gadget (behaviour inferred; arg0's meaning is unknown). */
boost::shared_ptr<GadgetModelInfo> fn_8015F800(int arg0, const char *fileName);

/* GadgetModelInfo methods that are not declared in its header yet (see ArkCannon.cpp, Stick.cpp,
 * CaptureCage.cpp and ElecBarrier.cpp). */
bool fn_80161FC8(GadgetModelInfo *model, int variant, const boost::weak_ptr<CharInfo> &info);
boost::weak_ptr<CharaColliAttack> fn_80161428(GadgetModelInfo *model, int variant);
void fn_801609F8(GadgetModelInfo *model, const char *motionName);
boost::weak_ptr<_stMtnPlayWork> fn_801601A0(GadgetModelInfo *model, const char *motionName);
void fn_8016268C(GadgetModelInfo *model, float arg1);
void fn_8016232C(GadgetModelInfo *model);
int fn_80162424(GadgetModelInfo *model, int id);
UnkMaterialAnim_801621F0 *fn_80162298(GadgetModelInfo *model, int id);

/* Motion work queries (see Stick.cpp and DevilDoomCtrlUnit.cpp). */
int fn_80416F28(_stMtnPlayWork *work, int arg1, int arg2, int arg3);
void fn_80415D40(_stMtnPlayWork *work, int arg1, BalloonPosition *info);
void fn_80416E20(_stMtnPlayWork *work, int arg1, float frame);

/* The lamp colours: off, lit, and lit for a negative score (names inferred; set up at 0x8040D304). */
extern UnkColor_8040D2D8 lbl_805F1360;
extern UnkColor_8040D2D8 lbl_805F1370;
extern UnkColor_8040D2D8 lbl_805F137C;
}

/* True when the manager's entry id is in state (inlined in the original; name inferred; see
 * BalloonGenerator.cpp). */
inline bool IsEntryState(BalloonTable *table, int id, int state) {
    if (id >= 0 && id < 0x100) {
        return table->m_entries[id].m_state == state;
    }
    return false;
}

namespace ShootingGame {

/* The flag word at 0x98 (struct name unknown). */
struct ScoreFlags {
    ScoreFlags() {
        fn_80014E04(&m_bits);
    }
    void Set(int bit) {
        fn_80014E1C(&m_bits, bit);
    }
    bool Test(int bit) {
        return fn_80014E34(&m_bits, bit);
    }

    u32 m_bits; /* bit 0: the panel was hit while its entry ran */
};

class Score : public Gadget {
  public:
    Score(Task *parent, UnkPlacement_800C9CD0 *owner);
    virtual ~Score();
    virtual void Behave(float dt);
    virtual void ReloadPlacement(UnkPlacement_800C9CD0 *owner);
    virtual bool HasEnded();

    /* Creates the panel under task layer 11 (name inferred). */
    static void Instantiate(UnkPlacement_800C9CD0 *owner);

    /* Copies the owner's position, rotation and parameters and restarts (name inferred). */
    void LoadFromOwner(UnkPlacement_800C9CD0 *owner);
    /* Places the model (name inferred). */
    void RebuildTransform();
    /* Clears the model's motion and state (name inferred). */
    void ResetModel();
    /* Shows model variant: 0 idle, 1 "hit", 2 "act" with its lamps (name inferred). */
    void SetVariant(int variant);
    /* The attack callback of the model's collision (name inferred). */
    void OnAttack(AttackCallbackParam &param);

    static const char *s_taskName;
    static const char *s_fileName;

    /* Member names are inferred. */
    /* 0x30 */ boost::shared_ptr<GadgetCharInfo> m_chara;
    /* 0x38 */ boost::shared_ptr<GadgetModelInfo> m_model;
    /* 0x40 */ boost::weak_ptr<CharInfo> m_hitter;
    /* 0x48 */ s32 m_id; /* the parameter "Id" */
    /* 0x4C */ s32 m_state;
    /* 0x50 */ float m_timer;
    /* 0x54 */ s32 m_score;
    /* 0x58 */ UnkMaterialAnim_801621F0 *m_lamps[16];
    /* 0x98 */ ScoreFlags m_hitFlags;
    /* 0x9C */ u32 pad_0x9C;
};
SIZE_ASSERT(Score, 0xA0);

const char *Score::s_taskName = "ShootingGame::Score";
const char *Score::s_fileName = "SHOOTINGGAMESCORE.BIN";

Score::Score(Task *parent, UnkPlacement_800C9CD0 *owner) : Gadget(parent, owner), m_id(-1) {
    m_name = s_taskName;
    m_chara = fn_8015CB88(0x49, 4, "Score");
    LoadFromOwner(owner);
    m_model = fn_8015F800(1, s_fileName);
    if (lbl_805EEFC0 != 0 && IsEntryState(lbl_805EEFC0, m_id, 1)) {
        if (!fn_8013D4D8(lbl_805EEFC0, m_id)) {
            SetVariant(1);
            m_model->fn_80160BA0("hit");
            m_model->fn_8015FF60();
            fn_801609F8(m_model.get(), "hit");
            boost::weak_ptr<_stMtnPlayWork> work = fn_801601A0(m_model.get(), "hit");
            if (boost::shared_ptr<_stMtnPlayWork> motion = work.lock()) {
                float frame = fn_8013D474(lbl_805EEFC0, m_id);
                fn_80416E20(motion.get(), 0, frame);
            }
            m_state = 2;
        } else {
            SetVariant(2);
            m_model->fn_80160BA0("act");
            m_model->fn_8015FF60();
            fn_801609F8(m_model.get(), "act");
            boost::weak_ptr<_stMtnPlayWork> work = fn_801601A0(m_model.get(), "act");
            if (boost::shared_ptr<_stMtnPlayWork> motion = work.lock()) {
                float frame = fn_8013D474(lbl_805EEFC0, m_id);
                fn_80416E20(motion.get(), 0, frame);
            }
            m_state = 3;
        }
    } else {
        SetVariant(0);
    }
    RebuildTransform();
}

Score::~Score() {}

void Score::Behave(float dt) {
    m_model->fn_80162FB0(dt);
    switch (m_state) {
    case 0:
        m_state = 1;
        break;
    case 1:
        if (lbl_805EEFC0 != 0) {
            if (IsEntryState(lbl_805EEFC0, m_id, 1)) {
                ResetModel();
                SetVariant(1);
                m_model->fn_80160BA0("hit");
                m_model->fn_8015FF60();
                fn_801609F8(m_model.get(), "hit");
                RebuildTransform();
                m_state = 2;
            } else if (IsEntryState(lbl_805EEFC0, m_id, 2)) {
                m_state = 5;
            }
        }
        break;
    case 2: {
        if (lbl_805EEFC0 != 0 && IsEntryState(lbl_805EEFC0, m_id, 2)) {
            fn_8016268C(m_model.get(), -1.0f);
            m_timer = 2.0f;
            m_state = 5;
            break;
        }
        boost::weak_ptr<_stMtnPlayWork> work = fn_801601A0(m_model.get(), "hit");
        if (!work.expired()) {
            boost::shared_ptr<_stMtnPlayWork> motion = work.lock();
            if (fn_80416F28(motion.get(), 0, 3, 1) != 0) {
                m_state = 3;
                ResetModel();
                SetVariant(2);
                m_model->fn_80160BA0("act");
                m_model->fn_8015FF60();
                fn_801609F8(m_model.get(), "act");
                fn_8013D524(lbl_805EEFC0, m_id);
            }
            if (m_hitFlags.Test(0)) {
                BalloonPosition position;
                fn_80415D40(motion.get(), 0, &position);
                fn_8013D49C(lbl_805EEFC0, m_id, &position);
            }
        }
        break;
    }
    case 3: {
        if (lbl_805EEFC0 != 0 && IsEntryState(lbl_805EEFC0, m_id, 2)) {
            fn_8016268C(m_model.get(), -1.0f);
            m_timer = 2.0f;
            m_state = 5;
            break;
        }
        boost::weak_ptr<_stMtnPlayWork> work = fn_801601A0(m_model.get(), "act");
        if (!work.expired()) {
            boost::shared_ptr<_stMtnPlayWork> motion = work.lock();
            if (fn_80416F28(motion.get(), 0, 3, 1) != 0) {
                m_state = 5;
                m_timer = 2.0f;
                fn_8016268C(m_model.get(), -1.0f);
            }
            if (m_hitFlags.Test(0)) {
                BalloonPosition position;
                fn_80415D40(motion.get(), 0, &position);
                fn_8013D49C(lbl_805EEFC0, m_id, &position);
            }
        }
        s32 score = fn_8013D794(lbl_805EEFC0, m_id);
        if (score != m_score) {
            m_score = score;
            if (m_score < 0) {
                for (int i = 0; i < 16; i++) {
                    m_lamps[i]->color = lbl_805F137C;
                }
            } else {
                int lit = m_score;
                if (lit > 16) {
                    lit = 16;
                }
                for (int i = 0; i < lit; i++) {
                    m_lamps[i]->color = lbl_805F1370;
                }
                for (int i = lit; i < 16; i++) {
                    m_lamps[i]->color = lbl_805F1360;
                }
            }
        }
        break;
    }
    case 5:
        m_timer -= dt;
        if (m_timer <= 0.0f) {
            m_state = 6;
        }
        break;
    case 6:
        if (m_hitFlags.Test(0) && lbl_805EEFC0 != 0) {
            fn_8013D5BC(lbl_805EEFC0, m_id, m_hitter);
        }
        fn_8015A794(this);
        m_flags |= 1;
        break;
    }
    RebuildTransform();
}

void Score::ReloadPlacement(UnkPlacement_800C9CD0 *owner) {
    LoadFromOwner(owner);
    RebuildTransform();
    m_model->fn_80162EF4();
    m_model->fn_8016108C();
}

void Score::LoadFromOwner(UnkPlacement_800C9CD0 *owner) {
    fn_800C9F14(owner, &m_chara->m_position);
    fn_800C9ED4(owner, &m_chara->m_rotation);
    fn_800C9DBC(owner, &m_id, sizeof(m_id));
    m_state = 0;
    m_timer = 0.0f;
    m_score = -1;
    for (int i = 0; i < 16; i++) {
        m_lamps[i] = 0;
    }
}

void Score::RebuildTransform() {
    Mtx34f matrix;

    fn_8015CEA0(m_chara.get(), &matrix);
    m_model->fn_801633DC(&matrix);
}

void Score::OnAttack(AttackCallbackParam &param) {
    param.m_result = 5;
    if (m_state == 1) {
        if (boost::shared_ptr<CharaColliAttack> attacker = param.m_attacker.lock()) {
            if (attacker->m_flags & 0x10) {
                param.m_result = 3;
                if (lbl_805EEFC0 != 0) {
                    fn_8013D568(lbl_805EEFC0, m_id);
                }
                return;
            }
            if (attacker->m_flags & 2) {
                param.m_result = 5;
                return;
            }
        }
        param.m_result = 1;
        {
            boost::weak_ptr<CharaColliAttack> attack = param.m_attacker;
            if (boost::shared_ptr<CharaColliAttack> attacker = attack.lock()) {
                m_hitter = attacker->m_owner;
            }
        }
        if (lbl_805EEFC0 != 0 && !IsEntryState(lbl_805EEFC0, m_id, 1)) {
            fn_8013D72C(lbl_805EEFC0, m_id);
            m_hitFlags.Set(0);
        }
    } else if (m_state == 2 || m_state == 3) {
        if (boost::shared_ptr<CharaColliAttack> attacker = param.m_attacker.lock()) {
            if (attacker->m_flags & 0x10) {
                param.m_result = 3;
                if (lbl_805EEFC0 != 0) {
                    fn_8013D568(lbl_805EEFC0, m_id);
                }
                return;
            }
        }
    }
}

void Score::SetVariant(int variant) {
    m_model->fn_80162A68(variant, 0);
    fn_80161FC8(m_model.get(), 0, m_chara);
    boost::weak_ptr<CharaColliAttack> attack = fn_80161428(m_model.get(), 0);
    if (boost::shared_ptr<CharaColliAttack> collision = attack.lock()) {
        collision->field_0x54 = 0;
        collision->field_0x55 = 1;
        collision->m_attackCallback = AttackCallback(boost::bind(&Score::OnAttack, this, _1));
    }
    if (variant == 2) {
        for (int i = 0; i < 16; i++) {
            fn_80162424(m_model.get(), i);
            m_lamps[i] = fn_80162298(m_model.get(), i);
        }
    }
}

void Score::ResetModel() {
    fn_8016232C(m_model.get());
    m_model->fn_80161880();
    m_model->fn_801604FC();
    m_model->fn_80162804();
}

bool Score::HasEnded() {
    if (m_owner != 0) {
        if (fn_800C9A64(m_owner)) {
            m_flags |= 1;
            return true;
        }
        if (m_hitFlags.Test(0) && lbl_805EEFC0 != 0 && IsEntryState(lbl_805EEFC0, m_id, 1)) {
            return false;
        }
        if (fn_800C996C(m_owner)) {
            m_flags |= 1;
            return true;
        }
    }
    return false;
}

} // namespace ShootingGame

extern "C" void fn_8013D91C(void) {
    fn_8006B474(ShootingGame::Score::s_fileName, 1);
}

extern "C" void fn_8013D918(void) {}

void ShootingGame::Score::Instantiate(UnkPlacement_800C9CD0 *owner) {
    Task *parent = fn_801EB304(fn_8001C4F8(), 11);
    new Score(parent, owner);
}

static GadgetParamDesc lbl_805292AC[] = {
    {1, "Id", 1, 1, 0xFF, 0.0f, 0.0f, 0.0f, fn_800CBC20, 0},
    {0},
};

GadgetClassDesc lbl_805292FC = {
    "SHOOTING_PANEL", fn_8013D91C, fn_8013D918, ShootingGame::Score::Instantiate, 0, 0, 0xC83, 1, 0, 10,
    lbl_805292AC,
};
