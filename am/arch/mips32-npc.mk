include $(AM_HOME)/am/arch/isa/mips32.mk

AM_SRCS := $(ISA)/npc/trm.c \
           nemu-devices/ioe.c \
           $(ISA)/nemu/trap.S \
           $(ISA)/nemu/cte.c \
           $(ISA)/nemu/vme.c \
           nemu-devices/nemu-timer.c \
           $(ISA)/npc/devices/keyboard.c \
           nemu-devices/nemu-video.c \
           $(ISA)/nemu/boot/start.S

LD_SCRIPT := $(AM_HOME)/am/src/$(ISA)/npc/boot/loader.ld

image:
	@echo + LD "->" $(BINARY_REL).elf
	@$(LD) $(LDFLAGS) --gc-sections -T $(LD_SCRIPT) -e _start -o $(BINARY).elf $(LINK_FILES)
	@$(OBJDUMP) -d $(BINARY).elf > $(BINARY).txt
	@echo + OBJCOPY "->" $(BINARY_REL).bin
	@$(OBJCOPY) -S --set-section-flags .bss=alloc,contents -O binary $(BINARY).elf $(BINARY).bin

run:
	make -C $(MIPS32_NEMU_HOME) ISA=$(ISA) run ARGS="-b -e $(BINARY).elf"

gdb: image
	make -C $(MIPS32_NEMU_HOME) ISA=$(ISA) gdb ARGS="-b -e $(BINARY).elf"
