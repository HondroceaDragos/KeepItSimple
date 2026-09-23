#include "../../include/rendering/panel.h"

void _panel_header_blit(Panel p) {
    if (!p || !p->header) return;

    absoluteCursorMove(p->header->offset.rows, p->header->offset.cols);

    p->header->tickFlush(p->header);
}

void _panel_body_blit(Panel p) {
    if (!p || !p->body) return;

    absoluteCursorMove(p->body->offset.rows, p->body->offset.cols);

    p->body->tickFlush(p->body);
}

void _panel_footer_blit(Panel p) {
    if (!p || !p->footer) return;

    absoluteCursorMove(p->footer->offset.rows, p->footer->offset.cols);

    p->footer->tickFlush(p->footer);
}

void _panel_header_addContent(Panel p, WritableRegion wr) {
    if (!p) return;

    p->header = wr;
}

void _panel_body_addContent(Panel p, WritableRegion wr) {
    if (!p) return;

    p->body = wr;
}

void _panel_footer_addContent(Panel p, WritableRegion wr) {
    if (!p) return;

    p->footer = wr;
}

void _panel_resize(Panel p) {
    if (!p) return;

    size_t dh = p->header->dimensions.rows;
    size_t df = p->footer->dimensions.rows;

    p->body->offset.rows = p->header->offset.rows + dh;
    p->body->dimensions.rows = p->dimensions.rows - dh - df;

    p->footer->offset.rows = p->body->offset.rows + p->body->dimensions.rows;
}

LoopEvent _panel_dispatch(Panel p, c_str key) {
    if (!p || !key || !p->actions) return newLoopEvent();

    if (p->actions->contains(p->actions, key)) {
        PanelAction action = p->actions->get(p->actions, key);
        if (action) return action(p, key);
    }

    if (!p->body || !p->body->currNode) return newLoopEvent();

    TextNode n = p->body->currNode;

    if (n->preamble.size != n->blitChCount) return newLoopEvent();
    if (!n->choices) return newLoopEvent();

    for (size_t idx = 0; idx < n->choices->size; idx++) {
        Choice c = n->choices->data[idx];

        if (c->trigger != USE_IDX) {
            if (c->trigger == key[0]) return newLoopEvent(EVENT_CHOICE, c);
            continue;
        }
        
        i8 relativeKey = (i8)(idx + '1');
        if (relativeKey == key[0]) return newLoopEvent(EVENT_CHOICE, c);
    }

    return newLoopEvent();
}

Panel newPanel(TerminalDimensions *td) {
    Panel p = calloc(1, sizeof(*p));
    if (!p) raise(ERROR, "OOM");

    p->dimensions = *td;
    p->contentChanged = true;

    p->blit.header = _panel_header_blit;
    p->blit.body = _panel_body_blit;
    p->blit.footer = _panel_footer_blit;

    p->addContent.header = _panel_header_addContent;
    p->addContent.body = _panel_body_addContent;
    p->addContent.footer = _panel_footer_addContent;

    p->resize = _panel_resize;
    p->dispatch = _panel_dispatch;

    return p;
}