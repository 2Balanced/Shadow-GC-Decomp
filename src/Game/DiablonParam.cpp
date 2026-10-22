/* The Diablon boss's tuning tables (Game/Diablon.h's DiablonParams), one for each stage the boss
 * appears in; the set object picks one by the stage number (0x802D8F54). A unit of its own, as
 * the tables lie between those of Game/DiablonDesireBase.cpp and Game/DiablonPartsBase.cpp; the
 * file name and the table names are inferred. */
#include "Game/Diablon.h"

namespace Boss {
namespace Diablon {

/* The tuning for stage 611 (name inferred). */
DiablonParams g_params611 = {
    200, 4.0f, 0.0f, 1.0f, 1.5f, 5.0f, 145.0f, 145.0f, 3.2f, 800.0f, 5.0f, 120.0f, 100.0f, 200.0f,
    20.0f, 80.0f, 300.0f, 5.5f, 0.2f, 1.0f, 30.0f, 45.0f, 1.0f, 25.0f, 100.0f, 9999.0f, 50.0f,
    4.0f, 250.0f, 0.5f, 0.0f, 250.0f, 1.0f, 45.0f, 0.0f, 100.0f, 266, 271, 267, {272, 268},
    {269, 273}, 274, 275, 278, 276, 277, 2.0f, 150.0f, 70.0f, 40.0f, 20.0f, 0.001f, 1.5f, 50.0f,
    75.0f,
};

/* The tuning for stage 613 (name inferred). */
DiablonParams g_params613 = {
    200, 4.0f, 0.0f, 1.0f, 1.5f, 5.0f, 145.0f, 145.0f, 3.2f, 800.0f, 5.0f, 120.0f, 100.0f, 200.0f,
    20.0f, 80.0f, 300.0f, 5.5f, 0.2f, 1.0f, 30.0f, 45.0f, 1.0f, 25.0f, 100.0f, 9999.0f, 50.0f,
    4.0f, 250.0f, 0.5f, 0.0f, 250.0f, 1.0f, 45.0f, 0.0f, 100.0f, 307, 312, 308, {313, 309},
    {310, 314}, 315, 316, 319, 317, 318, 2.0f, 150.0f, 70.0f, 40.0f, 20.0f, 0.25f, 1.0f, 20.0f,
    75.0f,
};

/* The tuning for the other stages (612) (name inferred). */
DiablonParams g_paramsDefault = {
    200, 4.0f, 0.0f, 1.0f, 1.5f, 5.0f, 145.0f, 145.0f, 3.2f, 800.0f, 5.0f, 120.0f, 100.0f, 200.0f,
    20.0f, 80.0f, 300.0f, 5.5f, 0.2f, 1.0f, 30.0f, 45.0f, 1.0f, 25.0f, 100.0f, 9999.0f, 50.0f,
    4.0f, 250.0f, 0.5f, 0.0f, 250.0f, 1.0f, 45.0f, 0.0f, 100.0f, 398, 403, 399, {404, 400},
    {401, 405}, 406, 407, 410, 408, 409, 1.0f, 150.0f, 70.0f, 40.0f, 20.0f, 0.0f, 0.5f, 30.0f,
    95.0f,
};

} // namespace Diablon
} // namespace Boss
