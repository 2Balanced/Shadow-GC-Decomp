/* Vehicle::TVehicleCar (class name from RTTI): the cars, vehicle kinds 1 to 4 ("Normal Car", "Open
 * Car", "Armed Car" and "Bike"; vehicle types 0x46 to 0x49). The car builds its model from parts
 * (a model table per handling type) and leaves the driving to a Vehicle::TVehicleCarMove it owns
 * through a shared_ptr; the files the cars share load once (Game/VehicleCarResource.cpp).
 * Member and function names are inferred. Definitions are in reverse address order (deferred
 * inlining emits functions last-defined first). The records' array (lbl_8053B948) is indexed by
 * kind, so the vehicle and gadget tables refer to its elements (Game/TVehicleCarRecord.h). */
#include <stdio.h>
/* The string's out-of-line copy constructor (see the MSL_C++ stand-in). */
#define MSL_STRING_COPY
#include <string>

#define CHARCOMMAND_IMPLEMENTATION
#include "Game/CharCommand.h"
#include "Game/GadgetClassDesc.h"
#include "Game/MathTypes.h"
#include "Game/TVehicleBase.h"
#include "Game/TVehicleCar.h"
#include "Game/TVehicleCarMove.h"
#include "Game/TVehicleCarRecord.h"
#include "Game/VehicleCarResource.h"
#include "boost/shared_ptr.hpp"
#include <dolphin/types.h>
#include "size_assert.h"

using Vehicle::CarModelDesc;
using Vehicle::CarPart;
using Vehicle::TVehicleBase;
using Vehicle::TVehicleCar;
using Vehicle::TVehicleCarMove;

/* An object that keeps a clump in step with a motion (class name unknown; as in
 * Game/BlackDoomCtrlUnit.cpp). */
class UnkClumpUpdater_80015A7C {
  public:
    UnkClumpUpdater_80015A7C(RpClump *clump, _stMtnPlayWork *motion);
    ~UnkClumpUpdater_80015A7C();

    /* 0x00 */ u8 field_0x0[0x1C];
};

/* The world of each stage area (as in Game/BlackDoomCtrlUnit.cpp). */
struct UnkWorldEntry_8051D208 {
    /* 0x00 */ u8 field_0x0[0x10];
    /* 0x10 */ void *m_world;
    /* 0x14 */ u8 field_0x14[0x18 - 0x14];
};
extern UnkWorldEntry_8051D208 lbl_8051D208[];

extern "C" {
/* Debug output, resource loading and the vehicle table (see Game/VehicleWalker.cpp). */
void fn_803A953C(const char *format, ...);
void fn_8006B50C(int id, int arg1);
void fn_801B4850(int kind);

/* The loader, its files and their offsets (see Game/BkLarva.cpp). */
void *fn_800122D4(void);
void *fn_8001277C(void *loader, int slot, int id);
void *fn_80012684(void *loader, int slot, int id);
int fn_80012608(void *loader, int slot, int id);

/* Clumps, their worlds and motions (see Game/BlackDoomCtrlUnit.cpp and Game/BkLarva.cpp). */
RpClump *fn_80457090(void *data);
void fn_80457468(RpClump *clump);
void *fn_8046256C(RpClump *clump);
void fn_804623EC(void *world, RpClump *clump);
void fn_804622FC(void *world, RpClump *clump);
/* The stage areas' worlds (see Game/BlackDoomCtrlUnit.cpp; the worlds are lbl_8051D208). */
void *fn_8003A840(void);
_stMtnPlayWork *fn_8041B908(void *data, int arg1);
void fn_8041B884(_stMtnPlayWork *work);
void fn_80417568(_stMtnPlayWork *work, u16 motion);

/* Meaning unknown (stored at 0x170). */
s32 fn_8040EA90(int arg);

/* TVehicleBase's kind and default command handling (see Game/VehicleWalker.cpp). */
s32 fn_801B6938(TVehicleBase *vehicle);
void fn_801B6860(TVehicleBase *vehicle, CharCommand *command);
}

