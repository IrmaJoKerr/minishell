/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   execute.c                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: bleow <bleow@student.42kl.edu.my>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/03/01 22:26:13 by bleow             #+#    #+#             */
/*   Updated: 2025/04/25 14:55:47 by bleow            ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../includes/minishell.h"

/*
Handles command execution status and updates error code.
- Processes exit status from waitpid() for child processes.
- For normal exits, stores the exit code (0-255) directly.
- For signals, adds 128 to the signal number (POSIX standard).
Returns:
- The final error code stored in vars->error_code.
Works with exec_external_cmd() and execute_pipes().
*/
int	handle_cmd_status(int status, t_vars *vars)
{
	int	exit_code;

	exit_code = 0;
	if (WIFEXITED(status))
	{
		exit_code = WEXITSTATUS(status);
	}
	else if (WIFSIGNALED(status))
	{
		exit_code = 128 + WTERMSIG(status);
	}
	if (vars)
		vars->error_code = exit_code;
	return (exit_code);
}

/*
Handles redirection setup for input files.
- Opens file for reading.
- Redirects stdin to read from the file.
- Properly handles and reports errors.
Returns:
- 1 on success.
- 0 on failure.
Works with setup_redirection().
*/
int	setup_in_redir(t_node *node, t_vars *vars)
{
	char	*file;

	if (!node->right || !node->right->args || !node->right->args[0])
		return (0);
	file = node->right->args[0];
	if (!chk_permissions(file, O_RDONLY, vars))
	{
		end_pipe_processes(vars);
		return (0);
	}
	vars->pipes->redirection_fd = open(file, O_RDONLY);
	if (vars->pipes->redirection_fd == -1)
	{
		not_found_error(file, vars);
		end_pipe_processes(vars);
		return (0);
	}
	if (dup2(vars->pipes->redirection_fd, STDIN_FILENO) == -1)
	{
		close(vars->pipes->redirection_fd);
		return (0);
	}
	return (1);
}

/*
Handles redirection setup for output files.
- Opens file for writing in truncate or append mode.
- Redirects stdout to the opened file.
- Properly handles and reports errors.
Returns:
- 1 on success.
- 0 on failure.
Works with setup_redirection().
*/
int	setup_out_redir(t_node *node, t_vars *vars)
{
	char	*file;
	int		flags;

	if (!node->right || !node->right->args || !node->right->args[0])
		return (0);
	flags = O_WRONLY | O_CREAT;
	file = node->right->args[0];
	if (vars->pipes->out_mode == OUT_MODE_APPEND)
		flags |= O_APPEND;
	else
		flags |= O_TRUNC;
	if (!chk_permissions(file, flags, vars))
		return (0);
	vars->pipes->redirection_fd = open(file, flags, 0644);
	if (vars->pipes->redirection_fd == -1)
	{
		shell_error(file, ERR_PERMISSIONS, vars);
		return (0);
	}
	if (dup2(vars->pipes->redirection_fd, STDOUT_FILENO) == -1)
	{
		close(vars->pipes->redirection_fd);
		return (0);
	}
	return (1);
}

/*
Sets up heredoc redirection handling.
- Checks if heredoc content is ready.
- Initiates interactive mode if end delimiter not found.
- Handles the actual heredoc setup via handle_heredoc().
Returns:
- 1 on success
- 0 on failure
Works with redir_mode_setup().
*/
int	setup_heredoc_redir(t_node *node, t_vars *vars)
{
	int	result;

	result = 1;
	if (!vars->hd_text_ready)
	{
		if (!interactive_hd_mode(vars))
			return (0);
	}
	result = handle_heredoc(node, vars);
	return (result);
}

/*
Terminates all active child processes in a pipeline.
- Safely iterates through the PID array.
- Sends SIGTERM to each valid child process.
- Prevents orphaned processes during error conditions.
Works with setup_in_redir() and other error handling paths.
*/
void	end_pipe_processes(t_vars *vars)
{
	int	i;

	if (vars->pipes->pids)
	{
		i = 0;
		while (i < vars->pipes->pipe_count)
		{
			if (vars->pipes->pids[i] > 0)
				kill(vars->pipes->pids[i], SIGTERM);
			i++;
		}
	}
}

