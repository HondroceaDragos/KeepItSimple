#include "../include/theme.h"

void initDefaultTheme() {
    FILE *t = fopen("data/terminal.txt", "r");
    if (!t) raise(ERROR, "(Nil).");

    i8 buffer[32];

    /* Read Background */
    fgets(buffer, sizeof(buffer), t);

    str currChannel = newStr(buffer);
    i8 cbuf[3];

    /* Channels */
    i16 r, g, b;

    currChannel.data += 4;  // skip 'bg' + '#'
    currChannel.size = 2;

    memcpy(cbuf, currChannel.data, currChannel.size);
    cbuf[2] = '\0';

    /* RED */
    r = (i16)strtol(cbuf, nullptr, 16);

    currChannel.data += currChannel.size;
    memcpy(cbuf, currChannel.data, currChannel.size);
    cbuf[2] = '\0';

    /* GREEN */
    g = (i16)strtol(cbuf, nullptr, 16);

    currChannel.data += currChannel.size;
    memcpy(cbuf, currChannel.data, currChannel.size);
    cbuf[2] = '\0';

    /* BLUE */
    b = (i16)strtol(cbuf, nullptr, 16);

    defaultTheme.bg = color(r, g, b);  // bg

    /* Read ForeGround */
    fgets(buffer, sizeof(buffer), t);

    currChannel = newStr(buffer);
    currChannel.data += 4;  // skip 'fg' + '#'
    currChannel.size = 2;

    memcpy(cbuf, currChannel.data, currChannel.size);
    cbuf[2] = '\0';

    /* RED */
    r = (i16)strtol(cbuf, nullptr, 16);

    currChannel.data += currChannel.size;
    memcpy(cbuf, currChannel.data, currChannel.size);
    cbuf[2] = '\0';

    /* GREEN */
    g = (i16)strtol(cbuf, nullptr, 16);

    currChannel.data += currChannel.size;
    memcpy(cbuf, currChannel.data, currChannel.size);
    cbuf[2] = '\0';

    /* BLUE */
    b = (i16)strtol(cbuf, nullptr, 16);

    defaultTheme.fg = color(r, g, b);  // set fg
}

void initDefaultStyle() {
    defaultStyle = _new_style_args(
        (StyleArgs){
            .color = defaultTheme.fg,
            .background = defaultTheme.bg
    });
}

void initDefaultDynamic() {
    defaultDynamic = _new_dynamic_args(
        (DynamicArgs){
            .delay = 120,
            .raw = true
    });
}
