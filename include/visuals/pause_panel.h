#pragma once

#include "../rendering/theme.h"
#include "../rendering/panel.h"

#include "../gameplay/actions.h"
#include "../gameplay/chapter.h"

#include "../../utils/Print/printer.h"
#include "../../utils/SeaCore/stdc.h"

extern StyleArgs defaultStyle;

Panel buildPausePanel(Terminal t, Chapter currentChapter);
void deletePausePanel(Panel *p);