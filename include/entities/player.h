#pragma once

#include "entity.h"
#include "../core/loop_event.h"

typedef struct _player *Player;
struct _player {
    Entity super;
};

Player _newPlayer(struct _player defaults);
#define newPlayer(...) _newPlayer((struct _player){__VA_ARGS__})

extern Player player;

LoopEvent setPlayerHealth(c_str, i32);
