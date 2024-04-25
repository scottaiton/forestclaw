/*
Copyright (c) 2012-2023 Carsten Burstedde, Donna Calhoun, Scott Aiton
All rights reserved.

Redistribution and use in source and binary forms, with or without
modification, are permitted provided that the following conditions are met:

 * Redistributions of source code must retain the above copyright notice, this
list of conditions and the following disclaimer.
 * Redistributions in binary form must reproduce the above copyright notice,
this list of conditions and the following disclaimer in the documentation
and/or other materials provided with the distribution.

THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND
ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
DISCLAIMED.  IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE
FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR
SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER
CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY,
OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
*/

#include <fclaw_context.h>

#define PACKING_VTABLE_NAME "fclaw_context_t"

typedef enum
{
    FCLAW_CONTEXT_INT,
    FCLAW_CONTEXT_DOUBLE,
    FCLAW_CONTEXT_POINTER
} type_t;

typedef struct value
{
    type_t type;
    union
    {
        int i;
        double d;
    } value;

    void *pointer;

} value_t;

static void
value_destroy(void *data)
{
    value_t *value = (value_t *)data;
    FCLAW_FREE(value);
}

struct fclaw_context
{
    int initializing;
    fclaw_pointer_map_t *values;
};

static fclaw_context_t *
context_new()
{
    fclaw_context_t *context = FCLAW_ALLOC(fclaw_context_t, 1);
    context->initializing = 1;
    context->values = fclaw_pointer_map_new();
    return context;
}

static void 
context_destroy(void *data)
{
    fclaw_context_t *context = (fclaw_context_t *)data;
    fclaw_pointer_map_destroy(context->values);
    FCLAW_FREE(context);
}

static void
reset_pointers(const char *key, void *data, void *user)
{
    value_t *value = (value_t *)data;
    value->pointer = NULL;
}

fclaw_context_t* fclaw_context_get(fclaw_global_t *glob, const char *name)
{
    fclaw_context_t *context = fclaw_global_get_attribute(glob, name);

    if(context == NULL)
    {
        context = context_new();
        fclaw_global_attribute_store(glob, name, context, PACKING_VTABLE_NAME, context_destroy);
    }
    else
    {
        context->initializing = 0;
        fclaw_pointer_map_iterate(context->values, reset_pointers, NULL);
    }

    return context;
}


void fclaw_context_get_int(fclaw_context_t *context, 
                           const char *name,
                           int *value,
                           int default_value)
{
    value_t *v = fclaw_pointer_map_get(context->values, name);
    if(v != NULL)
    {
        if(v->type != FCLAW_CONTEXT_INT)
        {
            fclaw_abortf("fclaw_context_get_int: Value %s is not an int", name);
        }
        *value = v->value.i;
    }
    else if (context->initializing)
    {
        v = FCLAW_ALLOC(value_t, 1);
        v->type = FCLAW_CONTEXT_INT;
        v->value.i = default_value;
        fclaw_pointer_map_insert(context->values, name, v, value_destroy);
        *value = default_value;
    }
    else
    {
        fclaw_abortf("fclaw_context_get_int: Value %s not found", name);
    }
    
    v->pointer = value;
}

void fclaw_context_get_double(fclaw_context_t *context, 
                              const char *name, 
                              double *value,
                              double default_value)
{
    value_t *v = fclaw_pointer_map_get(context->values, name);
    if(v != NULL)
    {
        if(v->type != FCLAW_CONTEXT_DOUBLE)
        {
            fclaw_abortf("fclaw_context_get_double: Value %s is not a double", name);
        }
        *value = v->value.d;
    }
    else if(context->initializing)
    {
        v = FCLAW_ALLOC(value_t, 1);
        v->type = FCLAW_CONTEXT_DOUBLE;
        v->value.d = default_value;
        fclaw_pointer_map_insert(context->values, name, v, value_destroy);
        *value = default_value;
    }
    else
    {
        fclaw_abortf("fclaw_context_get_double: Value %s not found", name);
    }

    v->pointer = value;
}

static
void save_value(const char *key, void *data, void *user)
{
    value_t *value = (value_t *)data;
    if(value->pointer == NULL)
    {
        fclaw_abortf("fclaw_context_save: Value %s has no pointer", key);
    }
    if(value->type == FCLAW_CONTEXT_INT)
    {
        value->value.i = *(int *)value->pointer;
    }
    else if(value->type == FCLAW_CONTEXT_DOUBLE)
    {
        value->value.d = *(double *)value->pointer;
    }
    else
    {
        SC_ABORT_NOT_REACHED();
    }
}

void fclaw_context_save(fclaw_context_t *context)
{
    fclaw_pointer_map_iterate(context->values, save_value, NULL);
}