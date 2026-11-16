/* Critical regions for the GameCube: the library runs single-threaded here, so entering and
 * leaving one does nothing. */

void __kill_critical_regions(void) {
}

void __begin_critical_region(int region) {
}

void __end_critical_region(int region) {
}
