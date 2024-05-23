# SPDX-License-Identifier: Apache-2.0
ARCH_FLAGS += -mcpu=cortex-m4 -mthumb -mfloat-abi=hard -mfpu=fpv4-sp-d16

CPPFLAGS += -DMPS2 -DMPS2_AN386

MBED_OS_TARGET_DIR = $(MBED_OS_DIR)/TARGET_MPS2_M4/device

include mk/mbed-os.mk
