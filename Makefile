CC = gcc
CFLAGS = -Wall -Wno-unused-variable -Wno-unused-function -Wno-main
# -lcurses # -Wextra -g
SRC_DIR = ./src
OBJ_DIR = ./obj
BIN_DIR = ./bin
# TARGET = $(BIN_DIR)/program

#	SRCS = $(wildcard $(SRC_DIR)/*.c)
#
#	OBJS = $(patsubst $(SRC_DIR)/%.c, $(OBJ_DIR)/%.o, $(SRCS))
#
#	all: $(BIN_DIR) $(TARGET) $(OBJ_DIR)
#
#	$(BIN_DIR):
#		mkdir $(BIN_DIR)
#
#	$(OBJ_DIR):
#		mkdir $(OBJ_DIR)
#
#	$(OBJ_DIR)/%.o: $(SRC_DIR)/%.c
#		$(CC) $(CFLAGS) -c $< -o $@
#
#	$(TARGET): $(OBJS)
#		$(CC) $(CFLAGS) $^ -o $@

server:
	$(CC) $(wildcard $(SRC_DIR)/server/*.c) -o $(BIN_DIR)/server $(CFLAGS)

client:
	$(CC) $(wildcard $(SRC_DIR)/client/*.c) -o $(BIN_DIR)/client $(CFLAGS)

run-server: $(BIN_DIR)/server
	./$^

run-server: $(BIN_DIR)/client
	./$^

clear:
	# rm -f $(OBJ_DIR)/*.o
	rm -rf $(BIN_DIR)/*

# clean-objects:
# 	rm -f $(OBJ_DIR)/*.o

clear-bins:
	rm -f $(BIN_DIR)/

#test:
#	./$(TARGET) # Припустимо, що my_program - це програма, яка потребує запуску для тестування

help:
	@echo "Available targets:"
	@echo "  all        	: Compile the project"
	@echo "  clear      	: Remove object files and executable"
	@echo "  clear-objects 	: Remove object files"
	@echo "  clear-target  	: Remove executable"
#	@echo "  testing       	: Run tests"
