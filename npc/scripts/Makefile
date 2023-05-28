NXDC_FILES = $(NPC_HOME)/playground/verilator_test/deps/top.nxdc
INC_PATH += $(PLAYGROUND_DIR)/include

VBIN = $(OBJ_DIR)/$(TOPNAME)
VGEN = $(BUILD_DIR)/$(TOPNAME).v

# SRC_AUTO_BIND = $(abspath $(OBJ_DIR)/auto_bind.cpp)

$(shell mkdir -p $(BUILD_DIR))
$(shell mkdir -p $(BUILD_DIR)/$(TOPNAME)_splited/)

SRC_AUTO_BIND = $(abspath $(BUILD_DIR)/auto_bind.cpp)
$(SRC_AUTO_BIND): $(NXDC_FILES)
	python3 $(NVBOARD_HOME)/scripts/auto_pin_bind.py $^ $@

CISSRCS = $(shell find $(abspath $(PLAYGROUND_DIR)/src/) -name "*.scala")

VSRCS = $(shell find $(abspath $(BUILD_DIR)/$(TOPNAME)_splited/) -name "*.v" ! -name "$(TOPNAME).v")
VSRCS += $(shell find $(abspath $(PLAYGROUND_DIR)/vsrc/) -name "*.v" -or -name "*.sv")
VSRC_TOP = $(BUILD_DIR)/$(TOPNAME)_splited/$(TOPNAME).v
EX_FILE = $(BUILD_DIR)/$(TOPNAME)_splited/~generated
CSRCS = $(shell find $(abspath ./playground/verilator_test/csrc/) -name "*.c" -or -name "*.cc" -or -name "*.cpp")
CSRCS += $(if $(CONFIG_NVBOARD_ON), $(SRC_AUTO_BIND))

include $(NVBOARD_HOME)/scripts/nvboard.mk

INCFLAGS = $(addprefix -I, $(INC_PATH))
CFLAGS += $(INCFLAGS) 
LDFLAGS +=  -lSDL2 -lSDL2_image

SIM_FLAGS += $(IMG_BIN)

default: all

all: $(VBIN)

$(VSRC_TOP): split

VBIN_DP = $(VSRC_TOP) $(VSRCS) $(CSRCS) $(NVBOARD_ARCHIVE)

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
	$(call git_commit, "generate verilog")
	mkdir -p $(BUILD_DIR)
	mill -i __.test.runMain Elaborate -td $(BUILD_DIR)

verilog: $(VGEN)

$(EX_FILE): $(VGEN)
	python3 $(SCRIPTS_DIR)/split.py

split: $(EX_FILE)	

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

include ../Makefile
