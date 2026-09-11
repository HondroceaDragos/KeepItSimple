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

Engine newEngine() {
    Engine e = calloc(1, sizeof(*e));
    if (!e) raise(ERROR, "OOM");

    e->clk = newClock();

    e->getFrameTime = _engine_getFrameTime;
    e->setTargetFps = _engine_setTargetFps;
    
    return e;
}
