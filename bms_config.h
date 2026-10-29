#ifndef BMS_CONFIG_H
#define BMS_CONFIG_H

#include <stdint.h>
#include <stdbool.h>

/* --- Safety Critical Thresholds (ISO 26262 / ASIL-C considerations) --- */
#define BMS_CELL_OVER_VOLTAGE_THRESHOLD_MV    (4250U)  /* 4.25 V max per cell */
#define BMS_CELL_UNDER_VOLTAGE_THRESHOLD_MV   (2800U)  /* 2.80 V min per cell */
#define BMS_OVER_TEMP_THRESHOLD_C             (60)     /* 60 deg C maximum */
#define BMS_UNDER_TEMP_THRESHOLD_C            (-10)    /* -10 deg C minimum */
#define BMS_OVER_CURRENT_CHARGE_LIMIT_MA      (50000U) /* 50A maximum charge */
#define BMS_OVER_CURRENT_DISCHARGE_LIMIT_MA   (120000U)/* 120A max discharge */

/* --- CAN Network Configurations --- */
#define BMS_CAN_BASE_ID                       (0x300U)
#define BMS_CAN_TELEMETRY_ID                  (BMS_CAN_BASE_ID + 0x10U) /* 0x310 */
#define BMS_CAN_FAULTS_ID                     (BMS_CAN_BASE_ID + 0x11U) /* 0x311 */

/* --- Scheduling Rates --- */
#define BMS_FAST_TASK_PERIOD_MS               (10U)    /* Fault checking loop */
#define BMS_SLOW_TASK_PERIOD_MS               (100U)   /* Telemetry & CAN loop */

#endif /* BMS_CONFIG_H */