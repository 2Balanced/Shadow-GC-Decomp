/* RenderWare: RpUserData (plugin 0x11F): named arrays of ints, reals or strings kept on geometries,
 * world sectors, frames, cameras, lights, materials and textures, their stream callbacks, the few
 * accessors the game keeps, and RpUserDataPluginAttach (fn_80452CDC).
 *
 * 0x80451F48-0x80453018 (.sbss 0x805F1458-0x805F1498). RpSkinGameCube.c comes before it and
 * RpUVAnimKeyFrame.c after it. The plugin's statics are laid out in reverse declaration order, as
 * GC/2.7 lays out every unit's .sbss: the module record is declared first and lands last.
 *
 * Built as C with cflags_rw_c (configure.py). The list loops live in inline helpers: written in
 * place, GC/2.7 starts a loop's byte offset with an `li` of its own, inlined it copies the zeroed
 * counter as retail does (docs/research-notes.md); the list is a named local of the caller, and the
 * copy's source and the size helper's declaration order follow retail's loads and registers
 * (probes/RenderWare_RpUserData_W41*.py).
 * fn_80452434 (the list copy) has retail's instructions but other registers for its inlined loop
 * induction variables (34-37 rows in every shape tried, W41d-W41f).
 */
#include <dolphin/types.h>
#include "Game/rw/r2.h"

static R2ModuleInfo userDataModule;
static u32 userDataTextureStreamOffset; /* registration result */
static u32 userDataTextureOffset; /* plugin offset */
static u32 userDataMaterialStreamOffset; /* registration result */
static u32 userDataMaterialOffset; /* plugin offset */
static u32 userDataLightStreamOffset; /* registration result */
static u32 userDataLightOffset; /* plugin offset */
static u32 userDataCameraStreamOffset; /* registration result */
static u32 userDataCameraOffset; /* plugin offset */
static u32 userDataFrameStreamOffset; /* registration result */
static u32 userDataFrameOffset; /* plugin offset */
static u32 userDataSectorStreamOffset; /* registration result */
static u32 userDataSectorOffset; /* plugin offset */
static u32 userDataGeometryStreamOffset; /* registration result */
static u32 userDataGeometryOffset;
/* The engine's memory and string functions (RwEngineInstance). */
static inline void *rwAlloc(u32 size, u32 tag) {
    return ((void *(*)(u32, u32))*(u32 *)((u8 *)lbl_805F15B4 + 0x108))(size, tag);
}

static inline void rwFree(void *pointer) {
    ((void (*)(void *))*(u32 *)((u8 *)lbl_805F15B4 + 0x10C))(pointer);
}

static inline u32 rwStringLength(const char *string) {
    return ((u32 (*)(const char *))*(u32 *)((u8 *)lbl_805F15B4 + 0xF4))(string);
}

static inline void rwStringCopy(char *dest, const char *source) {
    ((char *(*)(char *, const char *))*(u32 *)((u8 *)lbl_805F15B4 + 0xCC))(dest, source);
}

/* RpUserDataArray: a named array of ints, reals or strings. */
typedef struct UserDataArray {
    char *name;
    s32 format; /* 1 int, 2 real, 3 string */
    s32 numElements;
    void *data;
} UserDataArray;

/* The plugin's extension of every object it attaches to. */
typedef struct UserDataList {
    s32 numElements;
    UserDataArray *userData;
} UserDataList;

s32 fn_80485D78(u32, u32, void *, void *);
s32 fn_8045994C(u32, u32, void *, void *, void *);
s32 fn_8045998C(u32, void *, void *, void *);
s32 fn_8045F2F8(u32, u32, void *, void *, void *);
s32 fn_8045F338(u32, void *, void *, void *);
s32 fn_804878D4(u32, u32, void *, void *, void *);
s32 fn_804836EC(u32, void *, void *, void *);
s32 fn_804859B8(u32, u32, void *, void *, void *);
s32 fn_80483554(u32, void *, void *, void *);
s32 fn_8045AAA0(u32, u32, void *, void *, void *);
s32 fn_8045AAE0(u32, void *, void *, void *);
s32 fn_8045B2CC(u32, u32, void *, void *, void *);
s32 fn_8045B30C(u32, void *, void *, void *);
u32 fn_8048D280(u32, u32, void *, void *, void *);
s32 fn_80483AFC(u32, void *, void *, void *);

