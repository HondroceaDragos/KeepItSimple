#pragma once

#include "time.h"

#include "../../utils/SeaCore/stdc.h"

typedef struct _clock Clock;
struct _clock {
    struct timespec now;

    void (*wait)(ui64);
    f64 (*tick)(Clock *);
};

Clock newClock();
