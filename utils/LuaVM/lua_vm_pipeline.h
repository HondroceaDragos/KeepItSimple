#pragma once

#include "lua_vm.h"

typedef struct _table_pipeline *TablePipeline;
struct _table_pipeline {
    LuaTableObject ref;

    struct {
        lua_Integer integer;
        lua_Number number;
        bool boolean;
        c_str c_str;
        LuaTableObject table;
    } defaults;

    struct {
        TablePipeline (*integer)(TablePipeline, c_str, lua_Integer *);
        TablePipeline (*number)(TablePipeline, c_str, lua_Number *);
        TablePipeline (*boolean)(TablePipeline, c_str, bool *);
        TablePipeline (*c_str)(TablePipeline, c_str, c_str *);
        TablePipeline (*table)(TablePipeline, c_str, LuaTableObject *);
    } load;

    struct {
        TablePipeline (*integer)(TablePipeline, size_t, lua_Integer *);
        TablePipeline (*number)(TablePipeline, size_t, lua_Number *);
        TablePipeline (*boolean)(TablePipeline, size_t, bool *);
        TablePipeline (*c_str)(TablePipeline, size_t, c_str *);
        TablePipeline (*table)(TablePipeline, size_t, LuaTableObject *);
    } get;

    struct {
        TablePipeline (*integer)(TablePipeline, lua_Integer);
        TablePipeline (*number)(TablePipeline, lua_Number);
        TablePipeline (*boolean)(TablePipeline, bool);
        TablePipeline (*c_str)(TablePipeline, c_str);
        TablePipeline (*table)(TablePipeline, LuaTableObject);
    } config;

    TablePipeline (*consume)(TablePipeline *);
};

#define declareTablePipelineConfig(field, type) \
    static inline TablePipeline concat_layer2(_pipeline_config_, field)(TablePipeline self, type newValue) { \
        self->defaults.field = newValue; \
        return self; \
    }

declareTablePipelineConfig(integer, lua_Integer)
declareTablePipelineConfig(number, lua_Number)
declareTablePipelineConfig(boolean, bool)
declareTablePipelineConfig(c_str, c_str)
declareTablePipelineConfig(table, LuaTableObject)

#define declareTablePipelineLoad(name, type) \
    static inline TablePipeline concat_layer2(_pipeline_load_, name)(TablePipeline self, c_str fieldName, type *out) { \
        if (!self) raise(ERROR, "Broken Pipe"); \
        \
        if (!self->ref.stackIdx) { \
            *out = self->defaults.name; \
            return self; \
        } \
        \
        LuaTableObject table = self->ref; \
        LuaVM vm = table.ref; \
        \
        if (!table.push.field(table, fieldName)) { \
            *out = self->defaults.name; \
            return self; \
        } \
        \
        *out = vm->getOrElse.name(vm, self->defaults.name); \
        return self; \
    }

declareTablePipelineLoad(integer, lua_Integer)
declareTablePipelineLoad(number, lua_Number)
declareTablePipelineLoad(boolean, bool)
declareTablePipelineLoad(c_str, c_str)
declareTablePipelineLoad(table, LuaTableObject)

#define declareTablePipelineGet(name, type) \
    static inline TablePipeline concat_layer2(_pipeline_get_, name)(TablePipeline self, size_t idx, type *out) { \
        if (!self) raise(ERROR, "Broken Pipe"); \
        \
        if (!self->ref.stackIdx) { \
            *out = self->defaults.name; \
            return self; \
        } \
        \
        LuaTableObject table = self->ref; \
        LuaVM vm = table.ref; \
        \
        if (!table.push.index(table, idx)) { \
            *out = self->defaults.name; \
            return self; \
        } \
        \
        *out = vm->getOrElse.name(vm, self->defaults.name); \
        return self; \
    }

declareTablePipelineGet(integer, lua_Integer)
declareTablePipelineGet(number, lua_Number)
declareTablePipelineGet(boolean, bool)
declareTablePipelineGet(c_str, c_str)
declareTablePipelineGet(table, LuaTableObject)

static inline TablePipeline _pipeline_consume(TablePipeline *self) {
    if (!self || !*self) return *self;

    free(*self);
    *self = nullptr;

    return *self;
}

static inline TablePipeline newTablePipeline(LuaTableObject lto) {
    TablePipeline p = calloc(1, sizeof(*p));
    if (!p) raise(ERROR, "OOM");

    p->ref = lto;

    p->defaults.integer = 0;
    p->defaults.number = 0.0;
    p->defaults.boolean = false;
    p->defaults.c_str = (c_str)"";
    p->defaults.table = emptyLuaObject();

    p->load.integer = _pipeline_load_integer;
    p->load.number = _pipeline_load_number;
    p->load.boolean = _pipeline_load_boolean;
    p->load.c_str = _pipeline_load_c_str;
    p->load.table = _pipeline_load_table;

    p->get.integer = _pipeline_get_integer;
    p->get.number = _pipeline_get_number;
    p->get.boolean = _pipeline_get_boolean;
    p->get.c_str = _pipeline_get_c_str;
    p->get.table = _pipeline_get_table;

    p->config.integer = _pipeline_config_integer;
    p->config.number = _pipeline_config_number;
    p->config.boolean = _pipeline_config_boolean;
    p->config.c_str = _pipeline_config_c_str;
    p->config.table = _pipeline_config_table;

    p->consume = _pipeline_consume;

    return p;
}