u32 fn_8047D868(void *, void *, u32);
u32 fn_8047D458(void *, void *, u32);
u32 fn_8047D784(void *, void *, u32);
u32 fn_8047D350(void *, void *, u32);
u32 fn_80480D4C(void *, void *, u32);
u32 fn_80480ED0(void *, void *, u32);

u32 fn_80452C98(s32 type);

/* Frees an array's name, its strings and its data. */
static inline void UserDataArrayDestroy(UserDataArray *array) {
    if (array->name != NULL) {
        rwFree(array->name);
        array->name = NULL;
    }
    if (array->format == 3) {
        char **strings = (char **)array->data;
        s32 i;

        for (i = 0; i < array->numElements; i++) {
            if (strings[i] != NULL) {
                rwFree(strings[i]);
            }
        }
    }
    if (array->data != NULL) {
        rwFree(array->data);
        array->data = NULL;
    }
}

/* Frees every array of a list. */
static inline void UserDataListDestroy(UserDataList *list) {
    if (list->userData != NULL) {
        s32 i;

        for (i = 0; i < list->numElements; i++) {
            UserDataArrayDestroy(&list->userData[i]);
        }
        rwFree(list->userData);
    }
    list->userData = NULL;
    list->numElements = 0;
}

/* A copy of a string, or NULL. */
static inline void UserDataStringCopy(char **dest, const char *source) {
    *dest = NULL;
    if (source != NULL) {
        *dest = (char *)rwAlloc(rwStringLength(source) + 1, 0x30002);
        if (*dest != NULL) {
            rwStringCopy(*dest, source);
        }
    }
}

 /* plugin offset */


u32 fn_80451F48(u32 object) {
    userDataModule.numInstances++;
    return object;
}

u32 fn_80451F5C(u32 object) {
    userDataModule.numInstances--;
    return object;
}

void *fn_80451F70(u32 *out, void *stream) {
    s32 count;
    if (fn_8047D868(stream, &count, 4) == 0) return NULL;
    if (count > 0) {
        out[0] = (u32)((void *(*)(u32, u32))*(u32 *)((u8 *)lbl_805F15B4 + 0x108))(count, 0x3011F);
        if (out[0] == 0) return NULL;
        if (fn_80480D4C(stream, (void *)out[0], count) == 0) return NULL;
    }
    if (fn_8047D868(stream, out + 1, 4) == 0) return NULL;
    if (fn_8047D868(stream, out + 2, 4) == 0) return NULL;

    switch (out[1]) {
    case 1:
        out[3] = (u32)((void *(*)(u32, u32))*(u32 *)((u8 *)lbl_805F15B4 + 0x108))(out[2] * 4, 0x3011F);
        if (out[3] == 0) return NULL;
        if (fn_8047D868(stream, (void *)out[3], out[2] * 4) == 0) return NULL;
        break;
    case 2:
        out[3] = (u32)((void *(*)(u32, u32))*(u32 *)((u8 *)lbl_805F15B4 + 0x108))(out[2] * 4, 0x3011F);
        if (out[3] == 0) return NULL;
        if (fn_8047D784(stream, (void *)out[3], out[2] * 4) == 0) return NULL;
        break;
    case 3: {
        out[3] = (u32)((void *(*)(u32, u32))*(u32 *)((u8 *)lbl_805F15B4 + 0x108))(out[2] * 4, 0x3011F);
        if (out[3] == 0) return NULL;
        { u32 *entries = (u32 *)out[3];
        { s32 i; for (i = 0; i < (s32)out[2]; i++) {
            if (fn_8047D868(stream, &count, 4) == 0) return NULL;
            if (count > 0) {
                entries[i] = (u32)((void *(*)(u32, u32))*(u32 *)((u8 *)lbl_805F15B4 + 0x108))(count, 0x3011F);
                if (entries[i] == 0) return NULL;
                if (fn_80480D4C(stream, (void *)entries[i], count) == 0) return NULL;
            } else {
                entries[i] = 0;
            }
        } }
        break;
     }}
    default:
        return NULL;
    }
    return stream;
}

