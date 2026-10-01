#pragma once

#include "../../utils/SeaCore/stdc.h"

typedef enum : i8 {
    EVENT_NOP = 0,
    EVENT_KEY,
    EVENT_QUIT,
    EVENT_PAUSE,
    EVENT_CHOICE,
    EVENT_ENDGAME
} LoopEventId;

typedef struct _loop_event LoopEvent;
struct _loop_event {
    LoopEventId id;
    void *ctx;

    i64 target;  // this applies to whom?
    i8 *key;  // polymorphisms breaks too much - always store a key
};

LoopEvent _newLoopEvent(struct _loop_event);
#define newLoopEvent(...) _newLoopEvent((LoopEvent){__VA_ARGS__})

deleteType(LoopEvent)
ArrayType(LoopEvent)
NodeType(LoopEvent)
DequeType(LoopEvent)
