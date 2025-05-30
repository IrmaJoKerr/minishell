/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   forking.c                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: bleow <bleow@student.42kl.edu.my>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/05/30 12:14:46 by bleow             #+#    #+#             */
/*   Updated: 2025/05/30 12:27:33 by bleow            ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../includes/minishell.h"

/*
Creates child process for left side of a pipe command.
- Sets up process to write output to pipe
- Handles fork errors with appropriate cleanup
- Child process executes the command and exits
- Parent process closes write end and continues
Returns:
- Process ID of child on success
- (-1) on fork failure
*/
pid_t	fork_left_pipe_branch(t_node *node, int pipe_fd[2], t_vars *vars)
{
	pid_t	pid;

	pid = fork();
	if (pid == -1)
	{
		perror("fork (left)");
		close(pipe_fd[0]);
		close(pipe_fd[1]);
		return (-1);
	}
	if (pid == 0)
	{
		exit(exec_pipe_left(node, pipe_fd, vars));
	}
	close(pipe_fd[1]);
	return (pid);
}

/*
Creates child process for right side of a pipe command.
- Sets up process to read input from pipe
- Handles fork errors with cleanup of both pipe ends
- Terminates left process if already running when error occurs
- Child process executes the command and exits
- Parent process closes read end and continues
Returns:
- Process ID of child on success
- (-1) on fork failure
*/
pid_t	fork_right_pipe_branch(t_node *node, int pipe_fd[2], t_vars *vars,
			pid_t left_child_pid)
{
	pid_t	pid;

	pid = fork();
	if (pid == -1)
	{
		perror("fork (right)");
		close(pipe_fd[0]);
		if (left_child_pid > 0)
		{
			kill(left_child_pid, SIGTERM);
			waitpid(left_child_pid, NULL, 0);
		}
		return (-1);
	}
	if (pid == 0)
		exit(exec_pipe_right(node, pipe_fd, vars));
	close(pipe_fd[0]);
	return (pid);
}
