/* The progressive scan prompt (file and class names unknown): at boot, when the display can show
 * progressive video (fn_8037F358) and either the progressive flag is already set (fn_8037707C) or a
 * pad holds B, fn_80332E7C asks "Do you want to display the game in Progressive Mode?" (in English
 * or Japanese, by the system language) and switches the render mode by the answer. After a reset
 * with the flag set it applies progressive mode without asking. The names marked "(name inferred)"
 * are not original; fn_80332E7C keeps its fn_ name (the boot code calls it). Definitions are in
 * reverse address order (deferred inlining emits functions last-defined first). */
#include <dolphin/types.h>
#include "size_assert.h"

/* The render mode the boot code passes on (struct name unknown): m_renderMode is the GX render
 * mode (meaning inferred). */
struct UnkVideoMode_80332E7C {
    /* 0x0 */ const void *m_renderMode;
};

/* The boot code's video setup that fn_80332E7C receives (struct name unknown). */
struct UnkVideoSetup_80332E7C {
    /* 0x0 */ UnkVideoMode_80332E7C *m_mode;
};

/* A stick of a pad state (struct name unknown; layout from fn_8040DD24). */
struct UnkPadStick_8040DD24 {
    /* 0x0 */ float x;
    /* 0x4 */ float y;
    /* 0x8 */ s16 field_0x8;
    /* 0xA */ s16 field_0xA;
};

/* A pad's state for a frame (struct name unknown; the constructor is 0x8040DD24): m_pressed holds
 * the buttons pressed this frame (meaning inferred from the prompt's use: 0x100 left, 0x200 right,
 * 0x401 A or Start). */
struct UnkPadState_8040DD24 {
    UnkPadState_8040DD24();

    /* 0x00 */ u32 field_0x0;
    /* 0x04 */ u32 field_0x4;
    /* 0x08 */ u32 m_pressed;
    /* 0x0C */ u32 field_0xC;
    /* 0x10 */ UnkPadStick_8040DD24 m_stick;
    /* 0x1C */ UnkPadStick_8040DD24 m_subStick;
};
SIZE_ASSERT(UnkPadState_8040DD24, 0x28);

/* A pad of the pad manager (class name unknown). */
struct UnkPad_8040DB6C {
    /* Its state (name inferred; a weak function of this unit, 0x80333094). */
    const UnkPadState_8040DD24 &GetState() const {
        return m_state;
    }

    /* 0x00 */ UnkPadState_8040DD24 m_state;
    /* 0x28 */ s32 field_0x28; /* 0: connected (meaning inferred) */
};

/* The SDK's PADStatus, as fn_803801FC fills it. */
struct UnkPadStatus_803801FC {
    /* 0x0 */ u16 button;
    /* 0x2 */ s8 stickX;
    /* 0x3 */ s8 stickY;
    /* 0x4 */ s8 substickX;
    /* 0x5 */ s8 substickY;
    /* 0x6 */ u8 triggerL;
    /* 0x7 */ u8 triggerR;
    /* 0x8 */ u8 analogA;
    /* 0x9 */ u8 analogB;
    /* 0xA */ s8 err;
};

/* Two of the SDK's GX render modes (in .data; progressive and interlaced, meanings inferred). */
struct UnkRenderMode_805677C0 {
    u8 field_0x0[0x3C];
};
extern UnkRenderMode_805677C0 lbl_805677C0;
extern UnkRenderMode_805677C0 lbl_805677FC;

extern "C" {
/* The pad manager: updating it, its number of pads and a pad (behaviour inferred). */
void *fn_80009208(void);
void fn_8040D9EC(void *manager);
UnkPad_8040DB6C *fn_8040DB6C(void *manager, u32 index);
u32 fn_8040DB78(void *manager);

/* The SDK (behaviour inferred): whether the display can show progressive video, the progressive
 * flag and setting it, the language, reading the pads, waiting for the retrace, the reset code and
 * the SI sampling rate. */
u32 fn_8037F358(void);
u32 fn_8037707C(void);
void fn_803770EC(int progressive);
u16 fn_80374308(void);
void fn_803801FC(UnkPadStatus_803801FC *status);
void fn_80377BC0(void);
u32 OSGetResetCode(void);
void SISetSamplingRate(u32 msec);

/* The prompt's text drawing (behaviour inferred): starting and ending the system, starting and
 * ending a frame, clearing the screen, and a line of text in a colour at x, y. */
int fn_8032C190(void);
void fn_8032C0F4(void);
void fn_8032CC84(void);
void fn_8032CC30(void);
void fn_8032BB78(int arg0);
void fn_8032C8CC(const char *text, u32 color, int x, int y);
void fn_8032BAF8(void);

/* Restarting the video with the new render mode (behaviour inferred). */
void fn_80485EF4(void);
void fn_80486138(void);
void fn_80051850(int arg0, int arg1);
void fn_80486204(UnkVideoSetup_80332E7C *setup);
void fn_80485FE0(void);
}

