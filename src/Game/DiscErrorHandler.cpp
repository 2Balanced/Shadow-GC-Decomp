/* The disc error screen: when the drive reports an error (cover open, no disc, wrong disc, retry,
 * fatal), the game stops rumble and sound, borrows memory for the system font (saving what was there
 * to ARAM), draws the message in the console's language with GX until the drive recovers, then puts
 * the memory back and resumes. It also watches the reset button. There is no RTTI here: the file,
 * function and variable names are inferred; the SDK and RenderWare calls are named from their use.
 * Definitions are in reverse address order (deferred inlining emits functions last-defined first).
 */
#include <dolphin/types.h>
#include "size_assert.h"

/* The system font header (the SDK's OSFontHeader; only the fields read here are named). */
struct UnkFontHeader_8032C190 {
    /* 0x00 */ u16 field_0x0[6];
    /* 0x0C */ u16 m_width;
    /* 0x0E */ u16 m_leading;
    /* 0x10 */ u16 m_cellWidth;
    /* 0x12 */ u16 m_cellHeight;
    /* 0x14 */ u32 m_sheetSize;
    /* 0x18 */ u16 m_sheetFormat;
    /* 0x1A */ u16 m_sheetColumn;
    /* 0x1C */ u16 m_sheetRow;
    /* 0x1E */ u16 m_sheetWidth;
    /* 0x20 */ u16 m_sheetHeight;
};

/* An ARAM transfer request (the SDK's ARQRequest). */
struct UnkAramRequest_8032C0F4 {
    /* 0x00 */ u32 field_0x0[8];
};
SIZE_ASSERT(UnkAramRequest_8032C0F4, 0x20);

/* A thread queue (the SDK's OSThreadQueue). */
struct UnkThreadQueue_8032CCFC {
    /* 0x0 */ void *m_head;
    /* 0x4 */ void *m_tail;
};

/* RenderWare's video mode description (RwVideoMode). */
struct UnkVideoMode_8032C190 {
    /* 0x00 */ s32 m_width;
    /* 0x04 */ s32 m_height;
    /* 0x08 */ s32 m_depth;
    /* 0x0C */ s32 m_flags;
    /* 0x10 */ s32 m_refRate;
    /* 0x14 */ s32 m_format;
};
SIZE_ASSERT(UnkVideoMode_8032C190, 0x18);

/* RenderWare's globals (RwEngineInstance); only the memory functions are used here. */
struct UnkRwGlobals_805F15B4 {
    /* 0x000 */ u8 field_0x0[0x108];
    /* 0x108 */ void *(*m_malloc)(u32 size, u32 hint);
    /* 0x10C */ void (*m_free)(void *ptr);
};

/* The game state (struct name unknown; see Game/AdvMemoryCheck.cpp). */
struct UnkGameState_8057D6A0 {
    /* 0x000 */ u8 field_0x0[0x10];
    /* 0x010 */ u32 field_0x10;
    /* 0x014 */ u8 field_0x14[0x84 - 0x14];
    /* 0x084 */ u8 field_0x84;
    /* 0x085 */ u8 field_0x85[0x150 - 0x85];
};
SIZE_ASSERT(UnkGameState_8057D6A0, 0x150);

/* A GX colour (GXColor). */
struct UnkGXColor {
    u8 r;
    u8 g;
    u8 b;
    u8 a;
};

typedef void (*UnkResetCallback)(s32 arg);
typedef void (*UnkAramCallback)(u32 request);

/* The GX write-gather pipe. */
union UnkGXFifo {
    s16 s16;
    u16 u16;
    u32 u32;
};
#define GX_FIFO (*(volatile UnkGXFifo *)0xCC008000)

extern UnkGameState_8057D6A0 lbl_8057D6A0;
extern UnkRwGlobals_805F15B4 *lbl_805F15B4;
extern u32 lbl_805EE0B0;
extern void *lbl_805EF9F4;

