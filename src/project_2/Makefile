# ==========================================
#   Unity Project - A Test Framework for C
#   Copyright (c) 2007 Mike Karlesky, Mark VanderVoord, Greg Williams
#   [Released under MIT License. Please refer to license.txt for details]
# ==========================================

.PHONY: consumer creator

ifneq ($(findstring consumer,$(MAKECMDGOALS)),)
  TARGET_BASE=consumer
  SRC_FILES := $(wildcard src/consumer/*.c) $(wildcard src/sync/*.c) 
endif

ifneq ($(findstring creator,$(MAKECMDGOALS)),)
  TARGET_BASE=creator
  SRC_FILES := $(wildcard src/creator/*.c) $(wildcard src/sync/*.c) $(wildcard src/utils/*.c)
endif

ifneq ($(findstring finalizer,$(MAKECMDGOALS)),)
  TARGET_BASE=finalizer
  SRC_FILES := $(wildcard src/finalizer/*.c) $(wildcard src/sync/*.c) $(wildcard src/utils/*.c)
endif

ifneq ($(findstring producer,$(MAKECMDGOALS)),)
  TARGET_BASE=producer
  SRC_FILES := $(wildcard src/producer/*.c) $(wildcard src/sync/*.c)  $(wildcard src/utils/*.c) 
endif

CLEANUP = rm -f
MKDIR = mkdir -p
TARGET_EXTENSION=.out

C_COMPILER=gcc
CFLAGS += -g -Wall `pkg-config --cflags --libs gtk+-3.0` -export-dynamic

UNITY_ROOT=../Unity

#TARGET_BASE=project
TARGET = $(TARGET_BASE)$(TARGET_EXTENSION)
#SRC_FILES := $(wildcard src/*.c)
SRC_TEST_FILES=$(UNITY_ROOT)/src/unity.c $(filter-out src/main.c, $(SRC_FILES))  $(wildcard test/*.c)
INC_DIRS=-Isrc -I$(UNITY_ROOT)/src
LDFLAGS= -lm



all: clean
	make consumer --no-print-directory
	make creator --no-print-directory
	make finalizer --no-print-directory
	make producer --no-print-directory


consumer creator finalizer producer: $(SRC_FILES)
	$(C_COMPILER) -o $(TARGET) $(SRC_FILES) $(CFLAGS) $(LDFLAGS) $(INC_DIRS)

unittest: $(SRC_TEST_FILES)
	$(C_COMPILER) -o $(TARGET) $(SRC_TEST_FILES) $(CFLAGS) $(INC_DIRS)
	- ./$(TARGET)
	
clean:
	$(CLEANUP) *$(TARGET_EXTENSION)

ci: CFLAGS += -Wall `pkg-config --cflags --libs gtk+-3.0` -export-dynamic
ci: unittest
ci: production
