# SPDX-License-Identifier: Apache-2.0
DEVICE=stm32f407vg
OPENCM3_TARGET=lib/stm32/f4
override DEVICES_DATA := hal/devices.data

include mk/opencm3.mk
