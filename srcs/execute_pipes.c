/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   execute_pipes.c                                    :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: bleow <bleow@student.42kl.edu.my>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/04/25 23:05:19 by bleow             #+#    #+#             */
/*   Updated: 2025/05/29 17:12:20 by bleow            ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../includes/minishell.h"

/* 
Scans a command node for any attached redirections (e.g., cmd > file)
and processes them.

Returns:
- 0 on success or if no redirections are attached.
- vars->error_code if set by proc_redir_chain due to an error.
- 1 if proc_redir_chain fails and vars->error_code is not set.
*/
int	scan_cmd_redirs(t_node *cmd_node, t_vars *vars)
{
	int	redir_success;

	if (cmd_node && cmd_node->redir)
	{
		redir_success = proc_redir_chain(cmd_node->redir, vars);
		if (!redir_success)
		{
			if (vars->error_code)
				return (vars->error_code);
			else
				return (1);
		}
	}
	return (0);
}

/*
Processes the first encountered redirection node, linking the correct command
to it and executing the redirection.

Returns:
- 0 on success.
- 1 if the node to the left of redir_node is not a command, or if
  proc_redir_chain fails and vars->error_code is not set.
- vars->error_code if set by proc_redir_chain due to an error.
*/
int	proc_first_redir(t_node *redir_node, t_vars *vars, t_node **cmd_out)
{
	int	redir_success;

	*cmd_out = redir_node->left;
	if (!(*cmd_out) || (*cmd_out)->type != TYPE_CMD)
		return (1);
	redir_success = proc_redir_chain(redir_node, vars);
	if (!redir_success)
	{
		if (vars->error_code)
			return (vars->error_code);
		else
			return (1);
	}
	return (0);
}

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
