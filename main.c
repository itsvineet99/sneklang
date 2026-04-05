#include <stdio.h>
#include <string.h>
#include <math.h>

#include "snekobject.h"

static int tests_run = 0;
static int tests_failed = 0;

static void report(const char *name, int ok) {
    tests_run++;
    if (ok) {
        printf("[PASS] %s\n", name);
    } else {
        tests_failed++;
        printf("[FAIL] %s\n", name);
    }
}

static int is_int_obj(snek_object_t *obj, int expected) {
    return obj != NULL && obj->kind == INTEGER && obj->data.v_int == expected;
}

static int is_float_obj(snek_object_t *obj, float expected, float tol) {
    return obj != NULL && obj->kind == FLOAT && fabsf(obj->data.v_float - expected) <= tol;
}

static int is_string_obj(snek_object_t *obj, const char *expected) {
    return obj != NULL && obj->kind == STRING && strcmp(obj->data.v_string, expected) == 0;
}

static int is_vector3_ints(snek_object_t *obj, int x, int y, int z) {
    if (obj == NULL || obj->kind != VECTOR3) {return 0;}
    return is_int_obj(obj->data.v_vector3.x, x)
        && is_int_obj(obj->data.v_vector3.y, y)
        && is_int_obj(obj->data.v_vector3.z, z);
}

static int is_array_ints(snek_object_t *obj, const int *values, size_t count) {
    if (obj == NULL || obj->kind != ARRAY) {return 0;}
    if (obj->data.v_array.size != count) {return 0;}
    for (size_t i = 0; i < count; i++) {
        if (!is_int_obj(snek_array_get(obj, i), values[i])) {
            return 0;
        }
    }
    return 1;
}

int main() {
    printf("SnekLang smoke tests\n\n");

    snek_object_t *rc = new_snek_integer(42);
    report("refcount starts at 1", rc != NULL && rc->refcount == 1);
    refcount_inc(rc);
    report("refcount increment", rc != NULL && rc->refcount == 2);
    refcount_dec(rc);
    report("refcount decrement", rc != NULL && rc->refcount == 1);
    refcount_dec(rc);
    rc = NULL;

    snek_object_t *i2 = new_snek_integer(2);
    snek_object_t *i3 = new_snek_integer(3);
    snek_object_t *i5 = snek_add(i2, i3);
    report("integer add", is_int_obj(i5, 5));
    report("integer length", snek_length(i5) == 1);

    snek_object_t *f15 = new_snek_float(1.5f);
    snek_object_t *f35 = snek_add(i2, f15);
    report("int + float add", is_float_obj(f35, 3.5f, 0.0001f));

    snek_object_t *s1 = new_snek_string("snek");
    snek_object_t *s2 = new_snek_string("lang");
    snek_object_t *s3 = snek_add(s1, s2);
    report("string add", is_string_obj(s3, "sneklang"));
    report("string length", snek_length(s3) == 8);

    snek_object_t *vx = new_snek_integer(1);
    snek_object_t *vy = new_snek_integer(2);
    snek_object_t *vz = new_snek_integer(3);
    snek_object_t *v1 = new_snek_vector3(vx, vy, vz);
    report("vector3 length", snek_length(v1) == 3);
    report("vector3 stores components", is_vector3_ints(v1, 1, 2, 3));
    report("vector3 bumps refcounts", vx->refcount == 2 && vy->refcount == 2 && vz->refcount == 2);
    refcount_dec(vx);
    refcount_dec(vy);
    refcount_dec(vz);
    vx = vy = vz = NULL;

    snek_object_t *v2x = new_snek_integer(4);
    snek_object_t *v2y = new_snek_integer(5);
    snek_object_t *v2z = new_snek_integer(6);
    snek_object_t *v2 = new_snek_vector3(v2x, v2y, v2z);
    refcount_dec(v2x);
    refcount_dec(v2y);
    refcount_dec(v2z);
    v2x = v2y = v2z = NULL;

    snek_object_t *v3 = snek_add(v1, v2);
    report("vector3 add", is_vector3_ints(v3, 5, 7, 9));

    snek_object_t *arr1 = new_snek_array(2);
    snek_object_t *a1 = new_snek_integer(1);
    snek_object_t *a2 = new_snek_integer(2);
    report("array set index 0", snek_array_set(arr1, 0, a1));
    report("array set index 1", snek_array_set(arr1, 1, a2));
    report("array length", snek_length(arr1) == 2);
    report("array get index 1", is_int_obj(snek_array_get(arr1, 1), 2));
    report("array set bumps refcount", a1->refcount == 2 && a2->refcount == 2);
    refcount_dec(a1);
    refcount_dec(a2);
    a1 = a2 = NULL;

    snek_object_t *arr2 = new_snek_array(1);
    snek_object_t *a3 = new_snek_integer(3);
    report("array set index 0 (second array)", snek_array_set(arr2, 0, a3));
    refcount_dec(a3);
    a3 = NULL;

    snek_object_t *arr3 = snek_add(arr1, arr2);
    int expected_vals[] = {1, 2, 3};
    report("array add (concat)", is_array_ints(arr3, expected_vals, 3));
    report("array add length", snek_length(arr3) == 3);

    refcount_dec(i2);
    refcount_dec(i3);
    refcount_dec(i5);
    refcount_dec(f15);
    refcount_dec(f35);
    refcount_dec(s1);
    refcount_dec(s2);
    refcount_dec(s3);
    refcount_dec(v1);
    refcount_dec(v2);
    refcount_dec(v3);
    refcount_dec(arr1);
    refcount_dec(arr2);
    refcount_dec(arr3);

    printf("\nTests run: %d | Failures: %d\n", tests_run, tests_failed);
    return tests_failed == 0 ? 0 : 1;
}
