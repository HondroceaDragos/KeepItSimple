#pragma once

#include "../templates.h"

#include "../../utils/Print/printer.h"
#include "../../utils/SeaCore/stdc.h"

#include <pthread.h>

deleteType(c_str)
NodeType(c_str)
ArrayType(c_str)
DequeType(c_str)

typedef struct _input_interpreter *InputInterpreter;
struct _input_interpreter {
    pthread_t body;
    pthread_mutex_t mut;

    atomic_bool shouldRun;
    Deque(c_str) events;

    void (*listen)(InputInterpreter);
    bool (*poll)(InputInterpreter, i8 **);
    void (*close)(InputInterpreter);
};

InputInterpreter newInputInterpreter(void);

deleteDefine(InputInterpreter) {
    if (!self || !*self) return;

    (*self)->close(*self);
    pthread_mutex_destroy(&(*self)->mut);
    delete(Deque(c_str))(&(*self)->events);
    free(*self);
    *self = nullptr;
}
