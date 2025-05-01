/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   minishell.h                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: bleow <bleow@student.42kl.edu.my>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/01/13 15:16:53 by bleow             #+#    #+#             */
/*   Updated: 2025/04/29 21:27:55 by bleow            ###   ########.fr       */
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

/* Debug flags - controlled via MINISHELL_DEBUG environment variable */
# ifndef DEBUG_AST
#  define DEBUG_AST 1
# endif
# ifndef DEBUG_EXEC
#  define DEBUG_EXEC 1
# endif
# ifndef DEBUG_PIPES
#  define DEBUG_PIPES 1
# endif
# ifndef DEBUG_REDIR
#  define DEBUG_REDIR 1
# endif
# ifndef DEBUG_ERROR
#  define DEBUG_ERROR 1
# endif
# ifndef DEBUG_EXPAND
#  define DEBUG_EXPAND 1
# endif
# ifndef DEBUG_VERBOSE
#  define DEBUG_VERBOSE 1
# endif

extern volatile sig_atomic_t	g_signal_received;

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
# define HISTORY_FILE "temp/bleshell_history"
# define HISTORY_FILE_TMP "temp/bleshell_history_tmp"
# define TMP_BUF "temp/temp_buffer"
# define HISTORY_FILE_MAX 100
# define HIST_MEM_MAX 50
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
Terminal state constants
*/
# define TERM_SAVE    1
# define TERM_HEREDOC 2 
# define TERM_RESTORE 3

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
# define OUT_MODE_NONE 0
# define OUT_MODE_TRUNCATE 1
# define OUT_MODE_APPEND 2

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
Structure for managing read buffer processing.
Handles the variables needed for reading and processing.
content from a temporary file.
*/
typedef struct s_read_buf
{
	char			*file_content;
	char			*buffer;
	int				pos;
	int				in_quotes;
	char			quote_type;
}	t_read_buf;

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
	int			pipe_count;
	pid_t		*pids;
	int			saved_stdin;
	int			saved_stdout;
	int			heredoc_fd;
	char		*heredoc_delim;
	int			hd_expand;
	int			redirection_fd;
	int			out_mode;
	t_node		*current_redirect;
	t_node		*last_cmd;
	t_node		*last_pipe;
	t_node		*pipe_root;
	t_node		*redir_root;
	t_node		*last_in_redir;
	t_node		*last_out_redir;
	t_node		*cmd_redir;
}	t_pipe;

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
	int				adj_state[3];
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
	int				hd_text_ready;
	int				shell_level;
	struct termios	ori_term_settings;
	int				ori_term_saved;
	int				error_code;
	t_pipe			*pipes;
}	t_vars;

/* Builtin commands functions.
In srcs/builtins directory. */

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
Append arguments utility functions.
In append_args_utils.c
*/
int			*copy_int_arr(int *original, size_t length);
void		check_token_adj(char *input, t_vars *vars);
int			process_adj(int *i, t_vars *vars);

/*
Append arguments to a node's argument array.
In append_args.c
*/
char		**dup_node_args(t_node *node, size_t len);
int			**dup_quote_types(t_node *node, size_t len);
int			*set_char_quote_types(char *arg_text, int quote_type);
int			**resize_quotype_arr(t_node *node, char *new_arg, int quote_type,
				char **new_args);
void		append_arg(t_node *node, char *new_arg, int quote_type);

/*
AST token processing and AST tree building.
In buildast.c
*/
t_node		*proc_token_list(t_vars *vars);
t_node		*proc_pipes(t_vars *vars);
t_node		*proc_redir(t_vars *vars);
void		build_redir_ast(t_vars *vars);
void		process_redir_node(t_node *redir_node, t_vars *vars);

/*
Builtin control handling.
In builtin.c
*/
int			is_builtin(char *cmd);
int			execute_builtin(char *cmd, char **args, t_vars *vars);

/*
Permission checking functions.
In check_permissions.c
*/
char		*extract_dir_path(char *filename);
int			chk_read_permission(char *filename, t_vars *vars);
int			chk_file_write_permission(char *filename, t_vars *vars);
int			chk_dir_write_permission(char *filename, t_vars *vars);
int			chk_permissions(char *filename, int mode, t_vars *vars);

