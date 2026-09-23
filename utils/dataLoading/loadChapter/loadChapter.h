#pragma once

#include "../../LuaVM/lua_vm.h"
#include "../../../include/gameplay/chapter.h"

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
        root.push.index(root, idx);
        LuaTableObject currNode = vm->getOrElse.table(vm, emptyLuaObject());

        /* Node ID */
        currNode.push.field(currNode, "id");
        i64 id = (i64)vm->getOrElse.integer(vm, -1);

        /* Node Preamble */
        currNode.push.field(currNode, "preamble");
        c_str raw = vm->getOrElse.c_str(vm, "(nil)");
        c_str preamble = trimLuaLongString(raw);
        free(raw);

        /* Node Choices */
        currNode.push.field(currNode, "choices");
        LuaTableObject choicesTable = vm->getOrElse.table(vm, emptyLuaObject());

        Vector(Choice) choices = newVector(Choice);

        size_t choiceCount = choicesTable.size(choicesTable);
        for (size_t jdx = 1; jdx <= choiceCount; jdx++) {
            choicesTable.push.index(choicesTable, jdx);
            LuaTableObject currChoice = vm->getOrElse.table(vm, emptyLuaObject());

            /* Choice Text */
            currChoice.push.field(currChoice, "text");
            c_str text = vm->getOrElse.c_str(vm, "(nil)");

            currChoice.push.field(currChoice, "goingTo");
            i64 goingTo = vm->getOrElse.integer(vm, -1);

            choices->push(choices, newChoice(text, goingTo));
            free(text);

            vm->pop(vm);
        }

        vm->pop(vm);

        c->loadedNodes->put(c->loadedNodes, newTextNode(id, preamble, choices));
        free(preamble);

        vm->pop(vm);

    }

    vm->pop(vm);

    vm->close(vm);

    delete(LuaVM)(&vm);

    return c;
}