extern "C" {
/* OS. */
BOOL fn_8037649C(void);                                        /* OSGetResetButtonState */
UnkResetCallback fn_8037D6A8(UnkResetCallback callback);       /* sets the reset callback */
u64 fn_80051A94(void);                                         /* the time */
u16 fn_80374308(void);                                         /* OSGetFontEncode */
BOOL fn_80374F58(UnkFontHeader_8032C190 *font);                /* OSInitFont */
char *fn_80375164(const char *string, void **image, s32 *x, s32 *y, s32 *width); /* OSGetFontTexture */
void *fn_80377644(void);                                       /* OSGetCurrentThread */
void OSInitThreadQueue(UnkThreadQueue_8032CCFC *queue);
void fn_8037847C(UnkThreadQueue_8032CCFC *queue);              /* OSSleepThread */
void fn_80378568(UnkThreadQueue_8032CCFC *queue);              /* OSWakeupThread */
void fn_80377BC0(void);
void OSResetSystem(s32 reset, u32 resetCode, BOOL forceMenu);
void *OSGetArenaHi(void);
void DCInvalidateRange(void *addr, u32 nBytes);
void DCFlushRange(void *addr, u32 nBytes);
void fn_8037171C(void *addr, u32 nBytes);
s32 DVDGetDriveStatus(void);
BOOL fn_8037C9F4(void);                                        /* DVDCheckDisk */
u32 fn_80381A78(u32 length);                                   /* ARAlloc */
void fn_803836F4(UnkAramRequest_8032C0F4 *request, u32 owner, u32 type, u32 priority, u32 source,
                 u32 dest, u32 length, UnkAramCallback callback); /* ARQPostRequest */
s32 fn_8038D238(s32 channel);
void fn_803804FC(const u32 *commands);                         /* PADControlAllMotors */
char *strcpy(char *dst, const char *src);

/* VI. */
void fn_8037F094(BOOL black);                                  /* VISetBlack */
void fn_8037EEF0(void);                                        /* VIFlush */
void *fn_8037F08C(void);
void fn_8037F020(void *xfb);                                   /* VISetNextFrameBuffer */
void fn_8037DE8C(void);                                        /* VIWaitForRetrace */

/* MTX and GX. */
void fn_80378A18(f32 m[4][4], f32 t, f32 b, f32 l, f32 r, f32 n, f32 f); /* C_MTXOrtho */
void fn_803788F8(f32 m[3][4]);                                 /* PSMTXIdentity */
void fn_803789F0(f32 m[3][4], f32 xs, f32 ys, f32 zs);         /* PSMTXScale */
void fn_80399070(f32 m[4][4], s32 type);
void fn_803991E8(f32 m[3][4], s32 id);
void fn_80399288(s32 id);
void fn_80398DAC(BOOL compare, s32 func, BOOL update);
void fn_80396C3C(s32 n);
void fn_80396C78(s32 chan, BOOL enable, s32 ambSrc, s32 matSrc, s32 lights, s32 diffFn, s32 attnFn);
void fn_80396A6C(s32 chan, UnkGXColor color);
void fn_803969E4(void *light, UnkGXColor color);
void fn_803969B8(void *light, f32 x, f32 y, f32 z);
void fn_803969C8(void *light, f32 x, f32 y, f32 z);
void fn_80396758(void *light, f32 cutoff, s32 fn);
void fn_803968E8(void *light, f32 refDist, f32 refBright, s32 fn);
void fn_803969F0(void *light, s32 id);
void fn_803989A0(s32 n);
void fn_803982A8(s32 stage, s32 mode);
void fn_80398804(s32 stage, s32 coord, s32 map, s32 color);
void fn_80398D00(s32 type, s32 src, s32 dst, s32 op);
void fn_8039494C(void);
void fn_80394500(s32 attr, s32 type);
void fn_80394984(s32 fmt, s32 attr, s32 cnt, s32 type, u32 frac);
void fn_80395D48(s32 n);
void fn_80399458(f32 left, f32 top, f32 wd, f32 ht, f32 nearz, f32 farz);
void fn_80394F2C(void);
void fn_80397774(void);
void fn_8039616C(UnkGXColor color, u32 z);
void fn_80398D54(BOOL update);
void fn_80396400(void *dest, BOOL clear);
void fn_803956DC(void);
void fn_80395B28(s32 type, s32 fmt, u16 nverts);
void fn_80396DF0(void *obj, void *image, u16 width, u16 height, s32 format, s32 wrapS, s32 wrapT,
                 BOOL mipmap);
void fn_80397084(void *obj, s32 minFilt, s32 magFilt, f32 minLod, f32 maxLod, f32 lodBias,
                 BOOL biasClamp, BOOL edgeLod, s32 maxAniso);
void fn_803973F8(void *obj, s32 id);
void fn_803992BC(f32 m[3][4], s32 id, s32 type);
void fn_803951BC(s32 n);
void fn_80394F3C(s32 dst, s32 type, s32 src, s32 mtx, BOOL normalize, s32 postMtx);

/* RenderWare. */
void fn_804953C0(void **xfb1, void **xfb2);                    /* the two frame buffers */
s32 fn_80485E70(void);                                         /* RwEngineGetCurrentVideoMode */
void fn_80485DE4(UnkVideoMode_8032C190 *mode, s32 index);      /* RwEngineGetVideoModeInfo */

/* The game: rumble, sound, movie and task managers (behaviour inferred). */
void *fn_80177F38(void);
void fn_8034B390(void *manager);
u32 *fn_80053164(void);
void fn_801D515C(u32 *manager);
const char *fn_801D51CC(u32 *manager);
void fn_801D522C(u32 *manager);
void fn_801D4C5C(u32 *manager);
void fn_801D524C(u32 *manager, const char *name);
void fn_801D654C(u32 *manager);
void fn_801D658C(u32 *manager);
void fn_801D6760(u32 *manager);
void fn_8003723C(void);
s32 fn_800361D8(void);
void fn_8003636C(void);
void fn_80036244(void);
u32 *fn_801723E0(void);
void fn_801742E0(u32 *manager);
void fn_80342370(void);
void fn_801E1BD4(void);
void fn_801E3B40(void *manager, s32 arg);
void *fn_8001C4F8(void);
void *fn_801EB304(void *root, s32 layer);
void fn_801EB314(void *root);
}

