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

        self->queue->push(
            self->queue, 
            newLoopEvent(.id = EVENT_KEY, .key = key)
        );

        /**
         * This Segfaults the program?
         * WHY??
         */
        // free(key);
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

InputInterpreter newInputInterpreter(EventQueue q) {
    InputInterpreter i = calloc(1, sizeof(*i));
    if (!i) raise(ERROR, "OOM");

    i->queue = q;

    i->listen = _input_interpreter_listen;
    i->close = _input_interpreter_close;

    return i;
}
