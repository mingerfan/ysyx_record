VERILATOR = verilator
TOPNAME = top

OBJ_DIR = $(BUILD_DIR)/v_obj_dir
LOGS_DIR = $(NPC_HOME)/build/logs
BUILD_DIR = $(NPC_HOME)/build
SCRIPTS_DIR = $(NPC_HOME)/scripts
PLAYGROUND_DIR = $(NPC_HOME)/playground
CONFIG_DIR = $(PLAYGROUND_DIR)/config

include $(PLAYGROUND_DIR)/include/config/auto.conf

# So Verilator has stronger lint warnings enabled
VERILATOR_FLAGS += -Wall

# --cc to get C++ output (versus e.g. SystemC or only linting).
# --exe, along with our sim_main.cpp wrapper file, so the build will create an executable instead of only a
# library.
VERILATOR_FLAGS += --cc --exe -O3

# so Verilator will call make itself. This is we don't need to manually call make as a separate step.
VERILATOR_FLAGS += --build

VERILATOR_FLAGS += $(if $(CONFIG_NPC_WAVE), --trace)

VERILATOR_FLAGS += -Wno-UNUSED

# disable warings in alu.v
VERILATOR_FLAGS += lint_off -rule WIDTH -file "*/ALU.v"

CFLAGS += -DTOP_NAME="\"V$(TOPNAME)\""
# CFLAGS += -DUSE_NVBOARD=$(USE_NVBOARD)
CFLAGS += $(shell llvm-config --cxxflags) -fPIE

LDFLAGS += -lreadline
LDFLAGS += $(shell llvm-config --libs)
LDFLAGS += /home/xs/ysyx/ysyx-workbench/nemu/build/riscv64-nemu-interpreter-so

SIM_FLAGS += --lpath=$(LOGS_DIR)
SIM_FLAGS += $(if $(IMG_ELF), --ftrace=$(IMG_ELF)) 

include $(SCRIPTS_DIR)/Makefile
include $(SCRIPTS_DIR)/config.mk