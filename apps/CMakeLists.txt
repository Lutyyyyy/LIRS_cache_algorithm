set(
    APPS
    LIRS_cache_run
    ideal_cache_run
    LIRS_and_ideal_compare
)

foreach(APP ${APPS})
    add_subdirectory(${APP})
endforeach()

set(APPS "${APPS}" PARENT_SCOPE) 