void *fn_80452228(u32 *out, void *stream) {
    s32 size = out[0] != 0 ? ((u32 (*)(void *))*(u32 *)((u8 *)lbl_805F15B4 + 0xF4))((void *)out[0]) + 1 : 0;
    if (fn_8047D458(stream, &size, 4) == 0) return NULL;
    if (size > 0 && fn_80480ED0(stream, (void *)out[0], size) == 0) return NULL;
    if (fn_8047D458(stream, out + 1, 4) == 0) return NULL;
    if (fn_8047D458(stream, out + 2, 4) == 0) return NULL;

    switch (out[1]) {
    case 1:
        if (fn_8047D458(stream, (void *)out[3], out[2] * 4) == 0) return NULL;
        break;
    case 2:
        if (fn_8047D350(stream, (void *)out[3], out[2] * 4) == 0) return NULL;
        break;
    case 3: {
        u32 *entries = (u32 *)out[3];
        { s32 i; for (i = 0; i < (s32)out[2]; i++) {
            size = entries[i] != 0 ? ((u32 (*)(void *))*(u32 *)((u8 *)lbl_805F15B4 + 0xF4))((void *)entries[i]) + 1 : 0;
            if (fn_8047D458(stream, &size, 4) == 0) return NULL;
            if (size > 0 && fn_80480ED0(stream, (void *)entries[i], size) == 0) return NULL;
        } }
        break;
    }
    default:
        return NULL;
    }
    return stream;
}

/* Duplicates a string (the argument expressions are evaluated again, as a macro does). */
#define USERDATA_STRING_COPY(dest, source)                                              do {                                                                                    (dest) = NULL;                                                                      if ((source) != NULL) {                                                                 (dest) = (char *)rwAlloc(rwStringLength(source) + 1, 0x30002);                      if ((dest) != NULL) {                                                                   rwStringCopy((dest), (source));                                                 }                                                                               }                                                                               } while (0)

/* Copies one list into another (the copy callback's work): every array and string is duplicated. */
void fn_80452434(UserDataList *dest, const UserDataList *source) {
    UserDataArray *fromArrays;
    UserDataArray *from;
    UserDataArray *to;
    s32 i;
    s32 j;
    char **fromStrings;
    char **toStrings;
    u32 size;

    UserDataListDestroy(dest);
    dest->numElements = source->numElements;
    if (dest->numElements > 0) {
        dest->userData = (UserDataArray *)rwAlloc(dest->numElements * sizeof(UserDataArray), 0x3011F);
        for (i = 0; i < dest->numElements; i++) {
            from = &source->userData[i];
            to = &dest->userData[i];
            to->format = from->format;
            to->numElements = from->numElements;
            if (from->name != NULL) {
                USERDATA_STRING_COPY(to->name, from->name);
            }
            if (from->data != NULL) {
                size = to->numElements * fn_80452C98(to->format);
                to->data = rwAlloc(size, 0x3011F);
                if (to->format == 3) {
                    fromStrings = (char **)from->data;
                    toStrings = (char **)to->data;
                    for (j = 0; j < to->numElements; j++) {
                        if (fromStrings[j] == NULL) {
                            toStrings[j] = NULL;
                        } else {
                            USERDATA_STRING_COPY(toStrings[j], fromStrings[j]);
                        }
                    }
                } else {
                    memcpy(to->data, from->data, size);
                }
            }
        }
    }
}

u8 *fn_80452730(u8 *base, u32 offset) {
    u8 *target = base + offset;
    *(u32 *)target = 0;
    *(u32 *)(target + 4) = 0;
    return base;
}

/* Object destructor. */
u8 *fn_80452744(u8 *object, u32 offset)
{
    UserDataList *list = (UserDataList *)(object + offset);

    UserDataListDestroy(list);
    return object;
}

void *fn_80452860(u8 *object, u8 *other, u32 offset) {
    fn_80452434((UserDataList *)(object + offset), (UserDataList *)(other + offset));
    return object;
}