/* Puts clump in world (name inferred; as in Game/BlackDoomCtrlUnit.cpp, with the arguments the
 * other way round, which gives retail's register use). */
inline void SetClumpWorld(void *world, RpClump *clump) {
    void *current = fn_8046256C(clump);
    if (current != world) {
        if (current != 0) {
            fn_804623EC(current, clump);
        }
        fn_804622FC(world, clump);
    }
}

/* The world of a stage area (name inferred). */
inline void *GetWorld(int area) {
    fn_8003A840();
    return lbl_8051D208[area].m_world;
}

/* Loads a file, if there is one (name inferred). */
inline void LoadFile(int id) {
    if (id >= 0) {
        fn_8006B50C(id, 0);
    }
}

/* The model of each handling type (Game/TVehicleCarMove.cpp reads it too). */
const CarModelDesc lbl_804CDAF8[4] = {
    {
        96,
        {8},
        {{34, 6, -1, -1, 1, 0}, {38, 6, -1, -1, 1, 0}, {-1, 6, -1, -1, 1, 0}},
        {
            {{31, 6, -1, -1, 1, 1}, {35, 13, -1, -1, 1, 0}, {39, 11, -1, -1, 1, 0}},
            {{32, 6, 8, 1, 1, 1}, {36, 13, -1, -1, 1, 0}, {40, 11, -1, -1, 1, 0}},
            {{33, 6, 9, 2, 1, 1}, {37, 13, -1, -1, 1, 0}, {41, 11, -1, -1, 1, 0}},
        },
        {
            {1, 1, 2}, {1, 1, 1}, {1, 1, 4}, {1, 1, 3},
            {1, 1, 12}, {1, 1, 11}, {1, 1, 14}, {1, 1, 13},
            {1, 0, 15}, {1, 0, 10}, {0, 1, 20}, {1, -1, -1},
        },
    },
    {
        99,
        {13},
        {{51, 6, -1, -1, 1, 0}, {55, 6, -1, -1, 1, 0}, {-1, 6, -1, -1, 1, 0}},
        {
            {{48, 6, -1, -1, 1, 1}, {52, 13, -1, -1, 1, 0}, {56, 11, -1, -1, 1, 0}},
            {{49, 6, 13, 1, 1, 1}, {53, 13, -1, -1, 1, 0}, {57, 11, -1, -1, 1, 0}},
            {{50, 6, 14, 2, 1, 1}, {54, 13, -1, -1, 1, 0}, {58, 11, -1, -1, 1, 0}},
        },
        {
            {1, 1, 2}, {1, 1, 1}, {1, 1, 4}, {1, 1, 3},
            {1, 1, 12}, {1, 1, 11}, {1, 1, 14}, {1, 1, 13},
            {1, 0, 15}, {1, 0, 10}, {0, 1, 20}, {1, -1, -1},
        },
    },
    {
        98,
        {18},
        {{71, 4, -1, -1, 1, 0}, {75, 6, -1, -1, 1, 0}, {-1, 6, -1, -1, 1, 0}},
        {
            {{68, 6, -1, -1, 1, 1}, {72, 13, -1, -1, 1, 0}, {-1, 11, -1, -1, 1, 0}},
            {{69, 6, 18, 1, 1, 1}, {73, 13, -1, -1, 1, 0}, {-1, 11, -1, -1, 1, 0}},
            {{70, 6, 19, 2, 1, 1}, {74, 13, -1, -1, 1, 0}, {-1, 11, -1, -1, 1, 0}},
        },
        {
            {1, 1, 2}, {1, 1, 1}, {1, 1, 4}, {1, 1, 3},
            {1, 1, 12}, {1, 1, 11}, {1, 1, 14}, {1, 1, 13},
            {1, 0, 15}, {1, 0, 10}, {0, 1, 20}, {1, -1, -1},
        },
    },
    {
        94,
        {20},
        {{76, 6, -1, -1, 1, 0}, {80, 6, -1, -1, 1, 0}, {77, 13, -1, -1, 1, 0}},
        {
            {{-1, 13, -1, -1, 1, 1}, {79, 13, -1, -1, 1, 0}, {78, 11, -1, -1, 1, 0}},
            {{-1, 13, -1, -1, 1, 1}, {79, 13, -1, -1, 1, 0}, {78, 11, -1, -1, 1, 0}},
            {{-1, 13, -1, -1, 1, 1}, {79, 13, -1, -1, 1, 0}, {78, 11, -1, -1, 1, 0}},
        },
        {
            {1, 1, 2}, {1, 1, 1}, {1, 1, 4}, {1, 1, 3},
            {1, 1, 12}, {1, 1, 11}, {1, 1, 14}, {1, 1, 13},
            {1, 0, 15}, {1, 0, 10}, {0, 1, 20}, {1, 0, 7},
        },
    },
};

