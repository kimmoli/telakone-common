#include <stdlib.h>
#include <string.h>
#include "messaging.h"
#include "auxlink.h"
#include "hal.h"
#include "chprintf.h"
#include "helpers.h"
#include "pwm.h"

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
    replyMessage.destination = DEST_ACK;

    msg_t res;
    msg_t mmst;

    chRegSetThreadName("messaging");

    while (true)
    {
        res = chMBFetchTimeout(&messagingMBox, &mmst, TIME_INFINITE);
        if (res == MSG_OK)
        {
            messagingMessage_t *mmp = (messagingMessage_t *)mmst;

            if (mmp->messagingEvent & MESSAGING_EVENT_SEND)
            {
                if (mmp->message.header == TK_MESSAGE_HEADER && mmp->message.toNode == myAuxlinkAddress)
                {
                    switch (mmp->message.destination)
                    {
                        case DEST_ACK:
                            PRINT("OK\n\r");
                            break;

                        case DEST_PING:
                            PRINT("Got ping from %02x, replying...\n\r", mmp->message.fromNode);

                            replyMessage.toNode = mmp->message.fromNode;
                            replyMessage.sequence = mmp->message.sequence;
                            replyMessage.event = 0;
                            auxLinkTransmit(sizeof(tk_message_t), (uint8_t *) &replyMessage);
                            break;

                        case DEST_PWM:
                            pwmSetChannel((mmp->message.event & 0xff00) >> 8, 100, mmp->message.event & 0xff);

                            replyMessage.toNode = mmp->message.fromNode;
                            replyMessage.sequence = mmp->message.sequence;
                            replyMessage.event = 0;
                            auxLinkTransmit(sizeof(tk_message_t), (uint8_t *) &replyMessage);
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

