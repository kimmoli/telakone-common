#include <stdlib.h>
#include "hal.h"
#include "chprintf.h"
#include "shellcommands.h"
#include "pwm.h"
#include "auxmotor.h"

void cmd_auxmotor(BaseSequentialStream *chp, int argc, char *argv[])
{
    int newValue;
    int motor;

    if (argc == 2)
    {
        motor = strtol(argv[0], NULL, 0);
        newValue = strtol(argv[1], NULL, 0);

        if (motor >= MOTORS)
        {
            chprintf(chp, "Motor %d not available. This board has %d motors\n\r", motor, MOTORS);
            return;
        }

        if (newValue == 0)
        {
            chprintf(chp, "Aux motor %d stop\n\r", motor);
            chEvtBroadcastFlags(&motorconf[motor].event, AUXMOTOR_EVENT_STOP);
            return;
        }
        else if (newValue >= -100 && newValue <= 100)
        {
            chprintf(chp, "Aux motor %d direction %s at %d %% speed\n\r", motor, ((newValue<0) ? "in" : "out"), abs(newValue));
            chEvtBroadcastFlags(&motorconf[motor].event, AUXMOTOR_EVENT_SET | (uint8_t)(newValue & 0xff));
            return;
        }
    }

    chprintf(chp, "am motor speed\n\r");
    chprintf(chp, "motor = 0..%d\n\r", MOTORS-1);
    chprintf(chp, "speed = -100, 0, 100\n\r");
    chprintf(chp, "\n\r");

    for (int m=0 ; m < MOTORS ; m++)
    {
        chprintf(chp, "motor %d L1 %s H1 %d %% L2 %s H2 %d %%\n\r",
                 m,
                 palReadLine(motorconf[m].motorl1) == PAL_HIGH ? "high" : "low",
                 pwmGetChannel(motorconf[m].motorh1_pwm, 100),
                 palReadLine(motorconf[m].motorl2) == PAL_HIGH ? "high" : "low",
                 pwmGetChannel(motorconf[m].motorh2_pwm, 100));
    }
}
