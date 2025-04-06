#pragma once

#include <cstdint>
#include <inttypes.h>
#include <vector>

// Device address
#define BROADCAST_ADDRESS 0x00
#define RADIO_ADDRESS 0xEA
#define MODULE_ADDRESS 0xEE
#define RECEIVER_ADDRESS 0xEC

// Frame id
#define GPS_ID 0x02
#define CF_VARIO_ID 0x07
#define BATTERY_ID 0x08
#define BARO_ALT_ID 0x09
#define LINK_ID 0x14
#define CHANNELS_ID 0x16
#define LINK_RX_ID 0x1C
#define LINK_TX_ID 0x1D
#define ATTITUDE_ID 0x1E
#define FLIGHT_MODE_ID 0x21
#define PING_DEVICES_ID 0x28
#define DEVICE_INFO_ID 0x29
#define REQUEST_SETTINGS_ID 0x2A
#define COMMAND_ID 0x32
#define RADIO_ID 0x3A

#define UART_SYNC 0xC8
#define SUBCOMMAND_CRSF 0x10
#define COMMAND_MODEL_SELECT_ID 0x05
#define SUBCOMMAND_CRSF_BIND 0x01

#define CROSSFIRE_CHANNELS_COUNT 16

constexpr uint8_t CRSF_NAME_MAXSIZE = 16;

struct CrossfireSensor
{
    const uint8_t id;
    const uint8_t subId;
    //  const TelemetryUnit unit;
    const uint8_t precision;
    const char *name;
};

enum CrossfireSensorIndexes
{
    RX_RSSI1_INDEX,
    RX_RSSI2_INDEX,
    RX_QUALITY_INDEX,
    RX_SNR_INDEX,
    RX_ANTENNA_INDEX,
    RF_MODE_INDEX,
    TX_POWER_INDEX,
    TX_RSSI_INDEX,
    TX_QUALITY_INDEX,
    TX_SNR_INDEX,
    RX_RSSI_PERC_INDEX,
    RX_RF_POWER_INDEX,
    TX_RSSI_PERC_INDEX,
    TX_RF_POWER_INDEX,
    TX_FPS_INDEX,
    BATT_VOLTAGE_INDEX,
    BATT_CURRENT_INDEX,
    BATT_CAPACITY_INDEX,
    BATT_REMAINING_INDEX,
    GPS_LATITUDE_INDEX,
    GPS_LONGITUDE_INDEX,
    GPS_GROUND_SPEED_INDEX,
    GPS_HEADING_INDEX,
    GPS_ALTITUDE_INDEX,
    GPS_SATELLITES_INDEX,
    ATTITUDE_PITCH_INDEX,
    ATTITUDE_ROLL_INDEX,
    ATTITUDE_YAW_INDEX,
    FLIGHT_MODE_INDEX,
    VERTICAL_SPEED_INDEX,
    BARO_ALTITUDE_INDEX,
    UNKNOWN_INDEX,
};

enum ArmingMode
{
    ARMING_MODE_FIRST = 0,
    ARMING_MODE_CH5 = ARMING_MODE_FIRST,
    ARMING_MODE_SWITCH = 1,
    ARMING_MODE_LAST = ARMING_MODE_SWITCH,
};

enum CrossfireFrames
{
    CRSF_FRAME_CHANNEL,
    CRSF_FRAME_MODELID,
    CRSF_FRAME_MODELID_SENT
};

struct CrossfireModuleStatus
{
    uint8_t major;
    uint8_t minor;
    uint8_t revision;
    char name[CRSF_NAME_MAXSIZE];
    bool queryCompleted;
    bool isELRS;
};

extern CrossfireModuleStatus crossfireModuleStatus[2];

uint8_t createCrossfireBindFrame(uint8_t moduleIdx, uint8_t *frame);

template <class T> inline T limit(T vmin, T x, T vmax) { return std::min(std::max(vmin, x), vmax); }

uint8_t createCrossfireChannelsFrame(uint8_t moduleIdx, uint8_t *frame, int16_t *pulses, uint8_t armingMode = false);

const uint32_t CROSSFIRE_BAUDRATES[] = {
    115200, 400000, 921600, 1870000, 3750000, 5250000,
};
