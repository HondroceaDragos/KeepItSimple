#include "../../include/core/background_interpreter.h"

void *_background_interpreter_body_func(void *args) {
    BackgroundInterpreter self = (BackgroundInterpreter)args;

    while (atomic_load(&self->shouldRun)) {
        struct timespec ts = {};
        ts.tv_sec = 0;
        ts.tv_nsec = 1000;
        nanosleep(&ts, nullptr);
    }

    return nullptr;
}

void _background_interpreter_listen(BackgroundInterpreter self) {
    if (!self || atomic_load(&self->shouldRun)) return;
    atomic_store(&self->shouldRun, true);
    pthread_create(&self->body, nullptr, _background_interpreter_body_func, self);
}

void _background_interpreter_close(BackgroundInterpreter self) {
    if (!self || !atomic_load(&self->shouldRun)) return;
    atomic_store(&self->shouldRun, false);
    pthread_join(self->body, nullptr);
}

BackgroundInterpreter newBackgroundInterpreter(EventQueue q) {
    BackgroundInterpreter i = calloc(1, sizeof(*i));
    if (!i) raise(ERROR, "OOM");

    i->queue = q;

    i->listen = _background_interpreter_listen;
    i->close = _background_interpreter_close;

    return i;
}
