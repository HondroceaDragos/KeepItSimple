#pragma once

#include "templates.h"

typedef struct _choice *Choice;
struct _choice {
    c_str text;
    void *sideEffect;  // to-be-implemented;
};

Choice newChoice(c_str text, void *sideEffect);

deleteDefine(Choice) {
    if (!self || !*self) return;
    // TBD if anything else needs to-be-freed
    *self = nullptr;
}
ArrayType(Choice)
VectorType(Choice)
