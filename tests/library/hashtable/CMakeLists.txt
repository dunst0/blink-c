add_executable(waitui-test_hashtable)

target_sources(waitui-test_hashtable
        PRIVATE
        "test_hashtable.c"
        )

target_link_libraries(waitui-test_hashtable PRIVATE hashtable cmocka-static)

add_test(waitui-test_hashtable waitui-test_hashtable)
