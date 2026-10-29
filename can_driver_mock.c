#include "can_driver_mock.h"
#include <stdio.h>

void CAN_Init(void) {
    /* Virtual CAN Controller Initialized (Loopback / Virtual Bus) */
}

bool CAN_Transmit(const CAN_Message_t *msg) {
    if (msg == NULL) {
        return false;
    }

    /* Print CAN telemetry to simulate hardware bus frame output */
    printf("[CAN TX] ID: 0x%03X | DLC: %d | Data: ", msg->id, msg->dlc);
    for (uint8_t i = 0; i < msg->dlc; i++) {
        printf("%02X ", msg->data[i]);
    }
    printf("\n");

    return true;
}