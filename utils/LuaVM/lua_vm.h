#pragma once

#include "../SeaCore/stdc.h"
#include "lua.h"
#include "lauxlib.h"
#include "lualib.h"

typedef struct _lua_vm *LuaVM;
typedef struct _lua_table_object LuaTableObject;
typedef enum {
    VM_MEMORY = LUA_ERRMEM,
    VM_RUN = LUA_ERRRUN,
    VM_GENERIC = LUA_ERRERR,
    VM_OK = LUA_OK
} LuaErrorCode;

struct _lua_table_object {
    LuaVM ref;
    size_t stackIdx;

    struct {
        void (*field)(LuaTableObject, c_str);
        void (*index)(LuaTableObject, lua_Integer);
    } push;

    size_t (*size)(LuaTableObject);
};

struct _lua_vm {
    lua_State *state;

    void (*open)(LuaVM);
    void (*close)(LuaVM);

    struct {
        void (*integer)(LuaVM, lua_Integer);
        void (*number)(LuaVM, lua_Number);
        void (*c_str)(LuaVM, c_str);
        void (*boolean)(LuaVM, bool);
        void (*nil)(LuaVM);
        void (*global)(LuaVM, c_str);
    } push;

    void (*pop)(LuaVM);

    struct {
        lua_Integer (*integer)(LuaVM, lua_Integer);
        lua_Number (*number)(LuaVM, lua_Number);
        c_str (*c_str)(LuaVM, c_str);
        bool (*boolean)(LuaVM, bool);
        LuaTableObject (*table)(LuaVM, LuaTableObject);
    } getOrElse;

    LuaErrorCode (*dumpFile)(LuaVM, c_str);
    bool (*empty)(LuaVM);
};

static inline void _lua_vm_open(LuaVM vm) { luaL_openlibs(vm->state); }
static inline void _lua_vm_close(LuaVM vm) { lua_close(vm->state); }

static inline bool _lua_vm_empty(LuaVM vm) { return (lua_gettop(vm->state) < 1); }

static inline void _lua_vm_push_integer(LuaVM vm, lua_Integer i) {
    if (!lua_checkstack(vm->state, 1)) raise(ERROR, "Lua Virtual Stack cannot grow.");
    lua_pushinteger(vm->state, i);
}

static inline void _lua_vm_push_boolean(LuaVM vm, bool b) {
    if (!lua_checkstack(vm->state, 1)) raise(ERROR, "Lua Virtual Stack cannot grow.");
    lua_pushboolean(vm->state, b);
}

static inline void _lua_vm_push_number(LuaVM vm, lua_Number d) {
    if (!lua_checkstack(vm->state, 1)) raise(ERROR, "Lua Virtual Stack cannot grow.");
    lua_pushnumber(vm->state, d);
}

static inline void _lua_vm_push_c_str(LuaVM vm, c_str s) {
    if (!lua_checkstack(vm->state, 1)) raise(ERROR, "Lua Virtual Stack cannot grow.");
    lua_pushstring(vm->state, s);
}

static inline void _lua_vm_push_nil(LuaVM vm) {
    if (!lua_checkstack(vm->state, 1)) raise(ERROR, "Lua Virtual Stack cannot grow.");
    lua_pushnil(vm->state);
}

static inline void _lua_vm_push_global(LuaVM vm, c_str s) {
    if (!lua_checkstack(vm->state, 1)) raise(ERROR, "Lua Virtual Stack cannot grow.");
    lua_getglobal(vm->state, s);
}

static inline void _lua_vm_pop(LuaVM vm) {
    if (_lua_vm_empty(vm)) return;
    lua_pop(vm->state, 1);
}

static inline lua_Integer _lua_vm_getOrElse_integer(LuaVM vm, lua_Integer def) {
    if (_lua_vm_empty(vm)) return def;
    if (!lua_isinteger(vm->state, -1)) {
        _lua_vm_pop(vm);
        return def;
    }

    lua_Integer ret = lua_tointeger(vm->state, -1);
    _lua_vm_pop(vm);

    return ret;
}

static inline lua_Number _lua_vm_getOrElse_number(LuaVM vm, lua_Number def) {
    if (_lua_vm_empty(vm)) return def;
    if (lua_type(vm->state, -1) != LUA_TNUMBER) {
        _lua_vm_pop(vm);
        return def;
    }

    lua_Number ret = lua_tonumber(vm->state, -1);
    _lua_vm_pop(vm);

    return ret;
}

