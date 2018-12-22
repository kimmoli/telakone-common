#ifndef TK_AUXMOTOR_H
#define TK_AUXMOTOR_H

#define AUXMOTOR_EVENT_STOP    0x4000
#define AUXMOTOR_EVENT_SET     0x8000

#ifdef MOTORS
#if (MOTORS == 1)
#define LINE_MOTOR1_L1 LINE_MOTORL1
#define LINE_MOTOR1_L2 LINE_MOTORL2
#define TK_PWM_MOTOR1H1 TK_PWM_MOTORH1
#define TK_PWM_MOTOR1H2 TK_PWM_MOTORH2
#endif
#endif

typedef struct
{
    int motor;
    event_source_t event;
    ioline_t motorl1;
    ioline_t motorl2;
    int motorh1_pwm;
    int motorh2_pwm;
    int newValue;
    int prevValue;
    int currentValue;
} MotorConfig;

extern MotorConfig motorconf[2];

void startAuxmotorThread(int motor);
void auxmotorControl(MotorConfig *mc, int value);

#endif
