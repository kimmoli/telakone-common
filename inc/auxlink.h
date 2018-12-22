#ifndef TK_AUXLINK_H
#define TK_AUXLINK_H

#define AUXLINK_MAX_MSG_SIZE 64

extern uint8_t myAuxlinkAddress;

void startAuxLinkThread(void);
void auxLinkInit(uint8_t address);
void auxLinkTransmit(int count, uint8_t * buf);

#endif

