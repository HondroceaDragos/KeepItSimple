#include "../../include/gameplay/text_node.h"

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

    return n;
}

i32 TextNode_cmp_id(const void *a, const void *b) {
    TextNode tna = *(TextNode *)a;
    TextNode tnb = *(TextNode *)b;

    return (tna->id > tnb->id) - (tna->id < tnb->id);
}
