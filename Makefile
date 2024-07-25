CC = g++
CFLAGS = -std=c++20 -I./headers -I./external_deps -DTESTING

SRC_DIR = ./src
OBJ_DIR = ./obj
BIN_DIR = ./bin

SRC_FILES = $(wildcard $(SRC_DIR)/*.cpp)
OBJ_FILES = $(patsubst $(SRC_DIR)/%.cpp, $(OBJ_DIR)/%.o, $(SRC_FILES))

all: $(BIN_DIR)/copper
	sudo cp $(BIN_DIR)/copper /usr/local/bin

$(BIN_DIR)/copper: $(OBJ_FILES)
	$(CC) $(CFLAGS) -o $@ $^

$(OBJ_DIR)/%.o: $(SRC_DIR)/%.cpp
	$(CC) $(CFLAGS) -c $< -o $@

clean:
	rm -rf $(OBJ_DIR) && rm -rf $(BIN_DIR) && mkdir $(OBJ_DIR) && mkdir $(BIN_DIR)
