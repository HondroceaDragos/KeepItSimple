#pragma once

#include "writable_region.h"

typedef struct _panel *Panel;
struct _panel {
    WritableRegion header;
    WritableRegion footer;
    WritableRegion body;

    struct {
        void (*header)(Panel, f64);
        void (*footer)(Panel, f64);
        void (*body)(Panel, f64);
    } blit;
};
