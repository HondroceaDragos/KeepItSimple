#pragma once

#include "panel.h"

typedef struct _renderer *Renderer;
struct _renderer {
    void (*drawPanel)(Renderer, Panel, size_t);
};

Renderer newRenderer();

deleteDefine(Renderer) {
    if (!self || !*self) return;

    free(*self);
    *self = nullptr;
}
