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

TextNode fetchNodeFromChapter(size_t id) {
    if (!currentChapter || !currentChapter->loadedNodes) return nullptr;

    Vector(TextNode) nodes = currentChapter->loadedNodes;
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

    TextNode n = newTextNode(1, "Get out! I am here, [c: red]Richard[/]! Fear me!",
        newVector(Choice, .using = newArray(Choice, {
            newChoice("Kill him", 2, .trigger = 'K'),
            newChoice("Spare him?", 3)
        }))
    );
    TextNode n1 = newTextNode(2, "Ain't no way!", nullptr);
    TextNode n2 = newTextNode(3, "Damn, sorry dude. I was just bustin' [c: green]balls[/], that's all...Why\nAm\nI\nhere\nafter\nall\nthis time?\nAre\nyou\nwith\nme\nor\nare\nyou\nagainst\nme?\nKeep\nIt\nSimple\nJohn\nPlease\n;p\n\n",
        newVector(Choice, .using = newArray(Choice, {
            newChoice("Stupid...", 67),
            newChoice("Watch it, Chrissy!", 99, .trigger = 'o')
        }))
    );
    TextNode n4 = newTextNode(67, "He-hey, the king of breadsticks!",
        newVector(Choice, .using = newArray(Choice, {
            newChoice("Why wouldn't you do yourself a fucking favour and get the fuck out of my store?!", 876),
            newChoice("Haha, you ball buster...", 876, .trigger = '3')
        }))
    );
    TextNode n5 = newTextNode(99, "Sorry, I can't. I need to be loyale to my capo!", nullptr);
    TextNode n6 = newTextNode(876, "Fin.", nullptr);

    currentChapter = newChapter(
        "The Sun Rises",
        newVector(TextNode, .using = newArray(TextNode, {n, n1, n2, n4, n5, n6}))
    );

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
    sb->concat.c_str(sb, "\n[q] Quit [w] Scroll Up [s] Scroll Down");
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

    p->body->addNode(p->body, n);

    bool pause = false;
    while (true) {
        e->getFrameTime(e);
        size_t chs = 0;

        LoopEvent ev = e->handleEvent(e, p);

        if (ev.id == EVENT_QUIT) break;

        switch (ev.id) {
            case EVENT_PAUSE: {
                pause = !pause;
                break;
            }
            case EVENT_CHOICE: {
                Choice c = ev.ctx;
                TextNode n = fetchNodeFromChapter(c->goingTo);
                if (n) p->body->addNode(p->body, n);
                break;
            }
        }

        if (!pause) chs = tw->advance(tw, e->dt);
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
    delete(Panel)(&p);
    delete(Chapter)(&currentChapter);

    return 0;
}
