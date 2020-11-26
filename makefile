SRC=src
BIN=bin

CC=gcc
CPP=g++

CCFLAGS =-g
CPPFLAGS=-g

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

t:
	@echo SRCFILES :=$(SRC_FILES)
	@echo
	@echo EXEC := $(EXEC)
	@echo
	@echo KNUTH := $(KNUTH)

clean: 
	rm -rf $(BIN)

$(KNUTH):
	cd $(K_DIR) && $(MAKE)
	
tests:
	cd test && $(MAKE)

