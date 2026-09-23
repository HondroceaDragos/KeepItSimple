#include "../../include/gameplay/choice.h"

Choice _newChoice(struct _choice defaults) {
    Choice c = calloc(1, sizeof(*c));
    if (!c) raise(ERROR, "OOM");

    c->text = (defaults.text) ? (c_str)strdup(defaults.text) : (c_str)strdup("How did you get here?");
    c->goingTo = (defaults.goingTo) ? defaults.goingTo : END_OF_PATH;

    c->sideEffect = (defaults.sideEffect) ? defaults.sideEffect : nullptr;
    c->trigger = (defaults.trigger) ? defaults.trigger : USE_IDX;

    return c;
}
