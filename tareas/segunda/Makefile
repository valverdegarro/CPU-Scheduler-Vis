# ==========================================
#   Unity Project - A Test Framework for C
#   Copyright (c) 2007 Mike Karlesky, Mark VanderVoord, Greg Williams
#   [Released under MIT License. Please refer to license.txt for details]
# ==========================================


CLEANUP = rm -f
MKDIR = mkdir -p
TARGET_EXTENSION=.out

C_COMPILER=gcc
ifeq ($(shell uname -s), Darwin)
C_COMPILER=clang
endif

CFLAGS +=

UNITY_ROOT=../Unity

TARGET_BASE=bridge
TARGET = $(TARGET_BASE)$(TARGET_EXTENSION)
SRC_TEST_FILES=$(UNITY_ROOT)/src/unity.c src/bridge.c  test/unittest.c  test/unittest_Runner.c
SRC_FILES=src/bridge.c src/main.c
INC_DIRS=-Isrc -I$(UNITY_ROOT)/src
SYMBOLS=

all: clean production

production: $(SRC_FILES)
	$(C_COMPILER) $(CFLAGS) $(INC_DIRS) $(SYMBOLS) $(SRC_FILES) -o $(TARGET)

unittest: $(SRC_TEST_FILES)
	$(C_COMPILER) $(CFLAGS) $(INC_DIRS) $(SYMBOLS) $(SRC_TEST_FILES) -o $(TARGET)
	- ./$(TARGET)
	
clean:
	$(CLEANUP) $(TARGET)

ci: CFLAGS += -Werror
ci: unittest
ci: production
