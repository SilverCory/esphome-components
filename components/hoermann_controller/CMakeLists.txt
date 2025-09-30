idf_component_register(
    SRCS
        "hoermann_controller.cpp"
        "cover/hoermann_cover.cpp"
        "switch/hoermann_switch.cpp"
        "button/hoermann_button.cpp"
    INCLUDE_DIRS "." "cover" "switch" "button"
    REQUIRES esphome
)
