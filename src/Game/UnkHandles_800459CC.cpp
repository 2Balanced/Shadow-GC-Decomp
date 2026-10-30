/* The functions other units reach the handle table with (Game/UnkHandleTable.h): the table itself
 * (a function-local static), and a HandleUser's handle given, looked up and taken back. The file
 * name and the names here are inferred. Definitions are in reverse address order (deferred
 * inlining emits functions last-defined first). */
#include "Game/UnkHandleTable.h"
#include <dolphin/types.h>

/* The table's instance (inlined into fn_80045A00). */
inline UnkHandleTable_80045A00 &UnkHandleTable_80045A00_Instance() {
    static UnkHandleTable_80045A00 s_table;
    return s_table;
}

UnkHandleTable_80045A00 *UnkHandleTable_80045A00::s_cached;

/* The HandleUser of a handle, or 0 once the handle is stale. */
extern "C" HandleUser *fn_80045A98(const UnkHandle_80045A98 &handle) {
    UnkHandle_80045A98 copy = handle;
    return fn_80045A00()->Get(copy);
}

/* Gives user a handle. */
extern "C" bool fn_80045A64(HandleUser &user) {
    return fn_80045A00()->Register(&user);
}

/* The handle table. */
extern "C" UnkHandleTable_80045A00 *fn_80045A00(void) {
    if (UnkHandleTable_80045A00::s_cached == 0) {
        UnkHandleTable_80045A00::s_cached = &UnkHandleTable_80045A00_Instance();
    }
    return UnkHandleTable_80045A00::s_cached;
}

/* Takes user's handle back. */
extern "C" bool fn_800459CC(HandleUser &user) {
    return fn_80045A00()->Release(*reinterpret_cast<UnkHandle_80045A98 *>(&user.field_0x4));
}
