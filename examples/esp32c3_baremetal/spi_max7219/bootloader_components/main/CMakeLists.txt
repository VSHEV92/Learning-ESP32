
idf_build_get_property(target IDF_TARGET)

idf_component_register(
    SRCS 
        main.c 
        max7219.c
    INCLUDE_DIRS 
        . 
        ../../../custom_init/${target}
    REQUIRES 
        hal
)

set(scripts "${IDF_PATH}/components/bootloader/subproject/main/ld/${target}/bootloader.ld"
            "${IDF_PATH}/components/bootloader/subproject/main/ld/${target}/bootloader.rom.ld")

target_linker_script(${COMPONENT_LIB} INTERFACE "${scripts}")
