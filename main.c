#include "utils/SeaCore/stdc.h"
#include "utils/Print/printer.h"

#include "include/core/engine.h"
#include "include/core/input_interpreter.h"
#include "include/core/typewriter.h"
#include "include/rendering/renderer.h"
#include "include/gameplay/chapter.h"

#include "include/entities/entity.h"
#include "include/entities/player.h"

#include "utils/dataLoading/loadChapter/loadChapter.h"

#include <stdio.h>

#define CHAPTER_PATH "data/chapters/The_Great_Divide.lua"

Theme defaultTheme;
StyleArgs defaultStyle;
DynamicArgs defaultDynamic;

Chapter currentChapter = nullptr;

Color lerpColors(Color start, Color end, f64 dt) {
    dt = (dt < 0.0) ? 0.0 : dt;
    dt = (dt > 1.0) ? 1.0 : dt;

    return color(
        start.r + (end.r - start.r) * dt,
        start.g + (end.g - start.g) * dt,
        start.b + (end.b - start.b) * dt
    );
}

LoopEvent body_scroll_up(Panel p, void *) {
    if (p->body) p->body->scroll(p->body, 1);
    return newLoopEvent();
}

LoopEvent body_scroll_down(Panel p, void *) {
    if (p->body) p->body->scroll(p->body, -1);
    return newLoopEvent();
}

LoopEvent game_quit(Panel p, void *) {
    return newLoopEvent(EVENT_QUIT);
}

LoopEvent game_pause(Panel p, void *) {
    return newLoopEvent(EVENT_PAUSE);
}

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

void pause_on_enter(Panel p) {
    p->contentChanged = true;

    printf("\x1b[2J\x1b[H");
    fflush(stdout);
}
void pause_on_exit(Panel p) { return; }

TextNode fetchNodeFromChapter(size_t id) {
    if (!currentChapter || !currentChapter->loadedNodes) return nullptr;

    Set(TextNode) nodes = currentChapter->loadedNodes;
    for (size_t idx = 0; idx < nodes->size; idx++) {
        TextNode ret = nodes->data[idx];
        if (ret->id == id) return ret;
    }

    return nullptr;
}

