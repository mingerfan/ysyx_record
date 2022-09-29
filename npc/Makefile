VERILATOR = verilator

TOPNAME = top
NXDC_FILES = deps/constr/top.nxdc
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

BUILD_DIR = ./build
OBJ_DIR = $(BUILD_DIR)/obj_dir
LOGS_DIR = logs
VBIN = $(BUILD_DIR)/$(TOPNAME)

default: all

$(shell mkdir -p $(BUILD_DIR))

SRC_AUTO_BIND = $(abspath $(BUILD_DIR)/auto_bind.cpp)
$(SRC_AUTO_BIND): $(NXDC_FILES)
	python3 $(NVBOARD_HOME)/scripts/auto_pin_bind.py $^ $@


VSRCS = $(shell find $(abspath ./vsrc) -name "*.v")
CSRCS = $(shell find $(abspath ./csrc) -name "*.c" -or -name "*.cc" -or -name "*.cpp")
CSRCS += $(SRC_AUTO_BIND)

include $(NVBOARD_HOME)/scripts/nvboard.mk

INCFLAGS = $(addprefix -I, $(INC_PATH))
CFLAGS += $(INCFLAGS) -D TOP_NAME="\"V$(TOPNAME)\""
LDFLAGS +=  -lSDL2 -lSDL2_image

all: $(VBIN)

$(VBIN): $(VSRCS) $(CSRCS) $(NVBOARD_ARCHIVE)
	@rm -rf $(LOGS_DIR) $(OBJ_DIR)
	$(VERILATOR) $(VERILATOR_FLAGS) \
		--top-module $(TOPNAME) $^ \
		$(addprefix -CFLAGS , $(CFLAGS)) $(addprefix -LDFLAGS , $(LDFLAGS)) \
		--Mdir $(OBJ_DIR) --exe -o $(abspath $(VBIN))

	@mkdir -p $(LOGS_DIR)

sim: $(VBIN)
	$(call git_commit, "sim RTL") # DO NOT REMOVE THIS LINE!!!
	$(VBIN)

clean:
	rm -rf $(BUILD_DIR) $(LOGS_DIR)

include ../Makefile

.PHONY: default all sim clean