/* The messages for each language, by error (cover open, no disc, wrong disc, unreadable, fatal). */
static const char *s_english[5] = {
    "The Disc Cover is open.\nIf you want to continue the game,\nplease close the Disc Cover.",
    "Please insert the SHADOW THE\nHEDGEHOG\x99 Game Disc.",
    "This is not the SHADOW\nTHE HEDGEHOG\x99 Game Disc.\nPlease insert the SHADOW THE\nHEDGEHOG\x99 Game Disc.",
    "The Game Disc could not be read.\nPlease read the\nNintendo GameCube Instruction Booklet\nfor more information.",
    "An error has occurred.\nTurn the power off and refer to the\nNintendo GameCube Instruction Booklet\nfor further instructions.",
};
static const char *s_german[5] = {
    "Der Disc-Deckel ist ge\xF6" "ffnet.\nBitte den Disc-Deckel schlie\xDF" "en,\num mit dem Spiel fortzufahren.",
    "Bitte legen Sie die SHADOW\nTHE HEDGEHOG\x99-Disc ein.",
    "Dies ist nicht die SHADOW\nTHE HEDGEHOG\x99-Disc.\nBitte legen Sie die SHADOW\nTHE HEDGEHOG\x99-Disc ein.",
    "Diese Game Disc kann nicht gelesen\nwerden. Bitte lesen Sie die\nBedienungsanleitung des\nNintendo GameCube, um weitere\nInformationen zu erhalten.",
    "Ein Fehler ist aufgetreten.\nBitte schalten Sie den\nNintendo GameCube aus und lesen Sie die\nBedienungsanleitung, um weitere\nInformationen zu erhalten.",
};
static const char *s_french[5] = {
    "Le couvercle est ouvert.\nPour continuer \xE0 jouer,\nveuillez fermer le couvercle.",
    "Veuillez ins\xE9rer le disque\nSHADOW THE HEDGEHOG\x99.",
    "Ce n'est pas le disque\nSHADOW THE HEDGEHOG\x99.\nVeuillez ins\xE9rer le disque\nSHADOW THE HEDGEHOG\x99.",
    "La lecture du disque a \xE9" "chou\xE9.\nVeuillez vous r\xE9" "f\xE9rer au manuel d'instructions\nNintendo GameCube pour de plus\namples informations.",
    "Une erreur est survenue.\nEteignez la console et r\xE9" "f\xE9rez-vous\nau manuel d'instructions\nNintendo GameCube pour de plus\namples informations.",
};
static const char *s_spanish[5] = {
    "La tapa est\xE1 abierta.\nSi quieres seguir jugando,\ndebes cerrar la tapa.",
    "Coloca el disco de\nSHADOW THE HEDGEHOG\x99.",
    "Este disco no es el original\nSHADOW THE HEDGEHOG\x99.\nColoca el disco de\nSHADOW THE HEDGEHOG\x99.",
    "No se puede leer el disco.\nConsulta el manual de instrucciones de\nNintendo GameCube  para obtener\nm\xE1s informaci\xF3n.",
    "Se ha producido un error.\nApaga la consola y consulta el manual\nde instrucciones de\nNintendo GameCube para obtener\nm\xE1s informaci\xF3n.",
};
static const char *s_italian[5] = {
    "Il coperchio del disco \xE8 aperto.\nSe vuoi proseguire nel gioco,\nchiudi il coperchio del disco.",
    "Inserisci il disco di gioco\nSHADOW THE HEDGEHOG\x99.",
    "Questo non \xE8 il disco di gioco\nSHADOW THE HEDGEHOG\x99.\nInserisci il disco di gioco\nSHADOW THE HEDGEHOG\x99.",
    "Impossibile leggere il disco di gioco.\nPer ulteriori indicazioni consulta il\nmanuale di istruzioni del\nNintendo GameCube.",
    "Si \xE8 verificato un errore.\nSpegni e consulta il manuale di\nistruzioni del\nNintendo GameCube per ulteriori\nindicazioni.",
};
static const char *s_dutch[5] = {
    "De disc-deksel is open.\nSluit de disc-deksel als je verder\nwilt spelen.",
    "Plaats de spel-disc met\nSHADOW THE HEDGEHOG\x99.",
    "Plaats de spel-disc met\nSHADOW THE HEDGEHOG\x99.",
    "De spel-disc kon niet worden\ngelezen. Lees de handleiding van\nde Nintendo GameCube voor meer\ninformatie.",
    "Er is een fout opgetreden.\nZet de Nintendo GameCube uit en\nraadpleeg de handleiding van de\nNintendo GameCube voor nadere\ninstructies.",
};