static inline bool _lua_vm_getOrElse_boolean(LuaVM vm, bool def) {
    if (_lua_vm_empty(vm)) return def;
    if (lua_type(vm->state, -1) != LUA_TBOOLEAN) {
        _lua_vm_pop(vm);
        return def;
    }

    bool ret = lua_toboolean(vm->state, -1);
    _lua_vm_pop(vm);

    return ret;
}

static inline c_str _lua_vm_getOrElse_c_str(LuaVM vm, c_str def) {
    if (_lua_vm_empty(vm)) return strdup(def);
    if (lua_type(vm->state, -1) != LUA_TSTRING) {
        _lua_vm_pop(vm);
        return strdup(def);
    }

    c_str ret = strdup(lua_tostring(vm->state, -1));
    _lua_vm_pop(vm);

    return ret;
}

static inline LuaErrorCode _lua_vm_dumpFile(LuaVM vm, c_str path) {
    return luaL_dofile(vm->state, path);
}

static inline void _lua_table_object_push_field(LuaTableObject lto, c_str fieldName) {
    if (!lto.ref || !lto.stackIdx) return;
    if (!lua_checkstack(lto.ref->state, 1)) raise(ERROR, "Lua Virtual Stack cannot grow.");

    lua_getfield(lto.ref->state, lto.stackIdx, fieldName);

    if (lua_isnil(lto.ref->state, -1)) _lua_vm_pop(lto.ref);
}

static inline void _lua_table_object_push_index(LuaTableObject lto, lua_Integer index) {
    if (!lto.ref || !lto.stackIdx) return;
    if (!lua_checkstack(lto.ref->state, 1)) raise(ERROR, "Lua Virtual Stack cannot grow.");

    lua_geti(lto.ref->state, lto.stackIdx, index);

    if (lua_isnil(lto.ref->state, -1)) _lua_vm_pop(lto.ref);
}

static inline size_t _lua_table_size(LuaTableObject lto) {
    if (!lto.ref || !lto.stackIdx) return 0;

    size_t size = 0;
    LuaVM vm = lto.ref;

    vm->push.nil(vm);
    while (lua_next(vm->state, lto.stackIdx)) {
        size++;
        vm->pop(vm);
    }

    return size;
}

static inline LuaTableObject newLuaTableObject(LuaVM ref) {
    LuaTableObject lto = {};

    lto.ref = ref;
    lto.stackIdx = lua_gettop(ref->state);

    lto.push.field = _lua_table_object_push_field;
    lto.push.index = _lua_table_object_push_index;

    lto.size = _lua_table_size;

    return lto;
}

static inline LuaTableObject emptyLuaObject(void) {
    return (LuaTableObject){
        .push.field = _lua_table_object_push_field,
        .push.index = _lua_table_object_push_index,
        .size = _lua_table_size
    };
}

static inline LuaTableObject _lua_vm_getOrElse_LuaTabelObject(LuaVM vm, LuaTableObject def) {
    if (_lua_vm_empty(vm)) return def;
    if (lua_type(vm->state, -1) != LUA_TTABLE) {
        _lua_vm_pop(vm);
        return def;
    }

    return newLuaTableObject(vm);
}

static inline LuaVM newLuaVM(void) {
    LuaVM vm = calloc(1, sizeof(*vm));
    if (!vm) raise(ERROR, "OOM");

    vm->state = luaL_newstate();

    vm->open = _lua_vm_open;
    vm->close = _lua_vm_close;

    vm->empty = _lua_vm_empty;

    vm->push.integer = _lua_vm_push_integer;
    vm->push.number = _lua_vm_push_number;
    vm->push.c_str = _lua_vm_push_c_str;
    vm->push.boolean = _lua_vm_push_boolean;
    vm->push.nil = _lua_vm_push_nil;
    vm->push.global = _lua_vm_push_global;

    vm->pop = _lua_vm_pop;

    vm->getOrElse.integer = _lua_vm_getOrElse_integer;
    vm->getOrElse.number = _lua_vm_getOrElse_number;
    vm->getOrElse.c_str = _lua_vm_getOrElse_c_str;
    vm->getOrElse.boolean = _lua_vm_getOrElse_boolean;
    vm->getOrElse.table = _lua_vm_getOrElse_LuaTabelObject;

    vm->dumpFile = _lua_vm_dumpFile;

    return vm;
}

deleteDefine(LuaVM) {
    if (!self || !*self) return;
    free((*self));
    *self = nullptr;
}
