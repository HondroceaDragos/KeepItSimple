#include "../../include/core/input_interpreter.h"

void *_input_interpreter_body_func(void *args) {
    InputInterpreter self = (InputInterpreter)args;

    while (atomic_load(&self->shouldRun)) {
        if (!portable_kbhit()) {
            struct timespec ts = {};
            ts.tv_sec = 0;
            ts.tv_nsec = 1000;
            nanosleep(&ts, nullptr);
            continue;
        }

        i32 input = portable_getch();
        i8 *key = cstrfmt("%c", input);

        pthread_mutex_lock(&self->mut);
        self->events->push.rear(self->events, key);
        pthread_mutex_unlock(&self->mut);
    }

    return nullptr;
}

void _input_interpreter_listen(InputInterpreter self) {
    if (!self || atomic_load(&self->shouldRun)) return;
    atomic_store(&self->shouldRun, true);
    pthread_create(&self->body, nullptr, _input_interpreter_body_func, self);
}

void _input_interpreter_close(InputInterpreter self) {
    if (!self || !atomic_load(&self->shouldRun)) return;
    atomic_store(&self->shouldRun, false);
    pthread_join(self->body, nullptr);
}

bool _input_interpreter_poll(InputInterpreter self, i8 **ret) {
    if (!self || !ret) return false;

    pthread_mutex_lock(&self->mut);
    bool shouldPoll = false;
    if (self->events->size) {
        *ret = self->events->pop.front(self->events);
        shouldPoll = true;
    }
    pthread_mutex_unlock(&self->mut);

    return shouldPoll;
}

InputInterpreter newInputInterpreter(void) {
    InputInterpreter i = calloc(1, sizeof(*i));
    if (!i) raise(ERROR, "OOM");

    i->events = newDeque(c_str);
    pthread_mutex_init(&i->mut, nullptr);

    i->listen = _input_interpreter_listen;
    i->close = _input_interpreter_close;
    i->poll = _input_interpreter_poll;

    return i;
}
