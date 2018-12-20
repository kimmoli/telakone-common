#include <stdlib.h>
#include <string.h>
#include "hal.h"
#include "chprintf.h"
#include "shellcommands.h"
#include "auxlink.h"
#include "messaging.h"

void cmd_send(BaseSequentialStream *chp, int argc, char *argv[])
{
    tk_message_t message;

    if (argc != 3)
    {
        chprintf(chp, "send node dest eventdata\n\r");
        return;
    }

    message.header = TK_MESSAGE_HEADER;
    message.fromNode = myAuxlinkAddress;
    message.toNode = strtol(argv[0], NULL, 0);
    message.sequence = 0;
    message.destination = strtol(argv[1], NULL, 0);
    message.event = strtol(argv[2], NULL, 0);

    chprintf(chp, "sending to %02x, destination %04x, event data %08x\n\r", message.toNode,
                                                                            message.destination,
                                                                            message.event);

    auxLinkTransmit(sizeof(tk_message_t), (uint8_t *) &message);
}

