CC=gcc
CSTD=-std=c2x
CMACRODEF=-D_POSIX_C_SOURCE=200809L
CFLAGS=$(CSTD) $(CMACRODEF) -Wall -Wextra
LDFLAGS=-lpthread -lm

TARGET=main
SRC=main.c $(wildcard ./src/*.c) $(wildcard ./src/*/*.c)

GO_DIR=./utils/dataLoading/queryTerminal
GO_TARGET=queryTerminal
GO_SRC=$(GO_DIR)/queryTerminal.go
GO_FILE=queryTerminal.go

.PHONY: build clean run all

build: $(GO_TARGET) $(TARGET)

$(GO_TARGET): $(GO_SRC)
	cd $(GO_DIR) && go build -o $(GO_TARGET) $(GO_FILE)
	$(GO_DIR)/$(GO_TARGET)

$(TARGET): $(SRC)
	$(CC) $(CFLAGS) $(SRC) $(LDFLAGS) -o $(TARGET)

clean:
	rm -f $(TARGET) $(GO_TARGET) $(GO_DIR)/$(GO_TARGET)

run:
	./$(TARGET)

all: build run
