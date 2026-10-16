#include "utils/SeaCore/stdc.h"
#include "utils/Print/printer.h"

#include "include/core/engine.h"
#include "include/core/input_interpreter.h"
#include "include/core/typewriter.h"
#include "include/rendering/renderer.h"
#include "include/gameplay/chapter.h"

#include "include/entities/entity.h"
#include "include/entities/player.h"

#include "include/visuals/game_panel.h"
#include "include/visuals/pause_panel.h"

#include "utils/dataLoading/loadChapter/loadChapter.h"

#include "include/gameplay/actions.h"

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

    currentChapter = loadChapter(CHAPTER_PATH);

    defer(delete(Engine))
    Engine e = newEngine();
    e->setTargetFps(e, 120.0);

    defer(delete(TypeWriter))
    TypeWriter tw = newTypeWriter(25);

    defer(delete(Renderer))
    Renderer r = newRenderer();

    Panel gamePanel = buildGamePanel(t, currentChapter);
    Panel pausePanel = buildPausePanel(t, currentChapter);

    /* Problem with double freeing the Panels -> Orchestrator */
    Stack(Panel) panels = newStack(Panel);
    /* Like that? -> into one function -> orchestrator */
    panels->push(panels, gamePanel);
    panels->peek(panels)->onEnter(panels->peek(panels));

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
                if (c->apply(c, e->events)) break;

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

    terminalDisableRaw(&t);

    delete(Player)(&player);
    delete(Chapter)(&currentChapter);
    deleteGamePanel(&gamePanel);
    deletePausePanel(&pausePanel);

    /* ew */
    while (!panels->empty(panels)) panels->pop(panels);
    delete(Stack(Panel))(&panels);

    return 0;
}