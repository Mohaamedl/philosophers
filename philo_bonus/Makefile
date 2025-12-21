# **************************************************************************** #
#                                                                              #
#                                                         :::      ::::::::    #
#    Makefile                                           :+:      :+:    :+:    #
#                                                     +:+ +:+         +:+      #
#    By: mhaddadi <mhaddadi@student.42porto.com>    +#+  +:+       +#+         #
#                                                 +#+#+#+#+#+   +#+            #
#    Created: 2025/11/01 00:00:00 by mhaddadi          #+#    #+#              #
#    Updated: 2025/11/01 00:00:00 by mhaddadi         ###   ########.fr        #
#                                                                              #
# **************************************************************************** #

NAME = philo_bonus

CC = cc
CFLAGS = -Wall -Wextra -Werror -pthread -g
INCLUDES = -I./include

# Source files
SRC_DIR = src
SRC_FILES = main_bonus.c parsing.c time.c init_bonus.c cleanup_bonus.c \
			sync_bonus.c actions_bonus.c process_bonus.c monitor_bonus.c
SRCS = $(addprefix $(SRC_DIR)/, $(SRC_FILES))

# Object files
OBJ_DIR = obj
OBJS = $(SRCS:$(SRC_DIR)/%.c=$(OBJ_DIR)/%.o)

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

clean:
	@echo "$(RED)Cleaning object files...$(RESET)"
	@rm -rf $(OBJ_DIR)
	@echo "$(GREEN)✓ Clean complete!$(RESET)"

fclean: clean
	@echo "$(RED)Removing executables...$(RESET)"
	@rm -f $(NAME)
	@echo "$(GREEN)✓ Full clean complete!$(RESET)"

re: fclean all

.PHONY: all clean fclean re
