/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   minishell.h                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: bleow <bleow@student.42kl.edu.my>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/01/13 15:16:53 by bleow             #+#    #+#             */
/*   Updated: 2025/03/03 13:17:47 by bleow            ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef MINISHELL_H
# define MINISHELL_H

# include "libft.h"
# include <stdio.h>
# include <stdlib.h>
# include <unistd.h>
# include <errno.h>
# include <string.h>
# include <limits.h>
# include <signal.h>
# include <string.h>
# include <termios.h>
# include <fcntl.h> 
# include <readline/readline.h>
# include <readline/history.h>
# include <sys/wait.h>

extern volatile sig_atomic_t	g_signal_received;

# define PROMPT "bleshell$> "
# define HISTORY_FILE "bleshell_history"
# define HISTORY_FILE_TMP "bleshell_history_tmp"
# define HISTORY_FILE_MAX 2000
# define HIST_MEM_MAX 1000
# define HIST_BUFFER_SZ 4096
# define HIST_LINE_SZ 1024

typedef struct s_quote_context
{
	char	type;
	int		start_pos;
	int		depth;
}	t_quote_context;

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
	TYPE_EXIT_STATUS = 11,
	TYPE_HEAD = 12
}	t_tokentype;

/*
Node structure for linked list and AST.
Next and prev are for building linked list.
Left and right are for building AST.
*/
typedef struct s_node
{
	t_tokentype		type;
	char			**args;
	struct s_node	*next;
	struct s_node	*prev;
	struct s_node	*left;
	struct s_node	*right;
}	t_node;

typedef struct s_vars
{
	t_node			*astroot;
	t_node			*head;
	t_node			*current;
	t_tokentype		curr_type;
	char			**env;
	t_quote_context	quote_ctx[32];
	int				quote_depth;
	int				pos;
	int				start;
	int				error_code;
	char			*error_msg;
}	t_vars;

/*
Argument handling. In arguments.c
*/
void		create_args_array(t_node *node, char *token);
void		append_arg(t_node *node, char *new_arg);

/*
AST handling. In ast.c
*/
t_node		*init_head_node(t_vars *vars);
t_node		*build_ast(t_vars *vars);
t_node		*initnode(t_tokentype type, char *data);

/*
Builtin control handling. In builtin.c

int	is_builtin(char *cmd);
int	execute_builtin(char *cmd, char **args, t_vars *vars);
*/

/*
Cleanup functions. In cleanup.c
*/
void		cleanup_vars(t_vars *vars);
void		free_ast(t_node *node);

/*
Error handling. In error.c
*/
void		ft_error(t_vars *vars);
void		redirect_error(char *filename);

/*
Execution functions. In execute.c
*/
int			handle_cmd_status(int status, t_vars *vars);
int			execute_cmd(t_node *cmd_node, char **envp, t_vars *vars);

/*
Expansion handling. In expansion.c
*/
char		*handle_special_var(const char *var_name, t_vars *vars);
char		*get_env_value(const char *var_name, char **env);
char		*handle_expansion(char *input, int *pos, t_vars *vars);

/*
Here document handling. In heredoc.c
*/
int			write_to_heredoc(int fd, char *line, t_vars *vars, int expand_vars);
int			read_heredoc(int *fd, char *delimiter, t_vars *vars,
				int expand_vars);
int			expand_heredoc_check(char *delimiter);
int			handle_heredoc(t_node *node, t_vars *vars);
int			run_heredoc(t_node *node, t_vars *vars);

/*
Heredoc expansion handling. In heredoc_expand.c
*/
char		*join_and_free(char *str, char *chunk);
char		*expand_var_heredoc(char *line, int *pos, t_vars *vars);
char		*read_string(char *line, int *pos);
char		*expand_one_line(char *line, int *pos, t_vars *vars, char *result);
char		*expand_heredoc_line(char *line, t_vars *vars);

/*
History functions. In history.c
*/
int			init_history_fd(int mode);
int			append_history(int fd, const char *line);
int			get_history_count(void);

/*
History loading functions. In historyload.c
*/
void		skip_history_lines(int fd, int skip_count);
void		read_history_lines(int fd);
void		load_history(void);

/* 
History saving helper functions. In historysave_helper.c
*/
int			copy_file_content(int fd_src, int fd_dst);
int			copy_file(const char *src, const char *dst);

/*
History saving functions. In historysave.c
*/
int			copy_to_temp(int fd_read);
void		skip_lines(int fd, int count);
void		trim_history(int excess_lines);
void		save_history(void);

/*
Lexer functions. In lexer.c
*/
void		skip_whitespace(char *str, t_vars *vars);
void		handle_token(char *str, t_vars *vars);
void		lexerlist(char *str, t_vars *vars);

/*
Entry point functions. In main.c
*/
char		*reader(void);
void		init_shell(t_vars *vars, char **envp);
int			process_command(char *command, t_vars *vars);

/*
Node handling. In nodes.c
*/
int			makenode(t_vars *vars, char *data);
void		add_child(t_node *parent, t_node *child);
void		handle_pipe_node(t_node **root, t_node *pipe_node);
void		redirection_node(t_node *root, t_node *redir_node);

/*
Operator handling. In operators.c
*/
int			operators(char *input, int i, int token_start, t_vars *vars);
int			handle_string(char *input, int i, int token_start, t_vars *vars);
t_tokentype	get_operator_type(char op);
int			handle_double_operator(char *input, int i, t_vars *vars);
int			handle_single_operator(char *input, int i, t_vars *vars);

/*
Path finding functions. In paths.c
*/
char		**get_path_env(char **envp);
char		*try_path(char *path, char *cmd);
char		*get_cmd_path(char *cmd, char **envp);
char		**dup_env(char **envp);

/*
Pipes handling. In piping.c
*/
void		init_pipe(t_node *cmd, int *pipe_fd);
int			execute_pipeline(t_node *pipe_node, char **envp, t_vars *vars);

/*
Quote handling. In quotes.c
*/
void		handle_quotes(char *input, int *pos, t_vars *vars);
char		*handle_unclosed_quotes(char *input, t_vars *vars);
char		*read_quoted_content(char *input, int *pos, char quote);

/*
Redirection helper functions. In redirect_helper.c
*/
int			chk_permissions(char *filename, int mode);
int			set_output_flags(int append);
int			set_redirect_flags(int mode);

/*
Redirection handling. In redirect.c
*/
int			input_redirect(t_node *node, int *fd_in);
int			output_redirect(t_node *node, int *fd_out, int append);
int			open_redirect_file(t_node *node, int *fd, int mode);
int			handle_redirect(t_node *node, int *fd, int mode);

/*
Signal handlers. In signals.c
*/
void		load_signals(void);
void		sigint_handler(int sig);
void		sigquit_handler(int sig);

/*
Token classification handling. In tokenclass.c
*/
char		*handle_exit_status(t_vars *vars);
t_tokentype	redirection_type(char *str, int mode);
t_tokentype	classify(char *str);

/*
Tokenizing functions. In tokenize.c
*/
void		maketoken(char *token, t_vars *vars);
void		handle_quote_content(char *input, int *i, t_vars *vars);
int			handle_expand(t_vars *vars);
void		process_char(char *input, int *i, t_vars *vars);
void		tokenize(char *input, t_vars *vars);

/*
Token handling. In tokens.c
*/
t_node		*make_cmdnode(char *token);
t_node		*handle_cmd_token(char *token);
void		handle_quote_token(char *str, t_vars *vars, int *pos);
int			is_redirection(t_tokentype type);
t_node		*handle_other_token(char *token, t_tokentype type);

#endif