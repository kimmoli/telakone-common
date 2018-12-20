#include <stdlib.h>
#include <string.h>
#include "hal.h"
#include "chprintf.h"
#include "shellcommands.h"
#include "auxlink.h"
#include "messaging.h"

void cmd_ping(BaseSequentialStream *chp, int argc, char *argv[])
{
    tk_message_t message;

    if (argc != 1)
    {
        chprintf(chp, "ping node\n\r");
        return;
    }

    message.header = TK_MESSAGE_HEADER;
    message.fromNode = myAuxlinkAddress;
    message.toNode = strtol(argv[0], NULL, 0);
    message.sequence = 0;
    message.destination = DEST_PING;
    message.event = 0x12345678;

    chprintf(chp, "pinging %02x\n\r", message.toNode);

    auxLinkTransmit(sizeof(tk_message_t), (uint8_t *) &message);
}

