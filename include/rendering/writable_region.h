#pragma once

#include "../../utils/Print/printer.h"
#include "../templates.h"
#include "../gameplay/text_node.h"

typedef struct _writable_region *WritableRegion;
struct _writable_region {
    TerminalDimensions dimensions;
    TerminalDimensions offset;

    Vector(TextNode) nodeHistory;
    TextNode currNode;

    Vector(str) lineHistory;
    StringBuilder currLine;
    size_t builtChCount;

    size_t regionStart;
    bool autoScroll;

    void (*addNode)(WritableRegion, TextNode);
    void (*tickWrite)(WritableRegion, i64);
    void (*tickFlush)(WritableRegion);
    void (*use_viewport)(WritableRegion, size_t, size_t);
    void (*scroll)(WritableRegion, i64);
};

WritableRegion newWritableRegion(TerminalDimensions *dimensions, TerminalDimensions offset);

deleteDefine(WritableRegion) {
    if (!self || !*self) return;

    free((*self)->nodeHistory->data);
    free((*self)->nodeHistory);

    for (size_t idx = 0; idx < (*self)->lineHistory->size; idx++)
        free((void *)(*self)->lineHistory->data[idx].data);

    delete(Vector(str))(&(*self)->lineHistory);

    if ((*self)->currLine) {
        free((*self)->currLine->data);
        free((*self)->currLine);
        (*self)->currLine = nullptr;
    }

    free(*self);
    *self = nullptr;
}
