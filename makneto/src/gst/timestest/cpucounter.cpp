
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

bool CpuCounter::readCurrentTimes()
{
    last = times(&times_);
    return (last != (clock_t) -1);
}

clock_t CpuCounter::ticksRunning()
{
    readCurrentTimes();
    return last-beginning;
}

clock_t CpuCounter::userClock()
{
    readCurrentTimes();
    return times_.tms_utime;
}

clock_t CpuCounter::systemClock()
{
    readCurrentTimes();
    return times_.tms_stime;
}

clock_t CpuCounter::childUserClock()
{
    readCurrentTimes();
    return times_.tms_cutime;
}

clock_t CpuCounter::childSystemClock()
{
    readCurrentTimes();
    return times_.tms_cstime;
}

int  CpuCounter::clockPerSec()
{
    return sysconf(_SC_CLK_TCK);
}

float CpuCounter::userPercent()
{
    return ((float) 100.0 * userClock() / ticksRunning() );
}

float CpuCounter::systemPercent()
{
    return ((float) 100.0 * systemClock() / ticksRunning() );
}

float CpuCounter::childUserPercent()
{
    return ((float) 100.0 * childUserClock() / ticksRunning() );
}

float CpuCounter::childSystemPercent()
{
    return ((float) 100.0 * childSystemClock() / ticksRunning() );
}


