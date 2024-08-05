# SPDX-License-Identifier: Apache-2.0

IDF_PATH ?=
ESP_IDF_LIB ?=
LDSCRIPTS = \
	$(IDF_PATH)/components/bootloader/subproject/main/ld/esp32c3/bootloader.ld \
	$(IDF_PATH)/components/esp_rom/esp32c3/ld/esp32c3.rom.api.ld \
	$(IDF_PATH)/components/esp_rom/esp32c3/ld/esp32c3.rom.ld \
	$(IDF_PATH)/components/esp_rom/esp32c3/ld/esp32c3.rom.newlib.ld \
	$(IDF_PATH)/components/soc/esp32c3/ld/esp32c3.peripherals.ld

LIBHAL_SRC += hal/hal-esp-idf.c

obj/libhal.a: $(call objs,$(LIBHAL_SRC))

ARCH_FLAGS += -march=rv32imc_zicsr_zifencei 

CPPFLAGS += -DRISCV -DESP32C3

CFLAGS += $(ARCH_FLAGS)

# platform specific ldflags
LDFLAGS += \
	-lesp_rom -L$(ESP_IDF_LIB)/esp32c3/bootloader/esp-idf/esp_rom \
	-lsoc -L$(ESP_IDF_LIB)/esp32c3/bootloader/esp-idf/soc \
	-lesp_common -L$(ESP_IDF_LIB)/esp32c3/bootloader/esp-idf/esp_common \
	-lesp_hw_support -L$(ESP_IDF_LIB)/esp32c3/esp-idf/esp_hw_support \
	-lhal -L$(ESP_IDF_LIB)/esp32c3/esp-idf/hal \
	-lg_nano

LDFLAGS += \
	-nostdlib \
	-nostartfiles \
	$(addprefex -T,$(LDSCRIPTS)) \
	$(ARCH_FLAGS)
