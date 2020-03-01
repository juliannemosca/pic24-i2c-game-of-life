#include <xc.h>
#include "i2c.h"

/*-----------------------------------------------*
 * Initialization of pins
 *-----------------------------------------------*/

static void
init_analog(void)
{
    /* Set all pins that can be set to digital - some pins that aren't
     * even marked as potential analog inputs don't work correctly otherwise! */

    ANSB = 0;
    ANSD = 0;
    ANSE = 0;
    ANSG = 0;
}

/*-----------------------------------------------*
 * Initialization of the I2C subsystem
 *-----------------------------------------------*/

static void
init_i2c(void)
{
    I2C2ADD = SLAVE_I2C_ADDR;   // Set I2C slave address
    I2C2CONbits.I2CEN = 1;      // Enable the I2C module

    // Intelligent Platform Management Interface disabled
    I2C2CONbits.IPMIEN = 1;

    // 0 => Slew rate control is enabled for High-Speed mode (400 kHz)
    I2C2CONbits.DISSLW = 0;

    // I2CxADD register is a 7 bit address
    I2C2CONbits.A10M = 0;

    // General Call Enable bit (when operating as I2C slave)
    I2C2CONbits.GCEN = 1;

    // Enable clock stretching
    I2C2CONbits.SCLREL = 1;
    I2C2CONbits.STREN = 1;

    // Clear the I2C Slave interrupt flag
    IFS3bits.SI2C2IF = 0;

    IEC3bits.SI2C2IE = 1; // Enable the Slave interrupt

    // No mask, the exact address must be matched
    I2C2MSK = 0x00;
}

/*-----------------------------------------------*
 * Hardware initialization
 *-----------------------------------------------*/

void
init(void)
{
    OSCTUN = 0;

    // disable WDT
    RCONbits.SWDTEN = 0;

    init_analog();
    init_i2c();
}