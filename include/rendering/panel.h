#pragma once

#include "writable_region.h"

#include "../../utils/SeaCore/stdc.h"
#include "../core/loop_event.h"

typedef struct _panel *Panel;
typedef LoopEvent (*PanelAction)(Panel, void *ctx);

deleteType(PanelAction)
PairType(DictKey, PanelAction)
NodeType(Pair(DictKey, PanelAction))
ArrayType(Pair(DictKey, PanelAction))
DictType(PanelAction)

struct _panel {
    TerminalDimensions dimensions;

    WritableRegion header;
    WritableRegion footer;
    WritableRegion body;

    Dict(PanelAction) actions;

    struct {
        void (*header)(Panel);
        void (*footer)(Panel);
        void (*body)(Panel);
    } blit;

    struct {
        void (*header)(Panel, WritableRegion);
        void (*footer)(Panel, WritableRegion);
        void (*body)(Panel, WritableRegion);
    } addContent;

    void (*resize)(Panel);
    LoopEvent (*dispatch)(Panel, c_str key);

    void (*onEnter)(Panel);
    void (*onExit)(Panel);
};

Panel newPanel(TerminalDimensions *td);

deleteDefine(Panel) {
    if (!self || !*self) return;
    delete(WritableRegion)(&(*self)->header);
    delete(WritableRegion)(&(*self)->body);
    delete(WritableRegion)(&(*self)->footer);
    delete(Dict(PanelAction))(&(*self)->actions);
    free(*self);
    *self = nullptr;
}

NodeType(Panel)
ArrayType(Panel)
StackType(Panel)
