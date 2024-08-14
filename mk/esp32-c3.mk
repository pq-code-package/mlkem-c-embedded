# SPDX-License-Identifier: Apache-2.0
CROSS_PREFIX := riscv32-esp-elf-
include mk/gcc-config.mk

IDF_PATH ?=
ESP_IDF_LIB ?=

LDSCRIPTS = \
	hal/esp32-c3.ld \
	$(IDF_PATH)/components/esp_rom/esp32c3/ld/esp32c3.rom.api.ld \
	$(IDF_PATH)/components/esp_rom/esp32c3/ld/esp32c3.rom.ld \
	$(IDF_PATH)/components/esp_rom/esp32c3/ld/esp32c3.rom.newlib.ld \
	$(IDF_PATH)/components/soc/esp32c3/ld/esp32c3.peripherals.ld

ifeq ($(RNG),NISTKAT)
	LIBHAL_SRC = \
		test/common/nistkatrng.c \
		test/common/aes.c
	CPPFLAGS += -Itest/common
else
	RNG := NOTRAND
	LIBHAL_SRC = hal/notrandombytes.c
endif

LIBHAL_SRC += hal/hal-esp-idf.c hal/esp32c3_start.S

$(OBJ_DIR)/hal/libpqcphal.a: $(call objs,$(LIBHAL_SRC))

ARCH_FLAGS += -march=rv32imc_zicsr_zifencei 

CPPFLAGS += -DRISCV -DESP32C3

CFLAGS += \
	-I$(IDF_PATH)/components/soc/include \
	-I$(IDF_PATH)/components/soc/esp32c3/include \
	-I$(IDF_PATH)/components/esp_rom/include \
	-I$(IDF_PATH)/components/hal/include \
	-isystem$(IDF_PATH)/components/hal/esp32c3/include \
	-I$(IDF_PATH)/components/hal/platform_port/include \
	-I$(IDF_PATH)/components/esp_common/include \
	-I$(IDF_PATH)/components/esp_rom/include/esp32c3 \
	-I$(IDF_PATH)/components/esp_hw_support/include \
	-I$(IDF_PATH)/components/riscv/include \
	-I$(ESP_IDF_LIB)/esp32c3/bootloader/config \
	$(ARCH_FLAGS) \
	-fdata-sections -ffunction-sections \
	-nostartfiles -Og

# platform specific ldflags
LDLIBS += \
	$(addprefix -T,$(LDSCRIPTS)) \
	-lesp_rom -L$(ESP_IDF_LIB)/esp32c3/bootloader/esp-idf/esp_rom \
	-lsoc -L$(ESP_IDF_LIB)/esp32c3/bootloader/esp-idf/soc \
	-lesp_common -L$(ESP_IDF_LIB)/esp32c3/bootloader/esp-idf/esp_common \
	-lesp_hw_support -L$(ESP_IDF_LIB)/esp32c3/esp-idf/esp_hw_support \
	-lhal -L$(ESP_IDF_LIB)/esp32c3/esp-idf/hal \
	-lg_nano

LDFLAGS += \
	-nostdlib \
	-nostartfiles \
	-static \
	$(ARCH_FLAGS)
