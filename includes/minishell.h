/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   minishell.h                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: bleow <bleow@student.42kl.edu.my>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/01/13 15:16:53 by bleow             #+#    #+#             */
/*   Updated: 2025/03/01 08:06:21 by bleow            ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef MINISHELL_H
# define MINISHELL_H

#include "libft.h"
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <errno.h>
#include <string.h>
#include <limits.h>
#include <signal.h>
#include <termios.h>
#include <fcntl.h> 
#include <readline/readline.h>
#include <readline/history.h>

# define PROMPT "bleshell$> "
# define HISTORY_FILE "bleshell_history"
# define HIST_BUFFER_SZ 4096
# define HIST_LINE_SZ 1024

typedef enum e_quote_state
{
    QUOTE_NONE = 0,
    QUOTE_SINGLE,
    QUOTE_DOUBLE
} t_quote_state;

typedef enum e_tokentype
{
    TYPE_CMD = 0,
	TYPE_ARGS = 1,
    TYPE_STRING = 2,
    TYPE_DOUBLE_QUOTE = 3,
    TYPE_SINGLE_QUOTE = 4,
    TYPE_HEREDOC = 5,
    TYPE_IN_REDIRECT = 6,
    TYPE_OUT_REDIRECT = 7,
    TYPE_APPEND_REDIRECT = 8,
    TYPE_EXPANSION = 9,
    TYPE_PIPE = 10,
    TYPE_EXIT_STATUS = 11
} t_tokentype;

typedef struct s_node
{
	t_tokentype		type;
	char 			**args;
	struct s_node	*left;
	struct s_node	*right;
}	t_node;

typedef struct s_vars
{
	t_node		*astroot;
	t_node		*head;
    t_node		*current;
	t_tokentype	curr_type;
	char		**env;
	int			squoflag;
	int			dquoflag;
	int			pos;
	int			start;
	int			error_code;
    char		*error_msg; 
}	t_vars;

/*
Entry point functions. In main.c
*/
char	*reader(void);

/*
Signal handlers. In signals.c
*/
void	sigint_handler(int sig);
void	sigquit_handler(int sig);

/*
History functions. In history.c
*/
int		init_history_fd(int mode);
void	save_history(void);
void	load_history(void);

void    lexerlist(char *str, t_vars *vars);
t_node  *init_head_node(t_vars *vars);
int     makenode(t_vars *vars, char *data);
void    ft_error(t_vars *vars);
t_node  *initnode(char *data, t_tokentype type);
void    tokenize(char *input, t_vars *vars);
int     operators(char *input, int i, int token_start, t_vars *vars);
void    maketoken(char *input, t_vars *vars);
char	*extract_quoted_content(char *input, int *pos, char quote);
char	*handle_expansion(char *input, int *pos, t_vars *vars);
void	update_quote_state(char c, t_vars *vars);
char	*get_cmd_path(char *cmd, char **envp);
char	**dup_env(char **envp);
int		execute_cmd(t_node *cmd_node, char **envp);
void	handle_pipe(t_node *pipe_node, char **envp);
