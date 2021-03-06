#ifndef TK_MESSAGING_H
#define TK_MESSAGING_H

#include "hal.h"
#include "gps.h"

#define MSGBUFSIZE 64

#define TK_MESSAGE_HEADER 0x544b

#define MESSAGING_EVENT_SEND  0x1000
#define MESSAGING_EVENT_REPLY 0x2000

#define DEST_PING     0x0001
#define DEST_BLINKER  0x0002
#define DEST_PWM      0x0003
#define DEST_WIFI     0x0004
#define DEST_AUXMOTOR 0x0005
#define DEST_DRIVE    0x0006
#define DEST_JOYSTICK 0x0007
#define DEST_GPS      0x0008

#define REPLY         0x8000

#define MESSAGING_TCP     0x01
#define MESSAGING_UDP     0x02
#define MESSAGING_AUXLINK 0x03

#define PAYLOAD_EVENT 0x01
#define PAYLOAD_GPS   0x08

typedef struct
{
    uint16_t header;
    uint16_t toNode;
    uint16_t fromNode;
    uint16_t sequence;
    uint16_t destination;
    uint32_t event;
} tk_message_t;

typedef struct
{
    uint16_t header;
    uint16_t toNode;
    uint16_t fromNode;
    uint16_t sequence;
    uint16_t destination;
    gps_output_params_t data;
} tk_gpsmessage_t;

typedef struct
{
    uint8_t channel;
    uint8_t node;
    uint32_t ipAddress;
    uint16_t port;
} tk_source_t;

typedef struct
{
    uint16_t messagingEvent;
    tk_source_t source;
    uint8_t payloadType;
    uint8_t payload[64];
} messagingMessage_t;

extern void startMessagingThread(void);
extern msg_t sendMessage(messagingMessage_t *newMsg);

#endif
