#include "args.h"


#if !defined (_WIN32) && !defined (_WIN64)
#define LINUX

#include <unistd.h>

#else
#define WINDOWS

#include <windows.h>

#endif

Args::Args() {
    c1 = 1.0;
    c2 = 0.4;
    p1 = 0.1;
    p2 = 0.2;
    p3 = 0.2;
    p4 = 0.2;
    p5 = 0.1;
    p6 = 0.2;
    activateFuncName = "sigmoid";

    mutateTime = 1;
    mateOdds = 1;
    mutateOdds = 1;
    maxMutateTime = 10;
    weightA = -1;
    weightB = 1;
    biasA = -1;
    biasB = 1;

    cpuNum = 1;
#if defined (LINUX)
    cpuNum = sysconf(_SC_NPROCESSORS_CONF);
#elif defined (WINDOWS)
    SYSTEM_INFO si;
    GetSystemInfo(&si);
    cpuNum = si.dwNumberOfProcessors;
#endif

    cleanOdds = 0.01;
}

Args::~Args() {

}
