
#ifndef CPU_COUNTER_H
#define CPU_COUNTER_H

#include <iostream>
#include <sys/times.h>

/** @brief Helper to work with running times of current process. */
class CpuCounter
{
    public:
    CpuCounter();

    float   userPercent();
    float   systemPercent();
    float   childUserPercent();
    float   childSystemPercent();

    clock_t userClock();
    clock_t systemClock();
    clock_t childUserClock();
    clock_t childSystemClock();

    bool readCurrentTimes();
    clock_t ticksRunning();

    static int     clockPerSec();

    private:
    clock_t beginning;
    clock_t last;
    struct tms  times_;
};

#endif

