SRC=src
BIN=bin

CC=gcc
CPP=g++

CCFLAGS=-O3
CPPFLAGS=-O3 -std=c++14

SRC_FILES=$(wildcard $(SRC)/*.c $(SRC)/*.cpp)
_EXEC=$(SRC_FILES:%.c=%)
__EXEC=$(_EXEC:%.cpp=%)
EXEC=$(__EXEC:$(SRC)/%=$(BIN)/%)

K_DIR=knuth

.PHONY: all clean benchmarks instances analyse
.SILENT: all

all:$(BIN) $(EXEC)
	cd $(K_DIR) && $(MAKE)

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

clean:
	rm -rf $(BIN)
	rm -f test/results/*
	cd knuth && $(MAKE) clean

benchmarks:
	cd test && sh instances.sh

analyse: 
	cd test && python analyse.py

# rules for making instances from sources
instances: all menage langford

menage: 
	$(BIN)/menage-k-xcc 5 > instances/menage5.txt
	$(BIN)/menage-k-xcc 11 > instances/menage11.txt
	$(BIN)/menage-k-xcc 12 > instances/menage12.txt
	$(BIN)/menage-k-xcc 13 > instances/menage13.txt
	$(BIN)/menage-k-xcc 14 > instances/menage14.txt

langford:
	$(BIN)/langford-xcc 11 > instances/langford11.txt
	$(BIN)/langford-xcc 12 > instances/langford12.txt
	$(BIN)/langford-xcc 15 > instances/langford15.txt

