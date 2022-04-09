# This gives cJSON_INCLUDE_PUBLIC_DIRS, and cJSON_SOURCES
include(${CMAKE_CURRENT_LIST_DIR}/cJSON/cJSONFilePaths.cmake)

set(cJSON_INCLUDE_DIRS
    ${cJSON_INCLUDE_PUBLIC_DIRS}
)

set(cJSON_SRCS
    ${cJSON_SOURCES}
)

set(cJSON_REQUIRES
)

idf_component_register(
    SRCS
        ${cJSON_SRCS}
    INCLUDE_DIRS
        ${cJSON_INCLUDE_DIRS}
    REQUIRES
        ${cJSON_REQUIRES}
)