#pragma once

#include "../utils/Print/printer.h"
#include "templates.h"
#include "text_node.h"

typedef struct _writable_region *WritableRegion;
struct _writable_region {
    TerminalDimensions dimensions;
    TerminalDimensions offset;

    Vector(TextNode) nodeHistory;
    TextNode currNode;

    Vector(str) lineHistory;
    StringBuilder currLine;
    size_t builtChCount;

    void (*addNode)(WritableRegion, TextNode);
    void (*tickWrite)(WritableRegion, i64);
    void (*tickFlush)(WritableRegion);
    void (*use_viewport)(WritableRegion, size_t, size_t);
};

WritableRegion newWritableRegion(TerminalDimensions *dimensions, TerminalDimensions offset);
