#ifndef FAKEDPS310_H
#define FAKEDPS310_H

#include <stdint.h>

void fakedps310_power_on();
void fakedps310_power_off();

// Retourne un pointeur vers le registre simulé du DPS310
const uint8_t* fakedps310_get_regmap();

#endif // FAKEDPS310_H