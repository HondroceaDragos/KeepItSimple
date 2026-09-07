#pragma once

#include "writable_region.h"

typedef struct _panel *Panel;
struct _panel {
    TerminalDimensions dimensions;

    WritableRegion header;
    WritableRegion footer;
    WritableRegion body;

    struct {
        void (*header)(Panel);
        void (*footer)(Panel);
        void (*body)(Panel);
    } blit;

    struct {
        void (*header)(Panel, WritableRegion);
        void (*footer)(Panel, WritableRegion);
        void (*body)(Panel, WritableRegion);
    } addContent;

    void (*resize)(Panel);
};

Panel newPanel(TerminalDimensions *td);
