#include "../include/chapter.h"

Chapter newChapter(c_str name, Vector(TextNode) nodes) {
    Chapter c = calloc(1, sizeof(*c));
    if (!c) raise(ERROR, "OOM");

    c->name = name;
    c->loadedNodes = nodes;

    return c;
}
