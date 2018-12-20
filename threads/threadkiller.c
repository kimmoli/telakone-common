#include <string.h>
#include "hal.h"
#include "threadkiller.h"

/*
 * Runs once in a second to check threads in FINAL state, and calls wait for them to free memory
 */

static thread_t *threadKillerTp;

static THD_FUNCTION(threadKillerThread, arg)
{
    (void)arg;

    thread_t *tp;

    while (!chThdShouldTerminateX())
    {
        tp = chRegFirstThread();

        do
        {
            if (tp->state == CH_STATE_FINAL)
            {
                chThdWait(tp);
            }
            tp = chRegNextThread(tp);
        }
        while (tp != NULL);

        chThdSleepSeconds(1);
    }

    chThdExit(MSG_OK);
}

void startThreadKiller(void)
{
    threadKillerTp = chThdCreateFromHeap(NULL, THD_WORKING_AREA_SIZE(128), "threadkiller", NORMALPRIO+1, threadKillerThread, NULL);
}

void stopThreadKiller(void)
{
    if (threadKillerTp)
    {
        chThdTerminate(threadKillerTp);
        chThdWait(threadKillerTp);
        threadKillerTp = NULL;
    }
}
