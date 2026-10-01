#pragma once

#include "../../LuaVM/lua_vm.h"
#include "../../LuaVM/lua_vm_pipeline.h"
#include "../../../include/gameplay/chapter.h"
#include "../../../include/entities/player.h"

static inline c_str trimLuaLongString(c_str raw) {
    StringBuilder sb = newStringBuilder("");
    size_t rlen = strlen(raw);

    size_t lineStart = 0;
    for (size_t idx = 0; idx <= rlen; idx++) {
        if (raw[idx] == '\n' || idx == rlen) {
            str line = (str){raw + lineStart, idx - lineStart};

            StringBuilder tmp = newStringBuilder(line);
            c_str trimmed = tmp->trim.both(tmp)->release(&tmp);

            sb->concat.c_str(sb, trimmed);
            free(trimmed);

            if (idx != rlen) sb->append(sb, '\n');
            lineStart = (idx + 1);
        }
    }

    return sb->release(&sb);
}

static inline i8 underscoreToSpace(i8 ch) { return (ch == '_') ? ' ' : ch; }

static inline c_str normalizeChapterName(c_str path) {
    c_str root = strrchr(path, '/');
    root = (root) ? root + 1 : path;

    StringBuilder sb = newStringBuilder(root);
    c_str name = sb->chop.right(sb, 4)
        ->map(sb, underscoreToSpace)
        ->release(&sb);

    return name;
}

static inline Chapter loadChapter(c_str path) {
    c_str name = normalizeChapterName(path);
    Chapter c = newChapter(name, newSet(TextNode, TextNode_cmp_id));
    free(name);

    LuaVM vm = newLuaVM();

    vm->open(vm);

    if (vm->dumpFile(vm, path) != VM_OK) raise(ERROR, "Cannot load file: %s", path);

    LuaTableObject root = vm->getOrElse.table(vm, emptyLuaObject());
    size_t nodeCount = root.size(root);

    for (size_t idx = 1; idx <= nodeCount; idx++) {
        TablePipeline tp = nullptr;

        LuaTableObject currNode = emptyLuaObject();

        tp = newTablePipeline(root);
        tp->get.table(tp, idx, &currNode)
            ->consume(&tp);

        tp = newTablePipeline(currNode);
        lua_Integer id = -1;
        c_str preamble = "(nil)";
        LuaTableObject choicesTable;

        tp->config.integer(tp, id)
            ->config.c_str(tp, preamble);

        /* Load Node Data */
        tp->load.integer(tp, "id", &id)
            ->load.c_str(tp, "preamble", &preamble)
            ->load.table(tp, "choices", &choicesTable)
            ->consume(&tp);

        /* Strip Preamble */
        c_str raw = preamble;
        preamble = trimLuaLongString(preamble);
        free(raw);

        /* Node Choices */
        Vector(Choice) choices = newVector(Choice);
        size_t choiceCount = choicesTable.size(choicesTable);

        for (size_t jdx = 1; jdx <= choiceCount; jdx++) {
            tp = newTablePipeline(choicesTable);
            LuaTableObject currChoice = emptyLuaObject();

            tp->get.table(tp, jdx, &currChoice)
                ->consume(&tp);

            tp = newTablePipeline(currChoice);
            c_str text = "(nil)";
            lua_Integer trigger = 0;
            lua_Integer goingTo = END_OF_PATH;
            LuaTableObject sideEffectsTable = emptyLuaObject();

            tp->config.c_str(tp, text)
                ->config.integer(tp, goingTo);

            /* Load Choice Data */
            tp->load.c_str(tp, "text", &text)
                ->load.integer(tp, "goingTo", &goingTo)
                ->load.integer(tp, "trigger", &trigger)
                ->load.table(tp, "sideEffects", &sideEffectsTable)
                ->consume(&tp);

            Set(SideEffect) sideEffects = newSet(SideEffect, se_cmp);

            size_t sideEffectCount = sideEffectsTable.size(sideEffectsTable);
            for (size_t kdx = 1; kdx <= sideEffectCount; kdx++) {
                tp = newTablePipeline(sideEffectsTable);
                LuaTableObject currSideEffect = emptyLuaObject();

                tp->get.table(tp, kdx, &currSideEffect)
                    ->consume(&tp);

                 /* Load sideEffects */
                tp = newTablePipeline(currSideEffect);
                c_str id = "none";
                lua_Integer failsafe = END_OF_PATH;
                lua_Integer ammount = 0;

                tp->config.c_str(tp, id)
                    ->config.integer(tp, ammount);

                tp->load.c_str(tp, "type", &id)
                    ->load.integer(tp, "ammount", &ammount)
                    ->load.integer(tp, "failsafe", &failsafe)
                    ->consume(&tp);

                /**
                 * TODO: 
                 * Add Constructor
                 * Add id lookup
                 * Check what happens when you gain an item and you die
                 */
                SideEffect se = (SideEffect) {
                    .id = id,
                    .ammount = ammount,
                    .func = setPlayerHealth,
                    .failsafe = failsafe
                };

                sideEffects->put(sideEffects, se);
                currSideEffect.release(currSideEffect);
            }

            choices->push(
                choices,
                newChoice(text, goingTo, .trigger = (i8)trigger, .sideEffects = sideEffects));
            free(text);

            currChoice.release(currChoice);
        }

        choicesTable.release(choicesTable);
        currNode.release(currNode);

        c->loadedNodes->put(c->loadedNodes, newTextNode(id, preamble, choices));
        free(preamble);
    }

    root.release(root);

    vm->close(vm);

    delete(LuaVM)(&vm);

    return c;
}
