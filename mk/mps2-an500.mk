# SPDX-License-Identifier: Apache-2.0
ARCH_FLAGS += -mcpu=cortex-m7 -mthumb -mfloat-abi=hard -mfpu=fpv5-d16

CPPFLAGS += -DMPS2_AN500

MBED_OS_TARGET_DIR = $(MBED_OS_DIR)/TARGET_MPS2_M7/device

include mk/mbed-os.mk
