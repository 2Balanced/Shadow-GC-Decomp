#ifndef GAME_IAUDIO_H
#define GAME_IAUDIO_H

/* IAudio (class name from RTTI; vtable at 0x8053CE40): the sound back end's interface. AudioGCN
 * (class name from RTTI; vtable at 0x8053E324, Game/AudioGCN.cpp) implements it; the sound
 * manager (Game/AudioManagerTask.cpp) creates one and drives it. Three slots have empty bodies
 * (defined in Game/AudioManagerTask.cpp); the others are pure. Slot names are not known: each is
 * named after its vtable offset, and the parameters and return values follow the calls the game
 * makes and AudioGCN's bodies. The last slot is 0x68 (AudioGCN's vtable ends there). */

#include <dolphin/types.h>

class IAudio {
  public:
    virtual ~IAudio() {}
    virtual void vf_0x0C();
    virtual void vf_0x10();
    /* Called once a frame after the channels are updated (name inferred). */
    virtual void Update();
    /* Loads a sound bank file; whether it loaded (meaning inferred from AudioGCN). */
    virtual bool vf_0x18(const char *file) = 0;
    virtual bool vf_0x1C(int group, int arg2) = 0;
    virtual bool vf_0x20(int group) = 0;
    /* The channel calls below address the back end by a group (s8) and a voice within it (u8); see
     * Game/AudioChannel.cpp. AudioGCN sends them as MIDI-like messages (behaviour inferred). */
    virtual void vf_0x24(s8 bank, s8 program, u8 priority, s8 group, u8 voice, float range) = 0;
    virtual void vf_0x28() = 0;
    virtual void vf_0x2C() = 0;
    virtual void vf_0x30(s8 group, u8 voice) = 0;
    virtual void vf_0x34() = 0;
    virtual void vf_0x38(s8 volume, s8 group, u8 voice) = 0;
    virtual void vf_0x3C(s8 pan, s8 group, u8 voice) = 0;
    virtual void vf_0x40(s8 value, s8 group, u8 voice) = 0;
    virtual void vf_0x44(s8 surround, s8 group, u8 voice) = 0;
    virtual void vf_0x48(s8 group, u8 voice) = 0;
    virtual void vf_0x4C(s16 pitch, s8 group, u8 voice) = 0;
    virtual void vf_0x50(s8 group, u8 voice, s8 bank, s8 program) = 0;
    virtual void vf_0x54() = 0;
    virtual void vf_0x58(int mode) = 0;
    virtual void vf_0x5C(s8 group, u8 voice) = 0;
    virtual void vf_0x60() = 0;
    virtual void vf_0x64(s8 volume, s8 group, u8 voice) = 0;
    virtual void vf_0x68() = 0;
};

#ifdef AUDIOGCN_MEMBERS
/* AudioGCN's playing state (struct name unknown; member names inferred; Game/AudioGCN.cpp). */
struct UnkAudioState_801F1E24 {
    UnkAudioState_801F1E24() : m_playing(0), m_current(0), m_previous(0) {}

    /* Starts playing (0x801F1E24) and keeps the last value (0x801F1E88). */
    void Start();
    void Shift();
    /* Stops playing (inline in the game). */
    void Stop();

    /* 0x0 */ u8 m_playing;
    /* 0x1 */ u8 m_current;
    /* 0x2 */ u8 m_previous;
};
#endif

/* AudioGCN (class name from RTTI): the GameCube back end (Game/AudioGCN.cpp). Its members are
 * declared where AUDIOGCN_MEMBERS is defined; creating one needs only its size. */
class AudioGCN : public IAudio {
  public:
    AudioGCN();
    virtual ~AudioGCN();
    virtual bool vf_0x18(const char *file);
    virtual bool vf_0x1C(int group, int arg2);
    virtual bool vf_0x20(int group);
    virtual void vf_0x24(s8 bank, s8 program, u8 priority, s8 group, u8 voice, float range);
    virtual void vf_0x28();
    virtual void vf_0x2C();
    virtual void vf_0x30(s8 group, u8 voice);
    virtual void vf_0x34();
    virtual void vf_0x38(s8 volume, s8 group, u8 voice);
    virtual void vf_0x3C(s8 pan, s8 group, u8 voice);
    virtual void vf_0x40(s8 value, s8 group, u8 voice);
    virtual void vf_0x44(s8 surround, s8 group, u8 voice);
    virtual void vf_0x48(s8 group, u8 voice);
    virtual void vf_0x4C(s16 pitch, s8 group, u8 voice);
    virtual void vf_0x50(s8 group, u8 voice, s8 bank, s8 program);
    virtual void vf_0x54();
    virtual void vf_0x58(int mode);
    virtual void vf_0x5C(s8 group, u8 voice);
    virtual void vf_0x60();
    virtual void vf_0x64(s8 volume, s8 group, u8 voice);
    virtual void vf_0x68();

#ifdef AUDIOGCN_MEMBERS
    /* 0x04 */ void *m_heap;          /* the sound heap (name inferred) */
    /* 0x08 */ void *m_groups[8];     /* the sequence player of each group (name inferred) */
    /* 0x28 */ UnkAudioState_801F1E24 m_state;
#else
    /* 0x04 */ u8 field_0x4[0x2C - 0x4];
#endif
};

#endif
