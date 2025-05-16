#ifndef LIBDPS310_H
#define LIBDPS310_H

// Registres connus
enum RegOffset_e {
    REG_TEMP = 0x03,    // Température brute
    REG_PRESS = 0x00,   // Pression brute
    REG_COEF1 = 0x10,   // Coefficient 1
    REG_COEF2 = 0x11,   // Coefficient 2
    REG_COEF3 = 0x12,   // Coefficient 3
    REG_COEF4 = 0x13,   // Coefficient 4
    REG_COEF5 = 0x14,   // Coefficient 5
    REG_COEF6 = 0x15,   // Coefficient 6
    REG_COEF7 = 0x16,   // Coefficient 7
    REG_COEF8 = 0x17,   // Coefficient 8
    REG_COEF9 = 0x18,   // Coefficient 9
    REG_COEF10 = 0x19,  // Coefficient 10
    REG_COEF11 = 0x20,  // Coefficient 11
    REG_COEF12 = 0x21,  // Coefficient 12
    REG_COEF13 = 0x22,  // Coefficient 13
    REG_COEF14 = 0x23,  // Coefficient 14
    REG_COEF15 = 0x24,  // Coefficient 15
    REG_COEF16 = 0x25,  // Coefficient 16
    REG_COEF17 = 0x26,  // Coefficient 17
    REG_COEF18 = 0x27   // Coefficient 18
};

// Facteurs de suréchantillonnage
enum OversamplingRate_e {
    OSR_SINGLE = 0x00
};

// Initialise le capteur DPS310
void dps310_init();

// Éteint le capteur DPS310
void dps310_shutdown();

// Lit la température en degrés Celsius
float dps310_read_temperature();

// Lit la pression en millibars
float dps310_read_pressure();

#endif // LIBDPS310_H