/* The motor command for all four controllers: stop hard. */
const u32 lbl_8050E6F4[4] = {2, 2, 2, 2};

/* A buffer another unit sets aside for the font (used when the heap has no room). */
void *lbl_805F0B88;
/* The ARAM block the borrowed memory is saved to, and the font buffer's size. */
u32 lbl_805F0B8C;
u32 lbl_805F0B90;
/* Whether the font buffer came from the heap. */
u8 lbl_805F0B94;
void *lbl_805F0B98;
UnkFontHeader_8032C190 *lbl_805F0B9C;
/* The two frame buffers and the one drawn next. */
void *lbl_805F0BA0;
void *lbl_805F0BA4;
void *lbl_805F0BA8;
/* Set while an ARAM transfer runs. */
volatile u8 lbl_805F0BAC;
/* Set while the error screen is shown. */
s32 lbl_805F0BB0;
s32 lbl_805F0BB4;
s32 lbl_805F0BB8;
/* The threads waiting for the error screen to end. */
UnkThreadQueue_8032CCFC lbl_805F0BBC;
UnkThreadQueue_8032CCFC lbl_805F0BC4;
/* The thread that shows the error screen. */
void *lbl_805F0BCC;
/* Set once the reset button was pressed, and the reset callback this one replaced. */
u8 lbl_805F0BD0;
UnkResetCallback lbl_805F0BD4;
u64 lbl_805F0BD8;

UnkVideoMode_8032C190 lbl_80585760;

/* The size of the system font for the console's encoding, rounded up to 32 bytes. */
inline void RoundFontSize() {
    lbl_805F0B90 = fn_80374308() == 1 ? 0x120F00 : 0x20120;
    lbl_805F0B90 = (lbl_805F0B90 + 0x1F) & ~0x1F;
}

/* Invalidates the vertex and texture caches before drawing. */
inline void BeginFrame() {
    fn_80394F2C();
    fn_80397774();
}

/* Copies the frame to the next frame buffer, shows it and swaps the buffers. */
inline void EndFrame() {
    fn_80396400(lbl_805F0BA8, 1);
    fn_803956DC();
    fn_8037F020(lbl_805F0BA8);
    fn_8037EEF0();
    fn_8037DE8C();
    lbl_805F0BA8 = lbl_805F0BA8 == lbl_805F0BA0 ? lbl_805F0BA4 : lbl_805F0BA0;
}

extern "C" {
void fn_8032C594(u32 request);
void fn_8032CB3C(void *image);
void fn_8032CA68(s32 x, s32 y, s32 s, s32 t, u32 color);
void fn_8032C8CC(const char *text, u32 color, s32 x, s32 y);
void fn_8032C5A0(s32 status, s32 language);
BOOL fn_8032C190(void);
BOOL fn_8032BE74(s32 arg0, s32 arg1);
void fn_8032BB78(u8 noReset);
void fn_8032CCA8(void);
void fn_8032BB24(s32 arg);
}

/* Copies the borrowed memory back from ARAM, or frees the heap buffer. */
inline void RestoreMemory() {
    if (!lbl_805F0B94) {
        DCInvalidateRange(lbl_805F0B98, lbl_805F0B90);
        UnkAramRequest_8032C0F4 request;
        lbl_805F0BAC = 1;
        fn_803836F4(&request, 0, 1, 1, lbl_805F0B8C, (u32)lbl_805F0B98, lbl_805F0B90, fn_8032C594);
        while (lbl_805F0BAC) {
        }
        DCFlushRange(lbl_805F0B98, lbl_805F0B90);
    } else {
        lbl_805F15B4->m_free(lbl_805F0B98);
    }
    lbl_805F0B94 = 0;
}

inline UnkThreadQueue_8032CCFC *GetWaitQueue() {
    UnkThreadQueue_8032CCFC *queue = 0;
    if (lbl_805F0BB0 != 0) {
        queue = &lbl_805F0BBC;
    }
    return queue;
}

