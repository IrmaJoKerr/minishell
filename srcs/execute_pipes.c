/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   execute_pipes.c                                    :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: bleow <bleow@student.42kl.edu.my>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/04/25 23:05:19 by bleow             #+#    #+#             */
/*   Updated: 2025/05/30 13:10:42 by bleow            ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../includes/minishell.h"

/*
Prepares a command node for execution within a pipe by:
- Resolving the actual command if node_in_pipe is a redirection.
- Handling nested pipes if in the RPIPE context.
- Processing any redirections attached directly to the resolved command.

Returns:
- 0: Success, cmd_to_exec is set to a command node.
- 1: General error during setup or invalid node type.
- 2: Nested pipe detected (if lt_or_rt_side is RPIPE), cmd_to_exec
	 is set to the TYPE_PIPE node.
- Other non-zero values: vars->error codes from redirection processing.
*/
int	setup_pipe_cmd(t_node *node_in_pipe, t_vars *vars,
					t_node **cmd_to_exec, int lt_or_rt_side)
{
	t_node	*curr;
	int		status;

	curr = NULL;
	if (is_redirection(node_in_pipe->type))
	{
		status = proc_first_redir(node_in_pipe, vars, &curr);
		if (status != 0)
			return (status);
	}
	else if (node_in_pipe->type == TYPE_CMD)
		curr = node_in_pipe;
	else if (lt_or_rt_side == RPIPE && node_in_pipe->type == TYPE_PIPE)
	{
		*cmd_to_exec = node_in_pipe;
		return (2);
	}
	else
		return (1);
	status = scan_cmd_redirs(curr, vars);
	if (status != 0)
		return (status);
	*cmd_to_exec = curr;
	return (0);
}

/*
Handles execution of the command on the left side of a pipe.
- Redirects STDOUT of the child process to the write-end of the pipe.
- Closes unused pipe file descriptors.
- Prepares and executes the command node using setup_pipe_cmd and execute_cmd.

Returns:
- The exit status code of the executed command.
- 1 if a setup error (e.g., dup2 failure) occurs.
- Propagated error codes from setup_pipe_cmd.
*/
int	exec_pipe_left(t_node *cmd_node, int pipe_fd[2], t_vars *vars)
{
	int		cmd_code;
	t_node	*curr;
	int		setup_status;

	close(pipe_fd[0]);
	if (dup2(pipe_fd[1], STDOUT_FILENO) == -1)
	{
		close(pipe_fd[1]);
		return (1);
	}
	close(pipe_fd[1]);
	setup_status = setup_pipe_cmd(cmd_node, vars, &curr, LPIPE);
	if (setup_status != 0)
		return (setup_status);
	cmd_code = execute_cmd(curr, vars->env, vars);
	return (cmd_code);
}

/*
Handles execution within the right child process of a pipe.
- Redirects stdin to the pipe read end.
- Closes the pipe read end.
- Executes the command node.
Returns:
- The exit status code that should be used when exiting the process.
*/
int	exec_pipe_right(t_node *cmd_node, int pipe_fd[2], t_vars *vars)
{
	int		cmd_code;
	t_node	*curr;
	int		setup_status;

	if (dup2(pipe_fd[0], STDIN_FILENO) == -1)
	{
		close(pipe_fd[0]);
		return (1);
	}
	close(pipe_fd[0]);
	setup_status = setup_pipe_cmd(cmd_node, vars, &curr, RPIPE);
	if (setup_status == 2)
	{
		cmd_code = execute_pipes(curr, vars);
		return (cmd_code);
	}
	else if (setup_status != 0)
		return (setup_status);
	cmd_code = execute_cmd(curr, vars->env, vars);
	return (cmd_code);
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
*/
int	execute_pipes(t_node *pipe_node, t_vars *vars)
{
	int		pipe_fd[2];
	pid_t	left_pid;
	pid_t	right_pid;
	int		left_status;
	int		right_status;

	if (pipe(pipe_fd) == -1)
	{
		perror("pipe");
		vars->error_code = 1;
		return (1);
	}
	left_pid = fork_left_pipe_branch(pipe_node->left, pipe_fd, vars);
	if (left_pid == -1)
		return (vars->error_code = 1);
	right_pid = fork_right_pipe_branch(pipe_node->right, pipe_fd, vars,
			left_pid);
	if (right_pid == -1)
		return (vars->error_code = 1);
	waitpid(left_pid, &left_status, 0);
	waitpid(right_pid, &right_status, 0);
	handle_cmd_status(right_status, vars);
	return (vars->error_code);
}

/*
Executes a solo redirection by creating or modifying the target file.
- Validates that the redirection node has a valid filename argument.
- Creates files with standard permissions (0644) if they don't exist.
Used by proc_solo_redirs() to immediately execute orphaned redirections
that appear between pipes and commands (e.g., "cmd1 | >file cmd2").

Returns:
- 1 on success (file created/modified successfully).
- 0 on failure (invalid arguments, unsupported redirection type, or file error).
*/
int	exec_solo_redir(t_node *redir_node, t_vars *vars)
{
	char	*filename;
	int		fd;

	if (!redir_node->args || !redir_node->args[0])
		return (0);
	filename = redir_node->args[0];
	fd = -1;
	if (!chk_permissions(filename, O_WRONLY, vars))
		return (0);
	if (redir_node->type == TYPE_OUT_REDIR)
		fd = open(filename, O_WRONLY | O_CREAT | O_TRUNC, 0644);
	else if (redir_node->type == TYPE_APPD_REDIR)
		fd = open(filename, O_WRONLY | O_CREAT | O_APPEND, 0644);
	else
		return (0);
	if (fd == -1)
	{
		shell_error(filename, ERR_PERMISSIONS, vars);
		return (0);
	}
	close(fd);
	return (1);
}
