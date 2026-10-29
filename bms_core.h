#ifndef BMS_CORE_H
#define BMS_CORE_H

#include "bms_config.h"

/* States for BMS Finite State Machine (FSM) */
typedef enum {
    BMS_STATE_INIT = 0U,
    BMS_STATE_STANDBY,
    BMS_STATE_CHARGING,
    BMS_STATE_DISCHARGING,
    BMS_STATE_FAULT
} BMS_State_t;

/* Fault Flag bitmask */
typedef enum {
    BMS_FAULT_NONE         = 0x00U,
    BMS_FAULT_OVER_VOLT    = 0x01U,
    BMS_FAULT_UNDER_VOLT   = 0x02U,
    BMS_FAULT_OVER_TEMP    = 0x04U,
    BMS_FAULT_OVER_CURRENT = 0x08U
} BMS_Fault_t;

/* Battery telemetry data structure */
typedef struct {
    uint16_t pack_voltage_mv;
    int32_t  pack_current_ma;
    int8_t   pack_temperature_c;
    uint8_t  state_of_charge_pct; /* 0 - 100 % */
} BatteryTelemetry_t;

/* Main BMS Controller Handle */
typedef struct {
    BMS_State_t        current_state;
    uint8_t            fault_mask;
    BatteryTelemetry_t telemetry;
    bool               contactor_closed;
} BMS_Handle_t;

/* Core Function Prototypes */
void BMS_Init(BMS_Handle_t *bms);
void BMS_UpdateTelemetry(BMS_Handle_t *bms, uint16_t volt_mv, int32_t curr_ma, int8_t temp_c);
void BMS_Task_10ms(BMS_Handle_t *bms);
void BMS_Task_100ms(BMS_Handle_t *bms);

#endif /* BMS_CORE_H */