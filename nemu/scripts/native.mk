#***************************************************************************************
# Copyright (c) 2014-2022 Zihao Yu, Nanjing University
#
# NEMU is licensed under Mulan PSL v2.
# You can use this software according to the terms and conditions of the Mulan PSL v2.
# You may obtain a copy of Mulan PSL v2 at:
#          http://license.coscl.org.cn/MulanPSL2
#
# THIS SOFTWARE IS PROVIDED ON AN "AS IS" BASIS, WITHOUT WARRANTIES OF ANY KIND,
# EITHER EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO NON-INFRINGEMENT,
# MERCHANTABILITY OR FIT FOR A PARTICULAR PURPOSE.
#
# See the Mulan PSL v2 for more details.
#**************************************************************************************/

-include $(NEMU_HOME)/../Makefile
include $(NEMU_HOME)/scripts/build.mk

include $(NEMU_HOME)/tools/difftest.mk

compile_git:
	$(call git_commit, "compile NEMU")
$(BINARY): compile_git

# Some convenient rules

override ARGS ?= --log=$(BUILD_DIR)/nemu-log.txt
override ARGS += $(ARGS_DIFF)

# Command to execute NEMU
IMG ?=
NEMU_EXEC := $(BINARY) $(ARGS) $(IMG)

run-env: $(BINARY) $(DIFF_REF_SO)

run: run-env
	$(call git_commit, "run NEMU")
	$(NEMU_EXEC)

gdb: run-env
	$(call git_commit, "gdb NEMU")
	gdb -s $(BINARY) --args $(NEMU_EXEC)

clean-tools = $(dir $(shell find ./tools -maxdepth 2 -mindepth 2 -name "Makefile"))
$(clean-tools):
	-@$(MAKE) -s -C $@ clean
clean-tools: $(clean-tools)
clean-all: clean distclean clean-tools

AM_TEST_PATH = $(AM_HOME)/../am-kernels/tests

rtest: 
	@$(MAKE) -C $(AM_TEST_PATH)/cpu-tests/ ARCH=riscv64-nemu run
	@$(MAKE) -C $(AM_TEST_PATH)/klib-tests/ ARCH=riscv64-nemu run
	@timeout 6 $(MAKE) -C $(AM_TEST_PATH)/am-tests/ ARCH=riscv64-nemu mainargs=v run || echo "\n*******video test timed out*******\n"
	@timeout 8 $(MAKE) -C $(AM_TEST_PATH)/am-tests/ ARCH=riscv64-nemu mainargs=i run || echo "\n*******intrp test timed out*******\n"
	@timeout 6 $(MAKE) -C $(AM_TEST_PATH)/am-tests/ ARCH=riscv64-nemu mainargs=t run || echo "\n*******rtc   test timed out*******\n"

.PHONY: run gdb run-env clean-tools clean-all $(clean-tools) rtest
