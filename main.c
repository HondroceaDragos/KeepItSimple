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

i32 main(void) {
    initDefaultTheme();
    initDefaultStyle();
    initDefaultDynamic();


    Terminal t = initTerminal();
    terminalEnableRaw(&t);
    terminalDisableBuffer(&t);

    TextNode n = newTextNode(1, "Get out! I am here, [c: red]Richard[/]! Fear me!\n", nullptr);
    TextNode n1 = newTextNode(2, "Ain't no way!\n", nullptr);
    TextNode n2 = newTextNode(3, "Damn, sorry dude. I was just bustin' balls, that's all...\n", nullptr);

    WritableRegion wr = newWritableRegion(&t.dimensions);

    wr->addNode(wr, n);
    wr->addNode(wr, n1);
    wr->addNode(wr, n2);

    Clock clk = newClock();

    printf(CURSOR_H);
    printf("\x1b[2J\x1b[H");

    f64 acc = 0.0;
    i32 charsPerSecond = 20;

    while (true) {
        f64 dt = clk.tick(&clk);
        acc += dt * charsPerSecond;

        i64 rev = (i64)acc;
        if (rev > 0) {
            wr->tickWrite(wr, rev);
            acc -= (f64)rev;
        }

        wr->tickFlush(wr);
        fflush(stdout);
    }

    printf(CURSOR_S);

    terminalEnableBuffer(&t);
    terminalDisableRaw(&t);

    return 0;
}
