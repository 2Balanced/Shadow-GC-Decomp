/* The character model files (class names inferred; the game has no RTTI for them): a table of the
 * twenty character/<name>.one files, each loaded on first use and unloaded when its last user lets
 * go, and the set-up and clean-up of the twelve characters' tasks, which hold one reference each.
 * File name inferred. Definitions are in reverse address order (deferred inlining emits functions
 * last-defined first). */
#include "Game/CharacterFiles.h"
#include "Game/ResourceOneFile.h"
#include "Game/WeaponBase.h"
#include "boost/scoped_ptr.hpp"

extern "C" {
/* The loader's state of a slot (3 loaded; see BKTankController.cpp), and adding a resource to the
 * loader, which returns its slot (behaviours inferred). */
s32 fn_80012408(void *loader, s32 slot);
s32 fn_800128D8(void *loader, ResourceOneFile *resource);
}

/* The file index of each character type (meaning inferred). */
s32 lbl_8054D8D8[] = {
    0x14, 0x0, 0x1, 0x2, 0x3, 0x4, 0x5, 0x6, 0x7, 0x8, 0x9, 0xA, 0xB, 0x0, 0xC, 0xF, 0x10, 0x11, 0x12, 0x13,
};

/* The files. */
const char *lbl_8054D928[] = {
    "character/sonic.one",
    "character/tails.one",
    "character/knuckles.one",
    "character/amy.one",
    "character/rouge.one",
    "character/omega.one",
    "character/vector.one",
    "character/espio.one",
    "character/maria.one",
    "character/bee.one",
    "character/eggmonitor.one",
    "character/doomseye.one",
    "character/shadow.one",
    "character/eshadow.one",
    "character/supershadow.one",
    "character/shadow2py.one",
    "character/shadow2pmtlp.one",
    "character/shadow2pmtlb.one",
    "character/shadow2pcnno.one",
    "character/shadow2pcnng.one",
};

CharacterFile::CharacterFile() : m_handle(-1), m_refCount(0), m_acquired(false) {}

CharacterFile::~CharacterFile() {}

void CharacterFile::Load() {
    if (m_handle == -1) {
        void *loader = fn_800122D4();
        m_handle = fn_800128D8(loader, new ResourceOneFile(lbl_8054D928[m_index], false));
    }
    m_refCount++;
}

void CharacterFile::Unload() {
    m_refCount--;
    if (m_refCount <= 0) {
        fn_800127F8(fn_800122D4(), m_handle);
        m_handle = -1;
    }
}

void CharacterFile::Acquire() {
    if (!m_acquired) {
        Load();
        m_acquired = true;
    }
}

void CharacterFile::Release() {
    if (m_acquired) {
        Unload();
        m_acquired = false;
    }
}

CharacterFileTable::CharacterFileTable() {
    for (int i = 0; i < 20; i++) {
        m_files[i].m_index = i;
    }
}

CharacterFileManager::CharacterFileManager() : m_table(new CharacterFileTable) {}

CharacterFileManager::~CharacterFileManager() {}

bool CharacterFileManager::IsLoaded(int index) {
    void *loader = fn_800122D4();
    s32 handle = GetHandle(index);
    switch (fn_80012408(loader, handle)) {
    case 3:
        return true;
    case 4:
    default:
        return false;
    }
}

void CharacterFileManager::Load(int index) {
    m_table->m_files[index].Load();
}

void CharacterFileManager::Unload(int index) {
    m_table->m_files[index].Unload();
}

void CharacterFileManager::Acquire(int index) {
    m_table->m_files[index].Acquire();
}

void CharacterFileManager::Release(int index) {
    m_table->m_files[index].Release();
}

void CharacterFileManager::ReleaseAll() {
    for (int i = 0; i < 20; i++) {
        m_table->m_files[i].Release();
    }
}

extern "C" s32 fn_8027A994(int type) {
    return lbl_8054D8D8[type];
}

/* The character tasks' set-up and clean-up (names inferred). */
static void AcquireCharacter(int index) {
    fn_80078C88()->Acquire(index);
}

