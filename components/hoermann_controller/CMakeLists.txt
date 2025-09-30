# Collect all .cpp files from current directory and subdirectories
file(GLOB COMPONENT_SRCS
    "${CMAKE_CURRENT_LIST_DIR}/*.cpp"
    "${CMAKE_CURRENT_LIST_DIR}/cover/*.cpp"
    "${CMAKE_CURRENT_LIST_DIR}/switch/*.cpp"
    "${CMAKE_CURRENT_LIST_DIR}/button/*.cpp"
)

idf_component_register(
    SRCS ${COMPONENT_SRCS}
    INCLUDE_DIRS "." "cover" "switch" "button"
    REQUIRES esphome
)
