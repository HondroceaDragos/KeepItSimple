#include "../include/text_node.h"

void _text_node_flushPreamble(TextNode n, size_t row, size_t col) {
    size_t size = ((size_t)n->blitChCount > n->preamble.size) ? n->preamble.size : (size_t)n->blitChCount;

    absoluteCursorMove(row, col);
    size_t idx = 0;

    if (n->runs->size) _interpret_style_inline(stdout, n->runs->data[0].style);

    for (size_t jdx = 0; jdx < size; jdx++) {
        while (idx + 1 < n->runs->size && n->runs->data[idx + 1].start_idx == jdx) {
            _interpret_style_inline(stdout, n->runs->data[++idx].style);
        }
        printf("%c", n->preamble.data[jdx]);
    }
}

TextNode newTextNode(i64 id, c_str preamble, Vector(Choice) choices) {
    TextNode n = calloc(1, sizeof(*n));
    if (!n) raise(ERROR, "OOM");

    n->id = id;
    n->choices = choices;
    n->blitChCount = 0;

    auto parsed = extractBufferData(preamble, defaultStyle, defaultDynamic);

    n->runs = parsed.first;

    i8 *plainData = parsed.second->release((StringBuilder *)&parsed.second);

    n->preamble = newStr(plainData);

    n->flushPreamble = _text_node_flushPreamble;

    return n;
}
