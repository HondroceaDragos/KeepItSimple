#pragma once

#include "../templates.h"

typedef void (*sideEffectFunc)(i32);

#define END_OF_PATH -1
#define USE_IDX -1

typedef struct _choice *Choice;
struct _choice {
    c_str text;
    i64 goingTo;
    sideEffectFunc sideEffect;
    i8 trigger;
};

Choice _newChoice(struct _choice);
#define newChoice(...) _newChoice((struct _choice){__VA_ARGS__})

deleteDefine(Choice) {
    if (!self || !*self) return;
    free(*self);
    *self = nullptr;
}
ArrayType(Choice)
VectorType(Choice)
