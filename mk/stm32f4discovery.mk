# SPDX-License-Identifier: Apache-2.0
DEVICE=stm32f407vg
OPENCM3_TARGET=lib/stm32/f4
OPENCM3_DIR ?=
override LDSCRIPT := obj/generated.$(DEVICE).ld
override DEVICES_DATA := hal/devices.data

include $(OPENCM3_DIR)/mk/genlink-config.mk
include $(OPENCM3_DIR)/mk/gcc-config.mk
include $(OPENCM3_DIR)/mk/genlink-rules.mk
include $(OPENCM3_DIR)/mk/gcc-rules.mk
include mk/opencm3.mk
