COMMONSRC = $(COMMON)/helpers.c \
            $(COMMON)/shell/cmd_reboot.c \
            $(COMMON)/shell/cmd_dm.c \
            $(COMMON)/shell/cmd_ping.c \
            $(COMMON)/shell/cmd_send.c \
            $(COMMON)/threads/threadkiller.c \
            $(COMMON)/threads/auxlink.c \
            $(COMMON)/threads/messaging.c

AUXMOTORSRc = $(COMMON)/shell/cmd_auxmotor.c \
              $(COMMON)/threads/auxmotor.c

COMMONINC = $(COMMON)/inc
