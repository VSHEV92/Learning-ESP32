# project settings
PROJECT_NAME := hello_esp32
TARGET := esp32c3

# esptool options
ESPTOOL_TARGET := ESP32-C3
ESPTOOL_PORT := /dev/ttyACM0
ESPTOOL_BAUDRATE := 115200

EXECUTABLE_NAME := ${PROJECT_NAME}.elf
BINARY_NAME := ${PROJECT_NAME}.bin

# toolchain options
TOOLCHAIN_PREFIX := riscv32-esp-elf-
CC := ${TOOLCHAIN_PREFIX}gcc

# source files
SRC := main.c

# include directories
INC_DIRS := .
INC_DIRS += ../custom_init/${TARGET}

INC_DIRS += ${IDF_PATH}/components/esp_common/include

INC_DIRS += ${IDF_PATH}/components/esp_rom/include

INC_DIRS += ${IDF_PATH}/components/soc/include
INC_DIRS += ${IDF_PATH}/components/soc/${TARGET}/include

INC_DIRS += ${IDF_PATH}/components/hal/include
INC_DIRS += ${IDF_PATH}/components/hal/${TARGET}/include
INC_DIRS += ${IDF_PATH}/components/hal/platform_port/include

# compiler options
CFLAGS := -march=rv32imc
CFLAGS += -mabi=ilp32
CFLAGS += -nostdlib
CFLAGS += -nostartfiles

# linker options
LD_SCRIPTS := linker.ld

# add prefix to include directories and linker scripts
INC_DIRS := $(addprefix -I, $(INC_DIRS))
LD_SCRIPTS := $(addprefix -T, $(LD_SCRIPTS))

all: clean flash monitor

${EXECUTABLE_NAME}:
	${CC} ${CFLAGS} ${LD_SCRIPTS} ${INC_DIRS} ${SRC} -o ${EXECUTABLE_NAME}

${BINARY_NAME}: ${EXECUTABLE_NAME}
	esptool.py --chip ${TARGET} elf2image ${EXECUTABLE_NAME}

flash: ${BINARY_NAME}
	esptool.py --chip ${TARGET} --port ${ESPTOOL_PORT} write_flash \
		--flash_mode dio \
		--flash_freq 80m \
		--flash_size 2MB \
		0x0 ${BINARY_NAME} 

monitor:
	pyserial-miniterm ${ESPTOOL_PORT} ${ESPTOOL_BAUDRATE}

clean:
	rm -rf ${EXECUTABLE_NAME}
	rm -rf ${BINARY_NAME}

.PHONY: all flash monitor clean
