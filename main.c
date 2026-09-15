#include "utils/SeaCore/stdc.h"
#include "utils/Print/printer.h"

#include "include/core/engine.h"
#include "include/core/input_interpreter.h"
#include "include/core/typewriter.h"
#include "include/rendering/renderer.h"
#include "include/gameplay/chapter.h"

#include <stdio.h>

Theme defaultTheme;
StyleArgs defaultStyle;
DynamicArgs defaultDynamic;

Color lerpColors(Color start, Color end, f64 dt) {
    dt = (dt < 0.0) ? 0.0 : dt;
    dt = (dt > 1.0) ? 1.0 : dt;

    return color(
        start.r + (end.r - start.r) * dt,
        start.g + (end.g - start.g) * dt,
        start.b + (end.b - start.b) * dt
    );
}

LoopEvent body_scroll_up(Panel p) {
    if (p->body) p->body->scroll(p->body, 1);
    return EVENT_NOP;
}

LoopEvent body_scroll_down(Panel p) {
    if (p->body) p->body->scroll(p->body, -1);
    return EVENT_NOP;
}

LoopEvent game_quit(Panel p) {
    return EVENT_QUIT;
}

LoopEvent game_pause(Panel p) {
    return EVENT_PAUSE;
}

i32 main(void) {
    initDefaultTheme();
    initDefaultStyle();
    initDefaultDynamic();

    Terminal t = initTerminal();
    terminalEnableRaw(&t);
    // terminalDisableBuffer(&t);

    TextNode n = newTextNode(1, "Get out! I am here, [c: red]Richard[/]! Fear me!",
        newVector(Choice, .using = newArray(Choice, {
            newChoice("Kill him", nullptr, 2),
            newChoice("Spare him?", nullptr, 2)
        }))
    );
    TextNode n1 = newTextNode(2, "Ain't no way!", nullptr);
    TextNode n2 = newTextNode(3, "Damn, sorry dude. I was just bustin' [c: green]balls[/], that's all...Why\nAm\nI\nhere\nafter\nall\nthis time?\nAre\nyou\nwith\nme\nor\nare\nyou\nagainst\nme?\nKeep\nIt\nSimple\nJohn\nPlease\n;p\n\n",
        newVector(Choice, .using = newArray(Choice, {
            newChoice("Stupid...", nullptr, 2),
            newChoice("Watch it, Chrissy!", nullptr, 2)
        }))
    );

    defer(delete(Chapter))
    Chapter c = newChapter(
        "The Sun Rises",
        newVector(TextNode, .using = newArray(TextNode, {n, n1, n2}))
    );

    TerminalDimensions headerDime = (TerminalDimensions){2, 1};
    WritableRegion header = newWritableRegion(&headerDime, (TerminalDimensions){1, 0});

    StringBuilder sb = newStringBuilder(c->name);
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
    sb->concat.c_str(sb, "\n[q] Quit [w] Scroll Up [s] Scroll Down");
    i8 *fut = sb->release(&sb);

    defer(delete(TextNode))
    TextNode footerMsg = newTextNode(0, fut, nullptr);
    footerMsg->blitChCount = (i64)footerMsg->preamble.size;

    WritableRegion body = newWritableRegion(&t.dimensions, (TerminalDimensions){1, 1});

    defer(delete(Panel))
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

    // printf("Got: %p from %s\n", p->actions->get(p->actions, "w"), "w");

    printf("\x1b[?1049h");
    printf(CURSOR_H);

    // printf("\033[?2026h");

    p->resize(p);

    printf("\x1b[%zu;%zur",
        p->body->offset.rows,
        p->body->offset.rows + p->body->dimensions.rows - 1
    );

    defer(delete(Engine))
    Engine e = newEngine();
    e->setTargetFps(e, 120.0);

    defer(delete(TypeWriter))
    TypeWriter tw = newTypeWriter(25);

    defer(delete(Renderer))
    Renderer r = newRenderer();

    size_t cselect = 0;

    bool pause = false;
    while (true) {
        e->getFrameTime(e);
        LoopEvent ev = e->handleEvent(e, p);

        if (ev == EVENT_QUIT) break;
        if (ev == EVENT_PAUSE) pause = !pause;

        size_t chs = (!pause) ? tw->advance(tw, e->dt) : 0;

        if (!p->body->currNode ||
            p->body->currNode->blitChCount >= (i64)p->body->currNode->preamble.size) {

            if (cselect < c->loadedNodes->size)
                p->body->addNode(p->body, c->loadedNodes->data[cselect++]);
        }

        r->drawPanel(r, p, chs);
    }

    printf("\x1b[r");
    printf("\x1b[?1049l");
    printf(CURSOR_S);

    // printf("\033[?2026l");

    // terminalEnableBuffer(&t);
    terminalDisableRaw(&t);

    free(name);
    free(fut);

    // nul
    // nup
    // nuc

    return 0;
}
