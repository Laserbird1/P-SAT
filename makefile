SRC=src
BIN=bin

CC=gcc
CPP=g++

CCFLAGS =-O3
CPPFLAGS=-O3 -std=c++14

SRC_FILES=$(wildcard $(SRC)/*.c $(SRC)/*.cpp)
_EXEC=$(SRC_FILES:%.c=%)
__EXEC=$(_EXEC:%.cpp=%)
EXEC=$(__EXEC:$(SRC)/%=$(BIN)/%)

K_DIR=knuth
K_FILES=$(wildcard $(K_DIR)/*.w)
KNUTH=$(K_FILES:$(K_DIR)/%.w=$(BIN)/%)

all:$(BIN) $(KNUTH) $(EXEC)

$(BIN)/%:$(SRC)/%.c
	$(CC) $(CCFLAGS) $< -o $@

$(BIN)/%:$(SRC)/%.cpp
	$(CPP) $(CPPFLAGS) $< -o $@

$(BIN):
	mkdir $(BIN)

list:
	@echo SRCFILES :=$(SRC_FILES)
	@echo
	@echo EXEC := $(EXEC)
	@echo
	@echo KNUTH := $(KNUTH)

clean: FORCE
	rm -rf $(BIN)
	cd test && $(MAKE) clean
	cd knuth && $(MAKE) clean

$(KNUTH):
	cd $(K_DIR) && $(MAKE)

test: FORCE
	cd test && $(MAKE)

FORCE:

