#include <stdlib.h>
#include <string.h>
#include "hal.h"
#include "chprintf.h"
#include "shellcommands.h"
#include "helpers.h"

void cmd_dmw(BaseSequentialStream *chp, int argc, char *argv[])
{
    const int readblocksize = 4;

    if (argc > 0)
    {
        uint32_t count = 16;

        if (argc > 1)
            count = strtol(argv[1], NULL, 10);

        uint32_t address = strtol(argv[0], NULL, 16) & 0xFFFFFFFC;
        uint32_t end = address + ((count-1) * sizeof(uint32_t));

        do
        {
            int i = 0;
            int j = 0;
            uint32_t readbuf[readblocksize];

            chprintf(chp, "%08lX ", address);

            while ((i < readblocksize) && (address <= end))
            {
                readbuf[i++] = *(volatile uint32_t*)address;
                address += sizeof(uint32_t);
            }

            for(j=0;j<i;j++)
                chprintf(chp, " %08X", readbuf[j]);

            chprintf(chp, "\n\r");
        }
        while (address <= end);
    }
    else
    {
       chprintf(chp, "dmw address (count)\n\r");
    }
}

void cmd_dmb(BaseSequentialStream *chp, int argc, char *argv[])
{
    const int readblocksize = 16;

    if (argc > 0)
    {
        uint32_t count = 128;

        if (argc > 1)
            count = strtol(argv[1], NULL, 10);

        uint32_t address = strtol(argv[0], NULL, 16) & 0xFFFFFFFC;
        uint32_t end = address + ((count-1) * sizeof(uint8_t));

        do
        {
            int i = 0;
            int j = 0;
            uint8_t readbuf[readblocksize];

            chprintf(chp, "%08lX ", address);

            while ((i < readblocksize) && (address <= end))
            {
                readbuf[i++] = *(volatile uint8_t*)address;
                address += sizeof(uint8_t);
            }

            for(j=0;j<i;j++)
                chprintf(chp, " %02X", readbuf[j]);

            for(j=0;j<(readblocksize-i)+1;j++)
                chprintf(chp, "   ");

            for(j=0;j<i;j++)
                chprintf(chp, "%c", ISPRINT(readbuf[j]));

            chprintf(chp, "\n\r");
        }
        while (address <= end);
    }
    else
    {
       chprintf(chp, "dmb address (count)\n\r");
    }
}