/* Reads a list: the count, then each array. */
static inline void *UserDataListStreamRead(UserDataList *list, void *stream) {
    if (list != NULL) {
        s32 numElements;
        s32 i;

        if (fn_8047D868(stream, &numElements, 4) != 0) {
            list->numElements = numElements;
            list->userData = (UserDataArray *)rwAlloc(list->numElements * sizeof(UserDataArray), 0x3011F);
            for (i = 0; i < list->numElements; i++) {
                stream = fn_80451F70((u32 *)&list->userData[i], stream);
            }
        }
    }
    return stream;
}

/* Object stream read callback. */
void *fn_80452898(void *stream, u32 size, u8 *object, u32 offset)
{
    UserDataList *list = (UserDataList *)(object + offset);

    UserDataListStreamRead(list, stream);
    return stream;
}

/* Writes the number of arrays that have a format, then those arrays. */
static inline void *UserDataListStreamWrite(UserDataList *list, void *stream) {
    if (list != NULL && list->numElements > 0) {
        u32 count = 0;
        s32 i;

        for (i = 0; i < list->numElements; i++) {
            if (list->userData[i].format != 0) {
                count++;
            }
        }
        if (count != 0 && fn_8047D458(stream, &count, 4) != 0) {
            for (i = 0; i < list->numElements; i++) {
                if (list->userData[i].format != 0) {
                    stream = fn_80452228((u32 *)&list->userData[i], stream);
                }
            }
        }
    }
    return stream;
}

/* Object stream write callback. */
void *fn_8045294C(void *stream, u32 size, u8 *object, u32 offset)
{
    UserDataList *list = (UserDataList *)(object + offset);

    UserDataListStreamWrite(list, stream);
    return stream;
}

/* The stream size of one array. */
static inline s32 UserDataArrayGetStreamSize(UserDataArray *array) {
    s32 i;
    char **strings;
    s32 size = 0;

    if (array != NULL) {
        size = 4;
        if (array->name != NULL) {
            size = rwStringLength(array->name) + 5;
        }
        size += 8;
        switch (array->format) {
        case 1:
            size += array->numElements * 4;
            break;
        case 2:
            size += array->numElements * 4;
            break;
        case 3:
            strings = (char **)array->data;
            {
            for (i = 0; i < array->numElements; i++) {
                size += 4;
                if (strings[i] != NULL) {
                    size = size + rwStringLength(strings[i]) + 1;
                }
            }
            }
            break;
        }
    }
    return size;
}

static inline s32 UserDataListGetStreamSize(UserDataList *list) {
    s32 i;
    s32 size = 0;

    if (list != NULL && list->numElements > 0) {

        for (i = 0; i < list->numElements; i++) {
            if (list->userData[i].format != 0) {
                size += UserDataArrayGetStreamSize(&list->userData[i]);
            }
        }
        if (size > 0) {
            size += 4;
        }
    }
    return size;
}

/* Object stream size callback. */
s32 fn_80452A40(u8 *object, u32 offset)
{
    UserDataList *list = (UserDataList *)(object + offset);
    return UserDataListGetStreamSize(list);
}

/* The number of arrays that hold data. */
static inline s32 UserDataListCount(UserDataList *list) {
    s32 count = 0;
    s32 i;

    for (i = 0; i < list->numElements; i++) {
        if (list->userData[i].data != NULL) {
            count++;
        }
    }
    return count;
}

/* RpGeometryGetUserDataArrayCount */
s32 fn_80452B90(u8 *object)
{
    UserDataList *list = (UserDataList *)(object + userDataGeometryOffset);
    return UserDataListCount(list);
}

u32 fn_80452BD8(u8 *object, s32 index) {
    object += userDataGeometryOffset;
    { s32 count = *(s32 *)object;
    if (index < count) {
        return *(u32 *)(object + 4) + index * 0x10;
    }
    return 0;
 }}

/* RpWorldSectorGetUserDataArrayCount */
s32 fn_80452C04(u8 *object)
{
    UserDataList *list = (UserDataList *)(object + userDataSectorOffset);
    return UserDataListCount(list);
}

