#include "../include/choice.h"

Choice newChoice(c_str text, void *sideEffect) {
    Choice c = calloc(1, sizeof(*c));
    if (!c) raise(ERROR, "OOM");

    c->text = text;
    c->sideEffect = sideEffect;

    return c;
}
