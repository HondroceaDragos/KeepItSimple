#include "../../include/entities/player.h"

Player player = nullptr;

Player _newPlayer(struct _player defaults) {
    Player p = calloc(1, sizeof(*p));
    if (!p) raise(ERROR, "OOM");

    c_str name = defaults.super.name;
    i32 health = defaults.super.health;
    auto attributes = defaults.super.attributes;

    p->super = newEntity(name, health, attributes);

    return p;
}

LoopEvent setPlayerHealth(c_str, i32 ammount) {
    player->super.health += ammount;

    if (player->super.health <= 0) {
        player->super.health = 0;
        return newLoopEvent(EVENT_ENDGAME);
    }

    return newLoopEvent();
}
