#include "bms_core.h"
#include "can_driver_mock.h"
#include <stdio.h>

void BMS_Init(BMS_Handle_t *bms) {
    if (bms == NULL) {
        return;
    }

    bms->current_state = BMS_STATE_INIT;
    bms->fault_mask = BMS_FAULT_NONE;
    bms->contactor_closed = false;

    bms->telemetry.pack_voltage_mv = 3700U;
    bms->telemetry.pack_current_ma = 0;
    bms->telemetry.pack_temperature_c = 25;
    bms->telemetry.state_of_charge_pct = 50U;

    CAN_Init();

    /* Transition to STANDBY after successful initialization */
    bms->current_state = BMS_STATE_STANDBY;
}

void BMS_UpdateTelemetry(BMS_Handle_t *bms, uint16_t volt_mv, int32_t curr_ma, int8_t temp_c) {
    if (bms == NULL) {
        return;
    }

    bms->telemetry.pack_voltage_mv = volt_mv;
    bms->telemetry.pack_current_ma = curr_ma;
    bms->telemetry.pack_temperature_c = temp_c;

    /* Simple linear SoC approximation based on nominal voltage range (3000mV - 4200mV) */
    if (volt_mv <= 3000U) {
        bms->telemetry.state_of_charge_pct = 0U;
    } else if (volt_mv >= 4200U) {
        bms->telemetry.state_of_charge_pct = 100U;
    } else {
        bms->telemetry.state_of_charge_pct = (uint8_t)(((uint32_t)(volt_mv - 3000U) * 100U) / 1200U);
    }
}

/* Fast periodic task (10ms) dedicated to safety monitoring and fault handling */
void BMS_Task_10ms(BMS_Handle_t *bms) {
    if (bms == NULL) {
        return;
    }

    uint8_t faults = BMS_FAULT_NONE;

    /* Over / Under Voltage Checks */
    if (bms->telemetry.pack_voltage_mv > BMS_CELL_OVER_VOLTAGE_THRESHOLD_MV) {
        faults |= BMS_FAULT_OVER_VOLT;
    } else if (bms->telemetry.pack_voltage_mv < BMS_CELL_UNDER_VOLTAGE_THRESHOLD_MV) {
        faults |= BMS_FAULT_UNDER_VOLT;
    }

    /* Thermal Protection Checks */
    if ((bms->telemetry.pack_temperature_c > BMS_OVER_TEMP_THRESHOLD_C) ||
        (bms->telemetry.pack_temperature_c < BMS_UNDER_TEMP_THRESHOLD_C)) {
        faults |= BMS_FAULT_OVER_TEMP;
    }

    /* Overcurrent Protection */
    if ((bms->telemetry.pack_current_ma > (int32_t)BMS_OVER_CURRENT_CHARGE_LIMIT_MA) ||
        (bms->telemetry.pack_current_ma < -(int32_t)BMS_OVER_CURRENT_DISCHARGE_LIMIT_MA)) {
        faults |= BMS_FAULT_OVER_CURRENT;
    }

    bms->fault_mask = faults;

    /* State Machine Logic */
    if (bms->fault_mask != BMS_FAULT_NONE) {
        bms->current_state = BMS_STATE_FAULT;
        bms->contactor_closed = false; /* Open contactors for safety (Safe State) */
    } else {
        if (bms->telemetry.pack_current_ma > 500) {
            bms->current_state = BMS_STATE_CHARGING;
            bms->contactor_closed = true;
        } else if (bms->telemetry.pack_current_ma < -500) {
            bms->current_state = BMS_STATE_DISCHARGING;
            bms->contactor_closed = true;
        } else {
            bms->current_state = BMS_STATE_STANDBY;
            bms->contactor_closed = false;
        }
    }
}

/* Slow periodic task (100ms) dedicated to cyclic CAN telemetry broadcast */
void BMS_Task_100ms(BMS_Handle_t *bms) {
    if (bms == NULL) {
        return;
    }

    CAN_Message_t tx_msg;

    /* Message 1: Battery Telemetry (0x310) */
    tx_msg.id = BMS_CAN_TELEMETRY_ID;
    tx_msg.dlc = 8U;
    tx_msg.data[0] = (uint8_t)(bms->telemetry.pack_voltage_mv >> 8);
    tx_msg.data[1] = (uint8_t)(bms->telemetry.pack_voltage_mv & 0xFFU);
    tx_msg.data[2] = (uint8_t)(bms->telemetry.pack_current_ma >> 24);
    tx_msg.data[3] = (uint8_t)(bms->telemetry.pack_current_ma >> 16);
    tx_msg.data[4] = (uint8_t)(bms->telemetry.pack_current_ma >> 8);
    tx_msg.data[5] = (uint8_t)(bms->telemetry.pack_current_ma & 0xFFU);
    tx_msg.data[6] = (uint8_t)bms->telemetry.pack_temperature_c;
    tx_msg.data[7] = bms->telemetry.state_of_charge_pct;

    CAN_Transmit(&tx_msg);

    /* Message 2: BMS Status & Fault Flags (0x311) */
    tx_msg.id = BMS_CAN_FAULTS_ID;
    tx_msg.dlc = 3U;
    tx_msg.data[0] = (uint8_t)bms->current_state;
    tx_msg.data[1] = bms->fault_mask;
    tx_msg.data[2] = bms->contactor_closed ? 0x01U : 0x00U;

    CAN_Transmit(&tx_msg);
}