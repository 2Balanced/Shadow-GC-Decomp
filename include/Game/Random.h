#ifndef SHADOW_GAME_RANDOM_H
#define SHADOW_GAME_RANDOM_H

/* Random numbers, as the game's helpers produce them (ranges read from their code). */
extern "C" {
/* Uniform integer, 0 <= result < count. */
int fn_8040EA90(int count);
/* Uniform float, lo <= result < hi. */
float fn_8040EC80(float lo, float hi);
}

#endif
