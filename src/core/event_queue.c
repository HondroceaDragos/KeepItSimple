#include "../../include/core/event_queue.h"

void _event_queue_push(EventQueue self, LoopEvent event) {
    if (!self) return;

    pthread_mutex_lock(&self->mutex);

    self->events->push.rear(self->events, event);

    pthread_mutex_unlock(&self->mutex);
}

bool _event_queue_poll(EventQueue self, LoopEvent *ret) {
    if (!self || !ret) return false;

    pthread_mutex_lock(&self->mutex);

    if (!self->events->size) {
        pthread_mutex_unlock(&self->mutex);
        return false;
    }

    *ret = self->events->pop.front(self->events);

    pthread_mutex_unlock(&self->mutex);

    return true;
}

EventQueue newEventQueue(void) {
    EventQueue eq = calloc(1, sizeof(*eq));
    if (!eq) raise(ERROR, "OOM");

    eq->events = newDeque(LoopEvent);
    pthread_mutex_init(&eq->mutex, nullptr);

    eq->push = _event_queue_push;
    eq->poll = _event_queue_poll;

    return eq;
}
