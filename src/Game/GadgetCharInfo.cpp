/* GadgetCharInfo (class name from RTTI): the character info of a gadget (see
 * Game/GadgetCharInfo.h), with the functions that build one for a gadget, give it a command unit
 * and turn its placement into a matrix. File name inferred from the class. Names of the free
 * functions are placeholders; their behaviour is inferred. Definitions are in reverse address
 * order (deferred inlining emits functions last-defined first). */
#include "Game/CharCommand.h"
#include "Game/CharCommandUnit.h"
#include "Game/GadgetCharInfo.h"
#include "boost/detail/sp_counted_base_impl.hpp"
#include "boost/shared_ptr.hpp"

extern "C" {
/* Sets the character info's name (behaviour inferred). */
void fn_801DC984(CharInfo *charInfo, const char *name);
/* Hands command to the command unit (behaviour inferred). */
void fn_801D9C0C(boost::shared_ptr<CharCommandUnit> unit, CharCommand *command);
/* Matrix helpers: identity, translate, and rotations about Y, X and Z (as their callers use
 * them). */
void fn_8040B6F8(Mtx34f *matrix);
void fn_8040B59C(Mtx34f *matrix, const Vec3f *translation);
void fn_8040B848(Mtx34f *matrix, float angle);
void fn_8040B884(Mtx34f *matrix, float angle);
void fn_8040B80C(Mtx34f *matrix, float angle);
}

/* Sets the word at 0x5C (meaning unknown; name inferred). */
inline void SetField5C(CharInfo *charInfo, s32 value) {
    charInfo->field_0x5C = value;
}

/* Not called anywhere: the linker strips it. Retail's .sbss holds the empty checked_deleter
 * temporary of the CharCommandUnit shared_ptr before the GadgetCharInfo one; GC/2.7 allocates them
 * in that order only when a CharCommandUnit shared_ptr is built this early in the file (inferred;
 * the original function is unknown, and the name is a placeholder). */
static void UnkCommandUnitUser(GadgetCharInfo *charInfo) {
    charInfo->m_commandUnit = boost::shared_ptr<CharCommandUnit>(new CharCommandUnit);
}

GadgetCharInfo::GadgetCharInfo(int type) : CharInfo(type) {
    field_0x30 = 2;
    field_0x5C = 4;
}

GadgetCharInfo::~GadgetCharInfo() {}

void GadgetCharInfo::PushCommand(CharCommand *command) {
    if (m_commandUnit.get() != 0) {
        fn_801D9C0C(m_commandUnit, command);
    } else {
        delete command;
    }
}

/* Gives charInfo a command unit unless it has one. */
extern "C" void fn_8015CF7C(GadgetCharInfo *charInfo) {
    if (charInfo->m_commandUnit.get() == 0) {
        charInfo->m_commandUnit = boost::shared_ptr<CharCommandUnit>(new CharCommandUnit);
    }
}

/* Sets matrix to charInfo's placement, rotated in the order its rotation mode (0x30) selects. */
extern "C" void fn_8015CEA0(CharInfo *charInfo, Mtx34f *matrix) {
    if (matrix != 0) {
        fn_8040B6F8(matrix);
        fn_8040B59C(matrix, &charInfo->m_position);
        /* Retail lays out case 2 first. */
        switch (charInfo->field_0x30) {
        case 2:
            fn_8040B848(matrix, charInfo->m_rotation.y);
            fn_8040B884(matrix, charInfo->m_rotation.x);
            fn_8040B80C(matrix, charInfo->m_rotation.z);
            break;
        case 0:
            fn_8040B80C(matrix, charInfo->m_rotation.z);
            fn_8040B848(matrix, charInfo->m_rotation.y);
            fn_8040B884(matrix, charInfo->m_rotation.x);
            break;
        case 1:
            fn_8040B80C(matrix, charInfo->m_rotation.z);
            fn_8040B884(matrix, charInfo->m_rotation.x);
            fn_8040B848(matrix, charInfo->m_rotation.y);
            break;
        }
    }
}

/* Builds a gadget character info of the given type. */
extern "C" boost::shared_ptr<GadgetCharInfo> fn_8015CE20(int type) {
    boost::shared_ptr<GadgetCharInfo> charInfo = boost::shared_ptr<GadgetCharInfo>(new GadgetCharInfo(type));
    return charInfo;
}

/* Builds a gadget character info of the given type and sets its word at 0x5C. */
extern "C" boost::shared_ptr<GadgetCharInfo> fn_8015CD90(int type, int arg1) {
    boost::shared_ptr<GadgetCharInfo> charInfo = boost::shared_ptr<GadgetCharInfo>(new GadgetCharInfo(type));
    SetField5C(charInfo.operator->(), arg1);
    return charInfo;
}

/* Builds a gadget character info of the given type named name and sets its word at 0x5C. */
extern "C" boost::shared_ptr<GadgetCharInfo> fn_8015CB88(int type, int arg1, const char *name) {
    boost::shared_ptr<GadgetCharInfo> charInfo = boost::shared_ptr<GadgetCharInfo>(new GadgetCharInfo(type));
    SetField5C(charInfo.operator->(), arg1);
    fn_801DC984(charInfo.get(), name);
    return charInfo;
}
