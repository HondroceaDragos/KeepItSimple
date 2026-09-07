#pragma once

#include "templates.h"
#include "text_node.h"

typedef struct _chapter {
    c_str name;
    Vector(TextNode) loadedNodes;
} *Chapter;

Chapter newChapter(c_str name, Vector(TextNode) nodes);
