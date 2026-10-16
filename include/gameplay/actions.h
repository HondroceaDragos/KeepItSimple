#pragma once

#include "choice.h"

#include "../rendering/panel.h"
#include "../core/loop_event.h"

static inline LoopEvent body_scroll_up(Panel p, void *) {
    if (p->body) p->body->scroll(p->body, 1);
    return newLoopEvent();
}

static inline LoopEvent body_scroll_down(Panel p, void *) {
    if (p->body) p->body->scroll(p->body, -1);
    return newLoopEvent();
}

static inline LoopEvent game_quit(Panel p, void *) {
    return newLoopEvent(EVENT_QUIT);
}

static inline LoopEvent game_pause(Panel p, void *) {
    return newLoopEvent(EVENT_PAUSE);
}