extern "C" void fn_8032CCFC(void) {
    lbl_805F0BCC = fn_80377644();
    OSInitThreadQueue(&lbl_805F0BBC);
    OSInitThreadQueue(&lbl_805F0BC4);
}

extern "C" void fn_8032CCA8(void) {
    RoundFontSize();
    lbl_805F0B8C = fn_80381A78(lbl_805F0B90);
}

extern "C" void fn_8032CC84(void) {
    BeginFrame();
}

extern "C" void fn_8032CC30(void) {
    EndFrame();
}

/* Loads a sheet of the font as texture 0, with a texture matrix that maps texels to 0..1. */
extern "C" void fn_8032CB3C(void *image) {
    u32 tex[8];
    f32 m[3][4];
    fn_80396DF0(tex, image, lbl_805F0B9C->m_sheetWidth, lbl_805F0B9C->m_sheetHeight,
                lbl_805F0B9C->m_sheetFormat, 0, 0, 0);
    fn_80397084(tex, 1, 1, 0.0f, 0.0f, 0.0f, 0, 0, 0);
    fn_803973F8(tex, 0);
    fn_803789F0(m, 1.0f / lbl_805F0B9C->m_sheetWidth, 1.0f / lbl_805F0B9C->m_sheetHeight, 1.0f);
    fn_803992BC(m, 0x1E, 1);
    fn_803951BC(1);
    fn_80394F3C(0, 1, 4, 0x1E, 0, 0x7D);
}

inline void GXPosition3s16(s16 x, s16 y, s16 z) {
    GX_FIFO.s16 = x;
    GX_FIFO.s16 = y;
    GX_FIFO.s16 = z;
}

inline void GXColor1u32(u32 color) {
    GX_FIFO.u32 = color;
}

inline void GXTexCoord2s16(s16 s, s16 t) {
    GX_FIFO.s16 = s;
    GX_FIFO.s16 = t;
}

/* Draws one glyph: a quad at (x, y) textured with the cell at (s, t) of the loaded sheet. */
extern "C" void fn_8032CA68(s32 x, s32 y, s32 s, s32 t, u32 color) {
    s16 left = x;
    s16 right = left + lbl_805F0B9C->m_cellWidth;
    s16 top = y;
    s16 bottom = top + lbl_805F0B9C->m_cellHeight;
    u16 s2 = s + lbl_805F0B9C->m_cellWidth;
    u16 t2 = t + lbl_805F0B9C->m_cellHeight;
    fn_80395B28(0x80, 0, 4);
    GXPosition3s16(left, top, 0);
    GXColor1u32(color);
    GXTexCoord2s16(s, t);
    GXPosition3s16(right, top, 0);
    GXColor1u32(color);
    GXTexCoord2s16(s2, t);
    GXPosition3s16(right, bottom, 0);
    GXColor1u32(color);
    GXTexCoord2s16(s2, t2);
    GXPosition3s16(left, bottom, 0);
    GXColor1u32(color);
    GXTexCoord2s16(s, t2);
}

/* Draws text centred on the screen, offset by (x, y). */
extern "C" void fn_8032C8CC(const char *text, u32 color, s32 x, s32 y) {
    s32 maxWidth = 0;
    s32 width = 0;
    s32 height = 0;
    const char *p = text;
    while (*p) {
        if (*p == ' ') {
            p++;
            width += lbl_805F0B9C->m_width >> 1;
        } else if (*p != '\n') {
            s32 cx;
            s32 cy;
            s32 cw;
            void *image;
            p = fn_80375164(p, &image, &cx, &cy, &cw);
            width += cw;
        } else {
            if (maxWidth < width) {
                maxWidth = width;
            }
            width = 0;
            p++;
            height += lbl_805F0B9C->m_leading;
        }
    }
    if (maxWidth < width) {
        maxWidth = width;
    }
    height += lbl_805F0B9C->m_leading;
    s32 dy = (lbl_80585760.m_height - height) >> 1;
    s32 dx = (lbl_80585760.m_width - maxWidth) >> 1;
    y = dy + y;
    s32 left = dx + x;
    x = left;
    p = text;
    while (*p) {
        if (*p == ' ') {
            p++;
            x += lbl_805F0B9C->m_width >> 1;
        } else if (*p != '\n') {
            s32 cx;
            s32 cy;
            s32 cw;
            void *image;
            p = fn_80375164(p, &image, &cx, &cy, &cw);
            fn_8032CB3C(image);
            fn_8032CA68(x, y, cx, cy, color);
            x += cw;
        } else {
            x = left;
            p++;
            y += lbl_805F0B9C->m_leading;
        }
    }
}

