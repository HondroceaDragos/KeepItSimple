#pragma once

#include "../templates.h"
#include "text_node.h"

typedef struct _chapter *Chapter;
struct _chapter {
    c_str name;
    Set(TextNode) loadedNodes;
};

Chapter newChapter(c_str name, Set(TextNode) nodes);

deleteDefine(Chapter) {
    if (!self || !*self) return;
    delete(Set(TextNode))(&(*self)->loadedNodes);
    free((*self)->name);
    free(*self);
    *self = nullptr;
}

extern Chapter currentChapter;
