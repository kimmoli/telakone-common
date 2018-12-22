#ifdef (MOTORS)
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

    chprintf(chp, "am motor(0,1) speed(in -100..0..100 out)\n\r");
    chprintf(chp, "motor 0 L1 %s H1 %d %%\n\r", palReadLine(motorconf[0].motorl1) == PAL_HIGH ? "high" : "low", pwmGetChannel(motorconf[0].motorh1_pwm, 100));
    chprintf(chp, "motor 0 L2 %s H2 %d %%\n\r", palReadLine(motorconf[0].motorl2) == PAL_HIGH ? "high" : "low", pwmGetChannel(motorconf[0].motorh2_pwm, 100));
    chprintf(chp, "motor 1 L1 %s H1 %d %%\n\r", palReadLine(motorconf[1].motorl1) == PAL_HIGH ? "high" : "low", pwmGetChannel(motorconf[1].motorh1_pwm, 100));
    chprintf(chp, "motor 1 L2 %s H2 %d %%\n\r", palReadLine(motorconf[1].motorl2) == PAL_HIGH ? "high" : "low", pwmGetChannel(motorconf[1].motorh2_pwm, 100));
}

#endif // MOTORS
