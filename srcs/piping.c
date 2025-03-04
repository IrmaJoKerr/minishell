/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   piping.c                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: bleow <bleow@student.42kl.edu.my>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/03/01 21:39:57 by bleow             #+#    #+#             */
/*   Updated: 2025/03/04 13:58:50 by bleow            ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../includes/minishell.h"

/*
Prepares pipe redirections for a command type node.
1) Checks if the command is linked to a pipe type node.
2) Closes the pipe READ END (pipe_fd[0]) to avoid descriptor leaks
3) Redirects STDOUT (fd 1) to pipe WRITE END using dup2():
   - dup2(pipe_fd[1], STDOUT_FILENO) makes STDOUT point to the same FD
   as pipe_fd[1], sending command output flowing through pipe
4) Closes the original WRITE END (pipe_fd[1]) after redirection
   to prevent descriptor leaks
5) Dup2() connects the command's STDOUT to the pipe, enabling data flow from one
command to next command through pipe.
Used during command execution for proper I/O streams for piped commands.
Works with execute_cmd() and execute_pipeline().
*/
void	init_pipe(t_node *cmd, int *pipe_fd)
{
	if (cmd->type == TYPE_PIPE)
	{
		close(pipe_fd[0]);
		dup2(pipe_fd[1], STDOUT_FILENO);
		close(pipe_fd[1]);
	}
}

/*
Executes commands connected by a pipe node in the AST.
Piping command example: (cmd1 | cmd2)
1) Creates a pipe with two file descriptors:
   - pipefd[0]: READ END
   - pipefd[1]: WRITE END
2) Forking a child process to execute the left command:
   - Child process closes READ END (pipefd[0])
   - Child redirects STDOUT to pipe's WRITE END using dup2()
   - Child closes WRITE END after redirection
   - Child executes left side of pipe (pipe_node->left)
   - Data starts flowing to right side command through pipe
   - Child exits after command execution
3) Parent process:
   - Closes the WRITE END (pipefd[1]) to avoid descriptor leaks
   - Redirects STDIN to pipe's READ END using dup2()
   - Closes the READ END after redirection
   - Executes right side of the pipe (pipe_node->right)
The dup2() calls create the pipeline effect by connecting:
- Left command's stdout → pipe → Right command's stdin
Returns: Exit status of the rightmost command in the pipeline,
or 1 on error (pipe creation failure, NULL parameters).
Works with execute_cmd() in a recursive manner for multiple pipelines.
Example: ls | grep txt.c | wc -l
*/
int	execute_pipeline(t_node *pipe_node, char **envp, t_vars *vars)
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
