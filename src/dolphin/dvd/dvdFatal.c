/* Dolphin SDK DVD: the fatal-error screen. When a title enables it, an unrecoverable drive error
 * ends in OSFatal with a message in the console's language (Japanese or English on NTSC consoles,
 * by font encoding; one of six languages elsewhere). The messages are written as byte escapes
 * because they are Shift-JIS and Latin-1 text. */
#include <dolphin/dvd/__dvd.h>

typedef struct GXColor {
    u8 r, g, b, a;
} GXColor;

u32 VIGetTvFormat(void);
/* OS functions dtk has not named yet: font encoding, language setting, fatal screen. */
u16 fn_80374308(void);
u8 OSGetLanguage(void);
void OSFatal(GXColor fg, GXColor bg, const char *msg);

#define VI_NTSC 0
#define OS_FONT_ENCODE_SJIS 1

#define MSG_JAPANESE \
    "\n" \
    "\n" \
    "\n" \
    "\x83G\x83\x89\x81[\x82\xAA\x94\xAD\x90\xB6\x82\xB5\x82\xDC\x82\xB5\x82\xBD\x81" "B\n" \
    "\n" \
    "\x96{\x91\xCC\x82\xCC\x83p\x83\x8F\x81[\x83{\x83^\x83\x93\x82\xF0\x89\x9F\x82\xB5\x82\xC4\x93" "d\x8C\xB9\x82\xF0OFF\x82\xC9\x82\xB5\x81" "A\n" \
    "\x96{\x91\xCC\x82\xCC\x8E\xE6\x88\xB5\x90\xE0\x96\xBE\x8F\x91\x82\xCC\x8Ew\x8E\xA6\x82\xC9\x8F]\x82\xC1\x82\xC4\x82\xAD\x82\xBE\x82\xB3\x82\xA2\x81" "B"

#define MSG_ENGLISH \
    "\n" \
    "\n" \
    "\n" \
    "An error has occurred.\n" \
    "Turn the power off and refer to the\n" \
    "Nintendo GameCube Instruction Booklet\n" \
    "for further instructions."

#define MSG_GERMAN \
    "\n" \
    "\n" \
    "\n" \
    "Ein Fehler ist aufgetreten.\n" \
    "Bitte schalten Sie den Nintendo GameCube\n" \
    "aus und lesen Sie die Bedienungsanleitung,\n" \
    "um weitere Informationen zu erhalten."

#define MSG_FRENCH \
    "\n" \
    "\n" \
    "\n" \
    "Une erreur est survenue.\n" \
    "Eteignez la console et r\xE9" "f\xE9rez-vous au\n" \
    "manuel d'instructions Nintendo GameCube\n" \
    "pour de plus amples informations."

#define MSG_SPANISH \
    "\n" \
    "\n" \
    "\n" \
    "Se ha producido un error.\n" \
    "Apaga la consola y consulta el manual\n" \
    "de instrucciones de Nintendo GameCube\n" \
    "para obtener m\xE1s informaci\xF3n."

#define MSG_ITALIAN \
    "\n" \
    "\n" \
    "\n" \
    "Si \xE8 verificato un errore.\n" \
    "Spegni (OFF) e controlla il manuale\n" \
    "d'istruzioni del Nintendo GameCube\n" \
    "per ulteriori indicazioni."

#define MSG_DUTCH \
    "\n" \
    "\n" \
    "\n" \
    "Er is een fout opgetreden.\n" \
    "Zet de Nintendo GameCube uit en\n" \
    "raadpleeg de handleiding van de\n" \
    "Nintendo GameCube voor nadere\n" \
    "instructies."

static void (*FatalFunc)(void);

const char *Japanese = MSG_JAPANESE;
const char *English = MSG_ENGLISH;

const char *const Messages[] = {
    MSG_ENGLISH, MSG_GERMAN, MSG_FRENCH, MSG_SPANISH, MSG_ITALIAN, MSG_DUTCH,
};

static void fn_8037D0C8(void) {
    const char *message;
    GXColor bg = {0, 0, 0, 0};
    GXColor fg = {255, 255, 255, 0};

    if (VIGetTvFormat() == VI_NTSC) {
        if (fn_80374308() == OS_FONT_ENCODE_SJIS) {
            message = Japanese;
        } else {
            message = English;
        }
    } else {
        message = Messages[OSGetLanguage()];
    }
    OSFatal(fg, bg, message);
}

BOOL fn_8037D154(BOOL enable) {
    BOOL enabled;
    BOOL prev;

    enabled = OSDisableInterrupts();
    prev = FatalFunc ? TRUE : FALSE;
    FatalFunc = enable ? fn_8037D0C8 : NULL;
    OSRestoreInterrupts(enabled);
    return prev;
}

void fn_8037D1C4(void) {
    if (FatalFunc) {
        FatalFunc();
    }
}
