#pragma once

#include "../templates.h"
#include "text_node.h"

typedef struct _chapter {
    c_str name;
    Vector(TextNode) loadedNodes;
} *Chapter;

Chapter newChapter(c_str name, Vector(TextNode) nodes);

deleteDefine(Chapter) {
    if (!self || !*self) return;
    delete(Vector(TextNode))(&(*self)->loadedNodes);
    free(*self);
    *self = nullptr;
}

extern Chapter currentChapter;
