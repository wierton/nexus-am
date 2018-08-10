OBJ_DIR = build
SRCS = $(shell find src/ -name "*.S")
OBJS = $(patsubst %.S, $(OBJ_DIR)/%.o, $(SRCS))
APP = $(OBJ_DIR)/insttest

CFLAGS := -D_KERNEL -fno-builtin -nostdinc -nostdlib
CFLAGS += -EL -O2 -mno-abicalls -g -Iinclude -I.

.DEAULT_GOAL = all

all: $(APP)

$(OBJ_DIR)/%.o: %.S
	@mkdir -p $(@D)
	@mips-linux-gnu-gcc $(CFLAGS) -c $< -o $@
	@echo + $<

$(APP): $(OBJ_DIR)/src/start.o $(filter-out $(OBJ_DIR)/src/start.o,$(OBJS))
	@echo + $@
	@mips-linux-gnu-ld --gc-sections -EL -T ./loader.ld -e _start -o $@ --start-group $^ --end-group
	@mips-linux-gnu-objcopy --set-section-flags .bss=alloc,contents -O binary $@ $@.bin
	@echo + $@.bin
	@mips-linux-gnu-objdump -D $@ > $@.S
	@echo + $@.S
	@truncate -s %8 $@.bin
	@hexdump -ve '2/ "%08x " "\n"' $@.bin | awk '{print $2$1}' > $@.bin.txt
	@echo + $@.bin.txt

clean:
	rm -rf $(OBJ_DIR)