/* Whether the game runs in progressive mode (name inferred). */
static s32 s_progressive;

/* The prompt (class name unknown; 0x24 bytes, names inferred). */
class UnkProgressiveDialog_80332BEC {
  public:
    UnkProgressiveDialog_80332BEC(UnkVideoSetup_80332E7C *setup, s32 skip)
        : m_setup(setup), m_selection(0), m_state(0), m_fade(0), m_timer(0), m_alpha(0.0f),
          m_skip(skip), m_textReady(0) {}

    /* Asks; true for progressive mode. */
    BOOL Run();
    /* Runs a frame; false once the prompt has faded out. */
    BOOL Tick();

    /* Draws the mode the answer set (name inferred). */
    void DrawAnswer() {
        const char *on[2] = {"Screen has been set to\nProgressive Mode.", "\211\346\226\312\225\134\216\246\203\202\201[\203h\202\315\203v\203\215\203O\203\214\203b\203V\203u\203\202\201[\203h\n\202\311\203Z\203b\203g\202\263\202\352\202\334\202\265\202\275\201B"};
        const char *off[2] = {"Progressive Mode has been turned off.", "\211\346\226\312\225\134\216\246\203\202\201[\203h\202\315\203C\203\223\203^\201[\203\214\201[\203X\203\202\201[\203h\n\202\311\203Z\203b\203g\202\263\202\352\202\334\202\265\202\275\201B"};
        u8 alpha = 255.0f * m_alpha;
        u32 white = 0xFFFFFF00 | alpha;
        fn_8032CC84();
        fn_8032C8CC(m_selection == 1 ? off[m_language] : on[m_language], white, 0, 0);
        fn_8032CC30();
    }

    /* Draws the question and the answers, the chosen one in yellow (name inferred). */
    void DrawQuestion() {
        const char *question[2] = {"Do you want to display the game in\nProgressive Mode?", "\203v\203\215\203O\203\214\203b\203V\203u\203\202\201[\203h\202\305\225\134\216\246\202\265\202\334\202\267\202\251\201H"};
        const char *yes[2] = {"Yes", "\202\315\202\242"};
        const char *no[2] = {"No", "\202\242\202\242\202\246"};
        const char *questionText = question[m_language];
        const char *yesText = yes[m_language];
        const char *noText = no[m_language];
        u8 alpha = 255.0f * m_alpha;
        u32 white = 0xFFFFFF00 | alpha;
        u32 yellow = 0xFFFF0000 | alpha;
        fn_8032CC84();
        fn_8032C8CC(questionText, white, 0, -70);
        u32 yesColor;
        u32 noColor;
        if (m_selection == 1) {
            yesColor = white;
            noColor = yellow;
        } else {
            yesColor = yellow;
            noColor = white;
        }
        fn_8032C8CC(yesText, yesColor, -100, 50);
        fn_8032C8CC(noText, noColor, 100, 50);
        fn_8032CC30();
    }

    /* Switches the render mode by the answer. */
    void ApplyMode() {
        if (m_textReady) {
            fn_8032C0F4();
        }
        fn_80485EF4();
        fn_80486138();
        UnkVideoMode_80332E7C *mode = m_setup->m_mode;
        if (m_selection == 0) {
            mode->m_renderMode = &lbl_805677FC;
        } else {
            mode->m_renderMode = &lbl_805677C0;
        }
        fn_80051850(9, 0);
        fn_80486204(m_setup);
        fn_80485FE0();
        if (m_textReady) {
            fn_8032C190();
        }
        fn_8032BAF8();
        SISetSamplingRate(0);
    }

    /* 0x00 */ UnkVideoSetup_80332E7C *m_setup;
    /* 0x04 */ s32 m_language; /* 0 English, 1 Japanese (index of the texts) */
    /* 0x08 */ s32 m_selection; /* 0 yes, 1 no */
    /* 0x0C */ s32 m_state; /* 0 asking, 1 answered, 2 done */
    /* 0x10 */ s32 m_fade; /* 0 fading in, 1 shown, 2 fading out */
    /* 0x14 */ s32 m_timer;
    /* 0x18 */ float m_alpha;
    /* 0x1C */ s32 m_skip;
    /* 0x20 */ s32 m_textReady;
};
SIZE_ASSERT(UnkProgressiveDialog_80332BEC, 0x24);