i32 main(void) {
    initDefaultTheme();
    initDefaultStyle();
    initDefaultDynamic();

    Terminal t = initTerminal();
    terminalEnableRaw(&t);
    // terminalDisableBuffer(&t);

    currentChapter = loadChapter(CHAPTER_PATH);

    TerminalDimensions headerDime = (TerminalDimensions){2, 1};
    WritableRegion header = newWritableRegion(&headerDime, (TerminalDimensions){1, 0});

    StringBuilder sb = newStringBuilder(currentChapter->name);
    sb->concat.c_str(sb, "\n");
    for (size_t idx = 0; idx < t.dimensions.cols; idx++) {
        sb->append(sb, '=');
    }
    i8 *name = sb->release(&sb);

    defer(delete(TextNode))
    TextNode headerMsg = newTextNode(0, name, nullptr);
    headerMsg->blitChCount = (i64)headerMsg->preamble.size;

    TerminalDimensions footerDime = (TerminalDimensions){2, 1};
    WritableRegion footer = newWritableRegion(&footerDime, (TerminalDimensions){});

    sb = newStringBuilder("");
    for (size_t idx = 0; idx < t.dimensions.cols; idx++) {
        sb->append(sb, '=');
    }
    sb->concat.c_str(sb, "\n[q] Quit [w] Scroll Up [s] Scroll Down [P] Pause");
    i8 *fut = sb->release(&sb);

    defer(delete(TextNode))
    TextNode footerMsg = newTextNode(0, fut, nullptr);
    footerMsg->blitChCount = (i64)footerMsg->preamble.size;

    WritableRegion body = newWritableRegion(&t.dimensions, (TerminalDimensions){1, 1});

    Panel p = newPanel(&t.dimensions);

    p->addContent.header(p, header);
    p->addContent.footer(p, footer);
    p->addContent.body(p, body);

    p->header->addNode(p->header, headerMsg);
    p->header->currNode->blitChCount = (i64)p->header->currNode->preamble.size;
    p->footer->addNode(p->footer, footerMsg);
    p->footer->currNode->blitChCount = (i64)p->footer->currNode->preamble.size;

    Dict(PanelAction) gameActions = newDict(PanelAction);
    gameActions->emplace(gameActions, "w", body_scroll_up);
    gameActions->emplace(gameActions, "s", body_scroll_down);
    gameActions->emplace(gameActions, "q", game_quit);
    gameActions->emplace(gameActions, "p", game_pause);

    p->actions = gameActions;

    defer(delete(Engine))
    Engine e = newEngine();
    e->setTargetFps(e, 120.0);

    defer(delete(TypeWriter))
    TypeWriter tw = newTypeWriter(25);

    defer(delete(Renderer))
    Renderer r = newRenderer();

    p->body->addNode(p->body, currentChapter->loadedNodes->data[0]);
    p->onEnter = game_on_enter;
    p->onExit = game_on_exit;

    /* Problem with double freeing the Panels -> Orchestrator */
    defer(delete(Stack(Panel)))
    Stack(Panel) panels = newStack(Panel);
    /* Like that? -> into one function -> orchestrator */
    panels->push(panels, p);
    panels->peek(panels)->onEnter(panels->peek(panels));

    /* Remove this if pause -> quit */
    defer(delete(Panel))
    Panel pausePanel = newPanel(&t.dimensions);
    pausePanel->onEnter = pause_on_enter;
    pausePanel->onExit = pause_on_exit;

    defer(delete(TextNode))
    TextNode pauseMsg = newTextNode(-100, "Cool Pausing Stuff", nullptr);

    pausePanel->addContent.body(pausePanel, newWritableRegion(&t.dimensions, (TerminalDimensions){}));

    pausePanel->body->addNode(pausePanel->body, pauseMsg);
    pausePanel->body->currNode->blitChCount = (i64)pausePanel->body->currNode->preamble.size;

    pausePanel->actions = newDict(PanelAction);
    pausePanel->actions->emplace(pausePanel->actions, "p", game_pause);

    Entity et = {};

    bool pause = false;
    player = newPlayer(.super.name = "Gigi", .super.health = 10);
    while (true) {
        e->getFrameTime(e);
        size_t chs = 0;

        LoopEvent ev = e->handleEvent(e, panels->peek(panels));

        if (ev.id == EVENT_QUIT) {
            panels->peek(panels)->onExit(panels->peek(panels));
            break;
        }

        switch (ev.id) {
            case EVENT_PAUSE: {
                if (pause) {
                    panels->peek(panels)->onExit(panels->peek(panels));
                    panels->pop(panels);

                    panels->peek(panels)->contentChanged = true;
                } else {
                    panels->push(panels, pausePanel);
                    panels->peek(panels)->onEnter(panels->peek(panels));
                }
                pause = !pause;
                break;
            }
            case EVENT_CHOICE: {
                Choice c = ev.ctx;
                c->apply(c, e->events);

                TextNode n = fetchNodeFromChapter(c->goingTo);
                if (n) panels->peek(panels)->body->addNode(panels->peek(panels)->body, n);
                break;
            }
            case EVENT_ENDGAME: {
                TextNode n = fetchNodeFromChapter(ev.target);
                if (n) panels->peek(panels)->body->addNode(panels->peek(panels)->body, n);
                break;
            }
        }

        if (!pause) chs = tw->advance(tw, e->dt);
        r->drawPanel(r, panels->peek(panels), chs);
    }

    // terminalEnableBuffer(&t);
    terminalDisableRaw(&t);

    free(name);
    free(fut);
    // delete(Panel)(&p);
    delete(Chapter)(&currentChapter);

    return 0;
}
//
//
