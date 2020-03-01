#include <xc.h>
#include <stdint.h>

#include "i2c.h"
#include "game.h"

enum i2c_protocol_state {
  I2C_INTERNAL_STATE_WAIT_FOR_ADDR,
  I2C_INTERNAL_STATE_WAIT_FOR_WRITE_DATA,
  I2C_INTERNAL_STATE_SEND_READ_DATA,
  I2C_INTERNAL_STATE_SEND_READ_LAST
};

volatile enum i2c_protocol_state
i2c_internal_state = I2C_INTERNAL_STATE_WAIT_FOR_ADDR;

extern uint16_t read_index;
uint8_t evolve = 0;

uint8_t i2c_get_next_data_byte(void)
{
    uint8_t r = game_get_next_byte();
    if (read_index == 0)
        i2c_internal_state = I2C_INTERNAL_STATE_SEND_READ_LAST;
    else
        i2c_internal_state = I2C_INTERNAL_STATE_SEND_READ_DATA; //read transaction

    return r;
}

/* `no_auto_psv` does not allow the ISR to access
 * constant data (data stored in program memory).
 * This saves some clock cycles */
void
__attribute__((interrupt)) __attribute__ ((no_auto_psv))
_SI2C2Interrupt(void)
{

    uint8_t addr;

    _SI2C2IF = 0;
    switch (i2c_internal_state) {
        case I2C_INTERNAL_STATE_WAIT_FOR_ADDR:

            // RBF is set when an address is loaded into it.
            //
            // From the ref. manual:
            // When the CPU reads the receive buffer (I2CxRCV),
            // the RBF status bit is automatically cleared.

            addr = I2C2RCV; // clear RBF bit

            // is it a read or a write?
            if (I2C2STATbits.R_W) {

                // It's a read:

                I2C2TRN = i2c_get_next_data_byte();
                I2C2CONbits.SCLREL = 1; //release clock line
            } else {

                // It's a write:

                //i2c_internal_state = I2C_INTERNAL_STATE_WAIT_FOR_WRITE_DATA;

                // We don't support writing to the device on this program,
                // so here we just ignore it
                i2c_internal_state = I2C_INTERNAL_STATE_WAIT_FOR_ADDR;
            }

            break;

        case I2C_INTERNAL_STATE_WAIT_FOR_WRITE_DATA:

            // (should never reach this case on this program)

            // We don't support writing to the device on this program,
            // so here we just ignore it
            i2c_internal_state = I2C_INTERNAL_STATE_WAIT_FOR_ADDR;

        case I2C_INTERNAL_STATE_SEND_READ_DATA:

            I2C2TRN = i2c_get_next_data_byte();
            I2C2CONbits.SCLREL = 1; // release clock line
            break;

        case I2C_INTERNAL_STATE_SEND_READ_LAST: //this is interrupt for last byte finished shifting out

            i2c_internal_state = I2C_INTERNAL_STATE_WAIT_FOR_ADDR;
            I2C2CONbits.SCLREL = 1; // release clock line
            evolve = 1;
            break;

        default:
            i2c_internal_state = I2C_INTERNAL_STATE_WAIT_FOR_ADDR;
    }
}