static void InitializeNormalCar(void);
static void FinalizeNormalCar(void);
static void InitializeOpenCar(void);
static void FinalizeOpenCar(void);
static void InitializeArmedCar(void);
static void FinalizeArmedCar(void);
static void InitializeBike(void);
static void FinalizeBike(void);
static TVehicleBase *CreateCar(int kind);

UnkCarRecord_8053B948 lbl_8053B948[4] = {
    {
        {"Noraml Car", InitializeNormalCar, FinalizeNormalCar, 0, 0, 0, 0x0046, 1, 0, 0x0A, 0},
        "Normal Car",
        CreateCar,
    },
    {
        {"Open Car", InitializeOpenCar, FinalizeOpenCar, 0, 0, 0, 0x0047, 1, 0, 0x0A, 0},
        "Open Car",
        CreateCar,
    },
    {
        {"Armed Car", InitializeArmedCar, FinalizeArmedCar, 0, 0, 0, 0x0048, 1, 0, 0x0A, 0},
        "Armed Car",
        CreateCar,
    },
    {
        {"Bike", InitializeBike, FinalizeBike, 0, 0, 0, 0x0049, 1, 0, 0x0A, 0},
        "Bike",
        CreateCar,
    },
};

TVehicleCar::TVehicleCar(int kind) : TVehicleBase((void *)kind), m_started(0) {
    memset(&m_base, 0, 0xC8);
    Reset();
    m_move = boost::shared_ptr<TVehicleCarMove>(new TVehicleCarMove(this, TVehicleCarMove::ToType(kind)));
    field_0x30 = 1;
}

TVehicleCar::~TVehicleCar() {
    Release();
}

void TVehicleCar::OnCommand(CharCommand *command) {
    std::string name = command->vf_0x10();
    m_move.get()->OnCommand(command);
    if (command->Flags() & 2) {
        return;
    }
    fn_801B6860(this, command);
}

void TVehicleCar::SetPlacement(const Vec3f *position, const Vec3f *rotation) {
    TVehicleBase::SetPlacement(position, rotation);
    m_move.get()->SetPlacement(position, rotation);
}

void TVehicleCar::Tick(float dt) {
    if (m_started == 0) {
        m_move.get()->Start();
        m_started = 1;
    }
    m_move.get()->Tick(dt);
    m_move.get()->UpdateModel();
    TVehicleBase::SetPlacement(&m_move.get()->m_position, &m_move.get()->m_rotation);
}

void TVehicleCar::GetSeat(Vec3f *position, Vec3f *rotation) {
    if (position != 0) {
        fn_800091D0(position, &m_move.get()->m_body.m_position);
    }
    if (rotation != 0) {
        *rotation = m_move->GetBody()->m_rotation;
    }
}

void TVehicleCar::OnGetOn() {
    TVehicleBase::OnGetOn();
    m_move.get()->SetCamera(2, 0);
}

void TVehicleCar::OnGetOff() {
    TVehicleBase::OnGetOff();
    m_move.get()->SetCamera(1, 0);
}

