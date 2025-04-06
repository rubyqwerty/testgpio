#include "crossfire.h"
#include "crc.h"
#include <cstdint>
#include <vector>

#define CROSSFIRE_CH_BITS 11
#define CROSSFIRE_CENTER 0x3E0
#if defined(PPM_CENTER_ADJUSTABLE)
#define CROSSFIRE_CENTER_CH_OFFSET(ch) ((2 * limitAddress(ch)->ppmCenter) + 1) // + 1 is for rouding
#else
#define CROSSFIRE_CENTER_CH_OFFSET(ch) (0)
#endif

#define MIN_FRAME_LEN 3

#define MODULE_ALIVE_TIMEOUT 50 // if the module has sent a valid frame within 500ms it is declared alive
// static tmr10ms_t lastAlive[NUM_MODULES]; // last time stamp module sent CRSF frames
// static bool moduleAlive[NUM_MODULES];    // module alive status

uint8_t telemetryStreaming = 1; // >0 (true) == data is streaming in. 0 = no data detected for some time

inline bool TELEMETRY_STREAMING() { return telemetryStreaming > 0; }

uint8_t createCrossfireBindFrame(uint8_t moduleIdx, uint8_t *frame)
{
    uint8_t *buf = frame;
    *buf++ = UART_SYNC;  /* device address */
    *buf++ = 7;          /* frame length */
    *buf++ = COMMAND_ID; /* cmd type */
    if (TELEMETRY_STREAMING())
        *buf++ = RECEIVER_ADDRESS; /* Destination is receiver (unbind) */
    else
        *buf++ = MODULE_ADDRESS;   /* Destination is module */
    *buf++ = RADIO_ADDRESS;        /* Origin Address */
    *buf++ = SUBCOMMAND_CRSF;      /* sub command */
    *buf++ = SUBCOMMAND_CRSF_BIND; /* initiate bind */
    *buf++ = crc8_BA(frame + 2, 5);
    *buf++ = crc8(frame + 2, 6);
    return buf - frame;
}

uint8_t createCrossfirePingFrame(uint8_t moduleIdx, uint8_t *frame)
{
    uint8_t *buf = frame;
    *buf++ = UART_SYNC;         /* device address */
    *buf++ = 4;                 /* frame length */
    *buf++ = PING_DEVICES_ID;   /* cmd type */
    *buf++ = BROADCAST_ADDRESS; /* Destination Address */
    *buf++ = RADIO_ADDRESS;     /* Origin Address */
    *buf++ = crc8(frame + 2, 3);
    return buf - frame;
}

uint8_t createCrossfireModelIDFrame(uint8_t moduleIdx, uint8_t *frame)
{
    uint8_t *buf = frame;
    *buf++ = UART_SYNC;               /* device address */
    *buf++ = 8;                       /* frame length */
    *buf++ = COMMAND_ID;              /* cmd type */
    *buf++ = MODULE_ADDRESS;          /* Destination Address */
    *buf++ = RADIO_ADDRESS;           /* Origin Address */
    *buf++ = SUBCOMMAND_CRSF;         /* sub command */
    *buf++ = COMMAND_MODEL_SELECT_ID; /* command of set model/receiver id */
                                      //  *buf++ = g_model.header.modelId[moduleIdx]; /* model ID */
    *buf++ = crc8_BA(frame + 2, 6);
    *buf++ = crc8(frame + 2, 7);
    return buf - frame;
}

uint8_t createCrossfireChannelsFrame(uint8_t moduleIdx, uint8_t *frame, int16_t *pulses, uint8_t armingMode)
{
    //
    // sends channel data and also communicates commanded armed status in arming mode Switch.
    // frame len 24 -> arming mode CH5: module will use channel 5
    // frame len 25 -> arming mode Switch: send commanded armed status in extra byte after channel data
    //

    uint8_t lenAdjust = armingMode;

    uint8_t *buf = frame;
    *buf++ = MODULE_ADDRESS;
    *buf++ = 24 + lenAdjust; // 1(ID) + 22(channel data) + (+1 extra byte if Switch mode) + 1(CRC)
    uint8_t *crc_start = buf;
    *buf++ = CHANNELS_ID;
    uint32_t bits = 0;
    uint8_t bitsavailable = 0;
    for (int i = 0; i < CROSSFIRE_CHANNELS_COUNT; i++)
    {
        uint32_t val = limit(0, CROSSFIRE_CENTER + (CROSSFIRE_CENTER_CH_OFFSET(i) * 4) / 5 + (pulses[i] * 4) / 5,
                             2 * CROSSFIRE_CENTER);
        bits |= val << bitsavailable;
        bitsavailable += CROSSFIRE_CH_BITS;
        while (bitsavailable >= 8)
        {
            *buf++ = bits;
            bits >>= 8;
            bitsavailable -= 8;
        }
    }

    if (armingMode == ARMING_MODE_SWITCH)
    {
        *buf++ = true; // commanded armed status in Switch mode
    }

    *buf++ = crc8(crc_start, 23 + lenAdjust);
    return buf - frame;
}