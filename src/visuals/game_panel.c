#include "../../include/visuals/game_panel.h"

void game_on_enter(Panel p) {
    p->contentChanged = true;

    printf("\x1b[?1049h");
    printf(CURSOR_H);

    p->resize(p);

    printf("\x1b[48;2;%d;%d;%dm",
        defaultStyle.background.r,
        defaultStyle.background.g,
        defaultStyle.background.b
    );

    printf("\x1b[2J\x1b[H");

    printf("\x1b[%zu;%zur",
        p->body->offset.rows,
        p->body->offset.rows + p->body->dimensions.rows - 1
    );

    fflush(stdout);
}

void game_on_exit(Panel p) {
    printf("\x1b[r");
    printf("\x1b[?1049l");
    printf(CURSOR_S);
}

Panel buildGamePanel(Terminal t, Chapter currentChapter) {
    StringBuilder sb = nullptr;

    TerminalDimensions headerDime = (TerminalDimensions){2, 1};
    WritableRegion header = newWritableRegion(&headerDime, (TerminalDimensions){1, 0});

    sb = newStringBuilder(currentChapter->name);
    sb->concat.c_str(sb, "\n");
    for (size_t idx = 0; idx < t.dimensions.cols; idx++) sb->append(sb, '=');
    i8 *headerName = sb->release(&sb);

    TextNode headerMsg = newTextNode(0, headerName, nullptr);
    headerMsg->blitChCount = (i64)headerMsg->preamble.size;
    free(headerName);

    TerminalDimensions footerDime = (TerminalDimensions){2, 1};
    WritableRegion footer = newWritableRegion(&footerDime, (TerminalDimensions){});

    sb = newStringBuilder("");
    for (size_t idx = 0; idx < t.dimensions.cols; idx++) sb->append(sb, '=');
    sb->concat.c_str(sb, "\n[q] Quit [w] Scroll Up [s] Scroll Down [P] Pause");
    i8 *footerName = sb->release(&sb);

    TextNode footerMsg = newTextNode(0, footerName, nullptr);
    footerMsg->blitChCount = (i64)footerMsg->preamble.size;
    free(footerName);

    WritableRegion body = newWritableRegion(&t.dimensions, (TerminalDimensions){1, 1});

    Panel p = newPanel(&t.dimensions);

    p->addContent.header(p, header);
    p->addContent.footer(p, footer);
    p->addContent.body(p, body);

    p->header->addNode(p->header, headerMsg);
    p->header->currNode->blitChCount = (i64)p->header->currNode->preamble.size;
    p->footer->addNode(p->footer, footerMsg);
    p->footer->currNode->blitChCount = (i64)p->footer->currNode->preamble.size;

    p->body->addNode(p->body, currentChapter->loadedNodes->data[0]);
    p->onEnter = game_on_enter;
    p->onExit = game_on_exit;

    Dict(PanelAction) gameActions = newDict(PanelAction);
    gameActions->emplace(gameActions, "w", body_scroll_up);
    gameActions->emplace(gameActions, "s", body_scroll_down);
    gameActions->emplace(gameActions, "q", game_quit);
    gameActions->emplace(gameActions, "p", game_pause);

    p->actions = gameActions;

    return p;
}

void deleteGamePanel(Panel *p) {
    if (!p || !*p) return;

    TextNode header = (*p)->header->nodeHistory->data[0];
    TextNode footer = (*p)->footer->nodeHistory->data[0];

    delete(TextNode)(&header);
    delete(TextNode)(&footer);

    delete(Panel)(p);
}
