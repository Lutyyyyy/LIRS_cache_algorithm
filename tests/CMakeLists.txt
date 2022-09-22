set (
    TESTS
    LIRS_test
    ideal_test
)

foreach(TEST ${TESTS})
    add_subdirectory(${TEST})
endforeach()

set (TESTS "${TESTS}" PARENT_SCOPE)