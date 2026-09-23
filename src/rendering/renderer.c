#include "../../include/rendering/renderer.h"

void _renderer_drawPanel(Renderer self, Panel p, size_t chCount) {
    if (!p || (!p->header && !p->body && !p->footer)) return;

    if (p->contentChanged) {
        if (p->header && p->header->currNode) p->blit.header(p);
        if (p->footer && p->footer->currNode) p->blit.footer(p);

        p->contentChanged = false;
    }

    if (p->body) {
        if (p->body->currNode && (p->body->currNode->blitChCount < (i64)p->body->currNode->preamble.size)) {
            if (chCount > 0) p->body->tickWrite(p->body, chCount);
        }

        p->blit.body(p);
    }

    fflush(stdout);
}

Renderer newRenderer() {
    Renderer r = calloc(1, sizeof(*r));
    if (!r) raise(ERROR, "OOM");

    r->drawPanel = _renderer_drawPanel;

    return r;
}
