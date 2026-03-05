SRC_DIR := src
BIN_DIR := bin
TARGET := fluffyboy

DEBUG ?= 0

SOURCES := $(shell find $(SRC_DIR) -name "*.c")
OBJECTS := $(patsubst $(SRC_DIR)/%.c,$(BIN_DIR)/%.o,$(SOURCES))

CFLAGS :=
LDFLAGS :=

ifeq ($(DEBUG),1)
	CFLAGS += -g
endif

$(BIN_DIR)/%.o: $(SRC_DIR)/%.c
	mkdir -p $(shell dirname $@)
	gcc -c $^ $(CFLAGS) -o $@

$(TARGET): $(OBJECTS)
	gcc $^ $(LDFLAGS) -o $@

all: $(TARGET)

clean:
	rm -rf $(BIN_DIR)
