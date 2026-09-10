#include "utils/SeaCore/stdc.h"
#include "utils/Print/printer.h"

#include "include/theme.h"
#include "include/clock.h"
#include "include/templates.h"
#include "include/parser.h"
#include "include/choice.h"
#include "include/text_node.h"
#include "include/writable_region.h"
#include "include/panel.h"
#include "include/chapter.h"

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

deleteType(c_str)
ArrayType(c_str)
VectorType(c_str)

i32 main(void) {
    initDefaultTheme();
    initDefaultStyle();
    initDefaultDynamic();

    Terminal t = initTerminal();
    terminalEnableRaw(&t);
    terminalDisableBuffer(&t);

    TextNode n = newTextNode(1, "Get out! I am here, [c: red]Richard[/]! Fear me!",
        newVector(Choice, .using = newArray(Choice, {
            newChoice("Kill him", nullptr, 2),
            newChoice("Spare him?", nullptr, 2)
        }))
    );
    TextNode n1 = newTextNode(2, "Ain't no way!", nullptr);
    TextNode n2 = newTextNode(3, "Damn, sorry dude. I was just bustin' [c: green]balls[/], that's all...sd\nha\nsdjasd\nkahsdk\nasdh\najks\ncajcba\nscbas\nhcb\nahcb\nascjhds\nbchjs\ndbcsbchjs\nbchs\ncbsjcbs\njcbsj\ncbsjcb\nsjcbs\njcbsj\ncsjd\ncbsjc\nbsj",
        newVector(Choice, .using = newArray(Choice, {
            newChoice("Stupid...", nullptr, 2),
            newChoice("Watch it, Chrissy!", nullptr, 2)
        }))
    );

    Chapter c = newChapter(
        "The Sun Rises",
        newVector(TextNode, .using = newArray(TextNode, {n, n1, n2}))
    );

    TerminalDimensions headerDime = (TerminalDimensions){2, 1};
    WritableRegion header = newWritableRegion(&headerDime, (TerminalDimensions){1, 0});

    StringBuilder sb = newStringBuilder(c->name);
    sb->concat.c_str(sb, "\n===============================");
    i8 * name = sb->release(&sb);

    TextNode headerMsg = newTextNode(0, name, nullptr);
    headerMsg->blitChCount = (i64)headerMsg->preamble.size;

    TerminalDimensions footerDime = (TerminalDimensions){2, 1};
    WritableRegion footer = newWritableRegion(&footerDime, (TerminalDimensions){});

    TextNode footerMsg = newTextNode(0, "===============================\nDummy Footer", nullptr);
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

    Clock clk = newClock();

    printf("\x1b[?1049h");
    printf(CURSOR_H);

    p->resize(p);

    printf("\x1b[%zu;%zur",
        p->body->offset.rows,
        p->body->offset.rows + p->body->dimensions.rows - 1
    );

    p->blit.header(p);
    p->blit.footer(p);
    fflush(stdout);

    // absoluteCursorMove(2, 1);

    f64 acc = 0.0;
    i32 charsPerSecond = 30;

    size_t cselect = 0;
    bool newNode = false;
    i8 input = 0;
    while (
        cselect < c->loadedNodes->size ||
        (p->body->currNode &&
        p->body->currNode->blitChCount < (i64)p->body->currNode->preamble.size)
    ) {
        f64 dt = clk.tick(&clk);
        acc += dt * charsPerSecond;
        newNode = false;

        if (!p->body->currNode || p->body->currNode->blitChCount >= (i64)p->body->currNode->preamble.size) {
            p->body->addNode(p->body, c->loadedNodes->data[cselect++]);
            newNode = true;
        }

        i64 rev = (i64)acc;
        if (rev > 0) {
            p->body->tickWrite(p->body, rev);
            acc -= (f64)rev;
            p->blit.body(p);
        }

        fflush(stdout);

        // if (newNode) scanf("%c", &input);
    }

    // printf("LINES: %zu\n", p->body->lineHistory->size);

    clk.wait(10000);

    printf("\x1b[r");
    printf("\x1b[?1049l");
    printf(CURSOR_S);

    terminalEnableBuffer(&t);
    terminalDisableRaw(&t);

    // nul

    return 0;
}