/*
Sets up a specific type of redirection based on node type.
- Handles input, output, append, and heredoc redirections.
- Updates mode flags and calls appropriate setup functions.
- Centralizes error handling for all redirection types.
- Triggers interactive heredoc gathering if content is not ready.
Returns:
- 1 on success.
- 0 on failure (with error_code set).
Works with setup_redirection().
*/
int	redir_mode_setup(t_node *node, t_vars *vars)
{
	int	result;

	result = 0;
	if (node->type == TYPE_IN_REDIRECT)
		result = setup_in_redir(node, vars);
	else if (node->type == TYPE_OUT_REDIRECT)
	{
		vars->pipes->out_mode = OUT_MODE_TRUNCATE;
		result = setup_out_redir(node, vars);
	}
	else if (node->type == TYPE_APPEND_REDIRECT)
	{
		vars->pipes->out_mode = OUT_MODE_APPEND;
		result = setup_out_redir(node, vars);
	}
	else if (node->type == TYPE_HEREDOC)
		result = setup_heredoc_redir(node, vars);
	if (!result)
		vars->error_code = ERR_DEFAULT;
	return (result);
}

/*
Validates redirection target and processes quotes.
- Checks if redirection target exists
- Removes quotes from target if needed
- Reports syntax error if target is missing
Returns:
- 1 if target is valid
- 0 if target is invalid (with error_code set)
*/
int	proc_redir_target(t_node *node, t_vars *vars)
{
	if (node->right && node->right->args && node->right->args[0])
	{
		if (node->type != TYPE_HEREDOC)
			strip_outer_quotes(&node->right->args[0], vars);
		return (1);
	}
	else if (node->type != TYPE_HEREDOC)
	{
		tok_syntax_error_msg("newline", vars);
		return (0);
	}
	return (1);
}

/*
Sets up appropriate redirection based on node type.
- Handles all redirection types (input, output, append, heredoc).
- Creates or opens files with appropriate permissions.
- Redirects stdin/stdout as needed.
Returns:
- 1 on success.
- 0 on failure.
Works with exec_redirect_cmd().
*/
int	setup_redirection(t_node *node, t_vars *vars)
{
	int		result;
	t_node	*cmd_node;

	vars->pipes->current_redirect = node;
	cmd_node = find_cmd(vars->head, node, FIND_PREV, vars);
	if (!cmd_node)
	{
		vars->error_code = ERR_DEFAULT;
		return (0);
	}
	vars->pipes->cmd_redir = cmd_node;
	if (node->type == TYPE_IN_REDIRECT || node->type == TYPE_HEREDOC)
		vars->pipes->last_in_redir = node;
	else if (node->type == TYPE_OUT_REDIRECT
		|| node->type == TYPE_APPEND_REDIRECT)
	{
		vars->pipes->last_out_redir = node;
	}
	if (!proc_redir_target(node, vars))
		return (0);
	result = redir_mode_setup(node, vars);
	return (result);
}

/*
Processes a chain of redirections for a command node.
- Sets up each redirection in the chain.
- Navigates through connected redirections.
- Handles errors and cleanup if a redirection fails.
Returns:
- 1 on success (all redirections processed)
- 0 on failure (at least one redirection failed)
Works with exec_redirect_cmd().
*/
int	proc_redir_chain(t_node *start_node, t_node *cmd_node, t_vars *vars)
{
	t_node	*current_node;
	t_node	*next_redir;

	current_node = start_node;
	while (current_node && is_redirection(current_node->type))
	{
		vars->pipes->current_redirect = current_node;
		if (!setup_redirection(current_node, vars))
		{
			reset_redirect_fds(vars);
			return (0);
		}
		if (current_node->redir)
			current_node = current_node->redir;
		else
		{
			next_redir = get_next_redir(current_node, cmd_node);
			if (next_redir)
				current_node = next_redir;
			else
				break ;
		}
	}
	return (1);
}

