NXDC_FILES = $(NPC_HOME)/playground/verilator_test/deps/top.nxdc
INC_PATH += $(PLAYGROUND_DIR)/include

VBIN = $(OBJ_DIR)/$(TOPNAME)
VGEN = $(BUILD_DIR)/$(TOPNAME)_splited/$(TOPNAME).sv

# SRC_AUTO_BIND = $(abspath $(OBJ_DIR)/auto_bind.cpp)

$(shell mkdir -p $(BUILD_DIR))
$(shell mkdir -p $(BUILD_DIR)/$(TOPNAME)_splited/)

SRC_AUTO_BIND = $(abspath $(BUILD_DIR)/auto_bind.cpp)
$(SRC_AUTO_BIND): $(NXDC_FILES)
	python3 $(NVBOARD_HOME)/scripts/auto_pin_bind.py $^ $@

CISSRCS = $(shell find $(abspath $(PLAYGROUND_DIR)/src/) -name "*.scala")

VSRCS = $(shell find $(abspath $(BUILD_DIR)/$(TOPNAME)_splited/) -name "*.sv" ! -name "$(TOPNAME).sv")
VSRCS += $(shell find $(abspath $(PLAYGROUND_DIR)/vsrc/) -name "*.v" -or -name "*.sv" -or -name "*.vlt")
VSRC_TOP = $(BUILD_DIR)/$(TOPNAME)_splited/$(TOPNAME).sv
CSRCS = $(shell find $(abspath ./playground/verilator_test/csrc/) -name "*.c" -or -name "*.cc" -or -name "*.cpp")
CSRCS += $(if $(CONFIG_NVBOARD_ON), $(SRC_AUTO_BIND))

include $(NVBOARD_HOME)/scripts/nvboard.mk

# add rustlib support
RUST_CODE_PATH = $(NEMU_HOME)/../submodule/rustlib
INC_PATH += $(RUST_CODE_PATH)/c_include
RUST_LIB_PATH = $(RUST_CODE_PATH)/target/release
RUST_LIB = $(RUST_LIB_PATH)/libc_rustlib.a
RUST_LIB_BARE = c_rustlib

LDFLAGS += -L$(RUST_LIB_PATH) -l$(RUST_LIB_BARE)

$(RUST_LIB): 
	@cd $(RUST_CODE_PATH) && cargo build


INCFLAGS = $(addprefix -I, $(INC_PATH))
CFLAGS += $(INCFLAGS) 
LDFLAGS +=  -lSDL2 -lSDL2_image

CFLAGS += -Og -ggdb3

SIM_FLAGS += $(IMG_BIN)

default: all

all: $(VBIN)


VBIN_DP = $(VGEN) $(VSRCS) $(CSRCS) $(NVBOARD_ARCHIVE)

$(VBIN): $(VBIN_DP) $(NPC_HOME)/.config
	@rm -rf $(LOGS_DIR) $(OBJ_DIR)
	$(VERILATOR) $(VERILATOR_FLAGS) \
		--top-module $(TOPNAME) $(VBIN_DP) \
		$(addprefix -CFLAGS , $(CFLAGS)) $(addprefix -LDFLAGS , $(LDFLAGS)) \
		--Mdir $(OBJ_DIR) --exe -o $(abspath $(VBIN))

	@mkdir -p $(LOGS_DIR)

export PATH := $(PATH):$(abspath ./utils)

test:
	mill -i __.test

# mill playground.tools.run tools.precompiler
$(VGEN): $(CISSRCS)
	mkdir -p $(BUILD_DIR)
	mill -i __.test.runMain Elaborate -td $(BUILD_DIR)/$(TOPNAME)_splited/ --split-verilog --target verilog
	$(call git_commit, "generate verilog")

verilog: $(VGEN)

help:
	mill -i __.test.runMain Elaborate --help

compile:
	mill -i __.compile

bsp:
	mill -i mill.bsp.BSP/install

reformat:
	mill -i __.reformat

checkformat:
	mill -i __.checkFormat

clean:
	-rm -rf $(BUILD_DIR)

.PHONY: default all test verilog split help compile bsp reformat checkformat clean

sim: $(VBIN)
	$(VBIN) $(ARGS) $(SIM_FLAGS)
	$(call git_commit, "sim RTL") # DO NOT REMOVE THIS LINE!!!

gdb: $(VBIN)
	gdb -s $(VBIN) --args $(VBIN) $(ARGS) $(SIM_FLAGS)

include ../Makefile
