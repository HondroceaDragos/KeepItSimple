#include "../../include/gameplay/chapter.h"

Chapter newChapter(c_str name, Set(TextNode) nodes) {
    Chapter c = calloc(1, sizeof(*c));
    if (!c) raise(ERROR, "OOM");

    c->name = (c_str)strdup(name);
    c->loadedNodes = nodes;

    return c;
}