/*
Group A of cleanup functions.
In cleanup_a.c
*/
void		cleanup_env_error(char **env, int n);
void		cleanup_pipes(t_pipe *pipes);
void		cleanup_vars(t_vars *vars);
void		cleanup_exit(t_vars *vars);
void		cleanup_token_list(t_vars *vars);

/*
Group B of cleanup functions.
In cleanup_b.c
*/
void		free_token_node(t_node *node);

/*
Command finder function.
In cmd_finder.c
*/
t_node		*init_find_cmd(t_node *start, t_vars *vars);
t_node		*mode_action(t_node *current, t_node **last_cmd, t_vars *vars);
void		reset_find_cmd(t_vars *vars, t_node *start, t_node *target,
				int mode);
t_node		*find_cmd(t_node *start, t_node *target, int mode, t_vars *vars);

/*
DEBUG FUNCTIONS
In debug.c
*/
void		setup_debug_flags(void);
void		print_node_content(FILE *fp, t_node *node);
void		print_ast_node(FILE *fp, t_node *node, int indent_level);
void		print_ast(t_node *root, const char *filename);
void		print_token_list(t_node *head, const char *filename);

/*
Error handling.
In errormsg.c
*/
void		shell_error(char *element, int error_code, t_vars *vars);
void		tok_syntax_error_msg(char *token_str, t_vars *vars);
void		not_found_error(char *filename, t_vars *vars);
void		crit_error(t_vars *vars);
void		handle_fd_error(int fd, t_vars *vars, const char *error_msg);

/*
Pipe execution functions.
In execute_pipes.c
*/
void		exec_pipe_left(t_node *cmd_node, int pipe_fd[2], t_vars *vars);
void		exec_pipe_right(t_node *cmd_node, int pipe_fd[2], t_vars *vars);
int			fork_left_child(t_node *left_cmd, int pipe_fd[2], t_vars *vars,
				pid_t *left_pid_ptr);
int			init_pipe_exec(int pipe_fd[2], int *r_status_ptr,
				int *l_status_ptr);
int			execute_pipes(t_node *pipe_node, t_vars *vars);

/*
Redirect execution processing functions
In execute_redirects.c
*/
int			setup_in_redir(t_node *node, t_vars *vars);
int			setup_out_redir(t_node *node, t_vars *vars);
int			setup_heredoc_redir(t_node *node, t_vars *vars);
int			proc_redir_chain(t_node *start_node, t_node *cmd_node,
				t_vars *vars);

/*
Execution utility functions.
In execute_utils.c
*/
void		exec_child(char *cmd_path, char **args, char **envp);
int			setup_redirection(t_node *node, t_vars *vars);
int			proc_redir_target(t_node *node, t_vars *vars);
int			redir_mode_setup(t_node *node, t_vars *vars);
void		end_pipe_processes(t_vars *vars);

/*
Execution functions.
In execute.c
*/
int			handle_cmd_status(int status, t_vars *vars);
int			exec_redirect_cmd(t_node *node, char **envp, t_vars *vars);
int			exec_cmd_node(t_node *node, char **envp, t_vars *vars);
int			execute_cmd(t_node *node, char **envp, t_vars *vars);
int			exec_external_cmd(t_node *node, char **envp, t_vars *vars);

/*
Expansion utility functions
In expansion_utils.c
*/
char		*empty_var(char *var_name);

/*
Expansion handling.
In expansion.c
*/
char		*handle_special_var(const char *var_name, t_vars *vars);
char		*get_var_value(const char *var_name, t_vars *vars);
char		*expand_variable(char *input, int *pos, char *var_name,
				t_vars *vars);
char		*get_env_val(const char *var_name, char **env);
char		*get_var_name(char *input, int *pos);

/*
Heredoc delimiter handling utility functions.
*/
int			chk_quoted_delim(char *orig_delim, size_t len,
				char **clean_delim_ptr, int *quoted_ptr);
int			chk_normal_delim(char *orig_delim, size_t len,
				char **clean_delim_ptr, int *quoted_ptr);
void		store_cln_delim(t_vars *vars, char *clean_delim, int quoted);
char		*find_raw_delim(char *line_start, int len, const char *delim);		

