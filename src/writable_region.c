#include "../include/writable_region.h"

void _writable_region_add_node(WritableRegion wr, TextNode n) {
    if (!wr || !n) return;

    if (wr->currNode) {
        wr->currNode->blitChCount = (i64)wr->currNode->preamble.size;
        wr->nodeHistory->push(wr->nodeHistory, wr->currNode);
    }

    wr->currNode = n;
    wr->currNode->blitChCount = 0;
}

void _writable_region_tickWrite(WritableRegion wr, i64 chpTick) {
    if (!wr || !wr->currNode) return;

    TextNode currNode = wr->currNode;
    if (currNode->blitChCount < (i64)currNode->preamble.size) {
        currNode->blitChCount += chpTick;
        if (currNode->blitChCount > (i64)currNode->preamble.size)
            currNode->blitChCount = (i64)currNode->preamble.size;
        return;
    }
}

void _writable_region_tickFlush(WritableRegion wr) {
    if (!wr) return;

    Vector(TextNode) nodes = wr->nodeHistory;
    size_t size = nodes->size;

    absoluteCursorMove(wr->offset.rows, wr->offset.cols);

    for (size_t idx = 0; idx < size; idx++) {
        TextNode curr = nodes->at(nodes, idx);
        curr->flushNode(curr);
    }

    if (wr->currNode) wr->currNode->flushNode(wr->currNode);
}

WritableRegion newWritableRegion(TerminalDimensions *dimensions, TerminalDimensions offset) {
    WritableRegion wr = calloc(1, sizeof(*wr));
    if (!wr) raise(ERROR, "OOM");

    wr->dimensions = *dimensions;
    wr->offset = offset;
    
    wr->nodeHistory = newVector(TextNode, 8);

    wr->addNode = _writable_region_add_node;
    wr->tickWrite = _writable_region_tickWrite;
    wr->tickFlush = _writable_region_tickFlush;

    return wr;
}
