# SPDX-License-Identifier: Apache-2.0
# GCC config
CROSS_PREFIX ?=
CC := $(CROSS_PREFIX)gcc
CPP := $(CROSS_PREFIX)cpp
AR := $(CROSS_PREFIX)ar
LD := $(CC)
OBJCOPY := $(CROSS_PREFIX)objcopy
SIZE := $(CROSS_PREFIX)size