/* The state of the first connected pad; false if there is none (name inferred). */
static BOOL GetPadState(UnkPadState_8040DD24 *state) {
    BOOL found = FALSE;
    fn_8040D9EC(fn_80009208());
    for (u32 i = 0; i < fn_8040DB78(fn_80009208()); i++) {
        UnkPad_8040DB6C *pad = fn_8040DB6C(fn_80009208(), i);
        if (pad != 0 && pad->field_0x28 == 0) {
            *state = pad->GetState();
            found = TRUE;
            break;
        }
    }
    return found;
}

/* Asks for progressive mode if the display supports it (name inferred). NonMatching: everything
 * matches up to the call of Run; after it retail checks the dialog for null again before deleting it
 * and stores s_progressive with two branches, while GC/2.7 drops the second check (the dialog is
 * known to be non-null there) and stores (progressive != 0) without branches. Neither a second
 * explicit check, inline helpers for the delete or the store, a ternary nor auto_ptr changes that. */
extern "C" void fn_80332E7C(UnkVideoSetup_80332E7C *setup) {
    if (fn_8037F358() == 0) {
        return;
    }
    if (fn_8037707C() == 0) {
        BOOL retry;
        BOOL pressed = FALSE;
        do {
            UnkPadStatus_803801FC status[4];
            retry = FALSE;
            fn_803801FC(status);
            for (int i = 0; i < 4; i++) {
                if (status[i].err == 0) {
                    if (status[i].button & 0x200) {
                        pressed = TRUE;
                        break;
                    }
                } else if (status[i].err != -1) {
                    retry = TRUE;
                }
            }
            fn_80377BC0();
        } while (retry);
        if (!pressed) {
            return;
        }
    }
    BOOL progressive = FALSE;
    BOOL reset = OSGetResetCode() >> 31;
    s32 skip = reset & (fn_8037707C() == 1);
    UnkProgressiveDialog_80332BEC *dialog = new UnkProgressiveDialog_80332BEC(setup, skip);
    if (dialog != 0) {
        progressive = dialog->Run();
    }
    delete dialog;
    if (progressive) {
        s_progressive = 1;
    } else {
        s_progressive = 0;
    }
}

BOOL UnkProgressiveDialog_80332BEC::Run() {
    if (m_skip == 0) {
        m_language = fn_80374308();
        m_textReady = 1;
        if (fn_8032C190() != 0) {
            while (Tick()) {
                fn_8032BB78(0);
                if (m_state == 0) {
                    DrawQuestion();
                } else if (m_state == 1) {
                    DrawAnswer();
                }
            }
        }
        if (m_textReady) {
            fn_8032C0F4();
            m_textReady = 0;
        }
    } else {
        m_selection = 0;
        ApplyMode();
    }
    if (m_selection == 0) {
        return TRUE;
    }
    return FALSE;
}

BOOL UnkProgressiveDialog_80332BEC::Tick() {
    if (m_fade == 0) {
        if (m_timer++ >= 30) {
            m_fade = 1;
        }
        m_alpha = m_timer / 30.0f;
        m_alpha = m_alpha <= 1.0f ? m_alpha : 1.0f;
    } else if (m_fade == 2) {
        m_timer++;
        m_alpha = 1.0f - m_timer / 30.0f;
        m_alpha = m_alpha >= 0.0f ? m_alpha : 0.0f;
        if (m_timer > 31) {
            if (m_selection == 0) {
                m_state = 2;
            }
            return FALSE;
        }
    } else if (m_state == 0) {
        UnkPadState_8040DD24 pad;
        BOOL timeout = FALSE;
        BOOL found = GetPadState(&pad);
        if (++m_timer >= 600) {
            timeout = TRUE;
        }
        if (found || timeout) {
            if ((pad.m_pressed & 0x401) || timeout) {
                switch (m_selection) {
                case 1:
                    m_state = 1;
                    fn_803770EC(0);
                    break;
                case 0:
                    m_state = 1;
                    fn_803770EC(1);
                    ApplyMode();
                    break;
                }
                m_timer = 0;
            } else if (pad.m_pressed & 0x200) {
                if (m_selection < 1) {
                    m_selection++;
                }
                m_timer = 0;
            } else if (pad.m_pressed & 0x100) {
                if (m_selection > 0) {
                    m_selection--;
                }
                m_timer = 0;
            }
        }
    } else if (m_state == 1) {
        if (m_timer++ >= 240) {
            m_fade = 2;
            m_timer = 0;
        }
    }
    return TRUE;
}

/* Not original: an unused function, emitted first (it is defined last), that uses the float
 * constants in retail's .sdata2 order. GC/2.7 pools constants in emission order while the game's
 * compiler pooled them in definition order (see docs/research-notes.md); the linker drops it. */
float UnusedProgressiveConstants(float x) {
    float y = x * 255.0f;
    y += x / 30.0f;
    y += x * 1.0f;
    return y + 0.0f;
}
