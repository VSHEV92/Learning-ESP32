if(CONFIG_GENERAL_PERPOSE_TIMER_LL_API)
    set(src general_perpose_timer_ll.c)
else()
    set(src general_perpose_timer_hal.c ${IDF_PATH}/components/hal/timer_hal.c)
endif()

idf_build_get_property(target IDF_TARGET)

idf_component_register(
    SRCS ${src}
    INCLUDE_DIRS ../../../custom_init/${target}
    REQUIRES hal
)

set(scripts "${IDF_PATH}/components/bootloader/subproject/main/ld/${target}/bootloader.ld"
            "${IDF_PATH}/components/bootloader/subproject/main/ld/${target}/bootloader.rom.ld")

target_linker_script(${COMPONENT_LIB} INTERFACE "${scripts}")