/*
Heredoc delimiter handling functions.
*/
int			is_valid_delim(char *orig_delim, t_vars *vars);
t_node		*find_delim_token(t_node *head);
int			chk_hd_first_line(char *line_start, int len, t_vars *vars);
int			proc_hd_line(int write_fd, char *line, t_vars *vars);
void		strip_outer_quotes(char **delimiter, t_vars *vars);

/*
Heredoc expansion functions.
In heredoc_expansion.c
*/
char		*hd_expander(char *line, t_vars *vars);
char		*prepare_hd_line(char *line, t_vars *vars);
char		*expand_hd_var(char *line, int *pos, t_vars *vars);
char		*read_hd_str(char *line, int *pos);
int			write_to_hd(int fd, char *line, t_vars *vars);

/*
Heredoc main handling utility functions.
In heredoc_utils.c
*/
char		*merge_and_free(char *str, char *chunk);
void		cleanup_heredoc_fd(int write_fd);

/*
Heredoc main handling.
In heredoc.c
*/
int			interactive_hd_mode(t_vars *vars);
int			get_interactive_hd(int write_fd, t_vars *vars);
int			handle_heredoc(t_node *node, t_vars *vars);
int			process_heredoc(t_node *node, t_vars *vars);
int			read_tmp_buf(t_vars *vars);

/*
History loading functions.
In history_load.c
*/
void		read_history_lines(int fd);
void		load_history(void);

/*
History saving functions.
In history_save.c
*/
int			prepare_history_entries(HIST_ENTRY ***hist_list,
				int *history_count, int *start_idx);
void		save_history(void);
int			save_history_entries(int fd, HIST_ENTRY **hist_list,
				int start, int total);

/*
History utility functions.
In history_utils.c
*/
void		trim_history(int excess_lines);
int			chk_and_make_folder(const char *path);
int			copy_file_content(int fd_src, int fd_dst);
int			copy_file(const char *src, const char *dst);
int			copy_to_temp(int fd_read);

/*
History main functions.
In history.c
*/
void		skip_lines(int fd, int count);
int			init_history_fd(int mode);
int			get_history_count(void);

/*
Node initialization functions.
In initnode.c
*/
int			make_nodeframe(t_node *node, t_tokentype type, char *token);
void		set_quote_type(t_node *node, int quote_type);
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
char		*append_input(char *original, char *additional);
char		*fix_open_quotes(char *input, t_vars *vars);
char		*handle_quote_completion(char *cmd, t_vars *vars);
char		*complete_pipe_cmd(char *command, t_vars *vars);
int			handle_unfinished_pipes(char **processed_cmd, t_vars *vars);

/*
Input processing functions.
In input_handlers.c
*/
char		*allocate_and_read(int fd, size_t size);
char		*read_entire_file(const char *filename);
void		handle_input(char *input, t_vars *vars);
int			check_trailing_chars(const char *line, int start_pos);

/*
Lexer core functions.
*/
t_tokentype	get_token_at(char *input, int pos, int *moves);
int			tokenizer(char *input, t_vars *vars);
int			handle_token(char *input, t_vars *vars, int *hd_is_delim);
char		*get_quoted_str(char *input, t_vars *vars, int *quote_type);
int			process_operator_char(char *input, int *i, t_vars *vars);

/*
Make_exp_token utility functions.
In make_exp_token_utils.c
*/
int			new_exp_token(t_vars *vars, char *expanded_val, char *token);
int			proc_join_args(t_vars *vars, char *expanded_val);
int			handle_tok_join(char *input, t_vars *vars, char *expanded_val,
				char *token);
void		process_right_adj(char *input, t_vars *vars);
int			update_quote_types(t_vars *vars, int arg_idx, char *expanded_val);

/*
Make exp token functions.
In make_exp_token.c
*/
int			get_expn_name(char *input, t_vars *vars, char **token,
				char **var_name);
int			get_var_token(char *input, t_vars *vars, char **token,
				char **var_name);
int			addon_quo_type_arr(int *dest, int *src, int new_len);
int			sub_make_exp_token(char *input, t_vars *vars, char *expanded_val,
				char *token);
int			make_exp_token(char *input, t_vars *vars);

/*
Node array creation functions.
In make_node_arrays.c
*/
char		**setup_args(char *token);
int			**setup_quotes(int len);
void		make_node_arrays(t_node *node, char *token);

