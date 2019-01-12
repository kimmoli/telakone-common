COMMONSRC = $(COMMON)/helpers.c \
            $(COMMON)/shell/cmd_reboot.c \
            $(COMMON)/shell/cmd_dm.c \
            $(COMMON)/shell/cmd_diic.c \
            $(COMMON)/shell/cmd_piic.c \
            $(COMMON)/shell/cmd_probe.c \
            $(COMMON)/threads/threadkiller.c

MESSAGINGSRC = $(COMMON)/shell/cmd_send.c \
               $(COMMON)/shell/cmd_ping.c \
               $(COMMON)/threads/auxlink.c \
               $(COMMON)/threads/messaging.c

AUXMOTORSRC = $(COMMON)/shell/cmd_auxmotor.c \
              $(COMMON)/threads/auxmotor.c

COMMONINC = $(COMMON)/inc
