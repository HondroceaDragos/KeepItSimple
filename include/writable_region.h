#pragma once

#include "../utils/Print/printer.h"
#include "text_node.h"

typedef struct _writable_region *WritableRegion;
struct _writable_region {
    TerminalDimensions dimensions;
    TerminalDimensions offset;

    Vector(TextNode) nodes;
    size_t cursor;

    void (*addNode)(WritableRegion, TextNode);
    void (*tickWrite)(WritableRegion, i64);
    void (*tickFlush)(WritableRegion);
};

WritableRegion newWritableRegion(TerminalDimensions *td);
