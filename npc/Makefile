VERILATOR = verilator

# So Verilator has stronger lint warnings enabled
VERILATOR_FLAGS += -Wall

# --cc to get C++ output (versus e.g. SystemC or only linting).
# --exe, along with our sim_main.cpp wrapper file, so the build will create an executable instead of only a
# library.
VERILATOR_FLAGS += --cc --exe

# so Verilator will call make itself. This is we don’t need to manually call make as a separate step.
VERILATOR_FLAGS += --build

VERILATOR_FLAGS += --trace

VERILATOR_INPUT = vsrc/*.v csrc/*.cpp
all:
	$(VERILATOR) $(VERILATOR_FLAGS) $(VERILATOR_INPUT)
	rm -rf logs
	mkdir -p logs

sim:
	$(call git_commit, "sim RTL") # DO NOT REMOVE THIS LINE!!!
	obj_dir/Vtop

include ../Makefile
