/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   execute_pipes.c                                    :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: bleow <bleow@student.42kl.edu.my>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/04/25 23:05:19 by bleow             #+#    #+#             */
/*   Updated: 2025/05/28 20:11:22 by bleow            ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../includes/minishell.h"

/*
Handles execution within the left child process of a pipe.
- Closes unused pipe read end.
- Redirects stdout to the pipe write end.
- Closes the pipe write end.
- Executes the command node.
Returns:
- The exit status code that should be used when exiting the process
*/
int	exec_pipe_left(t_node *cmd_node, int pipe_fd[2], t_vars *vars)
{
	int		cmd_result;
	int		redir_success;
	// t_node	*cmd_node;

	if (close(pipe_fd[0]) == -1)
		close(pipe_fd[0]);
	// Setup pipe redirection for stdout
	if (dup2(pipe_fd[1], STDOUT_FILENO) == -1) {
		close(pipe_fd[1]);
		return (1);
	}
	if (close(pipe_fd[1]) == -1)
		close(pipe_fd[1]);
	// Process redirections based on node type
	if (is_redirection(cmd_node->type)) {
		// Find actual command in redirection->left
		if (cmd_node->left && cmd_node->left->type == TYPE_CMD)
		{
			cmd_node = cmd_node->left;
		}
		else
		{
			return 1;
		}
		redir_success = proc_redir_chain(cmd_node, cmd_node, vars);
		if (!redir_success)
		{
			// Special case: Test if a file exists but has invalid permissions
			if (vars->error_code == ERR_PERMISSIONS) {
				return vars->error_code; // Return actual error
			}
			return 0; // Other redirection errors should still allow pipeline to continue
		}
	} else if (cmd_node->type == TYPE_CMD) {
		cmd_node = cmd_node;
	} else {
		return 1;
	}

	// Check if redirections are attached to this command
	if (cmd_node->redir) {
		redir_success = proc_redir_chain(cmd_node->redir, cmd_node, vars);
		if (!redir_success) {
			return vars->error_code;
		}
	}
	
	cmd_result = execute_cmd(cmd_node, vars->env, vars);
	
	return cmd_result;
}

/*
Handles execution within the right child process of a pipe.
- Redirects stdin to the pipe read end.
- Closes the pipe read end.
- Executes the command node.
Returns:
- The exit status code that should be used when exiting the process
*/
int	exec_pipe_right(t_node *cmd_node, int pipe_fd[2], t_vars *vars)
{
	int		cmd_result;
	int		redir_success;

	if (dup2(pipe_fd[0], STDIN_FILENO) == -1)
	{
		close(pipe_fd[0]);
		return (1);
	}
	if (close(pipe_fd[0]) == -1)
		close(pipe_fd[0]);
	if (is_redirection(cmd_node->type))
	{
		if (cmd_node->left && cmd_node->left->type == TYPE_CMD)
			cmd_node = cmd_node->left;
		else
			return (1);
		redir_success = proc_redir_chain(cmd_node, cmd_node, vars);
		if (!redir_success)
			return (vars->error_code);
	}
	else if (cmd_node->type == TYPE_CMD)
		cmd_node = cmd_node;
	else if (cmd_node->type == TYPE_PIPE)
	{
		cmd_result = execute_pipes(cmd_node, vars);
		return (cmd_result);
	}
	else
		return (1);
	if (cmd_node->redir)
	{
		redir_success = proc_redir_chain(cmd_node->redir, cmd_node, vars);
		if (!redir_success)
			return (vars->error_code);
	}
	cmd_result = execute_cmd(cmd_node, vars->env, vars);
	return (cmd_result);
}

/*
Creates and sets up the left child process for a pipe.
Returns:
- 0 on successful fork
- 1 on fork failure (with appropriate cleanup)
Note: If successful, closes pipe_fd[1] in the parent process.
*/
int	fork_left_child(t_node *left_cmd, int pipe_fd[2], t_vars *vars
				, pid_t *left_pid_ptr)
{
	*left_pid_ptr = fork();
	if (*left_pid_ptr == -1)
	{
		ft_putendl_fd("fork: Creation failed (left)", 2);
		close(pipe_fd[0]);
		close(pipe_fd[1]);
		return (1);
	}
	if (*left_pid_ptr == 0)
	{
		exec_pipe_left(left_cmd, pipe_fd, vars);
	}
	close(pipe_fd[1]);
	pipe_fd[1] = -1;
	return (0);
}

/*
 * Initializes pipe execution variables and creates the pipe.
 * Parameters:
 * - pipe_fd: Array to store the created pipe file descriptors
 * - r_status_ptr: Pointer to the r_status variable to initialize
 * - l_status_ptr: Pointer to the l_status variable to initialize
 * Returns:
 * - 0 on successful initialization and pipe creation
 * - 1 on pipe creation failure (with appropriate error message)
 */
int	init_pipe_exec(int pipe_fd[2], int *r_status_ptr, int *l_status_ptr)
{
	*r_status_ptr = 0;
	*l_status_ptr = 0;
	if (pipe(pipe_fd) == -1)
	{
		ft_putendl_fd("pipe: Creation failed", 2);
		return (1);
	}
	return (0);
}

/*
Executes commands connected by a pipe.
- Sets up and launches pipeline child processes.
- Waits for both processes to complete.
- Captures status of command execution.
Returns:
- Status of right command.
- 1 on error.
Works with execute_cmd() for pipeline execution.

Example: For "ls -l | grep txt"
- Sets up pipeline processes
- Waits for both commands to complete
- Returns final execution status
*/
int	execute_pipes(t_node *pipe_node, t_vars *vars)
{
	int		pipe_fd[2];
	pid_t	left_pid;
	pid_t	right_pid;
	int		status;
	int		left_status;
	int		right_status;
	int		exit_code;

	if (pipe(pipe_fd) == -1)
	{
		perror("pipe");
		vars->error_code = 1;
		return (1);
	}
	left_pid = fork();
	if (left_pid == -1)
	{
		perror("fork (left)");
		close(pipe_fd[0]);
		close(pipe_fd[1]);
		return (1);
	}
	if (left_pid == 0)
	{
		exit_code = exec_pipe_left(pipe_node->left, pipe_fd, vars);
		exit(exit_code);
	}
	close(pipe_fd[1]);
	right_pid = fork();
	if (right_pid == -1)
	{
		perror("fork (right)");
		close(pipe_fd[0]);
		kill(left_pid, SIGTERM);
		waitpid(left_pid, NULL, 0);
		return (1);
	}
	if (right_pid == 0)
	{
		exit_code = exec_pipe_right(pipe_node->right, pipe_fd, vars);
		exit(exit_code);
	}
	close(pipe_fd[0]);
	waitpid(left_pid, &left_status, 0);
	waitpid(right_pid, &right_status, 0);
	status = right_status;
	if (WIFEXITED(status))
		vars->error_code = WEXITSTATUS(status);
	else if (WIFSIGNALED(status))
		vars->error_code = 128 + WTERMSIG(status);
	else
		vars->error_code = status;
	return (vars->error_code);
}