/* Shows the message for drive status in language (0 Japanese, 1.. the table's languages). The
 * Japanese messages are Shift-JIS. */
extern "C" void fn_8032C5A0(s32 status, s32 language) {
    const char *message;
    switch (status) {
    case 5: {
        const char *messages[] = {
            "\x83" "f\x83" "B\x83X\x83N\x83J\x83o\x81[\x82\xAA\x8AJ\x82\xA2\x82\xC4\x82\xA2\x82\xDC\x82\xB7\x81" "B\n\x83Q\x81[\x83\x80\x82\xF0\x91\xB1\x82\xAF\x82\xE9\x8F\xEA\x8D\x87\x82\xCD\x81" "A\n\x83" "f\x83" "B\x83X\x83N\x83J\x83o\x81[\x82\xF0\x95\xC2\x82\xDF\x82\xC4\x82\xAD\x82\xBE\x82\xB3\x82\xA2\x81" "B",
            s_english[0], s_german[0], s_french[0],
            s_spanish[0], s_italian[0], s_dutch[0],
        };
        message = messages[language];
        break;
    }
    case 4: {
        const char *messages[] = {
            "\x83V\x83\x83\x83h\x83" "E\x81" "E\x83U\x81" "E\x83w\x83" "b\x83W\x83z\x83" "b\x83O\n\x82\xCC\x83" "f\x83" "B\x83X\x83N\x82\xF0\x83Z\x83" "b\x83g\x82\xB5\x82\xC4\x82\xAD\x82\xBE\x82\xB3\x82\xA2\x81" "B",
            s_english[1], s_german[1], s_french[1],
            s_spanish[1], s_italian[1], s_dutch[1],
        };
        message = messages[language];
        break;
    }
    case 6: {
        const char *messages[] = {
            "\x83V\x83\x83\x83h\x83" "E\x81" "E\x83U\x81" "E\x83w\x83" "b\x83W\x83z\x83" "b\x83O\n\x82\xCC\x83" "f\x83" "B\x83X\x83N\x82\xF0\x83Z\x83" "b\x83g\x82\xB5\x82\xC4\x82\xAD\x82\xBE\x82\xB3\x82\xA2\x81" "B",
            s_english[2], s_german[2], s_french[2],
            s_spanish[2], s_italian[2], s_dutch[2],
        };
        message = messages[language];
        break;
    }
    case 11: {
        const char *messages[] = {
            "\x83" "f\x83" "B\x83X\x83N\x82\xF0\x93\xC7\x82\xDF\x82\xDC\x82\xB9\x82\xF1\x82\xC5\x82\xB5\x82\xBD\x81" "B\n\x82\xAD\x82\xED\x82\xB5\x82\xAD\x82\xCD\x81" "A\x96{\x91\xCC\x82\xCC\x8E\xE6\x88\xB5\x90\xE0\x96\xBE\x8F\x91\x82\xF0\x82\xA8\x93\xC7\x82\xDD\x82\xAD\x82\xBE\x82\xB3\x82\xA2\x81" "B",
            s_english[3], s_german[3], s_french[3],
            s_spanish[3], s_italian[3], s_dutch[3],
        };
        message = messages[language];
        break;
    }
    case -1: {
        const char *messages[] = {
            "\x83G\x83\x89\x81[\x82\xAA\x94\xAD\x90\xB6\x82\xB5\x82\xDC\x82\xB5\x82\xBD\x81" "B\n\x96{\x91\xCC\x82\xCC\x83p\x83\x8F\x81[\x83{\x83^\x83\x93\x82\xF0\x89\x9F\x82\xB5\x82\xC4\x93" "d\x8C\xB9\x82\xF0OFF\x82\xC9\x82\xB5\x81" "A\n\x96{\x91\xCC\x82\xCC\x8E\xE6\x88\xB5\x90\xE0\x96\xBE\x8F\x91\x82\xCC\x8Ew\x8E\xA6\x82\xC9\x8F]\x82\xC1\x82\xC4\x82\xAD\x82\xBE\x82\xB3\x82\xA2\x81" "B",
            s_english[4], s_german[4], s_french[4],
            s_spanish[4], s_italian[4], s_dutch[4],
        };
        message = messages[language];
        break;
    }
    default:
        message = "";
        break;
    }
    BeginFrame();
    fn_8032C8CC(message, 0xFFFFFFFF, 0, 0);
    EndFrame();
}

/* The ARAM transfer has finished. */
extern "C" void fn_8032C594(u32 request) {
    lbl_805F0BAC = 0;
}

