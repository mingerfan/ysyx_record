# Building
TARGET := riscv64imac-unknown-none-elf
MODE := release
KERNEL_ELF := target/$(TARGET)/$(MODE)/libos
KERNEL_ELF_ABS := $(realpath $(KERNEL_ELF))
KERNEL_BIN := $(KERNEL_ELF).bin
KERNEL_BIN_ABS := $(realpath $(KERNEL_BIN))
DISASM_TMP := target/$(TARGET)/$(MODE)/asm
LD_SCRIPT := src/linker.ld

ifeq ($(MODE), release)
	MODE_ARG := --release
endif

# Log
LOG_PATH := ./log
NEMU_LOG := $(LOG_PATH)/nemu-log.txt
NEMU_LOG_ABS := $(abspath $(NEMU_LOG))

# nemu
NEMUFLAGS += -l $(NEMU_LOG_ABS) -b

# KERNEL ENTRY
KERNEL_ENTRY_PA := 0x80000000

# Binutils
OBJDUMP := rust-objdump --arch-name=riscv64
OBJCOPY := rust-objcopy --binary-architecture=riscv64

# Disassembly
DISASM ?= -x

kernel: $(LD_SCRIPT)
	cargo build $(MODE_ARG)

build: $(KERNEL_BIN)

$(KERNEL_BIN): kernel
	@$(OBJCOPY) $(KERNEL_ELF) --strip-all -O binary $@

disasm: kernel
	@$(OBJDUMP) -d $(DISASM) $(KERNEL_ELF)

disasm-vim: kernel
	@$(OBJDUMP) -d $(DISASM) $(KERNEL_ELF) > $(DISASM_TMP)
	@vim $(DISASM_TMP)
	@rm $(DISASM_TMP)

run: build
	@mkdir -p $(LOG_PATH)
	$(MAKE) -C $(NEMU_HOME) ISA=$(ISA) run ARGS="$(NEMUFLAGS)" IMG=$(KERNEL_BIN_ABS)

.PHONY: kernel build disasm disasm-vim run