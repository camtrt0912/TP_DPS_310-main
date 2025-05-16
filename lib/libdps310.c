#include "../include/libdps310.h"
#include "../include/fakedps310.h"
#include <stdint.h>
#include <stdio.h>

// Table des facteurs d'échelle
static const uint32_t _scaleFactors[] = {
    524288,
    1572864,
    3670016,
    7864320,
    253952,
    516096,
    1040384,
    2088960
};

// Initialise le capteur DPS310
void dps310_init() {
    fakedps310_power_on();
}

// Éteint le capteur DPS310
void dps310_shutdown() {
    fakedps310_power_off();
}

// Convertit un entier signé 24 bits en 32 bits
static int32_t _signed24_to_signed32(int32_t value) {
    if (value & 0x800000) {
        value |= 0xFF000000;
    }
    return value;
}

// Récupère une valeur 24 bits signée à un offset donné dans le registre
static int32_t _get_s24_at(const uint8_t *regmap, int offset) {
    int32_t value = (regmap[offset] << 16) | (regmap[offset + 1] << 8) | regmap[offset + 2];
    return _signed24_to_signed32(value);
}

// Récupère la température brute
static int32_t _get_temperature_raw(const uint8_t *regmap) {
    return _get_s24_at(regmap, REG_TEMP);
}

// Calcule la température réelle
static float _get_temperature_real(const uint8_t *regmap) {
    int32_t C0 = ((regmap[REG_COEF1] << 4) | (regmap[REG_COEF2] >> 4)) & 0xFFF;
    if (C0 & 0x800) {
        C0 |= 0xFFFFF000;
    }

    int32_t C1 = (((regmap[REG_COEF2] & 0x0F) << 8) | regmap[REG_COEF3]) & 0xFFF;
    if (C1 & 0x800) {
        C1 |= 0xFFFFF000;
    }

    int32_t Traw = _get_temperature_raw(regmap);
    float Traw_sc = Traw / (float)_scaleFactors[OSR_SINGLE];
    return C0 * 0.5f + C1 * Traw_sc;
}

// Lit la température en degrés Celsius
float dps310_read_temperature() {
    const uint8_t *regmap = fakedps310_get_regmap();
    return _get_temperature_real(regmap);
}

static int32_t _get_pressure_raw(const uint8_t *regmap) {
    return _get_s24_at(regmap, REG_PRESS);
}

typedef struct {
    int32_t c00, c10;
    int16_t c01, c11, c20, c21, c30;
} dps310_coeffs_t;

static dps310_coeffs_t _get_pressure_coeffs(const uint8_t *reg) {
    dps310_coeffs_t coeffs;

    // c00 (20 bits signé)
    int32_t c00 = ((int32_t)reg[0x13] << 12) |
                  ((int32_t)reg[0x14] << 4) |
                  ((int32_t)(reg[0x15] >> 4) & 0x0F);
    if (c00 & (1 << 19)) c00 |= 0xFFF00000;
    coeffs.c00 = c00;

    // c10 (20 bits signé)
    int32_t c10 = ((int32_t)(reg[0x15] & 0x0F) << 16) |
                  ((int32_t)reg[0x16] << 8) |
                  (int32_t)reg[0x17];
    if (c10 & (1 << 19)) c10 |= 0xFFF00000;
    coeffs.c10 = c10;

    // 16-bit signed coefficients
    #define READ_S16(MSB, LSB) (int16_t)((int16_t)(reg[MSB] << 8) | reg[LSB])

    coeffs.c01 = READ_S16(0x18, 0x19); // c01
    coeffs.c11 = READ_S16(0x1A, 0x1B); // c11
    coeffs.c20 = READ_S16(0x1C, 0x1D); // c20
    coeffs.c21 = READ_S16(0x1E, 0x1F); // c21
    coeffs.c30 = READ_S16(0x20, 0x21); // c30

    return coeffs;
}

static float _get_pressure_real(const uint8_t *regmap) {
    dps310_coeffs_t coef = _get_pressure_coeffs(regmap);

    int32_t Praw = _get_pressure_raw(regmap);
    int32_t Traw = _get_temperature_raw(regmap); // nécessaire pour la correction

    float kP = (float)_scaleFactors[OSR_SINGLE];
    float kT = (float)_scaleFactors[OSR_SINGLE];

    float Praw_sc = Praw / kP;
    float Traw_sc = Traw / kT;

    float pcomp = coef.c00 +
                  Praw_sc * (coef.c10 + Praw_sc * (coef.c20 + Praw_sc * coef.c30)) +
                  Traw_sc * coef.c01 +
                  Traw_sc * Praw_sc * (coef.c11 + Praw_sc * coef.c21);
                  
    return pcomp; // en Pascals
}

float dps310_read_pressure() {
    const uint8_t *regmap = fakedps310_get_regmap();
    return _get_pressure_real(regmap) / 100.0f;
}
