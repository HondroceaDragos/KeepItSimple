#pragma once

#include "panel.h"

typedef enum : i8 {
    STATIC_RENDER,
    DYNAMIC_RENDER
} RenderType;

typedef struct _renderer *Renderer;
struct _renderer {
    RenderType type;
    void (*drawPanel)(Renderer, Panel, size_t);
};

Renderer newRenderer();

deleteDefine(Renderer) {
    if (!self || !*self) return;

    free(*self);
    *self = nullptr;
}
