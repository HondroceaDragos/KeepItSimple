#pragma once

#include "clock.h"

#include "input_interpreter.h"
#include "background_interpreter.h"
#include "../rendering/panel.h"  // kinda shitty

typedef struct _engine *Engine;
struct _engine {
    f64 dt;
    f64 targetFrameTime;

    Clock clk;

    EventQueue events;

    InputInterpreter input_interpreter;
    BackgroundInterpreter background_interpreter;

    void (*setTargetFps)(Engine, f64);
    void (*getFrameTime)(Engine);
    LoopEvent (*handleEvent)(Engine, Panel);
};

Engine newEngine(void);

deleteDefine(Engine) {
    if (!self || !*self) return;

    delete(InputInterpreter)(&(*self)->input_interpreter);
    delete(BackgroundInterpreter)(&(*self)->background_interpreter);

    free(*self);
    *self = nullptr;
}
