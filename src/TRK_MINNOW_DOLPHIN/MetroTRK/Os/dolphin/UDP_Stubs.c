#include <TRK_MINNOW_DOLPHIN/dolphin_trk.h>

/* The connection over the broadband adapter is not in this library: weak stubs that fail to
 * open and transfer nothing. */

#pragma weak udp_cc_initialize
#pragma weak udp_cc_shutdown
#pragma weak udp_cc_open
#pragma weak udp_cc_close
#pragma weak udp_cc_read
#pragma weak udp_cc_write
#pragma weak udp_cc_peek
#pragma weak udp_cc_pre_continue
#pragma weak udp_cc_post_stop

int udp_cc_initialize(void *inputPendingPtr, DBCommCallback callback) {
    return -1;
}

int udp_cc_shutdown(void) {
    return -1;
}

int udp_cc_open(void) {
    return -1;
}

int udp_cc_close(void) {
    return -1;
}

int udp_cc_read(u8 *data, int length) {
    return 0;
}

int udp_cc_write(const u8 *data, int length) {
    return 0;
}

int udp_cc_peek(void) {
    return 0;
}

int udp_cc_pre_continue(void) {
    return -1;
}

int udp_cc_post_stop(void) {
    return -1;
}
