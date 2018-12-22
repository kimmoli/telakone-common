#include <stdlib.h>
#include "hal.h"
#include "auxmotor.h"
#include "pwm.h"
#include "helpers.h"
#include "chevents.h"

static virtual_timer_t linearaccel_vt;

const int linearaccelstep = 10;
const int linearacceldelay = 50;

MotorConfig motorconf[2];

static THD_FUNCTION(auxmotorThread, arg)
{
    MotorConfig *mc = (MotorConfig *) arg;

    event_listener_t elAuxMotor;
    eventflags_t  flags;

    chEvtRegisterMask(&mc->event, &elAuxMotor, 0x100 << mc->motor);

    while (!chThdShouldTerminateX())
    {
        if (chEvtWaitAnyTimeout(0x100 << mc->motor, MS2ST(25)) != 0)
        {
            flags = chEvtGetAndClearFlags(&elAuxMotor);

            if (flags & AUXMOTOR_EVENT_STOP)
            {
                auxmotorControl(mc, 0);
            }
            else if (flags & AUXMOTOR_EVENT_SET)
            {
                auxmotorControl(mc, (int8_t)(flags & 0xff));
            }
        }
    }

    chThdExit(MSG_OK);
}

void startAuxmotorThread(int motor)
{
    motorconf[motor].motor = motor;

    if (motor == 0)
    {
        motorconf[motor].motorl1 = LINE_MOTOR1_L1;
        motorconf[motor].motorl2 = LINE_MOTOR1_L2;
        motorconf[motor].motorh1_pwm = TK_PWM_MOTOR1H1;
        motorconf[motor].motorh2_pwm = TK_PWM_MOTOR1H2;
    }
    else
    {
        motorconf[motor].motorl1 = LINE_MOTOR2_L1;
        motorconf[motor].motorl2 = LINE_MOTOR2_L2;
        motorconf[motor].motorh1_pwm = TK_PWM_MOTOR2H1;
        motorconf[motor].motorh2_pwm = TK_PWM_MOTOR2H2;
    }

    motorconf[motor].newValue = 0;
    motorconf[motor].prevValue = 0;
    motorconf[motor].currentValue = 0;

    chEvtObjectInit(&motorconf[motor].event);

    chThdCreateFromHeap(NULL, THD_WORKING_AREA_SIZE(1024), (motor == 0 ? "auxmotor0" : "auxmotor1"), NORMALPRIO+1, auxmotorThread, &motorconf[motor]);
}

void linearaccelcb(void *arg)
{
    MotorConfig *mc = arg;

    if (mc->newValue > (mc->currentValue + linearaccelstep))
        mc->currentValue += linearaccelstep;
    else if (mc->newValue < (mc->currentValue - linearaccelstep))
        mc->currentValue -= linearaccelstep;
    else
        mc->currentValue = mc->newValue;

    if (mc->prevValue * mc->currentValue < 0) /* change direction */
    {
        mc->currentValue = 0;
    }

    if (mc->currentValue == 0) /* stop */
    {
        palClearLine(mc->motorl1);//LINE_MOTORL1);
        palClearLine(mc->motorl2);//LINE_MOTORL2);
        pwmSetChannel(mc->motorh1_pwm, 100, 0);//TK_PWM_MOTORH1, 100, 0);
        pwmSetChannel(mc->motorh2_pwm, 100, 0);//TK_PWM_MOTORH2, 100, 0);
    }
    else if (mc->currentValue < 0) /* in */
    {
        palSetLine(mc->motorl1);//LINE_MOTORL1);
        palClearLine(mc->motorl2);//LINE_MOTORL2);
        pwmSetChannel(mc->motorh1_pwm, 100, 0);//TK_PWM_MOTORH1, 100, 0);
        pwmSetChannel(mc->motorh2_pwm, 100, abs(mc->currentValue));//TK_PWM_MOTORH2, 100, abs(currentValue));
    }
    else if (mc->currentValue) /* out */
    {
        palClearLine(mc->motorl1);//LINE_MOTORL1);
        palSetLine(mc->motorl2);//LINE_MOTORL2);
        pwmSetChannel(mc->motorh1_pwm, 100, abs(mc->currentValue));//TK_PWM_MOTORH1, 100, abs(currentValue));
        pwmSetChannel(mc->motorh2_pwm, 100, 0);//TK_PWM_MOTORH2, 100, 0);
    }

    mc->prevValue = mc->currentValue;

    if (mc->newValue != mc->currentValue)
    {
        chSysLockFromISR();
        chVTSetI(&linearaccel_vt, MS2ST(linearacceldelay), linearaccelcb, mc);
        chSysUnlockFromISR();
    }
}

void auxmotorControl(MotorConfig *mc, int value)
{
    if (value == mc->currentValue)
        return;

    mc->newValue = value;
    PRINT("motor %d\n\r", mc->motor);

    chVTSet(&linearaccel_vt, MS2ST(linearacceldelay), linearaccelcb, mc);
}
