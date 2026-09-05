#pragma once

#include "templates.h"
#include "theme.h"
#include "choice.h"
#include "parser.h"

typedef struct _text_node *TextNode;
struct _text_node {
    i64 id;
    str preamble;
    Vector(Choice) choices;

    Vector(InlineStyle) runs;
    i64 blitChCount;

    void (*flushPreamble)(TextNode, size_t, size_t);
};

TextNode newTextNode(i64 id, c_str preamble, Vector(Choice) choices);

deleteDefine(TextNode) {
    if (!self || !*self) return;
    delete(Vector(Choice))(&(*self)->choices);
    delete(Vector(InlineStyle))(&(*self)->runs);
    free((void *)(*self)->preamble.data);
    *self = nullptr;
}

ArrayType(TextNode)
VectorType(TextNode)
