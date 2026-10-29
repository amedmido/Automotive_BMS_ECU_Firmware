#ifndef CAN_DRIVER_MOCK_H
#define CAN_DRIVER_MOCK_H

#include <stdint.h>
#include <stdbool.h>

/* Standard CAN 2.0B Frame Representation */
typedef struct {
    uint32_t id;
    uint8_t  dlc;
    uint8_t  data[8];
} CAN_Message_t;

/* CAN Driver Prototypes */
void CAN_Init(void);
bool CAN_Transmit(const CAN_Message_t *msg);

#endif /* CAN_DRIVER_MOCK_H */