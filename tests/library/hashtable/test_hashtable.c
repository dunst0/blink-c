/**
 * @file test_hashtable.c
 * @author rick
 * @date 24.07.20
 * @brief Test for the HashTable implementation
 */

#include <setjmp.h>
#include <stdarg.h>
#include <stddef.h>
#include <stdint.h>

#include <cmocka.h>

#include "waitui/hashtable.h"

typedef struct value {
    int count;
    int i;
} value;

static value *value_new(int i) {
    value *this = calloc(1, sizeof(*this));
    if (!this) { return NULL; }
    this->i = i;
    return this;
}

static void value_destroy(value **this) {
    if (!this || !(*this)) { return; }
    free(*this);
    *this = NULL;
}

static int value_check_counter(value *this, int *count) {
    if (!this) { return 0; }
    return this->count == *count;
}

CREATE_HASHTABLE_TYPE(INTERFACE, value)

CREATE_HASHTABLE_TYPE(IMPLEMENTATION, value)

static int setup(void **state) {
    *state = value_hashtable_new(997);
    if (!(*state)) { return -1; }
    return 0;
}

static int teardown(void **state) {
    value_hashtable *table = *state;
    value_hashtable_destroy(&table);
    return 0;
}

static void test_hashtable_insert_has_lookup(void **state) {
    value_hashtable *table = (value_hashtable *) *state;

    str key       = STR_STATIC_INIT("foo");
    value *value1 = value_new(42);

    value *valueLookup1 = NULL;

    assert_false(waitui_hashtable_has(table, key));
    valueLookup1 = (value *) waitui_hashtable_lookup(table, key);
    assert_null(valueLookup1);
    assert_true(waitui_hashtable_insert(table, key, (void *) value1));
    assert_true(waitui_hashtable_has(table, key));
    valueLookup1 = (value *) waitui_hashtable_lookup(table, key);
    assert_non_null(valueLookup1);
    assert_int_equal(valueLookup1->i, 42);
}

static void test_hashtable_insert_has_lookup_collision(void **state) {
    (void) state; /* unused */

    value_hashtable *table = value_hashtable_new(1);
    assert_non_null(table);

    str key1      = STR_STATIC_INIT("foo1");
    value *value1 = value_new(42);
    str key2      = STR_STATIC_INIT("foo2");
    value *value2 = value_new(21);
    str key3      = STR_STATIC_INIT("foo3");
    value *value3 = value_new(1);
    str key4      = STR_STATIC_INIT("foo4");
    value *value4 = value_new(21);
    str key5      = STR_STATIC_INIT("foo5");
    value *value5 = value_new(1337);

    value *valueLookup5 = NULL;

    assert_true(value_hashtable_insert(table, key1, (void *) value1));
    assert_true(value_hashtable_has(table, key1));
    assert_true(value_hashtable_insert(table, key2, (void *) value2));
    assert_true(value_hashtable_has(table, key2));
    assert_true(value_hashtable_insert(table, key3, (void *) value3));
    assert_true(value_hashtable_has(table, key3));
    assert_true(value_hashtable_insert(table, key4, (void *) value4));
    assert_true(value_hashtable_has(table, key4));
    assert_true(value_hashtable_insert(table, key5, (void *) value5));
    assert_true(value_hashtable_has(table, key5));
    valueLookup5 = (value *) value_hashtable_lookup(table, key5);
    assert_non_null(valueLookup5);
    assert_int_equal(valueLookup5->i, 1337);

    value_hashtable_destroy(&table);
    assert_null(table);
}

static void test_hashtable_double_insert_fail_without_check(void **state) {
    value_hashtable *table = (value_hashtable *) *state;

    str key1      = STR_STATIC_INIT("foo1");
    value *value1 = value_new(1337);

    value *valueLookup1 = NULL;

    assert_true(value_hashtable_insert(table, key1, (void *) value1));
    assert_true(value_hashtable_has(table, key1));
    assert_false(value_hashtable_insert(table, key1, (void *) value1));
    assert_true(value_hashtable_has(table, key1));
    valueLookup1 = (value *) value_hashtable_lookup(table, key1);
    assert_non_null(valueLookup1);
    assert_int_equal(valueLookup1->i, 1337);
}

static void test_hashtable_double_insert_lookup_with_check(void **state) {
    value_hashtable *table = (value_hashtable *) *state;

    str key1      = STR_STATIC_INIT("foo1");
    value *value1 = value_new(1337);
    value *value2 = value_new(1337);
    value2->count = 1;

    int counter = 0;

    value *valueLookup1 = NULL;

    assert_true(value_hashtable_insert_check(
            table, key1, (void *) value1,
            (waitui_hashtable_value_check_fn) value_check_counter, &counter));
    assert_true(value_hashtable_has_check(
            table, key1, (waitui_hashtable_value_check_fn) value_check_counter,
            &counter));
    assert_false(value_hashtable_insert_check(
            table, key1, (void *) value1,
            (waitui_hashtable_value_check_fn) value_check_counter, &counter));
    assert_true(value_hashtable_has_check(
            table, key1, (waitui_hashtable_value_check_fn) value_check_counter,
            &counter));

    counter = 1;

    assert_true(value_hashtable_insert_check(
            table, key1, (void *) value2,
            (waitui_hashtable_value_check_fn) value_check_counter, &counter));
    assert_true(value_hashtable_has_check(
            table, key1, (waitui_hashtable_value_check_fn) value_check_counter,
            &counter));
    assert_false(value_hashtable_insert_check(
            table, key1, (void *) value2,
            (waitui_hashtable_value_check_fn) value_check_counter, &counter));
    assert_true(value_hashtable_has_check(
            table, key1, (waitui_hashtable_value_check_fn) value_check_counter,
            &counter));

    valueLookup1 = value_hashtable_lookup(table, key1);
    assert_non_null(valueLookup1);
    assert_int_equal(valueLookup1->i, 1337);
}

static void test_hashtable_double_destroy(void **state) {
    (void) state; /* unused */

    value_hashtable *table = value_hashtable_new(1);
    assert_non_null(table);

    value_hashtable_destroy(&table);
    assert_null(table);

    value_hashtable_destroy(&table);
    assert_null(table);
}

int main(void) {
    const struct CMUnitTest tests[] = {
            cmocka_unit_test_setup_teardown(test_hashtable_insert_has_lookup,
                                            setup, teardown),
            cmocka_unit_test(test_hashtable_insert_has_lookup_collision),
            cmocka_unit_test_setup_teardown(
                    test_hashtable_double_insert_fail_without_check, setup,
                    teardown),
            cmocka_unit_test_setup_teardown(
                    test_hashtable_double_insert_lookup_with_check, setup,
                    teardown),
            cmocka_unit_test(test_hashtable_double_destroy),
    };

    return cmocka_run_group_tests(tests, NULL, NULL);
}
