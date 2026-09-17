#pragma once

#include "../../utils/SeaCore/stdc.h"

typedef enum : i8 {
    EVENT_NOP = 0,
    EVENT_QUIT,
    EVENT_PAUSE,
    EVENT_CHOICE
} LoopEventId;

typedef struct _loop_event LoopEvent;
struct _loop_event {
    LoopEventId id;
    void *ctx;
};

LoopEvent _newLoopEvent(struct _loop_event);
#define newLoopEvent(...) _newLoopEvent((LoopEvent){__VA_ARGS__})

deleteType(LoopEvent);
