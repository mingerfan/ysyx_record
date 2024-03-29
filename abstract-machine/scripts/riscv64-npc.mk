include $(AM_HOME)/scripts/isa/riscv64.mk

AM_SRCS := riscv/npc/start.S \
           riscv/npc/trm.c \
           riscv/npc/ioe/ioe.c \
		   riscv/npc/ioe/uart.c \
           riscv/npc/timer.c \
           riscv/npc/input.c \
           riscv/npc/cte.c \
           riscv/npc/trap.S \
           platform/dummy/vme.c \
           platform/dummy/mpe.c


CFLAGS    += -fdata-sections -ffunction-sections
LDFLAGS   += -T $(AM_HOME)/scripts/linker.ld --defsym=_pmem_start=0x80000000 --defsym=_entry_offset=0x0
LDFLAGS   += --gc-sections -e _start
CFLAGS += -DMAINARGS=\"$(mainargs)\"
.PHONY: $(AM_HOME)/am/src/riscv/npc/trm.c

image: $(IMAGE).elf
	@$(OBJDUMP) -d $(IMAGE).elf > $(IMAGE).txt
	@echo + OBJCOPY "->" $(IMAGE_REL).bin
	@$(OBJCOPY) -S --set-section-flags .bss=alloc,contents -O binary $(IMAGE).elf $(IMAGE).bin


IMG_BIN = $(IMAGE).bin
IMG_ELF = $(IMAGE).elf
ARGS := --diff=$(NEMU_HOME)/build/riscv64-nemu-interpreter-so
ARGS_B := "-b --diff=$(NEMU_HOME)/build/riscv64-nemu-interpreter-so"

run: image
	$(MAKE) -C $(NPC_HOME) sim ARGS=$(ARGS) IMG_BIN=$(IMG_BIN) IMG_ELF=$(IMG_ELF)

brun: image
	$(MAKE) -C $(NPC_HOME) sim -- ARGS=$(ARGS_B) IMG_BIN=$(IMG_BIN) IMG_ELF=$(IMG_ELF)

gdb: image
	$(MAKE) -C $(NPC_HOME) gdb ARGS=$(ARGS) IMG_BIN=$(IMG_BIN) IMG_ELF=$(IMG_ELF)

.PHONY: run