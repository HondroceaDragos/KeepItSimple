#pragma once

#include "loop_event.h"
#include <pthread.h>

typedef struct _event_queue *EventQueue;
struct _event_queue {
    Deque(LoopEvent) events;

    pthread_mutex_t mutex;

    void (*push)(EventQueue, LoopEvent);
    bool (*poll)(EventQueue, LoopEvent *);
};

EventQueue newEventQueue(void);

deleteDefine(EventQueue) {
    if (!self || !*self) return;

    pthread_mutex_destroy(&(*self)->mutex);
    delete(Deque(LoopEvent))(&(*self)->events);

    free(*self);
    *self = nullptr;
}
