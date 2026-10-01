#pragma once

#include "../templates.h"

#include "../../utils/Print/printer.h"
#include "../../utils/SeaCore/stdc.h"

#include "event_queue.h"

typedef struct _background_interpreter *BackgroundInterpreter;
struct _background_interpreter {
    pthread_t body;
    atomic_bool shouldRun;

    EventQueue queue;

    void (*listen)(BackgroundInterpreter);
    void (*close)(BackgroundInterpreter);
};

BackgroundInterpreter newBackgroundInterpreter(EventQueue);

deleteDefine(BackgroundInterpreter) {
    if (!self || !*self) return;

    (*self)->close(*self);
    if (atomic_load(&(*self)->shouldRun)) (*self)->close(*self);

    free(*self);
    *self = nullptr;
}
