#include "../../include/core/loop_event.h"

LoopEvent _newLoopEvent(LoopEvent defaults) {
    LoopEvent ev = {};

    ev.id = (defaults.id) ? defaults.id : EVENT_NOP;
    ev.ctx = (defaults.ctx) ? defaults.ctx : nullptr;

    ev.target = (defaults.target) ? defaults.target : -1;
    ev.key = (defaults.key) ? defaults.key : (i8 *)"";

    return ev;
}
