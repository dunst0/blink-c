/**
 * @file test_list.c
 * @author rick
 * @date 09.07.21
 * @brief Test for the List implementation
 */

#include <setjmp.h>
#include <stdarg.h>
#include <stddef.h>
#include <stdint.h>
#include <stdlib.h>

#include <cmocka.h>

#include "waitui/list.h"

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

CREATE_LIST_TYPE(INTERFACE, value)

CREATE_LIST_TYPE(IMPLEMENTATION, value)

static int setup(void **state) {
    *state = value_list_new();
    if (!(*state)) { return -1; }
    return 0;
}

static int teardown(void **state) {
    value_list *list = *state;
    value_list_destroy(&list);
    return 0;
}

static void test_list_push_and_pop(void **state) {
    value_list *list = (value_list *) *state;

    value *value1 = value_new(1);
    value *value2 = value_new(2);
    value *test   = NULL;

    assert_true(value_list_push(list, value1));
    assert_true(value_list_push(list, value2));
    test = value_list_pop(list);
    assert_int_equal(test->i, 2);
    value_destroy(&test);
    test = value_list_pop(list);
    assert_int_equal(test->i, 1);
    value_destroy(&test);
}

int main(void) {
    const struct CMUnitTest tests[] = {
            cmocka_unit_test_setup_teardown(test_list_push_and_pop, setup,
                                            teardown),
    };

    return cmocka_run_group_tests(tests, NULL, NULL);
}