void TVehicleCar::Reset() {
    Release();
    field_0x170 = fn_8040EA90(5);
    LoadMotion();
    CreateModel(0);
}

void TVehicleCar::Release() {
    DestroyModel();
}

void TVehicleCar::LoadMotion() {
    void *loader = fn_800122D4();
    const CarModelDesc &desc = lbl_804CDAF8[fn_801B6938(this) - 1];
    if (desc.m_files[0] >= 0) {
        m_motionData[0] = fn_80012684(loader, 3, desc.m_files[0]);
    }
    if (desc.m_base > 0) {
        m_base = fn_80012608(loader, 3, desc.m_base);
    }
}

void TVehicleCar::CreateModel(int row) {
    CreateParts();
    CreateRow(row);
}

void TVehicleCar::DestroyModel() {
    DestroyParts();
    for (int i = 0; i < 3; i++) {
        DestroyRow(i);
    }
}

void TVehicleCar::CreatePart(int index) {
    void *loader = fn_800122D4();
    const CarModelDesc &desc = lbl_804CDAF8[fn_801B6938(this) - 1];
    if (m_parts[index].m_clump == 0 && desc.m_parts[index].m_model >= 0) {
        void *data = fn_8001277C(loader, 3, desc.m_parts[index].m_model);
        if (data != 0) {
            m_parts[index].m_clump = fn_80457090(data);
            m_parts[index].m_world = GetWorld(desc.m_parts[index].m_area);
            SetClumpWorld(m_parts[index].m_world, m_parts[index].m_clump);
            if (desc.m_parts[index].m_motion >= 0) {
                int motion = m_base + desc.m_parts[index].m_motion;
                m_parts[index].m_motion = fn_8041B908(m_motionData[index], 1);
                fn_80417568(m_parts[index].m_motion, motion);
                m_parts[index].m_updater =
                    new UnkClumpUpdater_80015A7C(m_parts[index].m_clump, m_parts[index].m_motion);
            } else {
                m_parts[index].m_updater = 0;
                m_parts[index].m_motion = 0;
            }
        }
    }
}

void TVehicleCar::CreateParts() {
    const CarModelDesc &desc = lbl_804CDAF8[fn_801B6938(this) - 1];
    for (int i = 0; i < 3; i++) {
        if (desc.m_parts[i].m_enabled) {
            CreatePart(i);
        }
    }
}

void TVehicleCar::CreateRowPart(int row, int index) {
    void *loader = fn_800122D4();
    const CarModelDesc &desc = lbl_804CDAF8[fn_801B6938(this) - 1];
    if (m_rows[row][index].m_clump == 0 && desc.m_rows[row][index].m_model >= 0) {
        void *data = fn_8001277C(loader, 3, desc.m_rows[row][index].m_model);
        if (data != 0) {
            m_rows[row][index].m_clump = fn_80457090(data);
            m_rows[row][index].m_world = GetWorld(desc.m_rows[row][index].m_area);
            SetClumpWorld(m_rows[row][index].m_world, m_rows[row][index].m_clump);
            if (desc.m_rows[row][index].m_motion >= 0) {
                int motion = m_base + desc.m_rows[row][index].m_motion;
                m_rows[row][index].m_motion = fn_8041B908(m_motionData[index], 1);
                fn_80417568(m_rows[row][index].m_motion, motion);
                m_rows[row][index].m_updater =
                    new UnkClumpUpdater_80015A7C(m_rows[row][index].m_clump, m_rows[row][index].m_motion);
            } else {
                m_rows[row][index].m_updater = 0;
                m_rows[row][index].m_motion = 0;
            }
        }
    }
}

void TVehicleCar::CreateRow(int row) {
    const CarModelDesc &desc = lbl_804CDAF8[fn_801B6938(this) - 1];
    for (int i = 0; i < 3; i++) {
        if (desc.m_rows[row][i].m_enabled) {
            CreateRowPart(row, i);
        }
    }
}

