# SPDX-License-Identifier: Apache-2.0
MLKEM_SOURCES = $(wildcard mlkem/*.c)
FIPS202_SOURCES = $(wildcard fips202/*.c)

OBJS = $(call objs,$(addprefix $(1)/,$(notdir $(basename $(MLKEM_SOURCES)))) $(basename $(FIPS202_SOURCES)))

# all tests x all schemes
SCHEMES_TESTS = $(foreach t,$(TESTS),$(KEM_SCHEMES:%=%-$(t)))

# set up tests dependencies for each scheme
define SCHEME_DEPS
$(TESTS:%=$(1)-%): CPPFLAGS += -DMLKEM_K=$(2) -DNTESTS=$(NTESTS)
endef

define SCHEME_ELF_DEPS
$(TESTS:%=elf/$(1)-%.elf): elf/%.elf: obj/test/%.o $(call OBJS,$(1)) $(LINKDEPS)
endef

define COMPILE_OBJ
	@echo "  CC      $@"
	$(Q)[ -d $(@D) ] || mkdir -p $(@D)
	$(Q)$(CC) -c -o $@ $(CFLAGS) $<
endef

# compile object file for each scheme
define SCHEME_OBJ
obj/$(1)/%.o: mlkem/%.c $(CONFIG)
	$$(COMPILE_OBJ)
endef

define SCHEME_TEST_OBJS
obj/test/$(1)-%.o: test/%.c $(CONFIG)
	$$(COMPILE_OBJ)
endef

$(SCHEMES_TESTS): % : bin/%.bin bin/%.hex

$(eval $(call SCHEME_DEPS,mlkem512,2))
$(eval $(call SCHEME_DEPS,mlkem768,3))
$(eval $(call SCHEME_DEPS,mlkem1024,4))

$(foreach scheme,$(KEM_SCHEMES), \
	$(eval $(call SCHEME_ELF_DEPS,$(scheme))))

$(foreach scheme,$(KEM_SCHEMES), \
	$(eval $(call SCHEME_TEST_OBJS,$(scheme))))

$(foreach scheme,$(KEM_SCHEMES), \
	$(eval $(call SCHEME_OBJ,$(scheme))))
