#include <stdlib.h>
#include <string.h>
#include <assert.h>

#include "snekobject.h"

void refcount_free(snek_object_t *obj);

void refcount_dec(snek_object_t *obj) {
    if (obj == NULL) {return;}

    obj->refcount--;
    if (obj->refcount == 0) {
        refcount_free(obj);
    }
}

void refcount_free(snek_object_t *obj) {
    if (obj == NULL) {return;}

    switch (obj->kind) {
        case INTEGER: break;
        case FLOAT: break;
        case STRING: {
            free(obj->data.v_string);
            break;
        }
        case VECTOR3: {
            refcount_dec(obj->data.v_vector3.x);
            refcount_dec(obj->data.v_vector3.y);
            refcount_dec(obj->data.v_vector3.z);
            break;
        }
        case ARRAY: {
            snek_array_t arr = obj->data.v_array;
            for (size_t i = 0; i < arr.size; i++) {
                refcount_dec(arr.elements[i]);
            }
            free(obj->data.v_array.elements);
            break;
        }
        default:
            assert(false);
    }

    free(obj);
}

void refcount_inc(snek_object_t *obj) {
    if (obj == NULL) {return;}
    obj->refcount++;
}

snek_object_t *_new_snek_object() {
    snek_object_t *obj = malloc(sizeof(snek_object_t));
    if (obj == NULL) {return NULL;}

    obj->refcount = 1;
    return obj;
}

snek_object_t *snek_add(snek_object_t *a, snek_object_t *b) {
    if (a == NULL || b == NULL) {return NULL;}

    switch (a->kind) {
    case INTEGER: {
        switch (b->kind) {
            case INTEGER: return new_snek_integer(a->data.v_int + b->data.v_int);
            case FLOAT: return new_snek_float((float)a->data.v_int + b->data.v_float);
            default: return NULL;
        }
    }
    case FLOAT: {
        switch (b->kind) {
            case INTEGER: return new_snek_float(a->data.v_float + (float)b->data.v_int);
            case FLOAT: return new_snek_float(a->data.v_float + b->data.v_float);
            default: return NULL;
        }
    }
    case STRING: {
        if (b->kind != STRING) {return NULL;}
        size_t length = strlen(a->data.v_string) + strlen(b->data.v_string) + 1; // adding 1 extra for null terminator
        char *new_str = malloc(length);
        if (new_str == NULL) {return NULL;}
        strcpy(new_str, a->data.v_string);
        strcat(new_str, b->data.v_string);
        snek_object_t *obj = new_snek_string(new_str);
        free(new_str);
        return obj;
    }
    case VECTOR3: {
        if (b->kind != VECTOR3) {return NULL;}
        return new_snek_vector3(
            snek_add(a->data.v_vector3.x, b->data.v_vector3.x),
            snek_add(a->data.v_vector3.y, b->data.v_vector3.y),
            snek_add(a->data.v_vector3.z, b->data.v_vector3.z)
        );
    }
    case ARRAY: {
        if (b->kind != ARRAY) {return NULL;}
        size_t length = a->data.v_array.size + b->data.v_array.size;
        snek_object_t *obj = new_snek_array(length);

        for (size_t i = 0; i < a->data.v_array.size; i++) {
            snek_array_set(
                obj,
                i,
                snek_array_get(a, i)
            );
        }

        for (size_t i = 0; i < b->data.v_array.size; i++) {
            snek_array_set(
                obj,
                i + a->data.v_array.size,
                snek_array_get(b, i)
            );
        }

        return obj;
    }
    default: return NULL;
    }
}

int snek_length(snek_object_t *obj) {
    if (obj == NULL) {return -1;}

    switch (obj->kind)
    {
    case INTEGER: return 1;
    case FLOAT: return 1;
    case STRING: return strlen(obj->data.v_string);
    case VECTOR3: return 3;
    case ARRAY: return obj->data.v_array.size;
    default: return -1;
    }
}

bool snek_array_set(snek_object_t *snek_obj, size_t index, snek_object_t *value) {
    if (snek_obj == NULL || value == NULL) {return false;}
    if (snek_obj->kind != ARRAY) {return false;}
    if (snek_obj->data.v_array.size <= index) {return false;}

    refcount_inc(value);
    if (snek_obj->data.v_array.elements[index] != NULL) {
        refcount_dec(snek_obj->data.v_array.elements[index]);
    }

    snek_obj->data.v_array.elements[index] = value;
    return true;
}

snek_object_t *snek_array_get(snek_object_t *snek_obj, size_t index) {
    if (snek_obj == NULL) {return NULL;}
    if (snek_obj->kind != ARRAY) {return NULL;}
    if (snek_obj->data.v_array.size <= index) {return NULL;}

    return snek_obj->data.v_array.elements[index];
}

snek_object_t *new_snek_array(size_t size) {
    snek_object_t *obj = _new_snek_object();
    if (obj == NULL) {return NULL;}

    obj->kind = ARRAY;

    snek_object_t **elements = calloc(size, sizeof(snek_object_t *));
    if (elements == NULL) {
        free(obj);
        return NULL;
    }

    snek_array_t array = {.size = size, .elements = elements};
    obj->data.v_array = array;

    return obj;
}

snek_object_t *new_snek_vector3(
    snek_object_t *x, snek_object_t *y, snek_object_t *z
) {
    if (x == NULL || y == NULL || z == NULL) {return NULL;}
    snek_object_t *obj = _new_snek_object();
    if (obj == NULL) {return NULL;}

    obj->kind = VECTOR3;
    refcount_inc(x);
    refcount_inc(y);
    refcount_inc(z);
    obj->data.v_vector3 = (snek_vector_t){.x = x, .y = y, .z = z};

    return obj;
}

snek_object_t *new_snek_string(char *value) {
    snek_object_t *obj = _new_snek_object();
    if (obj == NULL) {return NULL;}

    obj->kind = STRING;
    obj->data.v_string = malloc(strlen(value) + 1);
    if (obj->data.v_string == NULL) {
        free(obj);
        return NULL;
    }
    strcpy(obj->data.v_string, value);

    return obj;
}

snek_object_t *new_snek_float(float value) {
    snek_object_t *obj = _new_snek_object();
    if (obj == NULL) {return NULL;}

    obj->kind = FLOAT;
    obj->data.v_float = value;

    return obj;
}

snek_object_t *new_snek_integer(int value) {
    snek_object_t *obj = _new_snek_object();
    if (obj == NULL) {return NULL;}

    obj->kind = INTEGER;
    obj->data.v_int = value;

    return obj;
}
