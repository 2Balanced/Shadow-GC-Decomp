#ifndef GAME_IAUDIO_H
#define GAME_IAUDIO_H

/* IAudio (class name from RTTI; vtable at 0x8053CE40): the sound back end's interface. AudioGCN
 * (class name from RTTI; vtable at 0x8053E324, code at 0x801F2A98 and around) implements it; the
 * sound manager (Game/AudioManagerTask.cpp) creates one and drives it. Three slots have empty
 * bodies (defined in Game/AudioManagerTask.cpp); the others are pure. Slot names are not known:
 * each is named after its vtable offset, and the parameters follow the calls the game makes. */

#include <dolphin/types.h>

class IAudio {
  public:
    virtual ~IAudio() {}
    virtual void vf_0x0C();
    virtual void vf_0x10();
    /* Called once a frame after the channels are updated (name inferred). */
    virtual void Update();
    virtual void vf_0x18() = 0;
    virtual void vf_0x1C(int group, int arg2) = 0;
    virtual void vf_0x20(int group) = 0;
    virtual void vf_0x24() = 0;
    virtual void vf_0x28() = 0;
    virtual void vf_0x2C() = 0;
    virtual void vf_0x30(u8 arg1, u8 arg2) = 0;
    virtual void vf_0x34() = 0;
    virtual void vf_0x38() = 0;
    virtual void vf_0x3C() = 0;
    virtual void vf_0x40() = 0;
    virtual void vf_0x44() = 0;
    virtual void vf_0x48(u8 arg1, u8 arg2) = 0;
    virtual void vf_0x4C() = 0;
    virtual void vf_0x50() = 0;
    virtual void vf_0x54() = 0;
    virtual void vf_0x58(int mode) = 0;
    virtual void vf_0x5C(u8 arg1, u8 arg2) = 0;
    virtual void vf_0x60() = 0;
    virtual void vf_0x64(u8 arg1, u8 arg2, u8 arg3) = 0;
    virtual void vf_0x68() = 0;
    virtual void vf_0x6C() = 0;
};

/* AudioGCN (class name from RTTI): the GameCube back end. Only what creating one needs is declared
 * here; its overrides are defined with its code. */
class AudioGCN : public IAudio {
  public:
    AudioGCN();
    virtual ~AudioGCN();
    virtual void vf_0x18();
    virtual void vf_0x1C(int group, int arg2);
    virtual void vf_0x20(int group);
    virtual void vf_0x24();
    virtual void vf_0x28();
    virtual void vf_0x2C();
    virtual void vf_0x30(u8 arg1, u8 arg2);
    virtual void vf_0x34();
    virtual void vf_0x38();
    virtual void vf_0x3C();
    virtual void vf_0x40();
    virtual void vf_0x44();
    virtual void vf_0x48(u8 arg1, u8 arg2);
    virtual void vf_0x4C();
    virtual void vf_0x50();
    virtual void vf_0x54();
    virtual void vf_0x58(int mode);
    virtual void vf_0x5C(u8 arg1, u8 arg2);
    virtual void vf_0x60();
    virtual void vf_0x64(u8 arg1, u8 arg2, u8 arg3);
    virtual void vf_0x68();
    virtual void vf_0x6C();

    /* 0x04 */ u8 field_0x4[0x2C - 0x4];
};

#endif
