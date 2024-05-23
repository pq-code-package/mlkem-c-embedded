# SPDX-License-Identifier: Apache-2.0
ARCH_FLAGS += -mcpu=cortex-m3 -mthumb -mfloat-abi=soft

CPPFLAGS += -DMPS2 -DMPS2_AN385

MBED_OS_TARGET_DIR = $(MBED_OS_DIR)/TARGET_MPS2_M3/device

include mk/mbed-os.mk
