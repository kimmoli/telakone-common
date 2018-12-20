#include <string.h>
#include "ch.h"
#include "hal.h"
#include "chprintf.h"
#include "auxlink.h"
#include "helpers.h"
#include "messaging.h"

uint8_t myAuxlinkAddress;

static SerialConfig auxlinkConfig =
{
    /* speed */ 100000,
    /* CR1 */ 0,
    /* CR2 */ 0,
    /* CR3 */ 0
};

static THD_FUNCTION(auxLinkThread, arg)
{
    (void)arg;
    uint8_t rxBuf[AUXLINK_MAX_MSG_SIZE];
    int count = 0;

    while (!chThdShouldTerminateX())
    {
        msg_t charbuf;
        do
        {
            charbuf = chnGetTimeout(&SD2, MS2ST(10));

            if (charbuf != Q_TIMEOUT)
            {
                rxBuf[count++] = charbuf;

                if (count >= AUXLINK_MAX_MSG_SIZE)
                {
                    DEBUG("Overflow\n\r");
                    count = 0;
                }
            }
        }
        while (charbuf != Q_TIMEOUT);

        if (count > 0)
        {
            count = 0;
            messagingMessage_t m = {0};
            m.messagingEvent = MESSAGING_EVENT_SEND;
            m.source.channel = MESSAGING_AUXLINK;
            memcpy(&m.message, rxBuf, sizeof(tk_message_t));

            if (sendMessage(&m) != MSG_OK)
                DEBUG("error\n\r");
        }

        //chThdSleepMilliseconds(50);
    }

    chThdExit(MSG_OK);
}

void startAuxLinkThread(void)
{
    chThdCreateFromHeap(NULL, THD_WORKING_AREA_SIZE(2048), "auxlink", NORMALPRIO+1, auxLinkThread, NULL);
}

void auxLinkInit(uint8_t address)
{
    myAuxlinkAddress = address;

    sdStart(&SD2, &auxlinkConfig);
}

void auxLinkTransmit(int count, uint8_t * buf)
{
    int i;

    palSetLine(LINE_ACCLINKTXE);
    chThdSleepMicroseconds(10);

    for (i=0 ; i<count ; i++)
    {
        chnPutTimeout(&SD2, *(buf+i), MS2ST(10));
        chThdSleepMicroseconds(10);
    }

    palClearLine(LINE_ACCLINKTXE);
}