/* Borrows memory for the font (saving its contents to ARAM), loads the font and sets up GX. */
extern "C" BOOL fn_8032C190(void) {
    f32 projection[4][4];
    f32 view[3][4];
    u32 light[16];
    UnkAramRequest_8032C0F4 request;

    fn_8037F094(0);
    RoundFontSize();
    lbl_805F0B94 = 1;
    lbl_805F0B98 = 0;
    lbl_805F0B98 = lbl_805F15B4->m_malloc(lbl_805F0B90, 0);
    if (lbl_805F0B98 == 0) {
        lbl_805F0B94 = 0;
        if (lbl_805F0B88 != 0) {
            fn_8037171C(lbl_805F0B88, lbl_805F0B90);
            lbl_805F0B98 = lbl_805F0B88;
        } else {
            lbl_805F0B98 = (void *)(((u32)OSGetArenaHi() - lbl_805F0B90 - 1) & ~0x1F);
        }
        lbl_805F0BAC = 1;
        fn_803836F4(&request, 0, 0, 1, (u32)lbl_805F0B98, lbl_805F0B8C, lbl_805F0B90, fn_8032C594);
        while (lbl_805F0BAC) {
        }
        DCInvalidateRange(lbl_805F0B98, lbl_805F0B90);
    }
    lbl_805F0B9C = (UnkFontHeader_8032C190 *)lbl_805F0B98;
    fn_80374F58(lbl_805F0B9C);
    fn_804953C0(&lbl_805F0BA0, &lbl_805F0BA4);
    lbl_805F0BA8 = lbl_805F0BA4;
    fn_80485DE4(&lbl_80585760, fn_80485E70());
    UnkGXColor black = {0, 0, 0, 0xFF};
    fn_80378A18(projection, 0.0f, lbl_80585760.m_height, 0.0f, lbl_80585760.m_width, 0.0f,
                -100.0f);
    fn_80399070(projection, 1);
    fn_803788F8(view);
    fn_803991E8(view, 0);
    fn_80399288(0);
    fn_80398DAC(0, 7, 0);
    fn_80396C3C(1);
    fn_80396C78(4, 1, 0, 1, 1, 0, 2);
    fn_80396A6C(0, black);
    UnkGXColor white = {0xFF, 0xFF, 0xFF, 0xFF};
    fn_803969E4(light, white);
    fn_803969B8(light, 0.0f, 0.0f, 1000.0f);
    fn_803969C8(light, 0.0f, 0.0f, -1.0f);
    fn_80396758(light, 90.0f, 0);
    fn_803968E8(light, 1000.0f, 1.0f, 0);
    fn_803969F0(light, 1);
    fn_803989A0(1);
    fn_803982A8(0, 0);
    fn_80398804(0, 0, 0, 4);
    fn_80398D00(1, 4, 5, 0);
    fn_8039494C();
    fn_80394500(9, 1);
    fn_80394500(0xB, 1);
    fn_80394500(0xD, 1);
    fn_80394984(0, 9, 1, 3, 0);
    fn_80394984(0, 0xB, 1, 5, 0);
    fn_80394984(0, 0xD, 1, 3, 0);
    fn_80395D48(0);
    fn_80399458(0.0f, 0.0f, lbl_80585760.m_width, lbl_80585760.m_height, 0.0f, 1.0f);
    for (u32 i = 0; i < 2; i++) {
        BeginFrame();
        fn_8039616C(black, 0xFFFFFF);
        fn_80398D54(1);
        EndFrame();
    }
    return 1;
}

extern "C" void fn_8032C0F4(void) {
    RestoreMemory();
}

/* Shows the error screen until the drive recovers, then gives the memory back and resumes. */
extern "C" BOOL fn_8032BE74(s32 arg0, s32 arg1) {
    char name[0x100];
    bool loop = true;
    bool movie = false;
    bool other = false;
    lbl_805F0BB0 = 1;
    BOOL language = fn_80374308() != 1 ? 1 : 0;
    fn_8034B390(fn_80177F38());
    fn_803804FC(lbl_8050E6F4);
    if (*fn_80053164() != 0) {
        const char *current = fn_801D51CC(fn_80053164());
        if (current != 0) {
            strcpy(name, current);
        } else {
            name[0] = 0;
        }
        fn_801D515C(fn_80053164());
        fn_801D522C(fn_80053164());
        fn_801D4C5C(fn_80053164());
        if (lbl_805EE0B0 != 0 && fn_800361D8() == 0) {
            fn_8003636C();
            movie = true;
        } else if (*fn_801723E0() != 0) {
            fn_801742E0(fn_801723E0());
            other = true;
        }
        fn_80342370();
    }
    fn_8032C190();
    s32 shown = DVDGetDriveStatus();
    do {
        s32 status = DVDGetDriveStatus();
        lbl_805F0BB8 = 0;
        switch (status) {
        default:
            if (fn_8037C9F4()) {
                loop = false;
            }
            fn_8032BB78(0);
            break;
        case 4:
        case 5:
        case 6:
        case 11:
            lbl_805F0BB4 = 1;
            lbl_805F0BB8 = 1;
            fn_8032BB78(0);
        case -1:
            shown = status;
            break;
        }
        fn_8032C5A0(shown, language);
    } while (loop);
    RestoreMemory();
    fn_80378568(&lbl_805F0BBC);
    fn_80378568(&lbl_805F0BC4);
    if (*fn_80053164() != 0) {
        if (movie) {
            fn_80036244();
        } else if (!other && name[0] != 0) {
            fn_801D524C(fn_80053164(), name);
        }
    }
    lbl_805F0BB0 = 0;
    fn_801E1BD4();
    if (lbl_8057D6A0.field_0x84 == 0) {
        fn_801E3B40(lbl_805EF9F4, 0);
    }
    return 1;
}

