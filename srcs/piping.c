/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   piping.c                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: bleow <bleow@student.42kl.edu.my>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/03/01 21:39:57 by bleow             #+#    #+#             */
/*   Updated: 2025/03/02 17:35:24 by bleow            ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "includes\minishell.h"

/*
Prepares the pipe infrastructure for execution.
*/
void init_pipe(t_node *cmd, int *pipe_fd)
{
	if (cmd->type == TYPE_PIPE)
	{
		close(pipe_fd[0]);
		dup2(pipe_fd[1], STDOUT_FILENO);
		close(pipe_fd[1]);
	}
}

/*
Executes the pipeline.
*/
void execute_pipeline(t_node *pipe_node, char **envp, t_vars *vars)
{
	int		pipefd[2];
	pid_t	pid;

	if (!pipe_node || !envp)
		return (1);
	if (pipe(pipefd) == -1)
	{
		vars->error_code = 1;
		return (1);
	}
	pid = fork();
	if (pid == 0)
	{
		close(pipefd[0]);
		dup2(pipefd[1], STDOUT_FILENO);
		close(pipefd[1]);
		execute_cmd(pipe_node->left, envp, vars);
		exit(0);
	}
	close(pipefd[1]);
	dup2(pipefd[0], STDIN_FILENO);
	close(pipefd[0]);
	return (execute_cmd(pipe_node->right, envp, vars));
}
