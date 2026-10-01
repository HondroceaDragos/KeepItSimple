#pragma once

#include "../../utils/SeaCore/stdc.h"

deleteType(i32)
PairType(DictKey, i32)
ArrayType(Pair(DictKey, i32))
NodeType(Pair(DictKey, i32))
DictType(i32)

typedef struct _entity Entity;
struct _entity {
    c_str name;
    i32 health;
    Dict(i32) attributes;
};

Entity _newEntity(Entity defaults);
#define newEntity(...) _newEntity((Entity){__VA_ARGS__})
