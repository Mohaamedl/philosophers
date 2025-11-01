# **************************************************************************** #
#                                                                              #
#                                                         :::      ::::::::    #
#    Makefile                                           :+:      :+:    :+:    #
#                                                     +:+ +:+         +:+      #
#    By: mhaddadi <mhaddadi@student.42porto.com>    +#+  +:+       +#+         #
#                                                 +#+#+#+#+#+   +#+            #
#    Created: 2025/10/27 00:00:00 by mhaddadi          #+#    #+#              #
#    Updated: 2025/10/27 00:00:00 by mhaddadi         ###   ########.fr        #
#                                                                              #
# **************************************************************************** #

NAME = philo
TEST_NAME = test_phase1

CC = cc
CFLAGS = -Wall -Wextra -Werror -pthread -g
INCLUDES = -I./include

# Source files
SRC_DIR = src
SRC_FILES = main.c parsing.c time.c init.c cleanup.c sync.c actions.c routine.c
SRCS = $(addprefix $(SRC_DIR)/, $(SRC_FILES))

# Test files
TEST_DIR = tests
TEST_SRCS = $(TEST_DIR)/test_phase1.c $(SRCS)

# Object files
OBJ_DIR = obj
OBJS = $(SRCS:$(SRC_DIR)/%.c=$(OBJ_DIR)/%.o)
TEST_OBJS = $(TEST_SRCS:%.c=$(OBJ_DIR)/%.o)

# Colors
GREEN = \033[0;32m
RED = \033[0;31m
BLUE = \033[0;34m
RESET = \033[0m

all: $(NAME)

$(NAME): $(OBJS)
	@echo "$(BLUE)Compiling $(NAME)...$(RESET)"
	@$(CC) $(CFLAGS) $(OBJS) -o $(NAME)
	@echo "$(GREEN)✓ $(NAME) compiled successfully!$(RESET)"

$(OBJ_DIR)/%.o: $(SRC_DIR)/%.c
	@mkdir -p $(OBJ_DIR)
	@echo "$(BLUE)Compiling $<...$(RESET)"
	@$(CC) $(CFLAGS) $(INCLUDES) -c $< -o $@

$(OBJ_DIR)/$(TEST_DIR)/%.o: $(TEST_DIR)/%.c
	@mkdir -p $(OBJ_DIR)/$(TEST_DIR)
	@echo "$(BLUE)Compiling test $<...$(RESET)"
	@$(CC) $(CFLAGS) $(INCLUDES) -c $< -o $@

# Build and run Phase 1 tests
test: test1

test1: test_phase1
	@echo "$(BLUE)Running Phase 1 Unit Tests...$(RESET)"
	@./test_phase1

test_phase1: $(OBJ_DIR)/$(TEST_DIR)/test_phase1.o $(OBJ_DIR)/parsing.o $(OBJ_DIR)/time.o
	@echo "$(BLUE)Compiling Phase 1 test suite...$(RESET)"
	@$(CC) $(CFLAGS) $(INCLUDES) $^ -o test_phase1
	@echo "$(GREEN)✓ Phase 1 test suite compiled successfully!$(RESET)"

# Build and run Phase 2 tests
test2: test_phase2
	@echo "$(BLUE)Running Phase 2 Unit Tests...$(RESET)"
	@./test_phase2

test_phase2: $(OBJ_DIR)/$(TEST_DIR)/test_phase2.o $(OBJ_DIR)/parsing.o $(OBJ_DIR)/time.o $(OBJ_DIR)/init.o $(OBJ_DIR)/cleanup.o
	@echo "$(BLUE)Compiling Phase 2 test suite...$(RESET)"
	@$(CC) $(CFLAGS) $(INCLUDES) $^ -o test_phase2
	@echo "$(GREEN)✓ Phase 2 test suite compiled successfully!$(RESET)"

# Build and run Phase 3 tests
test3: test_phase3
	@echo "$(BLUE)Running Phase 3 Unit Tests...$(RESET)"
	@./test_phase3

test_phase3: $(OBJ_DIR)/$(TEST_DIR)/test_phase3.o $(OBJ_DIR)/parsing.o $(OBJ_DIR)/time.o $(OBJ_DIR)/init.o $(OBJ_DIR)/cleanup.o $(OBJ_DIR)/sync.o $(OBJ_DIR)/actions.o $(OBJ_DIR)/routine.o
	@echo "$(BLUE)Compiling Phase 3 test suite...$(RESET)"
	@$(CC) $(CFLAGS) $(INCLUDES) $^ -o test_phase3
	@echo "$(GREEN)✓ Phase 3 test suite compiled successfully!$(RESET)"

# Run all tests
test_all: test1 test2 test3
	@echo "$(GREEN)All test phases completed!$(RESET)"

# Legacy test name for backward compatibility
$(TEST_NAME): test_phase1

clean:
	@echo "$(RED)Cleaning object files...$(RESET)"
	@rm -rf $(OBJ_DIR)
	@echo "$(GREEN)✓ Clean complete!$(RESET)"

fclean: clean
	@echo "$(RED)Removing executables...$(RESET)"
	@rm -f $(NAME) test_phase1 test_phase2 test_phase3
	@echo "$(GREEN)✓ Full clean complete!$(RESET)"

re: fclean all

# Run norminette check
norm:
	@echo "$(BLUE)Running norminette...$(RESET)"
	@norminette include/ src/ || true

.PHONY: all clean fclean re test test1 test2 test3 test_all norm