static void ReleaseCharacter(int index) {
    fn_80078C88()->Release(index);
}

static void InitializeSonic() {
    AcquireCharacter(0);
}

static void InitializeTails() {
    AcquireCharacter(1);
}

static void InitializeKnuckles() {
    AcquireCharacter(2);
}

static void InitializeAmy() {
    AcquireCharacter(3);
}

static void InitializeRouge() {
    AcquireCharacter(4);
}

static void InitializeOmega() {
    AcquireCharacter(5);
}

static void InitializeVector() {
    AcquireCharacter(6);
}

static void InitializeEspio() {
    AcquireCharacter(7);
}

static void InitializeMaria() {
    AcquireCharacter(8);
}

static void InitializeCharmy() {
    AcquireCharacter(9);
}

static void InitializeEggman() {
    AcquireCharacter(10);
}

static void InitializeDoomsEye() {
    AcquireCharacter(11);
}

static void FinalizeSonic() {
    ReleaseCharacter(0);
}

static void FinalizeTails() {
    ReleaseCharacter(1);
}

static void FinalizeKnuckles() {
    ReleaseCharacter(2);
}

static void FinalizeAmy() {
    ReleaseCharacter(3);
}

static void FinalizeRouge() {
    ReleaseCharacter(4);
}

static void FinalizeOmega() {
    ReleaseCharacter(5);
}

static void FinalizeVector() {
    ReleaseCharacter(6);
}

static void FinalizeEspio() {
    ReleaseCharacter(7);
}

static void FinalizeMaria() {
    ReleaseCharacter(8);
}

static void FinalizeCharmy() {
    ReleaseCharacter(9);
}

static void FinalizeEggman() {
    ReleaseCharacter(10);
}

static void FinalizeDoomsEye() {
    ReleaseCharacter(11);
}

UnkWeaponTaskInfo lbl_8054D978 = {
    "Sonic", InitializeSonic, FinalizeSonic, 0, 0, 0x80, 0x01910400, 0, 0,
};

UnkWeaponTaskInfo lbl_8054D99C = {
    "Tails", InitializeTails, FinalizeTails, 0, 0, 0x80, 0x01920400, 0, 0,
};

UnkWeaponTaskInfo lbl_8054D9C0 = {
    "Knuckles", InitializeKnuckles, FinalizeKnuckles, 0, 0, 0x80, 0x01930400, 0, 0,
};

UnkWeaponTaskInfo lbl_8054D9E4 = {
    "Amy", InitializeAmy, FinalizeAmy, 0, 0, 0x80, 0x01940400, 0, 0,
};

UnkWeaponTaskInfo lbl_8054DA08 = {
    "Rouge", InitializeRouge, FinalizeRouge, 0, 0, 0x80, 0x01950400, 0, 0,
};

UnkWeaponTaskInfo lbl_8054DA2C = {
    "Omega", InitializeOmega, FinalizeOmega, 0, 0, 0x80, 0x01960400, 0, 0,
};

UnkWeaponTaskInfo lbl_8054DA50 = {
    "Vector", InitializeVector, FinalizeVector, 0, 0, 0x80, 0x01970400, 0, 0,
};

UnkWeaponTaskInfo lbl_8054DA74 = {
    "Espio", InitializeEspio, FinalizeEspio, 0, 0, 0x80, 0x01980400, 0, 0,
};

UnkWeaponTaskInfo lbl_8054DA98 = {
    "Maria", InitializeMaria, FinalizeMaria, 0, 0, 0x80, 0x01990400, 0, 0,
};

UnkWeaponTaskInfo lbl_8054DABC = {
    "Charmy", InitializeCharmy, FinalizeCharmy, 0, 0, 0x80, 0x019A0400, 0, 0,
};

UnkWeaponTaskInfo lbl_8054DAE0 = {
    "Eggman", InitializeEggman, FinalizeEggman, 0, 0, 0x80, 0x019B0400, 0, 0,
};

UnkWeaponTaskInfo lbl_8054DB04 = {
    "DoomsEye", InitializeDoomsEye, FinalizeDoomsEye, 0, 0, 0x80, 0x019C0400, 0, 0,
};
