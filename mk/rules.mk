# SPDX-License-Identifier: Apache-2.0
$(BIN_DIR)/%.elf: $(CONFIG)
	@echo "  LD      $@"
	$(Q)[ -d $(@D) ] || mkdir -p $(@D)
	$(Q)$(LD) $(LDFLAGS) -o $@ $(filter %.o,$^) -Wl,--start-group $(LDLIBS) -Wl,--end-group

$(OBJ_DIR)/%.a: $(CONFIG)
	@echo "  AR      $@"
	$(Q)[ -d $(@D) ] || mkdir -p $(@D)
	$(Q)rm -f $@
	$(Q)$(AR) rcs $@ $(filter %.o,$^)

$(BIN_DIR)/%.bin: $(BIN_DIR)/%.elf $(CONFIG)
	@echo "  OBJCOPY $@"
	$(Q)[ -d $(@D) ] || mkdir -p $(@D)
	$(Q)$(OBJCOPY) -Obinary $< $@

$(BIN_DIR)/%.hex: $(BIN_DIR)/%.elf $(CONFIG)
	@echo "  OBJCOPY $@"
	$(Q)[ -d $(@D) ] || mkdir -p $(@D)
	$(Q)$(OBJCOPY) -Oihex $< $@

$(OBJ_DIR)/%.o: %.c $(CONFIG)
	@echo "  CC      $@"
	$(Q)[ -d $(@D) ] || mkdir -p $(@D)
	$(Q)$(CC) -c -o $@ $(CFLAGS) $<

$(OBJ_DIR)/%.c.o: %.c $(CONFIG)
	@echo "  CC      $@"
	$(Q)[ -d $(@D) ] || mkdir -p $(@D)
	$(Q)$(CC) -c -o $@ $(CFLAGS) $<

$(OBJ_DIR)/%.S.o: %.S $(CONFIG)
	@echo "  AS      $@"
	$(Q)[ -d $(@D) ] || mkdir -p $(@D)
	$(Q)$(CC) -c -o $@ $(CFLAGS) $<
