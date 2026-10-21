#ifndef SHADOW_GAME_FILEQUEUE_H
#define SHADOW_GAME_FILEQUEUE_H

/* Gadget type descriptors' load hooks pass the files they need to this queue, which loads them
 * before the stage starts. */
extern "C" {
/* Adds path to the queue. What the second argument selects is not known yet. */
void fn_8006B474(const char *path, int mode);
}

#endif
