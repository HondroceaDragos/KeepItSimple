#include "../../include/entities/entity.h"

Entity _newEntity(Entity defaults) {
    Entity e = {};

    e.name = (defaults.name) ? defaults.name : (c_str)"John Doe";
    e.health = (defaults.health) ? defaults.health : 100;
    e.attributes = (defaults.attributes) ? defaults.attributes : nullptr;

    return e;
}
