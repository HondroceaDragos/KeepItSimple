#include "../../include/core/typewriter.h"

size_t _typewriter_advance(TypeWriter self, f64 dt) {
    self->acc += self->chPerSec * dt;

    size_t rev = (size_t)self->acc;
    self->acc -= rev;

    return rev;
}

TypeWriter newTypeWriter(f64 chPerSec) {
    TypeWriter tw = calloc(1, sizeof(*tw));
    if (!tw) raise(ERROR, "OOM");

    chPerSec = (chPerSec) ? chPerSec : 25;
    tw->chPerSec = chPerSec;

    tw->advance = _typewriter_advance;

    return tw;
}
