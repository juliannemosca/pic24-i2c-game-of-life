#include <stdlib.h>

#include "game.h"

uint8_t univ[HEIGHT][WIDTH];
volatile uint16_t read_index = 0;

uint8_t game_get_cell_with_index(unsigned int index)
{
    unsigned int y = index / WIDTH;
    unsigned int x = index % WIDTH;
    return univ[y][x];
}

void game_evolve(void)
{
	unsigned int w = WIDTH, h = HEIGHT;
	uint8_t new[h][w];
    int x = 0, y = 0;
	for (y = 0; y < h; y++) {
        for (x = 0; x < w; x++) {
            int n = 0;

            int y1;
            for (y1 = y - 1; y1 <= y + 1; y1++) {

                int x1;
                for (x1 = x - 1; x1 <= x + 1; x1++) {
                    if (univ[(y1 + h) % h][(x1 + w) % w])
                        n++;
                }
            }
            if (univ[y][x])
                n--;

            new[y][x] = (n == 3 || (n == 2 && univ[y][x]));
        }
    }
	for (y = 0; y < h; y++)  {
        for (x = 0; x < w; x++) {
            univ[y][x] = new[y][x];
        }
    }
    return;
}

uint8_t game_get_next_byte(void)
{
    uint8_t r = game_get_cell_with_index(read_index);
    read_index++;
    if (read_index == (WIDTH * HEIGHT)) {
        read_index = 0;
    }
    return r;
}

void game_init(void)
{
    unsigned int w = WIDTH, h = HEIGHT, x = 0, y = 0;
    for (x = 0; x < w; x++) {
        for (y = 0; y < h; y++) {
            univ[y][x] = rand() < RAND_MAX / 10 ? 1 : 0;
        }
    }
    return;
}
