#include "../../include/core/engine.h"

void _engine_setTargetFps(Engine self, f64 fps) {
    if (fps < 0.0) fps = 30.0;
    self->targetFrameTime = 1.0 / fps;
}

void _engine_getFrameTime(Engine self) {
    f64 dt = self->clk.tick(&self->clk);

    if (self->targetFrameTime > 0.0 && dt < self->targetFrameTime) {
        ui64 await = (self->targetFrameTime - dt) * 1e3 + 0.5;
        self->clk.wait(await);

        dt = self->clk.tick(&self->clk);
    }

    self->dt = dt;
}

LoopEvent _engine_handleEvent(Engine self, Panel p) {
    if (!self || !p || !p->body) return newLoopEvent();

    LoopEvent ret;

    if (!self->events->poll(self->events, &ret)) return newLoopEvent();
    if (ret.id == EVENT_KEY) return p->dispatch(p, ret.key);

    return ret;
}

Engine newEngine(void) {
    Engine e = calloc(1, sizeof(*e));
    if (!e) raise(ERROR, "OOM");

    e->clk = newClock();
    e->events = newEventQueue();

    e->input_interpreter = newInputInterpreter(e->events);
    e->background_interpreter = newBackgroundInterpreter(e->events);

    e->input_interpreter->listen(e->input_interpreter);
    e->background_interpreter->listen(e->background_interpreter);

    e->getFrameTime = _engine_getFrameTime;
    e->setTargetFps = _engine_setTargetFps;
    e->handleEvent = _engine_handleEvent;
    
    return e;
}
