/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   minishell.h                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: bleow <bleow@student.42kl.edu.my>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/01/13 15:16:53 by bleow             #+#    #+#             */
/*   Updated: 2025/04/12 19:13:51 by bleow            ###   ########.fr       */
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
# include <sys/types.h>
# include <sys/wait.h>
# include <sys/stat.h>

extern volatile sig_atomic_t	g_signal_received;

// Debugging flags - can be enabled/disabled as needed
#define DEBUG_TOKENIZE 1
#define DEBUG_QUOTES 1
#define DEBUG_ARGS 1
#define DEBUG_EXPAND 1
#define DEBUG_EXEC 1

// Debug print macro for cleaner code
#define DBG_PRINTF(flag, fmt, ...) \
	do { if (flag) fprintf(stderr, "[%s:%d] " fmt, __func__, __LINE__, ##__VA_ARGS__); } while (0)
// Debug print macro for cleaner code

/*
HISTORY_FILE - Stores the history from previous session
			   and is loaded into memory on startup.
HISTORY_FILE_TMP - Temporary file for copying history
				   to prevent data loss.
HISTORY_FILE_MAX - Maximum number of lines in history file.
HIST_MEM_MAX - Maximum number of lines to load in memory using add_history.
HIST_BUFFER_SZ - Buffer size for reading history file in bytes.
HIST_LINE_SZ - Buffer size for reading each history line in bytes.
*/
# define PROMPT "bleshell$> "
# define HISTORY_FILE "bleshell_history"
# define HISTORY_FILE_TMP "bleshell_history_tmp"
# define HISTORY_FILE_MAX 2000
# define HIST_MEM_MAX 1000
# define HIST_BUFFER_SZ 4096
# define HIST_LINE_SZ 1024

/*
String representations of token types.
These constants match the enum e_tokentype values.
This enables easy conversion between enum and string.
*/
# define TOKEN_TYPE_NULL			 "NULL"
# define TOKEN_TYPE_CMD              "CMD"
# define TOKEN_TYPE_ARGS             "ARGS"
# define TOKEN_TYPE_DOUBLE_QUOTE     "\""
# define TOKEN_TYPE_SINGLE_QUOTE     "'"
# define TOKEN_TYPE_HEREDOC          "<<"
# define TOKEN_TYPE_IN_REDIRECT      "<"
# define TOKEN_TYPE_OUT_REDIRECT     ">"
# define TOKEN_TYPE_APPEND_REDIRECT  ">>"
# define TOKEN_TYPE_EXPANSION        "$"
# define TOKEN_TYPE_PIPE             "|"
# define TOKEN_TYPE_EXIT_STATUS      "$?"

/*
Mode settings for the find_cmd function.
*/
# define FIND_LAST 1
# define FIND_PREV 2
# define FIND_NEXT 3
# define FIND_ALL 4

/*
Mode settings for the out_mode variable.
*/
#define OUT_MODE_NONE 0
#define OUT_MODE_TRUNCATE 1
#define OUT_MODE_APPEND 2

/*
Error code settings.
*/
# define ERR_DEFAULT 1
# define ERR_SYNTAX 2
# define ERR_ISDIRECTORY 2
# define ERR_PERMISSIONS 126
# define ERR_CMD_NOT_FOUND 127

/*
This enum stores the possible token types.
*/
typedef enum e_tokentype
{
	TYPE_NULL = 1,
	TYPE_CMD = 2,
	TYPE_ARGS = 3,
	TYPE_SINGLE_QUOTE = 4,
	TYPE_DOUBLE_QUOTE = 5,
	TYPE_HEREDOC = 6,
	TYPE_IN_REDIRECT = 7,
	TYPE_OUT_REDIRECT = 8,
	TYPE_APPEND_REDIRECT = 9,
	TYPE_EXPANSION = 10,
	TYPE_PIPE = 11,
	TYPE_EXIT_STATUS = 12,
}	t_tokentype;

/*
This structure is used to store the context of quotes.
Example: "'Hello 'world'!'" has 2 quotes, one single and one double.
*/
typedef struct s_quote_context
{
	char	type;
	int		start_pos;
	int		depth;
}	t_quote_context;

/*
Node structure for linked list and AST.
Next and prev are for building linked list.
Left and right are for building AST.
*/
typedef struct s_node
{
	t_tokentype		type;
	char			**args;
	int				**arg_quote_type;
	struct s_node	*next;
	struct s_node	*prev;
	struct s_node	*left;
	struct s_node	*right;
	struct s_node	*redir;
}	t_node;

/*
Structure for storing pipeline information.
Has variables tracking:
- Pipe structure
- Execution resources
- File descriptors
- AST structure
- Last command status
*/
typedef struct s_pipe
{
	int			pipe_count;     // Number of pipes in the chain
	int			*pipe_fds;      // Array of pipe file descriptors
	pid_t		*pids;          // Array of process IDs
	int			*status;        // Status for each process
	int			saved_stdin;    // Saved standard input
	int			saved_stdout;   // Saved standard output
	int			heredoc_fd;     // File descriptor for heredoc if present
	int			redirection_fd; // Current redirection file descriptor
	int			out_mode;    // Append flag for redirections
	t_node		*current_redirect; // Current redirection node
	t_node		*last_cmd;       // Last command node encountered during parsing
	t_node		*last_heredoc;   // Last heredoc node encountered during parsing
	t_node      *last_pipe;       // Last pipe node processed
	t_node      *pipe_root;       // Root of temporary pipe structure for AST
	t_node      *redir_root;      // Root redirection node
	t_node      *last_in_redir;   // Last input redirection encountered
	t_node      *last_out_redir;  // Last output redirection encountered
	t_node		*cmd_redir;      // Command node being targeted for redirection
	int			pipe_at_end;     // Flag indicating pipe at end (requires more input)
	// t_node		*current;        // Current node being processed in the token list
	// int			cmd_idx;         // Current index in the cmd_nodes array being processed
	// int			syntax_error;    // Syntax error code (0: none, 1: error, 2: incomplete)
	// int			serial_pipes;    // Counter for detecting consecutive pipes (syntax error)
	// int			pipe_at_front;   // Flag indicating pipe at beginning (syntax error)
	// int			fd_write;        // File descriptor for writing (ADDED for heredoc)
	// int			expand_vars;     // Flag for expanding variables (ADDED for heredoc)
} t_pipe;

/*
Main structure for storing variables and context.
Makes it easier to access and pass around.
Has variables tracking:
- Tokenization state
- AST structure
- Shell state
- Current pipes
*/
typedef struct s_vars
{
	char			**env;
	char			*partial_input;
	int				next_flag;
	t_node			*cmd_nodes[100];
	int				cmd_count;
	t_quote_context	quote_ctx[32];
	int				quote_depth;
	t_node			*find_start;
	t_node			*find_tgt;
	int				find_mode;
	t_node			*current;
	t_node			*head;
	t_node			*astroot;
	t_tokentype		curr_type;
	t_tokentype		prev_type;
	int				pos;
	int				start;
	int				heredoc_mode;   // Flag for heredoc mode. interactive or multiline
	int				heredoc_active; // Flag for heredoc status. is it active or not
	int				heredoc_fd;     // File descriptor for heredoc
	char			**heredoc_lines;   // Array of pending heredoc content lines
    int				heredoc_count;     // Number of stored lines
    int				heredoc_index;     // Current position in stored lines
	char			*heredoc_delim;    // Delimiter for heredoc
	int				shell_level;
	struct termios	ori_term_settings;
	int				ori_term_saved;
	int				error_code;
	t_pipe			*pipes;
} t_vars;

/* Builtin commands functions. In srcs/builtins directory. */

/*
builtin_cd.c - Builtin "cd" command. Changes the current working directory.
In builtin_cd.c
*/
int			builtin_cd(char **args, t_vars *vars);
int			handle_cd_special(char **args, t_vars *vars);
int			handle_cd_path(char **args, t_vars *vars);
int			update_env_pwd(t_vars *vars, char *oldpwd);

/*
Builtin "echo" command. Outputs arguments to STDOUT.
In builtin_echo.c
*/
int			builtin_echo(char **args, t_vars *vars);
int			process_echo_args(char **args, int start, int nl_flag);

/*
Builtin "env" command. Outputs the environment variables.
In builtin_env.c
*/
int			builtin_env(t_vars *vars);

/*
Builtin "exit" command. Exits the shell.
In builtin_exit.c
*/
int			builtin_exit(char **args, t_vars *vars);

/*
Builtin "export" command utility functions.
In builtin_export_utils.c
*/
char		*valid_export(char *args);
char		**asc_order(char **sort_env, int count);
char		**make_sorted_env(int count, t_vars *vars);
int			process_export_var(char *env_var);
int			process_var_with_val(char *name, char *value);

/*
Builtin "export" command. Sets an environment variable.
In builtin_export.c
*/
int			builtin_export(char **args, t_vars *vars);
int			export_without_args(t_vars *vars);
int			export_with_args(char **args, t_vars *vars);
int			sort_env(int count, t_vars *vars);

/*
Builtin "pwd" command. Outputs the current working directory.
In builtin_pwd.c
*/
int			builtin_pwd(t_vars *vars);

/*
Builtin "unset" command utility functions.
In builtin_unset_utils.c
*/
void		copy_env_front(char **src, char **dst, int pos);
void		copy_env_back(char **src, char **dst, int idx, int offset);

/*
Builtin "unset" command. Unsets an environment variable.
In builtin_unset.c
*/
int			builtin_unset(char **args, t_vars *vars);
int			set_next_pos(int changes, char **env, int pos);
char		**realloc_until_var(int changes, char **env, char *var, int count);
int			get_env_pos(char *var, char **env);
void		modify_env(char ***env, int changes, char *var);

/* Main minishell functions. In srcs directory. */

/*
Append arguments to a node's argument array.
In append_args.c
*/
char		**dup_node_args(t_node *node, size_t len);
int			*set_character_quote_types(char *arg_text, int quote_type);
int			**set_quote_type(t_node *node, size_t len, int quote_type);
void		update_node_args(t_node *node, char **new_args, int **quote_types);
void		clean_new_args(char **new_args, size_t len);
void		append_arg(t_node *node, char *new_arg, int quote_type);

/*
Argument handling.
In arguments.c
*/
void		create_args_array(t_node *node, char *token);

/*
AST token processing and AST tree building.
In buildast.c
*/
t_node		*proc_token_list(t_vars *vars);

/*
Builtin control handling.
In builtin.c
*/
int			is_builtin(char *cmd);
int			execute_builtin(char *cmd, char **args, t_vars *vars);

/*
Group A of cleanup functions.
In cleanup_a.c
*/
void		cleanup_env_error(char **env, int n);
void		cleanup_pipes(t_pipe *pipes);
void		cleanup_vars(t_vars *vars);
void		cleanup_exit(t_vars *vars);
void		cleanup_fds(int fd_in, int fd_out);

/*
Group B of cleanup functions.
In cleanup_b.c
*/
void		cleanup_ast(t_node *node);
void		free_token_node(t_node *node);
void		cleanup_token_list(t_vars *vars);

/*
Master command finder function.
In cmd_finder.c
*/
t_node		*init_find_cmd(t_node *start, t_vars *vars);
t_node		*mode_action(t_node *current, t_node **last_cmd, t_vars *vars);
void		reset_find_cmd(t_vars *vars, t_node *start, t_node *target
				,int mode);
t_node		*find_cmd(t_node *start, t_node *target, int mode, t_vars *vars);

/*
Error handling.
In errormsg.c
*/
void		shell_error(char *element, int error_code, t_vars *vars);
void		not_found_error(char *filename, t_vars *vars);
void		crit_error(t_vars *vars);

/*
Execution functions.
In execute.c
*/
int			handle_cmd_status(int status, t_vars *vars);
int			setup_out_redir(t_node *node, t_vars *vars);
void		end_pipe_processes(t_vars *vars);
int			setup_in_redir(t_node *node, t_vars *vars);
int			redir_mode_setup(t_node *node, t_vars *vars);
int			setup_redirection(t_node *node, t_vars *vars);
int			exec_redirect_cmd(t_node *node, char **envp, t_vars *vars);
int			exec_child_cmd(t_node *node, char **envp,
				t_vars *vars, char *cmd_path);
// void		debug_print_cmd_args(t_node *node);
int			exec_std_cmd(t_node *node, char **envp, t_vars *vars);
int			execute_cmd(t_node *node, char **envp, t_vars *vars);
int			exec_external_cmd(t_node *node, char **envp, t_vars *vars);

/*
Expansion handling.
In expansion.c
*/
char		*handle_special_var(const char *var_name, t_vars *vars);
char		*get_env_val(const char *var_name, char **env);
char		*get_var_name(char *input, int *pos);
char		*handle_expansion(char *input, int *pos, t_vars *vars);
void 		debug_cmd_args(t_node *node);
char		*expand_quoted_argument(char *arg, int *quote_types, t_vars *vars);

/*
Heredoc main handling.
In heredoc.c
*/
char		*merge_and_free(char *str, char *chunk);
char		*expand_heredoc_var(char *line, int *pos, t_vars *vars);
char		*read_heredoc_str(char *line, int *pos);
char		*expand_one_line(char *line, int *pos, t_vars *vars, char *result);
char		*expand_heredoc_line(char *line, t_vars *vars);
int			chk_expand_heredoc(char *delimiter);
int			write_to_heredoc(int fd, char *line,
				t_vars *vars, int expand_vars);
int			read_heredoc(int *fd, char *delimiter,
				t_vars *vars, int expand_vars);
int			handle_heredoc_err(t_vars *vars);
// int			cleanup_heredoc_fail(int *fd, t_vars *vars);
int			handle_heredoc(t_node *node, t_vars *vars);
// int			proc_heredoc(t_node *node, t_vars *vars);

/*
History loading functions.
In history_load.c
*/
void		skip_history_lines(int fd, int skip_count);
void		read_history_lines(int fd);
void		load_history(void);

/*
History saving functions.
In history_save.c
*/
int			copy_file_content(int fd_src, int fd_dst);
int			copy_file(const char *src, const char *dst);
int			copy_to_temp(int fd_read);
void		skip_lines(int fd, int count);
void		trim_history(int excess_lines);
void		save_history(void);
int			save_history_entries(int fd, HIST_ENTRY **hist_list,
				int start, int total);

/*
History main functions.
In history.c
*/
int			init_history_fd(int mode);
int			append_history(int fd, const char *line);
int			get_history_count(void);

/*
Node initialization functions.
In initnode.c
*/
int			make_nodeframe(t_node *node, t_tokentype type, char *token);
t_node		*initnode(t_tokentype type, char *token);

/*
Shell and structure initialization functions.
In initshell.c
*/
void		init_shell(t_vars *vars, char **envp);
void		init_vars(t_vars *vars);
t_pipe		*init_pipes(void);
void		reset_pipe_vars(t_vars *vars);
void		reset_shell(t_vars *vars);

/*
Input completion functions.
In input_completion.c
*/
int			is_input_complete(t_vars *vars);
char		*append_input(char *original, char *additional);

/*
Input processing functions.
In input_handlers.c
*/
void		proc_heredoc_input(char **cmdarr, int line_count, t_vars *vars);
void		proc_multiline_input(char **cmdarr, int line_count, t_vars *vars);
void		handle_input(char *input, t_vars *vars);

/*
Input verification functions.
In input_verify.c
*/
int			handle_adjacent_args(t_node *expansion_node, char *expanded_value, t_vars *vars);
int 		is_adjacent_in_original_input(t_node *expansion_node, char *arg);
char 		*expand_value(char *var_name, t_vars *vars);
void		process_expansions(t_vars *vars);
int			is_command_position(t_node *node, t_vars *vars);

/*
Lexer functions.
In lexer.c
*/
void		skip_whitespace(char *str, t_vars *vars);

/*
Minishell program entry point functions.
In minishell.c
*/
void		print_tokens(t_node *head); // Debug function
char		*reader(void);
void		setup_env(t_vars *vars, char **envp);
char		*handle_quote_completion(char *cmd, t_vars *vars);
t_node		*find_command_end(t_node *start_node);
void		build_and_execute(t_vars *vars);
int 		process_input_tokens(char *command, t_vars *vars);
void		process_command(char *command, t_vars *vars);
void		reset_terminal_after_heredoc(void);
int			main(int ac, char **av, char **envp);

/*
Operator handling.
In operators.c
*/
int			operators(char *input, t_vars *vars);
void		handle_string(char *input, t_vars *vars);
int			is_operator_token(t_tokentype type);
int			is_single_token(char *input, int pos, int *moves);
int			is_double_token(char *input, int pos, int *moves);
t_tokentype	get_token_at(char *input, int pos, int *moves);
int			handle_single_operator(char *input, t_vars *vars);
int			handle_double_operator(char *input, t_vars *vars);

/*
Path finding functions.
In paths.c
*/
char		**get_path_env(char **envp);
char		*try_path(char *path, char *cmd);
char		*search_in_env(char *cmd, char **envp);
char		*get_cmd_path(char *cmd, char **envp);
char		**dup_env(char **envp);

/*
Pipe analysis functions.
In pipe_analysis.c
*/
int			analyze_pipe_syntax(t_vars *vars);
char		*complete_pipe_command(char *command, t_vars *vars);

/*
Pipes main functions.
In pipes.c
*/
int			execute_pipes(t_node *pipe_node, t_vars *vars);
int			is_related_to_cmd(t_node *redir_node, t_node *cmd_node, t_vars *vars); //Possible to reuse
void 		reset_done_pipes(char **pipe_cmd, char **result, int mode);
int			check_unfinished_pipe(t_vars *vars);
char		*handle_pipe_completion(char *cmd, t_vars *vars, int syntax_chk);
int			handle_unfinished_pipes(char **processed_cmd, t_vars *vars);
void		setup_child_pipes(t_pipe *pipes, int cmd_idx, int pipe_count);
int			fork_processes(t_pipe *pipes, t_vars *vars);
int			count_pipes(t_vars *vars);
void 		close_all_pipe_fds(t_pipe *pipes);
int			wait_for_processes(t_pipe *pipes, t_vars *vars);

/*
Pipeline processing functions.
In process_pipes.c
*/
void		setup_pipe_node(t_node *pipe_node, t_node *left_cmd,
				t_node *right_cmd);
t_node		*process_first_pipe(t_vars *vars);
void		process_addon_pipes(t_vars *vars);
t_node		*proc_pipes(t_vars *vars);

/*
Redirection processing functions.
In process_redirect.c
*/
t_node		*proc_redir(t_vars *vars);
void		reset_redir_tracking(t_pipe *pipes);
void		build_redir_ast(t_vars *vars);
void 		process_redir_node(t_node *redir_node, t_vars *vars);
void		integrate_redirections_with_pipes(t_vars *vars);
void		set_redir_node(t_node *redir, t_node *cmd, t_node *target);
t_node		*get_redir_target(t_node *current, t_node *last_cmd);
void		upd_pipe_redir(t_node *pipe_root, t_node *cmd, t_node *redir);
int			is_valid_redir_node(t_node *current); //Possible to reuse

/*
Quote handling.
In quotes.c
*/
int 		validate_quotes(char *input, t_vars *vars);
char		*fix_open_quotes(char *input, t_vars *vars);
void		process_arg_quotes(char **arg);

/*
Redirection handling.
In redirect.c
*/
int			chk_permissions(char *filename, int mode, t_vars *vars); //Possible to reuse
int			is_redirection(t_tokentype type);
void		reset_redirect_fds(t_vars *vars);
int			output_redirect(t_node *node, int *fd_out,
				int append, t_vars *vars);

/*
Shell level handling.
In shell_level.c
*/
int			get_shell_level(t_vars *vars);
int			update_shlvl_env(char **env, int position, int new_level);
int			incr_shell_level(t_vars *vars);

/*
Signal handling.
In signals.c
*/
void		load_signals(void);
void		sigint_handler(int sig);
void		sigquit_handler(int sig);

/*
Tokenizing functions.
In tokenize.c
*/
void 		set_token_type(t_vars *vars, char *input);
size_t		count_nodes(t_node *head); //DEBUG FUNCTION
void		maketoken_with_type(char *token, t_tokentype type, t_vars *vars);
int			build_token_linklist_with_status(t_vars *vars, t_node *node);
int 		is_adjacent_token(char *input, int pos);
int			make_exp_token(char *input, int *i, t_vars *vars);
int			process_quote_char(char *input, int *i, t_vars *vars);
int			process_operator_char(char *input, int *i, t_vars *vars);
int			improved_tokenize(char *input, t_vars *vars);
void		token_link(t_node *node, t_vars *vars);
void		build_token_linklist(t_vars *vars, t_node *node);
void 		debug_token_list(t_vars *vars);

/*
Type conversion functions.
In typeconvert.c
*/
char	*get_token_str(t_tokentype type);

#endif
