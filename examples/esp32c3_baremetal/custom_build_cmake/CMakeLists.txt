set(CMAKE_TOOLCHAIN_FILE riscv-toolchain.cmake)
set(TARGET esp32c3)
set(PORT /dev/ttyACM0)
set(BAUD_RATE 115200)

cmake_minimum_required(VERSION 3.16)
project(hello_esp32 LANGUAGES C)

# ---------------------------------------------------
# ----------------- ELF file target -----------------
# ---------------------------------------------------
add_executable(${PROJECT_NAME} main.c)

target_include_directories(${PROJECT_NAME} PRIVATE 
    .
    ../custom_init/${TARGET}
    $ENV{IDF_PATH}/components/esp_common/include
    $ENV{IDF_PATH}/components/esp_rom/include
    $ENV{IDF_PATH}/components/soc/include
    $ENV{IDF_PATH}/components/soc/${TARGET}/include
    $ENV{IDF_PATH}/components/hal/include
    $ENV{IDF_PATH}/components/hal/${TARGET}/include
    $ENV{IDF_PATH}/components/hal/platform_port/include
)

target_compile_options(${PROJECT_NAME} PRIVATE
    -march=rv32imc
    -mabi=ilp32
)

target_link_options(${PROJECT_NAME} PRIVATE 
    -nostdlib
    -nostartfiles
    -T ${CMAKE_SOURCE_DIR}/linker.ld
)

# ---------------------------------------------------
# ---------------- Image file target ----------------
# ---------------------------------------------------
add_custom_target(
    generate_image ALL
    COMMAND ${COMMAND} esptool.py --chip ${TARGET} elf2image ${CMAKE_BINARY_DIR}/${PROJECT_NAME}
    DEPENDS ${PROJECT_NAME} 
    COMMENT "Generating image file"
) 

# ---------------------------------------------------
# ------------------- Flash target ------------------
# ---------------------------------------------------
add_custom_target(
    flash_image ALL
    COMMAND ${COMMAND} esptool.py 
                            --chip ${TARGET} 
                            --port ${PORT} 
                        write_flash
		                    --flash_mode dio
		                    --flash_freq 80m
		                    --flash_size 2MB
		                    0x0 ${CMAKE_BINARY_DIR}/${PROJECT_NAME}.bin 
    DEPENDS generate_image
    COMMENT "Flash image file"
) 

# ---------------------------------------------------
# ------------------ Monitor target -----------------
# ---------------------------------------------------
add_custom_target(
    monitor ALL
    COMMAND ${COMMAND} pyserial-miniterm ${PORT} ${BAUD_RATE}
    DEPENDS flash_image 
    COMMENT "Monitor serial port"
) 
