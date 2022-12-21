VERILATOR = verilator
TOPNAME = top
NXDC_FILES = playground/verilator_test/deps/top.nxdc
BUILD_DIR = ./build

INC_PATH ?=

# So Verilator has stronger lint warnings enabled
VERILATOR_FLAGS += -Wall

# --cc to get C++ output (versus e.g. SystemC or only linting).
# --exe, along with our sim_main.cpp wrapper file, so the build will create an executable instead of only a
# library.
VERILATOR_FLAGS += --cc --exe -O3

# so Verilator will call make itself. This is we don't need to manually call make as a separate step.
VERILATOR_FLAGS += --build

VERILATOR_FLAGS += --trace

OBJ_DIR = $(BUILD_DIR)/v_obj_dir

LOGS_DIR = ./out/v_logs

VBIN = $(OBJ_DIR)/$(TOPNAME)
VGEN = $(BUILD_DIR)/$(TOPNAME).v

SRC_AUTO_BIND = $(abspath $(OBJ_DIR)/auto_bind.cpp)

$(shell mkdir -p $(BUILD_DIR))
$(shell mkdir -p $(BUILD_DIR)/$(TOPNAME)_splited/)

SRC_AUTO_BIND = $(abspath $(BUILD_DIR)/auto_bind.cpp)
$(SRC_AUTO_BIND): $(NXDC_FILES)
	python3 $(NVBOARD_HOME)/scripts/auto_pin_bind.py $^ $@

VSRCS = $(shell find $(abspath $(BUILD_DIR)/$(TOPNAME)_splited/) -name "*.v" ! -name "top.v")
VSRC_TOP = $(BUILD_DIR)/$(TOPNAME)_splited/top.v
CSRCS = $(shell find $(abspath ./playground/verilator_test/csrc/) -name "*.c" -or -name "*.cc" -or -name "*.cpp")
CSRCS += $(SRC_AUTO_BIND)

include $(NVBOARD_HOME)/scripts/nvboard.mk

INCFLAGS = $(addprefix -I, $(INC_PATH))
CFLAGS += $(INCFLAGS) -D TOP_NAME="\"V$(TOPNAME)\""
LDFLAGS +=  -lSDL2 -lSDL2_image

default: all

all: $(VBIN)

$(VSRC_TOP): split
	echo $(VSRC_TOP)

VBIN_DP = $(VSRC_TOP) $(VSRCS) $(CSRCS) $(NVBOARD_ARCHIVE)

$(VBIN): $(VBIN_DP)
	@rm -rf $(LOGS_DIR) $(OBJ_DIR)
	$(VERILATOR) $(VERILATOR_FLAGS) \
		--top-module $(TOPNAME) $(VBIN_DP) \
		$(addprefix -CFLAGS , $(CFLAGS)) $(addprefix -LDFLAGS , $(LDFLAGS)) \
		--Mdir $(OBJ_DIR) --exe -o $(abspath $(VBIN))

	@mkdir -p $(LOGS_DIR)

export PATH := $(PATH):$(abspath ./utils)

test:
	mill -i __.test

$(VGEN):
	$(call git_commit, "generate verilog")
	mkdir -p $(BUILD_DIR)
	mill -i __.test.runMain Elaborate -td $(BUILD_DIR)

verilog: $(VGEN)
	
split: $(VGEN)
	python3 split.py

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
	@echo "Write this Makefile by yourself."
	$(VBIN)
	$(call git_commit, "sim RTL") # DO NOT REMOVE THIS LINE!!!

include ../Makefile
