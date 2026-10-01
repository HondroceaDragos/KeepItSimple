#include "../../include/gameplay/choice.h"

i32 se_cmp(const void *a, const void *b) {
    SideEffect sea = *(SideEffect *)a;
    SideEffect seb = *(SideEffect *)b;

    return (i32)strcmp(sea.id, seb.id);
}

void _choice_apply(Choice c, EventQueue events) {
    if (!c || !c->sideEffects || !events) return;

    for (size_t idx = 0; idx < c->sideEffects->size; idx++) {
        SideEffect effect = c->sideEffects->at(c->sideEffects, idx);

        if (!effect.func) continue;

        LoopEvent event = effect.func(effect.id, effect.ammount);

        if (event.id == EVENT_ENDGAME) {
            if (effect.failsafe == END_OF_PATH) continue;

            event.target = effect.failsafe;
            events->push(events, event);
            return;
        }

        if (event.id != EVENT_NOP) events->push(events, event);
    }
}

Choice _newChoice(struct _choice defaults) {
    Choice c = calloc(1, sizeof(*c));
    if (!c) raise(ERROR, "OOM");

    c->text = (defaults.text) ? (c_str)strdup(defaults.text) : (c_str)strdup("How did you get here?");
    c->goingTo = (defaults.goingTo) ? defaults.goingTo : END_OF_PATH;

    c->sideEffects = (defaults.sideEffects) ? defaults.sideEffects : nullptr;
    c->trigger = (defaults.trigger) ? defaults.trigger : USE_IDX;

    c->apply = _choice_apply;

    return c;
}
