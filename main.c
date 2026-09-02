#include "utils/SeaCore/stdc.h"
#include "utils/Print/printer.h"

#include "include/theme.h"
#include "include/clock.h"

Theme defaultTheme;

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

    Clock clk = newClock();

    print(CURSOR_H);

    for (size_t idx = 0; idx < 75; idx++) {
        Color c = lerpColors(defaultTheme.bg, red, (1.0) * idx / 75.0);
        println("Connor will remember that", .style = style(.color = c));
        move_row(up, 1);
        clk.wait(20);
    }

    clk.wait(150);

    for (size_t idx = 0; idx < 75; idx++) {
        Color c = lerpColors(red, defaultTheme.bg, (1.0) * idx / 75.0);
        println("Connor will remember that", .style = style(.color = c));
        move_row(up, 1);
        clk.wait(20);
    }

    print(CURSOR_S);

    return 0;
}