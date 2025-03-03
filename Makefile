# **************************************************************************** #
#                                                                              #
#                                                         :::      ::::::::    #
#    Makefile                                           :+:      :+:    :+:    #
#                                                     +:+ +:+         +:+      #
#    By: bleow <bleow@student.42kl.edu.my>          +#+  +:+       +#+         #
#                                                 +#+#+#+#+#+   +#+            #
#    Created: 2024/07/25 00:56:19 by bleow             #+#    #+#              #
#    Updated: 2025/03/03 12:51:46 by bleow            ###   ########.fr        #
#                                                                              #
# **************************************************************************** #

NAME = minishell

.PHONY: all clean fclean re debug sanitize default
all: $(NAME)

CC = gcc
CFLAGS = -Wall -Wextra -Werror -gdwarf-4
DEBUG_FLAGS = -gdwarf-4
SANITIZE_FLAGS = -fsanitize=address,undefined

LIBFT_DIR = libft
MINISHELL_DIR = srcs
INCLUDE_DIRS = includes $(LIBFT_DIR)/includes
INCLUDES = $(addprefix -I, $(INCLUDE_DIRS))

MINISHELL_SRCS = srcs/minishell.c \
             srcs/arguments.c \
             srcs/ast.c \
             srcs/cleanup.c \
             srcs/error.c \
             srcs/execute.c \
             srcs/expansion.c \
             srcs/heredoc.c \
             srcs/heredoc_expand.c \
             srcs/history.c \
			 srcs/historyload.c \
			 srcs/historysave_helper.c \
             srcs/historysave.c \
             srcs/lexer.c \
             srcs/nodes.c \
             srcs/operators.c \
             srcs/paths.c \
             srcs/piping.c \
             srcs/quotes.c \
             srcs/redirect.c \
             srcs/redirect_helper.c \
             srcs/signals.c \
             srcs/tokenclass.c \
             srcs/tokenize.c \
             srcs/tokens.c

MINISHELL_OBJS_DIR = objects
MINISHELL_OBJS = $(MINISHELL_SRCS:srcs/%.c=$(MINISHELL_OBJS_DIR)/%.o)

$(MINISHELL_OBJS_DIR):
	@mkdir -p $(MINISHELL_OBJS_DIR)

$(MINISHELL_OBJS_DIR)/%.o: srcs/%.c | $(MINISHELL_OBJS_DIR)
	 @echo "Compiling BleShell $<"
	$(CC) $(CFLAGS) $(INCLUDES) -c $< -o $@

OBJS = $(MINISHELL_OBJS)

all: $(NAME)

$(NAME): $(LIBFT_DIR)/libft.a $(OBJS) | $(MINISHELL_OBJS_DIR)
	@echo "Linking $(NAME)"
	$(CC) $(CFLAGS) $(OBJS) -L$(LIBFT_DIR) -lft -lreadline -o $(NAME)

$(LIBFT_DIR)/libft.a:
	@echo "Building libft"
	$(MAKE) -C $(LIBFT_DIR)

clean:
	@echo "Cleaning up..."
	rm -rf objects
	$(MAKE) -C $(LIBFT_DIR) clean

fclean: clean
	@echo "Performing full clean..."
	rm -f $(NAME)
	$(MAKE) -C $(LIBFT_DIR) fclean

re: fclean all

debug: CFLAGS += $(DEBUG_FLAGS)
debug: re

sanitize: CFLAGS += $(SANITIZE_FLAGS)
sanitize: re

default: all
