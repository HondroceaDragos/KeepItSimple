#pragma once

#include "templates.h"

typedef (*sideEffectFunc)(i32);

typedef struct _choice *Choice;
struct _choice {
    c_str text;
    sideEffectFunc sideEffect;
    i64 goingTo;
};

Choice newChoice(c_str text, sideEffectFunc sideEffect, i64 goingTo);

deleteDefine(Choice) {
    if (!self || !*self) return;
    // TBD if anything else needs to-be-freed
    *self = nullptr;
}
ArrayType(Choice)
VectorType(Choice)
