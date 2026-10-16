#include "../../include/visuals/pause_panel.h" 

void pause_on_enter(Panel p) {
    p->contentChanged = true;

    printf("\x1b[2J\x1b[H");
    fflush(stdout);
}

void pause_on_exit(Panel p) { return; }

Panel buildPausePanel(Terminal t, Chapter currentChapter) {
    (void)currentChapter;

    Panel p = newPanel(&t.dimensions);
    p->onEnter = pause_on_enter;
    p->onExit = pause_on_exit;

    TextNode pauseMsg = newTextNode(-100, "Cool Pausing Stuff", nullptr);

    p->addContent.body(p, newWritableRegion(&t.dimensions, (TerminalDimensions){}));

    p->body->addNode(p->body, pauseMsg);
    p->body->currNode->blitChCount = (i64)p->body->currNode->preamble.size;

    p->actions = newDict(PanelAction);
    p->actions->emplace(p->actions, "p", game_pause);

    return p;
}

void deletePausePanel(Panel *p) {
    if (!p || !*p) return;

    TextNode body = (*p)->body->nodeHistory->data[0];

    delete(TextNode)(&body);

    delete(Panel)(p);
}
