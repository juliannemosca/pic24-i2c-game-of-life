#include "init.h"
#include "game.h"

extern uint8_t evolve;

int main(void) {
    game_init();    // Game initialization
    init();         // Harware initialization

    while(1) {
        /*
         * We do the "evolve" here, since a sw timer on the
         * i2c master may not give us time enough to do the computation.
         *
         * We are of course assuming that the i2c master will give us
         * some time between its read to do this.
         */
        if (evolve) {
            game_evolve();
            evolve = 0;
        }
    }

    return 0;
}
