/* Dolphin SDK AX: frame profiling. The caller hands AX a ring of AXPROFILE records; each frame
 * AXOut fills the next one. Only the ring stepping survived dead-stripping. */
#include <dolphin/ax.h>

static AXPROFILE *__AXProfile;
static u32 __AXMaxProfiles;
static u32 __AXCurrentProfile;
static u32 __AXProfileInitialized;

AXPROFILE *__AXGetCurrentProfile(void) {
    AXPROFILE *profile;

    if (__AXProfileInitialized) {
        profile = &__AXProfile[__AXCurrentProfile];
        __AXCurrentProfile++;
        __AXCurrentProfile %= __AXMaxProfiles;
        return profile;
    }
    return NULL;
}
