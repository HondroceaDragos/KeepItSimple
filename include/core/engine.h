#pragma once

#include "clock.h"

typedef struct _engine *Engine;
struct _engine {
    f64 dt;
    f64 targetFrameTime;

    Clock clk;

    void (*setTargetFps)(Engine, f64);
    void (*getFrameTime)(Engine);
};

Engine newEngine();

deleteDefine(Engine) {
    if (!self || !*self) return;
    free(*self);
    *self = nullptr;
}
