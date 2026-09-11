#include "../../include/gameplay/parser.h"

Pair(Vector(InlineStyle), StringBuilder) extractBufferData(c_str rawData, StyleArgs style, DynamicArgs dynamic) {
    size_t dlen = strlen(rawData);

    StringBuilder sb = newStringBuilder("");
    Vector(InlineStyle) vis = newVector(InlineStyle, INLINE_RUN);

    int8_t style_op[5] = {0};

    int8_t style_args[5][64];
    size_t args_count = 0;
    size_t args_len = 0;

    StyleArgs bs = style;
    DynamicArgs bd = dynamic;
    bool use_ctx = false;

    vis->push(vis, (InlineStyle){sb->size, style, dynamic});

    for (size_t idx = 0; idx < dlen; idx++) {
        if (rawData[idx] != '[') {
            sb->append(sb, rawData[idx]);
            continue;
        }

        if (idx + 1 < dlen && rawData[idx + 1] == '[') {
            sb->append(sb, '[');
            continue;
        }

        size_t ctx_start = idx++;

        args_count = 0;
        args_len = 0;
        style_args[0][0] = '\0';
        use_ctx = false;

        while (idx < dlen && rawData[idx] != ']') {
            while (idx < dlen && isspace(rawData[idx])) idx++;
            if (idx >= dlen || rawData[idx] == ']') break;

            style_op[args_count] = rawData[idx++];

            while (idx < dlen && isspace(rawData[idx])) idx++;
            if (idx < dlen && rawData[idx] == ':') idx++;
            while (idx < dlen && isspace(rawData[idx])) idx++;

            args_len = 0;

            while (idx < dlen && rawData[idx] != ',' && rawData[idx] != ']') {
                if (!isspace(rawData[idx])) {
                    style_args[args_count][args_len++] = rawData[idx];
                }
                idx++;
            }

            style_args[args_count][args_len] = '\0';

            if (style_op[args_count] == '/') {
                style = bs;
                dynamic = bd;
                use_ctx = true;
            } else if (style_args[args_count][0] == '/') {
                switch (style_op[args_count]) {
                    case 'c': {
                        style.color = bs.color;
                        use_ctx = true;
                        break;
                    }
                    case 'b': {
                        style.background = bs.background;
                        use_ctx = true;
                        break;
                    }
                    case 's': {
                        style.stroke = bs.stroke;
                        use_ctx = true;
                        break;
                    }
                    case 'd': {
                        dynamic = bd;
                        use_ctx = true;
                        break;
                    }
                    default: break;
                }
            } else {
                switch (style_op[args_count]) {
                    case 'c': {
                        Color src = _getColor(&cd, style_args[args_count]);
                        style.color = src;
                        use_ctx = true;
                        break;
                    }
                    case 'b': {
                        Color src = _getColor(&cd, style_args[args_count]);
                        style.background = src;
                        use_ctx = true;
                        break;
                    }
                    case 's': {
                        if (!strcmp(style_args[args_count], "bold"))
                            {style.stroke |= bold; use_ctx = true;}
                        if (!strcmp(style_args[args_count], "underline"))
                            {style.stroke |= underline; use_ctx = true;}
                        if (!strcmp(style_args[args_count], "italic"))
                            {style.stroke |= italic; use_ctx = true;}
                        break;
                    }
                    case 'd':
                        dynamic.delay = atoi(style_args[args_count]);
                        use_ctx = true;
                        break;
                    default: break;
                }
            }

            args_count++;
            if (idx < dlen && rawData[idx] == ',') idx++;
        }

        if (idx < dlen && rawData[idx] == ']') {
            if (!use_ctx) {
                for (size_t jdx = ctx_start; jdx <= idx; jdx++) sb->append(sb, rawData[jdx]);
            } else {
                vis->push(vis, (InlineStyle){sb->size, style, dynamic});
            }
        } else {
            for (size_t jdx = ctx_start; jdx <= idx; jdx++) sb->append(sb, rawData[jdx]);
        }
    }

    return newPair(Vector(InlineStyle), StringBuilder, {vis, sb});
}