/*
Executes a command with redirection.
- Saves original file descriptors.
- Sets up redirection according to node type.
- Executes the command with redirection in place.
- Restores original file descriptors afterward.
Returns:
- Result of command execution.
Works with execute_cmd().
*/
int	exec_redirect_cmd(t_node *node, char **envp, t_vars *vars)
{
	int		result;
	t_node	*cmd_node;

	if (!node->left || !node->right)
		return (1);
	cmd_node = node->left;
	while (cmd_node && is_redirection(cmd_node->type))
		cmd_node = cmd_node->left;
	vars->pipes->saved_stdin = dup(STDIN_FILENO);
	vars->pipes->saved_stdout = dup(STDOUT_FILENO);
	if (!proc_redir_chain(node, cmd_node, vars))
		return (1);
	result = execute_cmd(cmd_node, envp, vars);
	reset_redirect_fds(vars);
	reset_terminal_after_heredoc();
	return (result);
}

/*
Executes a single command node.
- Handles both builtin and external commands.
- Manages empty command error cases.
- Updates error code appropriately.
Returns:
- Command execution result code.
Works with execute_cmd().
*/
int	exec_cmd_node(t_node *node, char **envp, t_vars *vars)
{
	int	result;

	if (node->args && node->args[0])
	{
		if (is_builtin(node->args[0]))
			result = execute_builtin(node->args[0], node->args, vars);
		else
			result = exec_external_cmd(node, envp, vars);
	}
	else
	{
		vars->error_code = 1;
		result = 1;
	}
	return (result);
}

/*
Main command execution function.
- Handles all command types (builtin, external, redirections, pipes).
- Ensures consistent error code handling in vars->error_code.
Returns:
Exit code which is also stored in vars->error_code.
*/
int	execute_cmd(t_node *node, char **envp, t_vars *vars)
{
	int	result;

	result = 0;
	if (!node)
		return (vars->error_code = 1);
	if (node->type == TYPE_CMD)
		result = exec_cmd_node(node, envp, vars);
	else if (is_redirection(node->type))
		result = exec_redirect_cmd(node, envp, vars);
	else if (node->type == TYPE_PIPE)
	{
		if (!node->left || !node->right)
			return (vars->error_code = 1);
		result = execute_pipes(node, vars);
	}
	else
		result = 1;
	vars->error_code = result;
	return (result);
}

/*
Handles command execution in the child process.
- Attempts to execute the command with execve
- On failure, determines appropriate error type from errno
- Reports errors using shell_error
- Cleans up resources and exits with code 127
Note: This function never returns (it either executes or exits)
*/
void	exec_child(char *cmd_path, char **args, char **envp)
{
	int	error_code;

	execve(cmd_path, args, envp);
	if (errno == EACCES)
		error_code = ERR_PERMISSIONS;
	else if (errno == ENOENT)
		error_code = ERR_CMD_NOT_FOUND;
	else if (errno == EISDIR)
		error_code = ERR_ISDIRECTORY;
	else
		error_code = ERR_CMD_NOT_FOUND;
	shell_error(cmd_path, error_code, NULL);
	free(cmd_path);
	exit(127);
}

/*
Executes an external command (non-builtin).
- Finds the command path in the PATH environment.
- Forks a child process to execute the command.
- In parent: waits for child and handles the exit status.
- Properly updates vars->error_code with the command result.
Returns:
Exit code from the command execution.
Works with execute_cmd().

Example: For "ls -la"
- Locates path to ls executable (/bin/ls)
- Executes in child process
- Returns exit code (0 for success)
*/
int	exec_external_cmd(t_node *node, char **envp, t_vars *vars)
{
	pid_t	pid;
	int		status;
	int		return_code;
	char	*cmd_path;

	cmd_path = get_cmd_path(node, envp, vars);
	if (!cmd_path)
		return (vars->error_code);
	pid = fork();
	if (pid < 0)
	{
		free(cmd_path);
		return (vars->error_code = 1);
	}
	if (pid == 0)
		exec_child(cmd_path, node->args, envp);
	free(cmd_path);
	waitpid(pid, &status, 0);
	return_code = handle_cmd_status(status, vars);
	return (return_code);
}
