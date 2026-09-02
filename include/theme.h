#pragma once

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#include "../utils/Print/printer.h"
#include "../utils/SeaCore/stdc.h"

typedef struct _theme {
    Color bg;
    Color fg;
} Theme;

extern Theme defaultTheme;

void initDefaultTheme();