/* Waits while the drive reports an error, showing the error screen. */
extern "C" void fn_8032BDEC(void) {
    if (lbl_805F0B8C == 0) {
        fn_8032CCA8();
    }
    while (true) {
        s32 status = DVDGetDriveStatus();
        lbl_805F0BB8 = 0;
        switch (status) {
        case 4:
        case 5:
        case 6:
        case 11:
            lbl_805F0BB4 = 1;
            lbl_805F0BB8 = 1;
        case -1:
            fn_8032BE74(4, 0);
            break;
        default:
            return;
        }
    }
}

/* The DVD error callback: shows the error screen on the main thread, or waits for it to end. */
extern "C" BOOL fn_8032BD7C(s32 status) {
    lbl_805F0BB8 = 0;
    switch (status) {
    case 3:
    case 4:
    case 5:
    case 6:
        lbl_805F0BB4 = 1;
        lbl_805F0BB8 = 1;
    case 2:
        if (lbl_805F0BCC == fn_80377644()) {
            fn_8032BE74(4, 0);
        } else {
            fn_8037847C(&lbl_805F0BC4);
        }
        break;
    }
    return 1;
}

/* Handles the reset button: stops the game's output, then restarts (to the menu when pressed twice
 * quickly during a disc error). */
extern "C" void fn_8032BB78(u8 noReset) {
    bool pressed = fn_8037649C();
    bool hot = false;
    if (!pressed) {
        if (lbl_805F0BB4 != 0 && lbl_805F0BB8 == 0) {
            if (fn_80051A94() - lbl_805F0BD8 <= 8000000) {
                hot = true;
            } else {
                lbl_805F0BB4 = 0;
            }
        } else {
            lbl_805F0BD8 = fn_80051A94();
        }
        if (lbl_805F0BD0) {
            static bool s_stopped = false;
            if (!s_stopped) {
                fn_8037F094(1);
                fn_8037EEF0();
                fn_8037F020(fn_8037F08C());
                fn_8037EEF0();
                fn_8037DE8C();
                fn_8034B390(fn_80177F38());
                fn_803804FC(lbl_8050E6F4);
                if (*fn_80053164() != 0) {
                    fn_801D515C(fn_80053164());
                }
                s_stopped = true;
                lbl_8057D6A0.field_0x10 = 0;
            }
            if (!noReset && fn_8038D238(0) != -1) {
                if (*fn_80053164() != 0) {
                    fn_8003723C();
                    fn_801D654C(fn_80053164());
                    fn_801D658C(fn_80053164());
                    fn_801D6760(fn_80053164());
                    fn_80377BC0();
                    if (GetWaitQueue()) {
                        OSResetSystem(1, 0, 0);
                    }
                    if (fn_801EB304(fn_8001C4F8(), 0) != 0) {
                        fn_801EB314(fn_8001C4F8());
                    }
                }
                if (!hot) {
                    OSResetSystem(0, 0, 0);
                } else {
                    OSResetSystem(1, 0, 0);
                }
            }
        }
    } else {
        lbl_805F0BD0 = 1;
    }
}

/* The reset callback: notes a press of the reset button and calls the replaced callback. */
extern "C" void fn_8032BB24(s32 arg) {
    if (fn_8037649C()) {
        lbl_805F0BD0 = 1;
    }
    if (lbl_805F0BD4) {
        lbl_805F0BD4(arg);
    }
}

extern "C" void fn_8032BAF8(void) {
    lbl_805F0BD4 = fn_8037D6A8(fn_8032BB24);
}

/* The queue to wait on while the error screen is shown, or null. */
extern "C" UnkThreadQueue_8032CCFC *fn_8032BAE0(void) {
    return GetWaitQueue();
}
