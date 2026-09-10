#include "../include/writable_region.h"

void _interpret_style_inline_sb(StringBuilder sb, StyleArgs sta) {
    sb->concat.c_str(sb, (sta.stroke & bold) ? "\033[1m" : RESET_BOLD);
    sb->concat.c_str(sb, (sta.stroke & underline) ? "\033[4m" : RESET_UNDERLINE);
    sb->concat.c_str(sb, (sta.stroke & italic) ? "\033[3m" : RESET_ITALIC);

    i8 buff[32];
    if (sta.background.set) {
        snprintf(buff, sizeof(buff), "\x1b[48;2;%d;%d;%dm", sta.background.r, sta.background.g, sta.background.b);
        sb->concat.c_str(sb, buff);
    }
    else sb->concat.c_str(sb, RESET_BACKGROUND);

    if (sta.color.set) {
        snprintf(buff, sizeof(buff), "\x1b[38;2;%d;%d;%dm", sta.color.r, sta.color.g, sta.color.b);
        sb->concat.c_str(sb, buff);
    }
    else sb->concat.c_str(sb, RESET_COLOR);
}

void _writable_region_open_line(WritableRegion wr) {
    if (!wr) return;
    wr->currLine = newStringBuilder("");
}

void _writable_region_push_line(WritableRegion wr) {
    StringBuilder sb = wr->currLine;
    if (!sb) return;

    size_t size = sb->size;
    i8 *data = sb->release(&sb);
    wr->currLine = nullptr;

    wr->lineHistory->push(wr->lineHistory, (str){.data = data, .size = size});
}

void _writable_region_push_choices(WritableRegion wr, TextNode n) {
    if (!wr || !n || !n->choices || !n->choices->size) return;

    size_t size = n->choices->size;

    wr->currLine = newStringBuilder("");
    _writable_region_push_line(wr);

    for (size_t idx = 0; idx < size; idx++) {
        StringBuilder sb = newStringBuilder("");

        _interpret_style_inline_sb(sb, style(.color = gray));

        i8 *choiceIdx = cstrfmt("[%zu] ", idx + 1);
        sb->concat.c_str(sb, choiceIdx);
        free(choiceIdx);

        _interpret_style_inline_sb(sb, (StyleArgs){});

        sb->concat.c_str(sb, n->choices->data[idx]->text);

        wr->currLine = sb;
        _writable_region_push_line(wr);
    }

    wr->currLine = newStringBuilder("");
    _writable_region_push_line(wr);
}

void _writable_region_new_line(WritableRegion wr) {
    TextNode n = wr->currNode;
    if (!n || !wr->currLine) return;

    size_t size = ((size_t)n->blitChCount > n->preamble.size) ? n->preamble.size : (size_t)n->blitChCount;

    if (size > wr->builtChCount) {
        size_t idx = 0;
        while (idx + 1 < n->runs->size && n->runs->data[idx + 1].start_idx < wr->builtChCount) idx++;

        if (!wr->builtChCount && n->runs->size) _interpret_style_inline_sb(wr->currLine, n->runs->data[0].style);

        for (size_t jdx = wr->builtChCount; jdx < size; jdx++) {
            while (idx + 1 < n->runs->size && n->runs->data[idx + 1].start_idx == jdx) {
                _interpret_style_inline_sb(wr->currLine, n->runs->data[++idx].style);
            }

            if (n->preamble.data[jdx] == '\n') {
                _writable_region_push_line(wr);
                _writable_region_open_line(wr);
                continue;
            }

            wr->currLine->append(wr->currLine, n->preamble.data[jdx]);
        }

        wr->builtChCount = size;
    }

    if (wr->builtChCount >= n->preamble.size) {
        _writable_region_push_line(wr);
        _writable_region_push_choices(wr, n);
    }
}

void _writable_region_add_node(WritableRegion wr, TextNode n) {
    if (!wr || !n) return;

    if (wr->currNode && wr->currLine) {
        wr->currNode->blitChCount = (i64)wr->currNode->preamble.size;
        _writable_region_new_line(wr);
    }

    wr->nodeHistory->push(wr->nodeHistory, n);
    wr->currNode = n;
    wr->currNode->blitChCount = 0;
    _writable_region_open_line(wr);
    wr->builtChCount = 0;
}

void _writable_region_tickWrite(WritableRegion wr, i64 chpTick) {
    if (!wr || !wr->currNode) return;

    TextNode currNode = wr->currNode;
    if (currNode->blitChCount < (i64)currNode->preamble.size) {
        currNode->blitChCount += chpTick;
        if (currNode->blitChCount > (i64)currNode->preamble.size)
            currNode->blitChCount = (i64)currNode->preamble.size;
    }
}

void _writable_region_viewport(WritableRegion wr, size_t start, size_t end) {
    // _writable_region_new_line(wr);

    size_t lineCount = wr->lineHistory->size;
    size_t size = lineCount + (wr->currLine ? 1 : 0);

    if (!size || start > size) return;
    if (end >= size) end = size - 1;
    if (start > end) return;

    size_t screenRow = 0;
    for (size_t idx = start; idx <= end; idx++) {
        absoluteCursorMove(wr->offset.rows + screenRow, wr->offset.cols);
        screenRow++;
        fprintf(stdout, endline);

        if (idx < lineCount) {
            str line = wr->lineHistory->at(wr->lineHistory, idx);
            fprintf(stdout, "%.*s", line.size, line.data);
        } else {
            fprintf(stdout, "%.*s", wr->currLine->size, wr->currLine->data);
        }
    }

    fflush(stdout);
}

void _writable_region_tickFlush(WritableRegion wr) {
    if (!wr) return;

    _writable_region_new_line(wr);

    size_t size = wr->lineHistory->size;
    size_t height = size + (wr->currLine ? 1 : 0);

    if (!height) return;

    size_t th = wr->dimensions.rows;

    if (height <= th) wr->use_viewport(wr, 0, height - 1);
    else wr->use_viewport(wr, height - th, height - 1);
}

WritableRegion newWritableRegion(TerminalDimensions *dimensions, TerminalDimensions offset) {
    WritableRegion wr = calloc(1, sizeof(*wr));
    if (!wr) raise(ERROR, "OOM");

    wr->dimensions = *dimensions;
    wr->offset = offset;
    
    wr->nodeHistory = newVector(TextNode, 8);
    wr->lineHistory = newVector(str, 8);

    wr->addNode = _writable_region_add_node;
    wr->tickWrite = _writable_region_tickWrite;
    wr->tickFlush = _writable_region_tickFlush;
    wr->use_viewport = _writable_region_viewport;

    return wr;
}
