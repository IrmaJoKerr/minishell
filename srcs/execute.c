/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   execute.c                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: bleow <bleow@student.42kl.edu.my>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/03/01 22:26:13 by bleow             #+#    #+#             */
/*   Updated: 2025/05/28 10:42:41 by bleow            ###   ########.fr       */
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
	{
		vars->error_code = exit_code;
	}
	return (exit_code);
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
	int		is_pipeline_context;

	fprintf(stderr, "DEBUG-EXEC-REDIR: Starting redirection execution for node type=%s\n", 
			get_token_str(node->type));

	if (!node->left)
	{
		fprintf(stderr, "DEBUG-EXEC-REDIR: Missing left node, returning error\n");
		return (1);
	}

	cmd_node = node->left;
	is_pipeline_context = vars->pipes->in_pipe;
	
	fprintf(stderr, "DEBUG-EXEC-REDIR: Found command node: '%s' (pipeline_context=%d)\n",
			cmd_node && cmd_node->args ? cmd_node->args[0] : "NULL", is_pipeline_context);

	// Save original file descriptors
	vars->pipes->saved_stdin = dup(STDIN_FILENO);
	vars->pipes->saved_stdout = dup(STDOUT_FILENO);
	
	fprintf(stderr, "DEBUG-EXEC-REDIR: Saved stdin=%d, stdout=%d\n",
			vars->pipes->saved_stdin, vars->pipes->saved_stdout);

	// Process redirection chain - check the return value
	fprintf(stderr, "DEBUG-EXEC-REDIR: About to process redirection chain\n");
	
	if (!proc_redir_chain(node, cmd_node, vars))
	{
		fprintf(stderr, "DEBUG-EXEC-REDIR: Redirection failed, cleaning up and returning error_code=%d\n",
				vars->error_code);
		fprintf(stderr, "DEBUG-EXEC-REDIR-FIX: NOT executing command due to redirection failure\n");
		reset_redirect_fds(vars);
		reset_terminal_after_heredoc();
		
		// NEW: In pipeline context with input errors, exit with 0 to let pipeline continue
		if (is_pipeline_context && vars->error_code == 1)  // Likely input redirection error
		{
			fprintf(stderr, "DEBUG-EXEC-REDIR-FIX: Pipeline context - exiting with 0 for input redirection error\n");
			return (0);  // Let pipeline continue
		}
		
		return (vars->error_code != 0 ? vars->error_code : 1);
	}
	
	// Only execute command if proc_redir_chain succeeded
	fprintf(stderr, "DEBUG-EXEC-REDIR: Redirection successful, executing command '%s'\n",
			cmd_node && cmd_node->args ? cmd_node->args[0] : "NULL");
	result = execute_cmd(cmd_node, envp, vars);
	
	fprintf(stderr, "DEBUG-EXEC-REDIR: Command execution completed with result=%d\n", result);

	// Clean up
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
	int	prev_error;
	// Save current error code to detect redirection errors
	prev_error = vars->error_code;
	result = 0;
	if (!node)
		return (vars->error_code = 1);
	if (node->type == TYPE_CMD)
	{
		result = exec_cmd_node(node, envp, vars);
	}
	else if (is_redirection(node->type))
	{
		result = exec_redirect_cmd(node, envp, vars);
	}
	else if (node->type == TYPE_PIPE)
	{
		if (!node->left || !node->right)
			return (vars->error_code = 1);
		result = execute_pipes(node, vars);
	}
	else
		result = 1;
	// Preserve redirection errors that occurred before command execution
	if (prev_error == ERR_REDIRECTION)
		vars->error_code = prev_error;
	vars->error_code = result;
	return (vars->error_code);
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
