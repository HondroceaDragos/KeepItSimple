#pragma once

#include "../templates.h"

#include "../../utils/Print/printer.h"
#include "../../utils/SeaCore/stdc.h"

#include "event_queue.h"

typedef struct _input_interpreter *InputInterpreter;
struct _input_interpreter {
    pthread_t body;
    atomic_bool shouldRun;

    EventQueue queue;

    void (*listen)(InputInterpreter);
    void (*close)(InputInterpreter);
};

InputInterpreter newInputInterpreter(EventQueue);

deleteDefine(InputInterpreter) {
    if (!self || !*self) return;

    (*self)->close(*self);
    delete(EventQueue)(&(*self)->queue);
    free(*self);
    *self = nullptr;
}
