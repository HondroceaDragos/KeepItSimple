#pragma once

#include "templates.h"
#include "../utils/Print/printer.h"

Pair(Vector(InlineStyle), StringBuilder) extractBufferData(c_str rawData, StyleArgs style, DynamicArgs dynamic);
