#pragma once

#include "../templates.h"
#include "../core/loop_event.h"
#include "../core/event_queue.h"

typedef LoopEvent (*sideEffectFunc)(c_str, i32);

typedef struct _side_effect {
    sideEffectFunc func;
    c_str id;
    i32 ammount;
    i64 failsafe;
} SideEffect;

deleteType(SideEffect)
ArrayType(SideEffect)
SetType(SideEffect)

#define END_OF_PATH -1
#define USE_IDX -1

typedef struct _choice *Choice;
struct _choice {
    c_str text;
    i64 goingTo;
    Set(SideEffect) sideEffects;
    i8 trigger;

    void (*apply)(Choice, EventQueue);
};

Choice _newChoice(struct _choice);
#define newChoice(...) _newChoice((struct _choice){__VA_ARGS__})

deleteDefine(Choice) {
    if (!self || !*self) return;
    free((*self)->text);
    delete(Set(SideEffect))(&(*self)->sideEffects);
    free(*self);
    *self = nullptr;
}
ArrayType(Choice)
VectorType(Choice)

i32 se_cmp(const void *a, const void *b);
