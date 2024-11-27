#ifndef SX1276DEV_H
#define SX1276DEV_H

#include <stdint.h>


typedef struct   {
    void (* Reset)(void);
    void (* ConfigureRfSwitch)(uint8_t opMode, int8_t power);
    void (* Write)(uint8_t addr, uint8_t *buffer, uint8_t size);
    void (* Read)(uint8_t addr, uint8_t *buffer, uint8_t size);

    
} SX1276Dev;

#endif