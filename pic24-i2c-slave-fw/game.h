#ifndef GAME_H_
#define GAME_H_

#include <stdint.h>

#define WIDTH   30
#define HEIGHT  30

uint8_t game_get_next_byte(void);
void game_evolve(void);
void game_init(void);

#endif