/*
Make quoted token functions.
In make_quo_token.c
*/
int			merge_quoted_token(char *input, char *content, t_vars *vars);
int			make_quoted_cmd(char *content, char *input, t_vars *vars);

/*
Token creation core functions.
In maketoken.c
*/
void		maketoken(char *token, t_tokentype type, t_vars *vars);
int			build_token_linklist(t_vars *vars, t_node *node);
void		token_link(t_node *node, t_vars *vars);
int			merge_arg_with_cmd(t_vars *vars, t_node *arg_node);
void		free_if_orphan_node(t_node *node, t_vars *vars);

/*
Minishell program entry point functions.
In minishell.c
*/
char		*reader(void);
void		build_and_execute(t_vars *vars);
int			handle_pipe_syntax(t_vars *vars);
void		process_command(char *command, t_vars *vars);
int			main(int ac, char **av, char **envp);

/*
Multiline input buffer handling functions
In multiline_input_buffer.c
*/
char		*extract_next_line(char **current_pos_ptr, t_vars *vars);
int			proc_hd_buffer(int fd, char *content, t_vars *vars);
int			process_standard(char *input, t_vars *vars);
int			open_hd_tmp_buf(t_vars *vars);
int			handle_interactive_hd(int fd, int found_in_buf, t_vars *vars);

/*
Multiline validation functions.
In multiline_validation.c
*/
int			tokenize_first_line(char *input, char *nl_ptr, t_vars *vars);
int			chk_hd_tail(char *line_start, char *raw_delim_ptr, char *delim,
				t_vars *vars);
char		*chk_raw_delim(char *line_start, int len, char *delim_arg,
				t_vars *vars);

/*
Operator handling utility functions.
In operators_utils.c.
*/
int			is_single_token(char *input, int pos, int *moves);
int			is_double_token(char *input, int pos, int *moves);

/*
Operator handling.
In operators.c
*/
int			is_operator_token(t_tokentype type);
void		handle_string(char *input, t_vars *vars);
int			handle_single_operator(char *input, t_vars *vars);
int			handle_double_operator(char *input, t_vars *vars);

/*
Parsing functions.
In parser.c
*/
void		set_token_type(t_vars *vars, char *input);
int			proc_hd_delim(char *input, t_vars *vars, int *hd_is_delim);
int			proc_opr_token(char *input, t_vars *vars, int *hd_is_delim,
				t_tokentype token_type);
int			finish_tokenizing(char *input, t_vars *vars, int hd_is_delim);
int			handle_quotes(char *input, t_vars *vars, int *adj_saved);

/*
Path finding utility functions.
In paths_utils.c
*/
char		*try_path(char *path, char *cmd);

/*
Path finding functions.
In paths.c
*/
char		**get_path_env(char **envp);
char		*search_in_env(char *cmd, char **envp, t_vars *vars);
char		*handle_direct_path(char *cmd, t_vars *vars);
char		*get_cmd_path(t_node *node, char **envp, t_vars *vars);
char		**dup_env(char **envp);

/*
Pipe analysis functions.
In pipe_analysis.c
*/
int			check_pipe_at_start(t_vars *vars);
int			check_consecutive_pipes(t_vars *vars);
int			check_pipe_completion_needed(t_vars *vars);
int			analyze_pipe_syntax(t_vars *vars);
int			finalize_pipes(t_vars *vars);

/*
Pipes main functions.
In pipes.c
*/
char		*read_until_complete(void);
int			append_to_cmdline(char **cmd_ptr, const char *addition);

/*
Process multiline_input functions.
In process_multiline_input.c
*/
int			process_multiline_input(char *input, t_vars *vars);
int			proc_first_line(char *input, char *first_line_end, t_vars *vars);
int			process_heredoc_path(char *input, char *first_line_end,
				char *content_start, t_vars *vars);
void		exec_first_line(char *input, char *nl_ptr, t_vars *vars);
int			hd_proc_end(int fd, char *input, char *nl_ptr, t_vars *vars);

/*
Pipeline processing functions.
In process_pipes.c
*/
void		setup_pipe_node(t_node *pipe_node, t_node *left_cmd,
				t_node *right_cmd);
t_node		*process_first_pipe(t_vars *vars);
void		process_addon_pipes(t_vars *vars);

