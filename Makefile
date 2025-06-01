# **************************************************************************** #
#                                                                              #
#                                                         :::      ::::::::    #
#    Makefile                                           :+:      :+:    :+:    #
#                                                     +:+ +:+         +:+      #
#    By: bleow <bleow@student.42kl.edu.my>          +#+  +:+       +#+         #
#                                                 +#+#+#+#+#+   +#+            #
#    Created: 2024/07/25 00:56:19 by bleow             #+#    #+#              #
#    Updated: 2025/06/01 20:32:00 by bleow            ###   ########.fr        #
#                                                                              #
# **************************************************************************** #

NAME = minishell

.PHONY: all clean fclean re debug sanitize default
all: $(NAME)

CC = gcc
CFLAGS = -Wall -Wextra -Werror -gdwarf-4
DEBUG_FLAGS = -gdwarf-4
SANITIZE_FLAGS = -fsanitize=address,undefined -fno-omit-frame-pointer

LIBFT_DIR = libft
MINISHELL_DIR = srcs
TEMP_DIR = temp
INCLUDE_DIRS = includes $(LIBFT_DIR)/includes
INCLUDES = $(addprefix -I, $(INCLUDE_DIRS))

MINISHELL_SRCS = \
			srcs/append_args_utils.c \
			srcs/append_args.c \
			srcs/execute_heredocs.c \
			srcs/buildast_utils.c \
			srcs/buildast.c \
			srcs/builtin.c \
			srcs/check_permissions.c \
			srcs/cleanup_a.c \
			srcs/cleanup_b.c \
			srcs/cmd_finder.c \
			srcs/errormsg.c \
			srcs/execute_pipes.c \
			srcs/execute_redirects.c \
			srcs/execute_utils.c \
			srcs/execute.c \
			srcs/expansion_utils.c \
			srcs/expansion.c \
			srcs/forking.c \
			srcs/heredoc_delim_utils.c \
			srcs/heredoc_delim.c \
			srcs/heredoc_expansion.c \
			srcs/heredoc_utils.c \
			srcs/heredoc.c \
			srcs/history_load.c \
			srcs/history_save.c \
			srcs/history_utils.c \
			srcs/history.c \
			srcs/initnode.c \
			srcs/initshell.c \
			srcs/input_completion.c \
			srcs/input_handlers.c \
			srcs/lexer_utils.c \
			srcs/lexer.c \
			srcs/make_exp_token_utils.c \
			srcs/make_exp_token.c \
			srcs/make_node_arrays.c \
			srcs/make_quo_token.c \
			srcs/maketoken.c \
			srcs/minishell.c \
			srcs/multiline_input_buffer.c \
			srcs/multiline_validation.c \
			srcs/operators_utils.c \
			srcs/operators.c \
			srcs/parser.c \
			srcs/paths_utils.c \
			srcs/paths.c \
			srcs/pipe_analysis.c \
			srcs/pipes.c \
			srcs/proc_quoted_redir_tgt.c \
			srcs/process_multiline_input.c \
			srcs/process_pipes.c \
			srcs/process_quote_char_utils.c \
			srcs/process_quote_char.c \
			srcs/process_redir_node.c \
			srcs/process_redirect_utils.c \
			srcs/process_redirect.c \
			srcs/quoted_expansion.c \
			srcs/quotes.c \
			srcs/redirect_utils.c \
			srcs/redirect.c \
			srcs/setup_redirects.c \
			srcs/shell_level.c \
			srcs/signals.c \
			srcs/terminal.c \
			srcs/tmp_buf_reader.c \
			srcs/tokenize_utils.c \
			srcs/tokenize.c \
			srcs/typeconvert.c \
			 
MINISHELL_BUILTIN_SRCS = \
			srcs/builtins/builtin_cd.c \
			srcs/builtins/builtin_echo.c \
			srcs/builtins/builtin_env.c \
			srcs/builtins/builtin_envops_calc.c \
			srcs/builtins/builtin_envops_utils.c \
			srcs/builtins/builtin_envops.c \
			srcs/builtins/builtin_exit.c \
			srcs/builtins/builtin_export_utils.c \
			srcs/builtins/builtin_export.c \
			srcs/builtins/builtin_pwd.c \
			srcs/builtins/builtin_unset.c

MINISHELL_OBJS_DIR = objects
MINISHELL_BI_OBJS_DIR = $(MINISHELL_OBJS_DIR)/builtins

$(MINISHELL_OBJS_DIR):
	@mkdir -p $(MINISHELL_OBJS_DIR)
	@mkdir -p $(MINISHELL_BI_OBJS_DIR)
	@mkdir -p $(TEMP_DIR)

$(MINISHELL_OBJS_DIR)/%.o: srcs/%.c | $(MINISHELL_OBJS_DIR)
	@echo "Compiling BleShell $<"
	$(CC) $(CFLAGS) $(INCLUDES) -c $< -o $@

$(MINISHELL_BI_OBJS_DIR)/%.o: srcs/builtins/%.c | $(MINISHELL_OBJS_DIR)
	@echo "Compiling BleShell $<"
	$(CC) $(CFLAGS) $(INCLUDES) -c $< -o $@

MINISHELL_OBJS = $(MINISHELL_SRCS:srcs/%.c=$(MINISHELL_OBJS_DIR)/%.o)
MINISHELL_BUILTIN_OBJS = $(MINISHELL_BUILTIN_SRCS:srcs/builtins/%.c=$(MINISHELL_BI_OBJS_DIR)/%.o)

OBJS = $(MINISHELL_OBJS) $(MINISHELL_BUILTIN_OBJS)

all: $(NAME)

$(NAME): $(LIBFT_DIR)/libft.a $(OBJS) | $(MINISHELL_OBJS_DIR) $(MINISHELL_BI_OBJS_DIR)
	@echo "Linking $(NAME)"
	$(CC) $(CFLAGS) $(OBJS) -L$(LIBFT_DIR) -lft -lreadline -o $(NAME)

$(LIBFT_DIR)/libft.a:
	@echo "Building libft"
	$(MAKE) -C $(LIBFT_DIR)

clean:
	@echo "Cleaning up..."
	rm -rf objects
	rm -rf temp
	$(MAKE) -C $(LIBFT_DIR) clean

fclean: clean
	@echo "Performing full clean..."
	rm -f $(NAME)
	$(MAKE) -C $(LIBFT_DIR) fclean
	@if [ -f $(TEMP_DIR)/bleshell_history ]; then \
		echo "Removing history file..."; \
		rm -f $(TEMP_DIR)/bleshell_history; \
	fi
	@if [ -f $(TEMP_DIR)/bleshell_history_tmp ]; then \
		echo "Removing temporary history file..."; \
		rm -f $(TEMP_DIR)/bleshell_history_tmp; \
	fi

re: fclean all

debug: CFLAGS += $(DEBUG_FLAGS)
debug: re

sanitize: CFLAGS += $(SANITIZE_FLAGS)
sanitize: re

default: all
