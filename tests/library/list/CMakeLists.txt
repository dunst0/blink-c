add_executable(waitui-test_list)

target_sources(waitui-test_list
        PRIVATE
        "test_list.c"
        )

target_link_libraries(waitui-test_list PRIVATE list cmocka-static)

add_test(waitui-test_list waitui-test_list)
