find_package(CMocka CONFIG REQUIRED)

add_executable(blink-test_hashtable)

target_sources(blink-test_hashtable
        PRIVATE
        "test_hashtable.c"
        )

target_link_libraries(blink-test_hashtable PRIVATE hashtable ${CMOCKA_LIBRARIES})

add_test(blink-test_hashtable blink-test_hashtable)