/*
Process quote character utility functions
In process_quote_char_utils.c
*/
int			token_cleanup_error(char *content, t_vars *vars);
void		cleanup_and_process_adj(char *content, char *input, t_vars *vars);
t_node		*find_last_redir(t_vars *vars);
int			validate_single_redir(t_node *redir_node, t_vars *vars);

/*
Process quote characters.
In process_quote_char.c
*/
t_node		*process_quoted_str(char **content_ptr, int quote_type,
				t_vars *vars);
void		link_file_to_redir(t_node *redir_node, t_node *file_node,
				t_vars *vars);
int			handle_redir_target(char *content, t_vars *vars);
int			process_quote_char(char *input, t_vars *vars, int is_redir_target);
int			validate_redir_targets(t_vars *vars);

/*
Process redirection nodes functions.
In process_redir_node.c
*/
void		link_prev_redirs(t_node *redir_node, t_node *cmd, t_vars *vars);
void		track_redirs(t_node *redir_node, t_node *cmd, t_vars *vars);
void		link_in_out_redirs(t_vars *vars);
t_node		*get_redir_target(t_node *current, t_node *last_cmd);
void		upd_pipe_redir(t_node *pipe_root, t_node *cmd, t_node *redir);

/*
Redirection processing utility functions.
In process_redirect_utils.c
*/
void		reset_redir_tracking(t_pipe *pipes);
int			is_valid_redir_node(t_node *current);
t_node		*find_cmd_redirection(t_node *redir_root, t_node *cmd_node,
				t_vars *vars);

/*
Redirection processing functions.
In process_redirect.c
*/
int			chk_redir_nodes(t_vars *vars, t_node *current);
t_node		*find_redir_chain_head(t_node *current, t_node *last_cmd);
void		link_redirs_pipes(t_vars *vars);
void		set_redir_node(t_node *redir, t_node *cmd, t_node *target);

/*
Handles expansion in quotes.
In quoted_expansion.c
*/
char		*expand_quoted_str(char *content, t_vars *vars);
char		*expand_quoted_var(char *expanded, char *content, int *pos,
				t_vars *vars);
char		*append_basic_strs(char *expanded, char *content, int *pos);
char		*append_substr(char *dest, char *src, int start, int len);

/*
Quote handling.
In quotes.c
*/
void		track_quote_ctx(char quote_char, char *in_quote, int pos,
				t_vars *vars);
int			validate_quotes(char *input, t_vars *vars);
char		*quote_prompt(char quote_type);

/*
Redirection handling.
In redirect.c
*/
int			is_redirection(t_tokentype type);
void		restore_fd(int *saved_fd_ptr, int target_fd);
void		reset_pipe_redir_state(t_pipe *pipes);
void		reset_redirect_fds(t_vars *vars);
t_node		*get_next_redir(t_node *current, t_node *cmd);

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
Terminal state modification functions.
In terminal.c
*/
void		term_heredoc(t_vars *vars);
void		manage_terminal_state(t_vars *vars, int action);
void		restore_terminal_fd(int target_fd, int source_fd, int mode);
void		reset_terminal_after_heredoc(void);
void		setup_terminal_mode(t_vars *vars);

/*
Temporary buffer reader and processing.
In tmp_buf_reader.c
*/
int			init_read_buf(t_read_buf *rb);
void		cleanup_rd_buf(t_read_buf *rb, int remove_tmp);
void		update_quote_state(char c, t_read_buf *rb);
void		process_buffer_command(t_read_buf *rb, t_vars *vars);
void		tmp_buf_reader(t_vars *vars);

/*
Tokenizing utility functions.
In tokenize_utils.c
*/
int			chk_move_pos(t_vars *vars, int hd_is_delim);
void		handle_right_adj(char *input, t_vars *vars);
char		*get_delim_str(char *input, t_vars *vars, int *error_code);
void		handle_text(char *input, t_vars *vars);

/*
Tokenizing functions.
In tokenize.c
*/
int			process_input_tokens(char *command, t_vars *vars);

void		tokenize_quote(char *input, t_vars *vars);
void		tokenize_expan(char *input, t_vars *vars);
void		tokenize_white(char *input, t_vars *vars);
void		init_tokenizer(t_vars *vars);

/*
Type conversion functions.
In typeconvert.c
*/
char		*get_token_str(t_tokentype type);

#endif
