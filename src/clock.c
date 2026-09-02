#include "../include/clock.h"

void _clock_wait(ui64 ms) {
    struct timespec ts = {};

    ts.tv_sec = (time_t)(ms / 1000);
    ts.tv_nsec = (ms % 1000) * 1000000;

    nanosleep(&ts, nullptr);
}

Clock newClock() {
    return (Clock){.wait = _clock_wait};
}