void TVehicleCar::DestroyParts() {
    for (int i = 0; i < 3; i++) {
        if (m_parts[i].m_clump != 0) {
            if (m_parts[i].m_updater != 0) {
                delete m_parts[i].m_updater;
                m_parts[i].m_updater = 0;
            }
            if (m_parts[i].m_motion != 0) {
                fn_8041B884(m_parts[i].m_motion);
                m_parts[i].m_motion = 0;
            }
            m_parts[i].m_world = 0;
            void *world = fn_8046256C(m_parts[i].m_clump);
            if (world != 0) {
                fn_804623EC(world, m_parts[i].m_clump);
            }
            fn_80457468(m_parts[i].m_clump);
            m_parts[i].m_clump = 0;
        }
    }
}

void TVehicleCar::DestroyRow(int row) {
    for (int i = 0; i < 3; i++) {
        if (m_rows[row][i].m_clump != 0) {
            if (m_rows[row][i].m_updater != 0) {
                delete m_rows[row][i].m_updater;
                m_rows[row][i].m_updater = 0;
            }
            if (m_rows[row][i].m_motion != 0) {
                fn_8041B884(m_rows[row][i].m_motion);
                m_rows[row][i].m_motion = 0;
            }
            m_rows[row][i].m_world = 0;
            void *world = fn_8046256C(m_rows[row][i].m_clump);
            if (world != 0) {
                fn_804623EC(world, m_rows[row][i].m_clump);
            }
            fn_80457468(m_rows[row][i].m_clump);
            m_rows[row][i].m_clump = 0;
        }
    }
}

static void Initialize(int kind);
static void Finalize(int kind);

static void InitializeNormalCar(void) {
    TVehicleCarMove::Initialize(1);
    Initialize(1);
}

static void InitializeOpenCar(void) {
    TVehicleCarMove::Initialize(2);
    Initialize(2);
}

static void InitializeArmedCar(void) {
    TVehicleCarMove::Initialize(3);
    Initialize(3);
}

static void InitializeBike(void) {
    TVehicleCarMove::Initialize(4);
    Initialize(4);
}

/* Loads a kind's files (name inferred). */
static void Initialize(int kind) {
    fn_803A953C("%s::Initialize\n", lbl_8053B948[kind - 1].desc.typeName);
    fn_801B4850(kind);
    UnkCarResource_801B9950::GetInstance()->Load();
    int i;
    int part;
    int row;
    for (i = 0; i < 3; i++) {
        LoadFile(lbl_804CDAF8[kind - 1].m_files[i]);
    }
    fn_8006B50C(lbl_804CDAF8[kind - 1].m_base, 0);
    for (row = 0; row < 3; row++) {
        for (part = 0; part < 3; part++) {
            LoadFile(lbl_804CDAF8[kind - 1].m_rows[row][part].m_model);
            LoadFile(lbl_804CDAF8[kind - 1].m_rows[row][part].m_file);
        }
    }
    for (i = 0; i < 3; i++) {
        LoadFile(lbl_804CDAF8[kind - 1].m_parts[i].m_model);
    }
}

static void FinalizeNormalCar(void) {
    Finalize(1);
}

static void FinalizeOpenCar(void) {
    Finalize(2);
}

static void FinalizeArmedCar(void) {
    Finalize(3);
}

static void FinalizeBike(void) {
    Finalize(4);
}

UnkCarResource_801B9950 *UnkCarResource_801B9950::GetInstance() {
    if (s_resource == 0) {
        static UnkCarResource_801B9950 s_instance;
        s_resource = &s_instance;
    }
    return s_resource;
}

UnkCarResource_801B9950 *UnkCarResource_801B9950::s_resource;

/* Drops a kind's files (name inferred). */
static void Finalize(int kind) {
    UnkCarResource_801B9950::GetInstance()->Reset();
    fn_803A953C("%s::Finalize\n", lbl_8053B948[kind - 1].desc.typeName);
}

static TVehicleBase *CreateCar(int kind) {
    return new TVehicleCar(kind);
}

bool TVehicleCar::vf_0x5C() {
    return true;
}