u32 fn_80452C4C(u8 *object, s32 index) {
    object += userDataSectorOffset;
    { s32 count = *(s32 *)object;
    if (index < count) {
        return *(u32 *)(object + 4) + index * 0x10;
    }
    return 0;
 }}

u32 fn_80452C78(u32 *object) {
    return object[0];
}

u32 fn_80452C80(u32 *object) {
    return object[1];
}

u32 fn_80452C88(u32 *object, u32 index) {
    return ((u32 *)object[3])[index];
}

/* Byte width of one value for an entry of the given type. */
u32 fn_80452C98(s32 type)
{
    switch (type) {
    case 1:
        return 4;
    case 2:
        return 4;
    case 3:
        return 4;
    default:
        return 0;
    }
}

/* Install the chunk 0x11F plugin. Each registration hands back an id, and the
   guard after a pair tests the *previous* pair's global, which is what retail
   does. The later groups reuse the same callbacks over and over; the pattern is
   a repeating window across the interpolators built below. */
u32 fn_80452CDC(void)
{
    if (fn_80485D78(0, 0x11F, fn_80451F48, fn_80451F5C) < 0) {
        return 0;
    }
    userDataGeometryOffset = fn_8045994C(8, 0x11F, fn_80452730, fn_80452744, fn_80452860);
    if ((s32)userDataGeometryOffset < 0) {
        return 0;
    }
    userDataGeometryStreamOffset = fn_8045998C(0x11F, fn_80452898, fn_8045294C, fn_80452A40);
    if ((s32)userDataGeometryStreamOffset < 0) {
        return 0;
    }
    userDataSectorOffset = fn_8045F2F8(8, 0x11F, fn_80452730, fn_80452744, fn_80452860);
    if ((s32)userDataGeometryOffset < 0) {
        return 0;
    }
    userDataSectorStreamOffset = fn_8045F338(0x11F, fn_80452898, fn_8045294C, fn_80452A40);
    if ((s32)userDataGeometryStreamOffset < 0) {
        return 0;
    }
    userDataFrameOffset = fn_804878D4(8, 0x11F, fn_80452730, fn_80452744, fn_80452860);
    if ((s32)userDataFrameOffset < 0) {
        return 0;
    }
    userDataFrameStreamOffset = fn_804836EC(0x11F, fn_80452898, fn_8045294C, fn_80452A40);
    if ((s32)userDataFrameStreamOffset < 0) {
        return 0;
    }
    userDataCameraOffset = fn_804859B8(8, 0x11F, fn_80452730, fn_80452744, fn_80452860);
    if ((s32)userDataCameraOffset < 0) {
        return 0;
    }
    userDataCameraStreamOffset = fn_80483554(0x11F, fn_80452898, fn_8045294C, fn_80452A40);
    if ((s32)userDataCameraStreamOffset < 0) {
        return 0;
    }
    userDataLightOffset = fn_8045AAA0(8, 0x11F, fn_80452730, fn_80452744, fn_80452860);
    if ((s32)userDataLightOffset < 0) {
        return 0;
    }
    userDataLightStreamOffset = fn_8045AAE0(0x11F, fn_80452898, fn_8045294C, fn_80452A40);
    if ((s32)userDataLightStreamOffset < 0) {
        return 0;
    }
    userDataMaterialOffset = fn_8045B2CC(8, 0x11F, fn_80452730, fn_80452744, fn_80452860);
    if ((s32)userDataMaterialOffset < 0) {
        return 0;
    }
    userDataMaterialStreamOffset = fn_8045B30C(0x11F, fn_80452898, fn_8045294C, fn_80452A40);
    if ((s32)userDataMaterialStreamOffset < 0) {
        return 0;
    }
    userDataTextureOffset = fn_8048D280(8, 0x11F, fn_80452730, fn_80452744, fn_80452860);
    if ((s32)userDataTextureOffset < 0) {
        return 0;
    }
    userDataTextureStreamOffset = fn_80483AFC(0x11F, fn_80452898, fn_8045294C, fn_80452A40);
    if ((s32)userDataTextureStreamOffset < 0) {
        return 0;
    }
    return 1;
}

