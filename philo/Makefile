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
SRC_FILES = parsing.c time.c
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
test: $(TEST_NAME)
	@echo "$(BLUE)Running Phase 1 Unit Tests...$(RESET)"
	@./$(TEST_NAME)

$(TEST_NAME): $(OBJ_DIR)/$(TEST_DIR)/test_phase1.o $(OBJ_DIR)/parsing.o $(OBJ_DIR)/time.o
	@echo "$(BLUE)Compiling test suite...$(RESET)"
	@$(CC) $(CFLAGS) $(INCLUDES) $^ -o $(TEST_NAME)
	@echo "$(GREEN)✓ Test suite compiled successfully!$(RESET)"

clean:
	@echo "$(RED)Cleaning object files...$(RESET)"
	@rm -rf $(OBJ_DIR)
	@echo "$(GREEN)✓ Clean complete!$(RESET)"

fclean: clean
	@echo "$(RED)Removing executables...$(RESET)"
	@rm -f $(NAME) $(TEST_NAME)
	@echo "$(GREEN)✓ Full clean complete!$(RESET)"

re: fclean all

# Run norminette check
norm:
	@echo "$(BLUE)Running norminette...$(RESET)"
	@norminette include/ src/ || true

.PHONY: all clean fclean re test norm
