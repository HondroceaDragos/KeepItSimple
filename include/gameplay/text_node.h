#pragma once

#include "../templates.h"
#include "../rendering/theme.h"
#include "choice.h"
#include "parser.h"
#include "../../utils/Print/printer.h"

typedef struct _text_node *TextNode;
struct _text_node {
    i64 id;
    str preamble;
    Vector(InlineStyle) runs;
    Vector(Choice) choices;

    i64 blitChCount;
};

TextNode newTextNode(i64 id, c_str preamble, Vector(Choice) choices);

deleteDefine(TextNode) {
    if (!self || !*self) return;
    delete(Vector(Choice))(&(*self)->choices);
    delete(Vector(InlineStyle))(&(*self)->runs);
    free((void *)(*self)->preamble.data);
    free(*self);
    *self = nullptr;
}

ArrayType(TextNode)
VectorType(TextNode)
