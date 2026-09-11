#include "../../include/gameplay/choice.h"

Choice newChoice(c_str text, sideEffectFunc sideEffect, i64 goingTo) {
    Choice c = calloc(1, sizeof(*c));
    if (!c) raise(ERROR, "OOM");

    c->text = text;
    c->sideEffect = sideEffect;
    c->goingTo = goingTo;

    return c;
}
