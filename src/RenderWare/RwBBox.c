/* RenderWare core: the bounding box module (0x8048349C-0x80483554); only RwBBoxCalculate is
 * left in retail (the camera's frustum update calls it). No data.
 * C, cflags_rw_core (configure.py). */
#include "Game/rw/r6.h"

/* RwBBoxCalculate. */
RwBBox *fn_8048349C(RwBBox *box, const RwV3d *verts, s32 numVerts) {
    box->inf = *verts;
    box->sup = *verts;
    verts++;
    numVerts--;
    while (numVerts--) {
        if (box->inf.x > verts->x) {
            box->inf.x = verts->x;
        }
        if (box->inf.y > verts->y) {
            box->inf.y = verts->y;
        }
        if (box->inf.z > verts->z) {
            box->inf.z = verts->z;
        }
        if (box->sup.x < verts->x) {
            box->sup.x = verts->x;
        }
        if (box->sup.y < verts->y) {
            box->sup.y = verts->y;
        }
        if (box->sup.z < verts->z) {
            box->sup.z = verts->z;
        }
        verts++;
    }
    return box;
}
