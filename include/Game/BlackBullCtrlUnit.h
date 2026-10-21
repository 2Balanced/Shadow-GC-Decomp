#ifndef GAME_BLACKBULLCTRLUNIT_H
#define GAME_BLACKBULLCTRLUNIT_H

/* The Black Bull boss's state and the bases of its control units (class names from RTTI). The
 * boss has two stages, 0210 and 0412, each with its own BossVarParam subclass and units. Only the
 * members the units use are mapped; member names and meanings are inferred. */

#include "Game/BossCtrlUnit.h"
#include <dolphin/types.h>

/* BlackBullVarParam (class name from RTTI; vtable at 0x805474AC): the state the Black Bull stages
 * share. Offsets are from the start of the object. */
class BlackBullVarParam : public BossVarParam {
  public:
    BlackBullVarParam();
    virtual ~BlackBullVarParam();

    /* 0x074 */ float field_0x74;
    /* 0x078 */ float field_0x78;
    /* 0x07C */ float m_arriveDistance; /* a move ends this close to its destination */
    /* 0x080 */ float field_0x80;
    /* 0x084 */ float field_0x84;
    /* 0x088 */ float m_turnMotionSpeed; /* in degrees a second: turning faster plays a turn */
    /* 0x08C */ float m_waitTime;
    /* 0x090 */ float m_stunTime;
    /* 0x094 */ s32 m_score;
    /* 0x098 */ s32 m_comboLimit;
    /* 0x09C */ float field_0x9C;
    /* 0x0A0 */ float field_0xA0;
    /* 0x0A4 */ float field_0xA4;
    /* 0x0A8 */ float field_0xA8;
    /* 0x0AC */ float field_0xAC;
    /* 0x0B0 */ float field_0xB0;
    /* 0x0B4 */ float field_0xB4;
    /* 0x0B8 */ float field_0xB8;
    /* 0x0BC */ float field_0xBC;
    /* 0x0C0 */ float field_0xC0;
    /* 0x0C4 */ s32 m_waveCount;
    /* 0x0C8 */ float m_waveBlend;
    /* 0x0CC */ float field_0xCC;
    /* 0x0D0 */ float m_fireSpeed;
    /* 0x0D4 */ float field_0xD4;
    /* 0x0D8 */ float field_0xD8;
    /* 0x0DC */ float field_0xDC;
    /* 0x0E0 */ float m_turnSpeedMax; /* in degrees a second */
    /* 0x0E4 */ float field_0xE4;
    /* 0x0E8 */ float m_damageLimit;
    /* 0x0EC */ float field_0xEC;
    /* 0x0F0 */ float field_0xF0;
    /* 0x0F4 */ float m_cameraNear;
    /* 0x0F8 */ float m_cameraFar;
    /* 0x0FC */ Vec3f m_destination;
    /* 0x108 */ Vec3f m_position;
    /* 0x114 */ float m_destinationDistance;
    /* 0x118 */ float m_turnSpeed; /* in radians a second */
    /* 0x11C */ float m_angle;
    /* 0x120 */ u8 m_motionLocked;
    /* 0x121 */ u8 m_motionLocked2;
    /* 0x122 */ u8 field_0x122;
    /* 0x123 */ u8 m_moving;
    /* 0x124 */ u8 m_dizzy;
    /* 0x125 */ u8 m_dizzyEnded;
    /* 0x126 */ u8 field_0x126[0x138 - 0x126];
    /* 0x138 */ Vec3f m_testDestination;
    /* 0x144 */ u8 field_0x144[0x154 - 0x144];
    /* 0x154 */ float m_height; /* the player's height above the boss (meaning inferred) */
    /* 0x158 */ u32 field_0x158;
    /* 0x15C */ float field_0x15C;
    /* 0x160 */ float m_targetAngle;
    /* 0x164 */ u32 field_0x164;
    /* 0x168 */ u8 field_0x168;
    /* 0x169 */ u8 m_hit;
    /* 0x16C */ float field_0x16C;
    /* 0x170 */ u8 field_0x170;
    /* 0x171 */ u8 field_0x171;
    /* 0x172 */ u8 m_arrived;
    /* 0x174 */ float m_damage;
    /* 0x178 */ float field_0x178;
    /* 0x17C */ s32 m_comboCount;
    /* 0x180 */ s32 m_sound;
    /* 0x184 */ u32 m_attribute; /* set by the motion's attribute callback */
    /* 0x188 */ u32 field_0x188;
};

/* BlackBull0412VarParam (class name from RTTI; vtable at 0x805484B8): the state of the 0412
 * stage. */
class BlackBull0412VarParam : public BlackBullVarParam {
  public:
    virtual ~BlackBull0412VarParam();
};

/* BlackBullCtrlUnit (class name from RTTI; vtable at 0x805471D8): the base of the boss's units. */
class BlackBullCtrlUnit : public virtual BossCtrlUnit {
  public:
    BlackBullCtrlUnit(BlackBullVarParam *owner);
    virtual ~BlackBullCtrlUnit();

    /* 0x8 */ BlackBullVarParam *m_owner;
};

/* BlackBull0412CtrlUnit (class name from RTTI; vtable at 0x8054847C): the base of the 0412 stage's
 * units. Its constructor (0x80243A48) and destructor (0x802439B8) are out of line. */
class BlackBull0412CtrlUnit : public BlackBullCtrlUnit {
  public:
    BlackBull0412CtrlUnit(BlackBull0412VarParam *owner);
    virtual ~BlackBull0412CtrlUnit();

    /* 0xC */ BlackBull0412VarParam *m_owner;
};

#endif
