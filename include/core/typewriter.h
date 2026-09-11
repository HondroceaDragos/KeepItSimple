#pragma once

#include "../templates.h"

typedef struct _typewriter *TypeWriter;
struct _typewriter {
    f64 chPerSec;
    f64 acc;

    size_t (*advance)(TypeWriter, f64);
};

TypeWriter newTypeWriter(f64);

deleteDefine(TypeWriter) {
    if (!self || !*self) return;

    free(*self);
    *self = nullptr;
}
