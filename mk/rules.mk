MLKEM_SOURCES = $(wildcard mlkem/*.c)
FIPS202_SOURCES = $(wildcard fips202/*.c)

OBJS = $(call objs,$(addprefix $(1)/,$(notdir $(basename $(MLKEM_SOURCES)))) $(basename $(FIPS202_SOURCES)))

KEM_SCHEMES = mlkem512 mlkem768 mlkem1024

define scheme-elf
elf/$(1)-test.elf: obj/test/$(1)-test.o $(call OBJS,$(1)) $(LINKDEPS)
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

define scheme-test
obj/test/$(1)-test.o: test/test.c
	$$(compile-obj)
endef

$(foreach scheme,$(KEM_SCHEMES), \
	$(eval $(call scheme-elf,$(scheme))))

$(foreach scheme,$(KEM_SCHEMES), \
	$(eval $(call scheme-obj,$(scheme))))

$(foreach scheme,$(KEM_SCHEMES), \
	$(eval $(call scheme-test,$(scheme))))

elf/%.elf:
	@echo "  LD      $@"
	$(Q)[ -d $(@D) ] || mkdir -p $(@D)
	$(Q)$(LD) $(LDFLAGS) -o $@ $(filter %.o,$^) -Wl,--start-group $(LDLIBS) -Wl,--end-group

obj/%.a:
	@echo "  AR      $@"
	$(Q)[ -d $(@D) ] || mkdir -p $(@D)
	$(Q)$(AR) rcs $@ $(filter %.o,$^)

bin/%.bin: elf/%.elf
	@echo "  OBJCOPY $@"
	$(Q)[ -d $(@D) ] || mkdir -p $(@D)
	$(Q)$(OBJCOPY) -Obinary $< $@

bin/%.hex: elf/%.elf
	@echo "  OBJCOPY $@"
	$(Q)[ -d $(@D) ] || mkdir -p $(@D)
	$(Q)$(OBJCOPY) -Oihex $< $@

obj/%.o: %.c
	@echo "  CC      $@"
	$(Q)[ -d $(@D) ] || mkdir -p $(@D)
	$(Q)$(CC) -c -o $@ $(CFLAGS) $<

obj/%.c.o: %.c $(CONFIG)
	@echo "  CC      $@"
	$(Q)[ -d $(@D) ] || mkdir -p $(@D)
	$(Q)$(CC) -c -o $@ $(CFLAGS) $<
