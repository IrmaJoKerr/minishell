/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   execute_pipes.c                                    :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: bleow <bleow@student.42kl.edu.my>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/04/25 23:05:19 by bleow             #+#    #+#             */
/*   Updated: 2025/05/05 04:09:25 by bleow            ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../includes/minishell.h"

/*
Handles execution within the left child process of a pipe.
- Closes unused pipe read end.
- Redirects stdout to the pipe write end.
- Closes the pipe write end.
- Executes the command node.
- Exits with the command's status.
*/
void	exec_pipe_left(t_node *cmd_node, int pipe_fd[2], t_vars *vars)
{
	close(pipe_fd[0]);
	if (dup2(pipe_fd[1], STDOUT_FILENO) == -1)
	{
		perror("dup2 (left child)");
		exit(EXIT_FAILURE);
	}
	close(pipe_fd[1]);
	exit(execute_cmd(cmd_node, vars->env, vars));
}

/*
Handles execution within the right child process of a pipe.
- Closes unused pipe write end (already done by parent before fork).
- Redirects stdin to the pipe read end.
- Closes the pipe read end.
- Executes the command node.
- Exits with the command's status.
*/
void	exec_pipe_right(t_node *cmd_node, int pipe_fd[2], t_vars *vars)
{
	if (dup2(pipe_fd[0], STDIN_FILENO) == -1)
	{
		perror("dup2 (right child)");
		exit(EXIT_FAILURE);
	}
	close(pipe_fd[0]);
	exit(execute_cmd(cmd_node, vars->env, vars));
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
	int		r_status;
	int		l_status;

	if (init_pipe_exec(pipe_fd, &r_status, &l_status))
		return (1);
	if (fork_left_child(pipe_node->left, pipe_fd, vars, &left_pid))
		return (1);
	right_pid = fork();
	if (right_pid == -1)
	{
		ft_putendl_fd("fork: Creation failed (right)", 2);
		close(pipe_fd[0]);
		waitpid(left_pid, &l_status, 0);
		return (1);
	}
	if (right_pid == 0)
		exec_pipe_right(pipe_node->right, pipe_fd, vars);
	close(pipe_fd[0]);
	waitpid(left_pid, &l_status, 0);
	waitpid(right_pid, &r_status, 0);
	return (handle_cmd_status(r_status, vars));
}
