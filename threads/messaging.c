#include <stdlib.h>
#include <string.h>
#include "messaging.h"
#include "auxlink.h"
#include "hal.h"
#include "chprintf.h"
#include "helpers.h"
#include "pwm.h"
#include "time.h"
#include "auxmotor.h"

#define MESSAGEBUFFERSIZE 10

static mailbox_t messagingMBox;
static msg_t messagingBuff[MESSAGEBUFFERSIZE];
static messagingMessage_t messages[MESSAGEBUFFERSIZE];
static GUARDEDMEMORYPOOL_DECL(messagingMBoxPool, sizeof(messages), PORT_NATURAL_ALIGN);

static THD_WORKING_AREA(messagingThreadWA, 4096);

static THD_FUNCTION(messagingThread, arg)
{
    (void)arg;
    tk_message_t replyMessage;

    replyMessage.header = TK_MESSAGE_HEADER;
    replyMessage.fromNode = myAuxlinkAddress;

    msg_t res;
    msg_t mmst;

    chRegSetThreadName("messaging");

    while (true)
    {
        res = chMBFetchTimeout(&messagingMBox, &mmst, TIME_INFINITE);
        if (res == MSG_OK)
        {
            messagingMessage_t *mmp = (messagingMessage_t *)mmst;

            if ((mmp->messagingEvent & MESSAGING_EVENT_SEND) && (mmp->payloadType == PAYLOAD_EVENT))
            {
                tk_message_t *msg = (tk_message_t *) mmp->payload;

                if (msg->header == TK_MESSAGE_HEADER && msg->toNode == myAuxlinkAddress)
                {
                    switch (msg->destination)
                    {
                        case DEST_PING:
                            PRINT("Got ping from %02x, replying...\n\r", msg->fromNode);

                            replyMessage.toNode = msg->fromNode;
                            replyMessage.sequence = msg->sequence;
                            replyMessage.event = 0;
                            replyMessage.destination = REPLY | DEST_PING;
                            auxLinkTransmit(sizeof(tk_message_t), (uint8_t *) &replyMessage);
                            break;

                        case DEST_PWM:
                            pwmSetChannel((msg->event & 0xff00) >> 8, 100, msg->event & 0xff);
                            break;
#ifdef BOARD_AUXGPS
                        case DEST_GPS:
                            chEvtBroadcastFlags(&gpsEvent, GPSEVENT_GET | (msg->fromNode << 16));
                            break;
#endif

#ifdef BOARD_AUXIO
                        case DEST_AUXMOTOR:
                            chEvtBroadcastFlags(&motorconf[(msg->event & 0xF0000) >> 16].event, msg->event & 0xFFFF);
                            break;
#endif

                        default:
                            PRINT("Unknown message received from %02x, destination %04x\n\r", msg->fromNode,
                                                                                              msg->destination);
                            break;
                    }
                }
            }
            else if ((mmp->messagingEvent & MESSAGING_EVENT_REPLY) && (mmp->payloadType == PAYLOAD_EVENT))
            {
                tk_message_t *msg = (tk_message_t *) mmp->payload;

                if (msg->header == TK_MESSAGE_HEADER && msg->toNode == myAuxlinkAddress)
                {
                    switch (msg->destination)
                    {
                        case REPLY | DEST_PING:
                            PRINT("Ping replied ok\n\r");
                            break;

                        default:
                            break;
                    }
                }
            }
            else if ((mmp->messagingEvent & MESSAGING_EVENT_REPLY) && (mmp->payloadType == PAYLOAD_GPS))
            {
                tk_gpsmessage_t *msg = (tk_gpsmessage_t *) mmp->payload;

                if (msg->header == TK_MESSAGE_HEADER && msg->toNode == myAuxlinkAddress)
                {
                    switch (msg->destination)
                    {
                        case REPLY | DEST_GPS:
                            PRINT("Got GPS reply\n\r");

                            gps_output_params_t *gps = (gps_output_params_t *)&msg->data;

                            struct tm *timp;
                            time_t utc_sec;

                            utc_sec = gps->UTC_sec;
                            timp = localtime(&utc_sec);

                            PRINT("GPS Fix=%d, Lat=%.6f Lon=%.6f Speed=%.1f Time=%s\r",
                                gps->fix_status,
                                gps->latitude,
                                gps->longitude,
                                gps->speed,
                                asctime(timp));

                            break;

                        default:
                            break;
                    }
                }
            }

            chGuardedPoolFree(&messagingMBoxPool, (void*) mmst);
        }
    }
}

void startMessagingThread(void)
{
    chGuardedPoolObjectInit(&messagingMBoxPool, sizeof(messagingMessage_t));
    chGuardedPoolLoadArray(&messagingMBoxPool, messages, MESSAGEBUFFERSIZE);
    chMBObjectInit(&messagingMBox, messagingBuff, MESSAGEBUFFERSIZE);

    (void)chThdCreateStatic(messagingThreadWA, sizeof(messagingThreadWA), HIGHPRIO, messagingThread, NULL);
}

msg_t sendMessage(messagingMessage_t *newMsg)
{
    msg_t m;
    msg_t res = MSG_OK;

    m = (msg_t) chGuardedPoolAllocTimeout(&messagingMBoxPool, TIME_IMMEDIATE);

    if ((void *) m != NULL)
    {
        memcpy((void *) m, newMsg, sizeof(messagingMessage_t));
        res = chMBPostTimeout(&messagingMBox, (msg_t) m, TIME_IMMEDIATE);
    }
    else
    {
        res = MSG_RESET;
    }
    return res;
}

