#include "../include/clock.h"

void _clock_wait(ui64 ms) {
    struct timespec ts = {};

    ts.tv_sec = (time_t)(ms / 1000);
    ts.tv_nsec = (ms % 1000) * 1000000;

    nanosleep(&ts, nullptr);
}

f64 _clock_tick(Clock *self) {
    struct timespec ts = {};
    clock_gettime(CLOCK_MONOTONIC, &ts);

    f64 elapsed = (ts.tv_sec - self->now.tv_sec) +
        1.0 * (ts.tv_nsec - self->now.tv_nsec) / 1e9;

    self->now = ts;

    return elapsed;
}

Clock newClock() {
    Clock c = {.wait = _clock_wait, .tick = _clock_tick};
    clock_gettime(CLOCK_MONOTONIC, &c.now);
    return c;
}
