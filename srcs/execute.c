/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   execute.c                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: bleow <bleow@student.42kl.edu.my>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/03/01 22:26:13 by bleow             #+#    #+#             */
/*   Updated: 2025/04/29 16:20:14 by bleow            ###   ########.fr       */
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

	if (DEBUG_EXEC) //DEBUG PRINT
		fprintf(stderr, "In handle_cmd_status().Processing command status: %d\n", status); //DEBUG PRINT
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
	if (DEBUG_EXEC) //DEBUG PRINT
		fprintf(stderr, "In handle_cmd_status().Final exit code: %d\n", exit_code); //DEBUG PRINT
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

	if (node->args && node->args[0]) //DEBUG PRINT
	{ //DEBUG PRINT
		if (DEBUG_EXEC && node->args && node->args[0]) //DEBUG PRINT
		{ //DEBUG PRINT
			fprintf(stderr, "In exec_cmd_node().Executing command: %s\n", node->args[0]); //DEBUG PRINT
			fprintf(stderr, "In exec_cmd_node(). Is builtin: %s\n", //DEBUG PRINT
				is_builtin(node->args[0]) ? "yes" : "no"); //DEBUG PRINT
		} //DEBUG PRINT
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
	if (DEBUG_EXEC) //DEBUG PRINT
		fprintf(stderr, "In exec_cmd_node().Command result: %d\n", result); //DEBUG PRINT
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
	if (DEBUG_ERROR) //DEBUG PRINT
		fprintf(stderr, "[DEBUG] In execute_cmd(%s) before start: error_code=%d\n", //DEBUG PRINT
			node->args ? node->args[0] : "NULL", vars->error_code); //DEBUG PRINT
	if (DEBUG_AST) //DEBUG_PRINT
	{ //DEBUG_PRINT
		fprintf(stderr, "In execute_cmd().Executing node type: %s\n", get_token_str(node->type));  //DEBUG_PRINT
		if (node->args && node->args[0]) //DEBUG_PRINT
			fprintf(stderr, "Command: %s\n", node->args[0]); //DEBUG_PRINT
	} //DEBUG_PRINT
	
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
	if (DEBUG_ERROR) //DEBUG PRINT
		fprintf(stderr, "[DEBUG] In execute_cmd() at end: error_code=%d\n", vars->error_code);
	return (result);
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
	if (DEBUG_EXEC) //DEBUG PRINT
	{ //DEBUG PRINT
		fprintf(stderr, "In exec_external_cmd().External command execution:\n"); //DEBUG PRINT
		fprintf(stderr, "  Command: %s\n", node->args[0]); //DEBUG PRINT
		fprintf(stderr, "  Path: %s\n", cmd_path ? cmd_path : "(not found)"); //DEBUG PRINT
	} //DEBUG PRINT
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
	if (DEBUG_EXEC) //DEBUG PRINT
		fprintf(stderr, "In exec_external_cmd().Command exit code: %d\n", return_code); //DEBUG PRINT
	if (DEBUG_ERROR) //DEBUG PRINT
		fprintf(stderr, "[DEBUG] In exec_external_cmd(%s) set error_code=%d\n", 
			node->args[0], return_code);
	return (return_code);
}
