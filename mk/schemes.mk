# SPDX-License-Identifier: Apache-2.0
MLKEM_SOURCES = $(wildcard mlkem/*.c)
FIPS202_SOURCES = $(wildcard fips202/*.c)

OBJS = $(call objs,$(addprefix $(1)/,$(notdir $(basename $(MLKEM_SOURCES)))) $(basename $(FIPS202_SOURCES)))

define scheme-test
$(1)-test: CPPFLAGS += -DKYBER_K=$(2) -DMUPQ_NAMESPACE=$$(MUPQ_NAMESPACE) -DNTESTS=$$(NTESTS)
$(1)-test: bin/$(1)-test.hex

$(1)-speed: CPPFLAGS += -DKYBER_K=$(2) -DMUPQ_NAMESPACE=$$(MUPQ_NAMESPACE) -DNTESTS=$$(NTESTS)
$(1)-speed: bin/$(1)-speed.hex

$(1)-stack: CPPFLAGS += -DKYBER_K=$(2) -DMUPQ_NAMESPACE=$$(MUPQ_NAMESPACE) -DMUPQ_ITERATIONS=$$(MUPQ_ITERATIONS)
$(1)-stack: bin/$(1)-stack.hex
endef

define scheme-elf
elf/$(1)-test.elf: obj/test/$(1)-test.o $(call OBJS,$(1)) $(LINKDEPS)
elf/$(1)-speed.elf: obj/test/$(1)-speed.o $(call OBJS,$(1)) $(LINKDEPS)
elf/$(1)-stack.elf: obj/test/$(1)-stack.o $(call OBJS,$(1)) $(LINKDEPS)
endef

define compile-obj
	@echo "  CC      $@"
	$(Q)[ -d $(@D) ] || mkdir -p $(@D)
	$(Q)$(CC) -c -o $@ $(CFLAGS) $<
endef

define scheme-obj
obj/$(1)/%.o: mlkem/%.c
	$$(compile-obj)
endef

define scheme-test-obj
obj/test/$(1)-test.o: test/test.c
	$$(compile-obj)

obj/test/$(1)-speed.o: test/speed.c
	$$(compile-obj)

obj/test/$(1)-stack.o: test/stack.c
	$$(compile-obj)
endef

# iterate over schemes and params to generate targets for functional test and speed benchmark
iter=$(let scheme k rest,$(1), \
	$(eval $(call scheme-test,$(scheme),$(k))) \
	$(if $(rest),$(call iter,$(rest)) ))
$(call iter,$(subst -, ,$(join $(addsuffix -,$(KEM_SCHEMES)),$(KEM_PARAMS))))

$(foreach scheme,$(KEM_SCHEMES), \
	$(eval $(call scheme-elf,$(scheme))))

$(foreach scheme,$(KEM_SCHEMES), \
	$(eval $(call scheme-obj,$(scheme))))

$(foreach scheme,$(KEM_SCHEMES), \
	$(eval $(call scheme-test-obj,$(scheme))))

