# pic24-i2c-game-of-life
A game of life over the PIC24's I2C bus

![Alt text](https://user-images.githubusercontent.com/19293817/75632447-5f7af000-5bfc-11ea-8090-3a9e8d74740c.gif "That's life!")

This is a set of two programs that implement Conway's Game of Life over the I2C bus.

The main one, inside the `pic24-i2c-slave-fw` directory, is the PIC24 firmware that acts as an I2C slave device, from which a complete grid of the game's universe can be read by an I2C master device. After each complete read the pic "evolves" the game into the next iteration.

The exact PIC variant this program targets is the PIC24FJ64GA306. If you're building this for a different PIC24 variant please consult your data sheet for the necessary changes to the program.

The other program, inside the `i2c-master-dev-program` directory, is the one that acts as the I2C master device and displays the game's universe to the user's screen.

This was written originally targeting a custom ARM board with an AT91SAM9260-EK, but it should be fairly easy to port to other archs.
