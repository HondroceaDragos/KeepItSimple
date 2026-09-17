CC=gcc
CSTD=-std=c2x
CMACRODEF=-D_POSIX_C_SOURCE=200809L
WFLAGS=-Wall -Wextra

DFLAGS=$(CSTD) $(CMACRODEF) $(WFLAGS) -O3 -g
RFLAGS=$(CSTD) $(CMACRODEF) $(WFLAGS) -Os
LDFLAGS=-lpthread -lm

DTARGET=main
RTARGET=KeepItSimple
SRC=main.c $(wildcard ./src/*.c) $(wildcard ./src/*/*.c)

GO_DIR=./utils/dataLoading/queryTerminal
GO_TARGET=queryTerminal
GO_SRC=$(GO_DIR)/queryTerminal.go
GO_FILE=queryTerminal.go

.PHONY: build dev release clean run all

build: dev release

dev: $(GO_TARGET) $(DTARGET)

release: $(GO_TARGET) $(RTARGET)
	strip $(RTARGET)

$(GO_TARGET): $(GO_SRC)
	cd $(GO_DIR) && go build -o $(GO_TARGET) $(GO_FILE)
	$(GO_DIR)/$(GO_TARGET)

$(DTARGET): $(SRC)
	$(CC) $(DFLAGS) $(SRC) $(LDFLAGS) -o $(DTARGET)

$(RTARGET): $(SRC)
	$(CC) $(RFLAGS) $(SRC) $(LDFLAGS) -o $(RTARGET)

clean:
	rm -f $(DTARGET) $(RTARGET) $(GO_TARGET) $(GO_DIR)/$(GO_TARGET)

run: dev
	./$(DTARGET)

all: build
