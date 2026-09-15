#pragma once

#include "../templates.h"

typedef void (*sideEffectFunc)(i32);

typedef struct _choice *Choice;
struct _choice {
    c_str text;
    sideEffectFunc sideEffect;
    i64 goingTo;
};

Choice newChoice(c_str text, sideEffectFunc sideEffect, i64 goingTo);

deleteDefine(Choice) {
    if (!self || !*self) return;
    free(*self);
    *self = nullptr;
}
ArrayType(Choice)
VectorType(Choice)
