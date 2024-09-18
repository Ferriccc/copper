JOBS ?= 4
MAKEFLAGS += -j$(JOBS)

CC = g++
CFLAGS = -std=c++20 -I./headers -I./external_deps -DTOML_HEADER_ONLY=0
TEST_FLAGS = -DTESTING
RELEASE_FLAGS = -O3

SRC_DIR = ./src
OBJ_DIR = ./obj
BIN_DIR = ./bin
INSTALL_DIR = /usr/local/bin
LOCAL_TEST_DIR = /home/shrey/scripts

SRC_FILES = $(wildcard $(SRC_DIR)/*.cpp)
OBJ_FILES = $(patsubst $(SRC_DIR)/%.cpp, $(OBJ_DIR)/%.o, $(SRC_FILES))

release: CFLAGS += $(RELEASE_FLAGS)
release: $(BIN_DIR)/copper
	sudo cp $(BIN_DIR)/copper $(INSTALL_DIR)

test: CFLAGS += $(TEST_FLAGS)
test: $(BIN_DIR)/copper
	sudo cp $(BIN_DIR)/copper $(INSTALL_DIR)

$(BIN_DIR)/copper: $(OBJ_FILES)
	mkdir -p $(BIN_DIR)
	$(CC) $(CFLAGS) -o $@ $^

$(OBJ_DIR)/%.o: $(SRC_DIR)/%.cpp
	mkdir -p $(OBJ_DIR)
	$(CC) $(CFLAGS) -c $< -o $@

clean:
	rm -rf $(OBJ_DIR) && rm -rf $(BIN_DIR)
