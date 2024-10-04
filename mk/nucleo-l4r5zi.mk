# SPDX-License-Identifier: Apache-2.0 or CC0-1.0
DEVICE=stm32l4r5zi
OPENCM3_TARGET=lib/stm32/l4
override DEVICES_DATA := hal/devices.data

include mk/opencm3.mk
