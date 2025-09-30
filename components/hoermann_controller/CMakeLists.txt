idf_component_register(
    SRCS
        "hoermann_controller.cpp"
        "hoermann_cover.cpp"
        "hoermann_switch.cpp"
        "hoermann_button.cpp"
    INCLUDE_DIRS "."
    REQUIRES esphome
)
