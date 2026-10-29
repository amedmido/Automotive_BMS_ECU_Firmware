#include <stdio.h>
#include "bms_core.h"

static const char* StateToString(BMS_State_t state) {
    switch (state) {
        case BMS_STATE_INIT:        return "INIT";
        case BMS_STATE_STANDBY:     return "STANDBY";
        case BMS_STATE_CHARGING:    return "CHARGING";
        case BMS_STATE_DISCHARGING: return "DISCHARGING";
        case BMS_STATE_FAULT:       return "FAULT (SAFE STATE)";
        default:                    return "UNKNOWN";
    }
}

int main(void) {
    BMS_Handle_t bms_ecu;

    printf("============================================================\n");
    printf("   Automotive BMS ECU Firmware Simulation (ISO 26262 / MISRA)\n");
    printf("============================================================\n\n");

    /* Step 1: System Initialization */
    BMS_Init(&bms_ecu);
    printf("[System Boot] State: %s | Contactors: %s\n\n",
           StateToString(bms_ecu.current_state),
           bms_ecu.contactor_closed ? "CLOSED" : "OPEN");

    /* Step 2: Normal Charging Scenario (3.8V, +15A, 28 deg C) */
    printf("--- [Scenario 1: Normal Charging Phase] ---\n");
    BMS_UpdateTelemetry(&bms_ecu, 3800U, 15000, 28);
    BMS_Task_10ms(&bms_ecu);
    BMS_Task_100ms(&bms_ecu);
    printf("State: %s | SoC: %u%% | Contactors: %s\n\n",
           StateToString(bms_ecu.current_state),
           bms_ecu.telemetry.state_of_charge_pct,
           bms_ecu.contactor_closed ? "CLOSED" : "OPEN");

    /* Step 3: Normal Driving / Discharging Scenario (3.6V, -40A, 35 deg C) */
    printf("--- [Scenario 2: Dynamic Discharging Phase] ---\n");
    BMS_UpdateTelemetry(&bms_ecu, 3600U, -40000, 35);
    BMS_Task_10ms(&bms_ecu);
    BMS_Task_100ms(&bms_ecu);
    printf("State: %s | SoC: %u%% | Contactors: %s\n\n",
           StateToString(bms_ecu.current_state),
           bms_ecu.telemetry.state_of_charge_pct,
           bms_ecu.contactor_closed ? "CLOSED" : "OPEN");

    /* Step 4: Critical Fault Scenario - Thermal Runaway Risk (65 deg C > 60 deg C Limit) */
    printf("--- [Scenario 3: Over-Temperature Fault Injection] ---\n");
    BMS_UpdateTelemetry(&bms_ecu, 3650U, -20000, 65);
    BMS_Task_10ms(&bms_ecu);
    BMS_Task_100ms(&bms_ecu);
    printf("State: %s | Fault Mask: 0x%02X | Contactors: %s\n\n",
           StateToString(bms_ecu.current_state),
           bms_ecu.fault_mask,
           bms_ecu.contactor_closed ? "CLOSED" : "OPEN");

    printf("============================================================\n");
    printf("   Simulation Finished Successfully.\n");
    printf("============================================================\n");

    return 0;
}