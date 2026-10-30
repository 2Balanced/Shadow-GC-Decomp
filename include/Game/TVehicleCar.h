#ifndef GAME_TVEHICLECAR_H
#define GAME_TVEHICLECAR_H

#include "Game/TVehicleBase.h"
#include "boost/shared_ptr.hpp"
#include <dolphin/types.h>
#include "size_assert.h"

struct RpClump;
struct _stMtnPlayWork;
class UnkClumpUpdater_80015A7C;

namespace Vehicle {

class TVehicleCarMove;

/* A model part of a car in the cars' model table (struct name unknown; names inferred). */
struct CarPartDesc {
    /* 0x00 */ s32 m_model;  /* the clump's file, -1 none */
    /* 0x04 */ s32 m_area;   /* the stage area whose world takes the clump */
    /* 0x08 */ s32 m_file;   /* a further file the loader loads, -1 none */
    /* 0x0C */ s32 m_motion; /* the motion played on it, -1 none */
    /* 0x10 */ u8 m_enabled;
    /* 0x11 */ u8 field_0x11;
};

/* A node of a car's model the driving reads (struct name unknown; names inferred). */
struct CarNodeDesc {
    /* 0x0 */ u8 m_inParts; /* in the parts, else in the current row of the row parts */
    /* 0x4 */ s32 m_part;
    /* 0x8 */ s32 m_node;
};

/* A car's model: its files, parts and nodes (struct name unknown; names inferred). One per handling
 * type (lbl_804CDAF8). */
struct CarModelDesc {
    /* 0x000 */ s32 m_base;     /* the loader's file whose offset the motions take */
    /* 0x004 */ s32 m_files[1]; /* the motion file; the loader loads three words from here */
    /* 0x008 */ CarPartDesc m_parts[3];
    /* 0x044 */ CarPartDesc m_rows[3][3];
    /* 0x0F8 */ CarNodeDesc m_nodes[12];
};

/* A model part of a car (struct name unknown; names inferred). */
struct CarPart {
    /* 0x0 */ RpClump *m_clump;
    /* 0x4 */ void *m_world;
    /* 0x8 */ _stMtnPlayWork *m_motion;
    /* 0xC */ UnkClumpUpdater_80015A7C *m_updater;
};

/* Vehicle::TVehicleCar (class name from RTTI; vtable at 0x8053BA18; Game/TVehicleCar.cpp): the cars
 * (kinds 1 to 4: "Normal Car", "Open Car", "Armed Car", "Bike"). Its driving is a
 * Vehicle::TVehicleCarMove it owns; the car holds the model's parts. Constructor 0x801B94F8,
 * destructor 0x801B943C. Member and function names are inferred. */
class TVehicleCar : public TVehicleBase {
  public:
    TVehicleCar(int kind);
    virtual ~TVehicleCar();
    virtual void SetPlacement(const Vec3f *position, const Vec3f *rotation);
    virtual void GetSeat(Vec3f *position, Vec3f *rotation);
    virtual void OnCommand(CharCommand *command);
    virtual void Tick(float dt);
    virtual void OnGetOn();
    virtual void OnGetOff();
    virtual bool vf_0x5C();

    void Reset();
    void Release();
    void LoadMotion();
    void CreateModel(int row);
    void DestroyModel();
    void CreatePart(int index);
    void CreateParts();
    void CreateRowPart(int row, int index);
    void CreateRow(int row);
    void DestroyParts();
    void DestroyRow(int row);

    /* 0x09C */ s32 m_started;
    /* 0x0A0 */ s32 m_base;
    /* 0x0A4 */ void *m_motionData[1];
    /* 0x0A8 */ CarPart m_parts[3];
    /* 0x0D8 */ CarPart m_rows[3][3];
    /* 0x168 */ boost::shared_ptr<TVehicleCarMove> m_move;
    /* 0x170 */ s32 field_0x170;
};
SIZE_ASSERT(TVehicleCar, 0x174);

} // namespace Vehicle

/* The cars' models, one per handling type (Game/TVehicleCar.cpp; Game/TVehicleCarMove.cpp reads
 * it too). */
extern const Vehicle::CarModelDesc lbl_804CDAF8[4];

#endif
