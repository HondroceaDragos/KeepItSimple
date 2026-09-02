#pragma once

#include "time.h"

#include "../utils/SeaCore/stdc.h"

typedef struct _clock {
    void (*wait)(ui64);
} Clock;

Clock newClock();
