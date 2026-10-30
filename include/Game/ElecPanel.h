#ifndef GAME_ELECPANEL_H
#define GAME_ELECPANEL_H

#include "Game/Gadget.h"
#include "Game/MathTypes.h"
#include "Game/SetAdapterState.h"
#include "Game/Task.h"
#include "boost/shared_ptr.hpp"
#include "boost/weak_ptr.hpp"
#include "size_assert.h"
#include <map>
#include <set>

/* ElecPanel, ElecPanelData and ElecPanelManagerTask (class names from RTTI): the ELEC_PANEL
 * gadgets of the electric circuit area (Game/ElecPanel.cpp), their state kept by the owner, and the
 * task that watches them (Game/ElecPanelManagerTask.cpp). A panel shows one of four colours and
 * steps to the next one when a character steps on it or attacks it; a "move" panel slides along its
 * move vector once its circuit's switch is on. The manager turns a circuit's switch on once every
 * panel of the circuit shows the same colour. The panels' shared tables are the singleton
 * fn_801E6114 returns (Game/UnkSystemManager_801E2128.cpp makes it on first use). Member names are
 * inferred.
 *
 * A unit that includes this header defines MSL_MULTIMAP_TREE before it (the tables' multimap). */

class CharInfo;
class GadgetCharCollision;
class GadgetCharInfo;
class GadgetModelInfo;
struct ColliDetectArgs;
namespace SonicteamUSA {
namespace System {
namespace CharaColli {
struct AttackCallbackParam;
} // namespace CharaColli
} // namespace System
} // namespace SonicteamUSA
class ElecPanel;
class ElecPanelManagerTask;

/* The panels' tables (class name unknown): the owners of the stage's ELEC_PANEL records by circuit
 * id, the live panels by owner, the circuit ids whose panels have changed colour, and the manager
 * task. Method names are inferred. */
class UnkElecPanelTables_801E6114 {
  public:
    UnkElecPanelTables_801E6114();
    ~UnkElecPanelTables_801E6114();

    /* Turns on the switch of every circuit in m_changed whose panels all show one colour, then
     * forgets the circuits. */
    void Update();
    /* Collects the owners of the stage's ELEC_PANEL records (gadget type 0x7D2). */
    void Load();
    /* Empties the tables. */
    void Clear();
    /* Marks circuit id as changed. */
    void SetChanged(u8 id);
    /* Adds owner under its circuit id (not called in the game). */
    void AddOwner(UnkPlacement_800C9CD0 *owner);
    /* Forgets owner. */
    void RemoveOwner(UnkPlacement_800C9CD0 *owner);
    /* Adds and removes a live panel. */
    void AddPanel(ElecPanel *panel);
    void RemovePanel(ElecPanel *panel);

    /* 0x00 */ std::multimap<unsigned char, UnkPlacement_800C9CD0 *> m_owners;
    /* 0x10 */ std::map<UnkPlacement_800C9CD0 *, ElecPanel *> m_panels;
    /* 0x20 */ std::set<int> m_changed;
    /* 0x2C */ ElecPanelManagerTask *m_task;
};
SIZE_ASSERT(UnkElecPanelTables_801E6114, 0x30);

extern "C" {
/* The tables (made on first use). */
UnkElecPanelTables_801E6114 *fn_801E6114(void);
}

/* ElecPanelData (class name from RTTI): the colour a switch panel shows, kept by the owner so
 * that it survives the gadget being re-created. */
class ElecPanelData : public SetAdapterState {
  public:
    /* 0x4 */ s32 m_color;
};
SIZE_ASSERT(ElecPanelData, 0x8);

/* The parameters (names from the parameter table). */
struct ElecPanelParams {
    ElecPanelParams() : color(0), type(0), accel(0.0f), moveX(0.0f), moveY(0.0f), moveZ(0.0f) {}

    /* 0x00 */ s32 color; /* 0 red, 1 yellow, 2 green, 3 blue */
    /* 0x04 */ s32 type;  /* 0 switch, 1 move */
    /* 0x08 */ float accel;
    /* 0x0C */ float moveX;
    /* 0x10 */ float moveY;
    /* 0x14 */ float moveZ;
};
SIZE_ASSERT(ElecPanelParams, 0x18);

class ElecPanel : public Gadget {
  public:
    ElecPanel(Task *parent, UnkPlacement_800C9CD0 *owner);
    virtual ~ElecPanel();
    virtual void Behave(float dt);
    virtual void ReloadPlacement(UnkPlacement_800C9CD0 *owner);

    /* Creates the gadget under task layer 11 (name inferred). */
    static void Instantiate(UnkPlacement_800C9CD0 *owner);

    /* Method names are inferred. */
    /* Copies the owner's position, rotation, parameters and circuit id. */
    void ReadPlacement(UnkPlacement_800C9CD0 *owner);
    /* Places the model at the character info's position and rotation. */
    void RebuildTransform();
    /* Shows the next colour and marks the circuit as changed. */
    void NextColor();
    /* The callbacks of the collision handle and of the model's attack collision. */
    void OnColliDetect(ColliDetectArgs &args);
    void OnColliLeave(ColliDetectArgs &args);
    void OnAttack(SonicteamUSA::System::CharaColli::AttackCallbackParam &param);
    /* The panel's state kept by the owner (inline in Game/ElecPanel.cpp). */
    ElecPanelData *GetData();

    static const char *s_taskName;

    /* 0x30 */ s32 m_type; /* the type parameter */
    /* 0x34 */ boost::shared_ptr<GadgetCharInfo> m_chara;
    /* 0x3C */ GadgetModelInfo *m_model;
    /* 0x40 */ boost::shared_ptr<GadgetCharCollision> m_collision;
    /* 0x48 */ ElecPanelParams m_params;
    /* 0x60 */ s32 m_moveState; /* 1 set up the move, 0 wait for the switch, 2 moving, 3 done */
    /* 0x64 */ Vec3f m_velocity;
    /* 0x70 */ Vec3f m_acceleration;
    /* 0x7C */ float m_distance;
    /* 0x80 */ u8 m_circuitId;
    /* 0x84 */ s32 m_color;
    /* 0x88 */ float m_timer;   /* while positive, attacks do not change the colour */
    /* 0x8C */ float m_fade;    /* the material's brightness once the circuit is on */
    /* 0x90 */ u8 m_changed;    /* the colour changed while the characters were on the panel */
    /* 0x94 */ std::set<boost::weak_ptr<CharInfo> > m_inside;
};
SIZE_ASSERT(ElecPanel, 0xA0);

/* ElecPanelManagerTask (class name from RTTI): updates the tables every frame. */
class ElecPanelManagerTask : public Task {
  public:
    ElecPanelManagerTask(Task *parent);
    virtual ~ElecPanelManagerTask();
    virtual void Tick(float dt);
};
SIZE_ASSERT(ElecPanelManagerTask, 0x28);

#endif
