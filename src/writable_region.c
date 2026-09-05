#include "../include/writable_region.h"

void _writable_region_add_node(WritableRegion wr, TextNode n) {
    wr->nodes->push(wr->nodes, n);
}

void _writable_region_tickWrite(WritableRegion wr, i64 chpTick) {
    Vector(TextNode) nodes = wr->nodes;

    if (nodes->empty(nodes)) return;

    TextNode currNode = nullptr;
    while (wr->cursor < nodes->size) {
        currNode = nodes->at(nodes, wr->cursor);

        if (currNode->blitChCount < (i64)currNode->preamble.size) {
            currNode->blitChCount += chpTick;
            if (currNode->blitChCount > (i64)currNode->preamble.size)
                currNode->blitChCount = (i64)currNode->preamble.size;
            return;
        }

        wr->cursor++;
    }
}

void _writable_region_tickFlush(WritableRegion wr) {
    Vector(TextNode) nodes = wr->nodes;

    TextNode currNode = nullptr;
    for (size_t idx = 0; idx < nodes->size; idx++) {
        currNode = nodes->at(nodes, idx);
        currNode->flushPreamble(currNode, idx + 1, 0);
    }
}

WritableRegion newWritableRegion(TerminalDimensions *td) {
    WritableRegion wr = calloc(1, sizeof(*wr));
    if (!wr) raise(ERROR, "OOM");

    wr->dimensions = *td;
    wr->offset = (TerminalDimensions){};
    
    wr->nodes = newVector(TextNode, 8);

    wr->addNode = _writable_region_add_node;
    wr->tickWrite = _writable_region_tickWrite;
    wr->tickFlush = _writable_region_tickFlush;

    return wr;
}
