file(GLOB_RECURSE COMPONENT_SRCS "*.cpp")

idf_component_register(
    SRCS ${COMPONENT_SRCS}
    INCLUDE_DIRS "." "cover" "switch" "button"
    REQUIRES esphome
)
