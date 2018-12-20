#ifndef TK_GPS_H
#define TK_GPS_H
#include "hal.h"

#define NMEA_MAX_SIZE 512

#define GPSEVENT_1PPS     0x0001
#define GPSEVENT_DEBUGON  0x0002
#define GPSEVENT_DEBUGOFF 0x0004
#define GPSEVENT_SETRTC   0x0008
#define GPSEVENT_GET      0x0010

typedef struct //gps_output_params_t
{
    // Link statistics
    uint32_t  link_heartbeat;  // Incremented when an NMEA sentence has been received
    uint32_t  err_cntr;        // Incremented when there is a (checksum) error in received NMEA sentence

    // Parsed NMEA data
    uint32_t   UTC_sec;        // Epoch seconds
    uint32_t   UTC_ms;         // Milliseconds
    float      longitude;      // Longitude in +/- decimal degrees
    float      latitude;       // Latitude in +/- decimal degrees
    float      speed;          // Speed in km/h
    float      altitude;       // Altitude in meters
    float      pdop;           // Position dilution of precision (from GPS, not GLONASS)
    uint8_t    fix_status;     // Fix status (0: Invalid, 1: GNSS fix, 2: DGPS fix, 6: Estimated mode)
    uint8_t    pos_mode;       // Positioning mode (0: No fix, 1: Autonomous GNSS fix, 2: Differential GNSS fix)
    uint8_t    antenna_status; // Antenna status (0: Undefined, 1: OK, 2: Open, 3: Short) (Note, this is only available with A3569 board, Quectel L26 receiver)
    uint8_t    num_sats_used;  // Number of satellites being used (0 - 12)
    int32_t    tz;             // timezone

} gps_output_params_t;// __attribute__((packed));

extern gps_output_params_t gps_output;
extern event_source_t gpsEvent;

void startGpsThread(void);

#endif

