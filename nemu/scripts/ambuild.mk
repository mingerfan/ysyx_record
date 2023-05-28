MK_PATH = /home/xs/ysyx/ysyx-workbench/am-kernels/tests/cpu-tests
TEST_PATH = $(MK_PATH)/tests/
ISA = risv64-nemu
TEST_TAR = add

cpu-test-list:
	@ls $(TEST_PATH)

# include $(MK_PATH)/Makefile

ARCH=$(ISA)
ALL=${TEST_TAR}

# cpu-test: run
# 	bash $(MK_PATH)/test.sh $(TEST_TAR)