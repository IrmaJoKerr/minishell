/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   execute_pipes.c                                    :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: bleow <bleow@student.42kl.edu.my>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/04/25 23:05:19 by bleow             #+#    #+#             */
/*   Updated: 2025/05/28 02:46:42 by bleow            ###   ########.fr       */
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
	int cmd_result;
	int redir_success;
	t_node *actual_cmd;

	fprintf(stderr, "PIPE-DBG-LEFT: === LEFT PROCESS START ===\n");
	fprintf(stderr, "PIPE-DBG-LEFT: Node type=%s, args[0]='%s'\n", 
			get_token_str(cmd_node->type),
			cmd_node->args ? cmd_node->args[0] : "NULL");

	// Close read end of pipe - we're only writing to the pipe
	if (close(pipe_fd[0]) == -1)
		fprintf(stderr, "PIPE-DBG-LEFT: Error closing read end: %s\n", strerror(errno));
	else
		fprintf(stderr, "PIPE-DBG-LEFT: Closed read end fd=%d\n", pipe_fd[0]);

	// Setup pipe redirection for stdout
	fprintf(stderr, "PIPE-DBG-LEFT: Redirecting stdout to pipe write end fd=%d\n", pipe_fd[1]);
	if (dup2(pipe_fd[1], STDOUT_FILENO) == -1) {
		fprintf(stderr, "PIPE-DBG-LEFT: dup2 failed: %s\n", strerror(errno));
		close(pipe_fd[1]);
		return 1;
	}
	
	if (close(pipe_fd[1]) == -1)
		fprintf(stderr, "PIPE-DBG-LEFT: Error closing write end: %s\n", strerror(errno));
	else
		fprintf(stderr, "PIPE-DBG-LEFT: Closed write end fd=%d after dup2\n", pipe_fd[1]);

	// Process redirections based on node type
	if (is_redirection(cmd_node->type)) {
		fprintf(stderr, "PIPE-DBG-LEFT: Node is redirection type=%s\n", 
				get_token_str(cmd_node->type));
		
		// Find actual command in redirection->left
		if (cmd_node->left && cmd_node->left->type == TYPE_CMD) {
			actual_cmd = cmd_node->left;
			fprintf(stderr, "PIPE-DBG-LEFT: Found command in left: '%s'\n",
					actual_cmd->args ? actual_cmd->args[0] : "NULL");
		} else {
			fprintf(stderr, "PIPE-DBG-LEFT: ERROR: Missing command in redirection->left\n");
			fprintf(stderr, "PIPE-DBG-LEFT: Left node type=%s\n",
					cmd_node->left ? get_token_str(cmd_node->left->type) : "NULL");
			return 1;
		}
		
		fprintf(stderr, "PIPE-DBG-LEFT: Processing redirection chain\n");
		redir_success = proc_redir_chain(cmd_node, actual_cmd, vars);
		
		if (!redir_success) {
			fprintf(stderr, "PIPE-DBG-LEFT: Redirection failed, error_code=%d\n", 
					vars->error_code);
			
			// Special case: Test if a file exists but has invalid permissions
			if (vars->error_code == ERR_PERMISSIONS) {
				fprintf(stderr, "PIPE-DBG-LEFT: Permission error detected\n");
				return vars->error_code; // Return actual error
			}
			return 0; // Other redirection errors should still allow pipeline to continue
		}
	} else if (cmd_node->type == TYPE_CMD) {
		fprintf(stderr, "PIPE-DBG-LEFT: Direct command node: '%s'\n", 
				cmd_node->args ? cmd_node->args[0] : "NULL");
		actual_cmd = cmd_node;
	} else {
		fprintf(stderr, "PIPE-DBG-LEFT: Unexpected node type: %s\n",
				get_token_str(cmd_node->type));
		return 1;
	}

	// Check if redirections are attached to this command
	if (actual_cmd->redir) {
		fprintf(stderr, "PIPE-DBG-LEFT: Command has attached redirections\n");
		redir_success = proc_redir_chain(actual_cmd->redir, actual_cmd, vars);
		if (!redir_success) {
			fprintf(stderr, "PIPE-DBG-LEFT: Attached redirection failed, error_code=%d\n", 
					vars->error_code);
			return vars->error_code;
		}
	}
	
	fprintf(stderr, "PIPE-DBG-LEFT: Executing command '%s'\n",
			actual_cmd->args ? actual_cmd->args[0] : "NULL");
	
	cmd_result = execute_cmd(actual_cmd, vars->env, vars);
	
	fprintf(stderr, "PIPE-DBG-LEFT: Command execution completed with status %d\n", cmd_result);
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
int exec_pipe_right(t_node *cmd_node, int pipe_fd[2], t_vars *vars)
{
	int cmd_result;
	int redir_success;
	t_node *actual_cmd;

	fprintf(stderr, "PIPE-DBG-RIGHT: === RIGHT PROCESS START ===\n");
	fprintf(stderr, "PIPE-DBG-RIGHT: Node type=%s, args[0]='%s'\n", 
			get_token_str(cmd_node->type),
			cmd_node->args ? cmd_node->args[0] : "NULL");

	// Setup pipe redirection for stdin
	fprintf(stderr, "PIPE-DBG-RIGHT: Redirecting stdin from pipe read end fd=%d\n", pipe_fd[0]);
	if (dup2(pipe_fd[0], STDIN_FILENO) == -1) {
		fprintf(stderr, "PIPE-DBG-RIGHT: dup2 failed: %s\n", strerror(errno));
		close(pipe_fd[0]);
		return 1;
	}

	if (close(pipe_fd[0]) == -1)
		fprintf(stderr, "PIPE-DBG-RIGHT: Error closing read end after dup2: %s\n", strerror(errno));
	else
		fprintf(stderr, "PIPE-DBG-RIGHT: Closed read end fd=%d after dup2\n", pipe_fd[0]);

	// Process redirections based on node type
	if (is_redirection(cmd_node->type)) {
		fprintf(stderr, "PIPE-DBG-RIGHT: Node is redirection type=%s\n", 
				get_token_str(cmd_node->type));
		
		// Find actual command in redirection->left
		if (cmd_node->left && cmd_node->left->type == TYPE_CMD) {
			actual_cmd = cmd_node->left;
			fprintf(stderr, "PIPE-DBG-RIGHT: Found command in left: '%s'\n",
					actual_cmd->args ? actual_cmd->args[0] : "NULL");
		} else {
			fprintf(stderr, "PIPE-DBG-RIGHT: ERROR: Missing command in redirection->left\n");
			fprintf(stderr, "PIPE-DBG-RIGHT: Left node type=%s\n",
					cmd_node->left ? get_token_str(cmd_node->left->type) : "NULL");
			return 1;
		}
		
		fprintf(stderr, "PIPE-DBG-RIGHT: Processing redirection chain\n");
		redir_success = proc_redir_chain(cmd_node, actual_cmd, vars);
		
		if (!redir_success) {
			fprintf(stderr, "PIPE-DBG-RIGHT: Redirection failed, error_code=%d\n", 
					vars->error_code);
			return vars->error_code;
		}
	} else if (cmd_node->type == TYPE_CMD) {
		fprintf(stderr, "PIPE-DBG-RIGHT: Direct command node: '%s'\n", 
				cmd_node->args ? cmd_node->args[0] : "NULL");
		actual_cmd = cmd_node;
	} else if (cmd_node->type == TYPE_PIPE) {
		// CRITICAL FIX: Handle nested pipes by recursively executing them
		fprintf(stderr, "PIPE-DBG-RIGHT: Found nested pipe, executing recursively\n");
		cmd_result = execute_pipes(cmd_node, vars);
		fprintf(stderr, "PIPE-DBG-RIGHT: Nested pipe execution completed with status %d\n", cmd_result);
		return cmd_result;
	} else {
		fprintf(stderr, "PIPE-DBG-RIGHT: Unexpected node type: %s\n",
				get_token_str(cmd_node->type));
		return 1;
	}

	// Check if redirections are attached to this command
	if (actual_cmd->redir) {
		fprintf(stderr, "PIPE-DBG-RIGHT: Command has attached redirections\n");
		redir_success = proc_redir_chain(actual_cmd->redir, actual_cmd, vars);
		if (!redir_success) {
			fprintf(stderr, "PIPE-DBG-RIGHT: Attached redirection failed, error_code=%d\n", 
					vars->error_code);
			return vars->error_code;
		}
	}

	// Check stdout file descriptor
	int stdout_fileno = fileno(stdout);
	fprintf(stderr, "PIPE-DBG-RIGHT: Current stdout fileno=%d\n", stdout_fileno);
	
	// Check if stdout is pointing to a terminal
	if (isatty(STDOUT_FILENO))
		fprintf(stderr, "PIPE-DBG-RIGHT: stdout is connected to a terminal\n");
	else
		fprintf(stderr, "PIPE-DBG-RIGHT: stdout is NOT connected to a terminal\n");
	
	fprintf(stderr, "PIPE-DBG-RIGHT: Executing command '%s'\n",
			actual_cmd->args ? actual_cmd->args[0] : "NULL");
	
	cmd_result = execute_cmd(actual_cmd, vars->env, vars);
	
	fprintf(stderr, "PIPE-DBG-RIGHT: Command execution completed with status %d\n", cmd_result);
	return cmd_result;
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
int execute_pipes(t_node *pipe_node, t_vars *vars)
{
	int     pipe_fd[2];
	pid_t   left_pid, right_pid;
	int     status, left_status, right_status;
	
	fprintf(stderr, "DEBUG-PIPELINE: === PIPELINE EXECUTION START ===\n");
	fprintf(stderr, "DEBUG-PIPELINE: %p Left type=%s Right type=%s\n", 
			(void*)pipe_node,
			pipe_node->left ? get_token_str(pipe_node->left->type) : "NULL",
			pipe_node->right ? get_token_str(pipe_node->right->type) : "NULL");
	
	// Setup pipe
	if (pipe(pipe_fd) == -1) {
		perror("pipe");
		vars->error_code = 1;
		return 1;
	}
	
	fprintf(stderr, "DEBUG-PIPELINE: read_fd=%d, write_fd=%d\n", pipe_fd[0], pipe_fd[1]);
	
	// Execute left command
	left_pid = fork();
	if (left_pid == -1) {
		perror("fork (left)");
		close(pipe_fd[0]);
		close(pipe_fd[1]);
		return 1;
	}
	
	if (left_pid == 0) {
		// Child process for left command
		int exit_code = exec_pipe_left(pipe_node->left, pipe_fd, vars);
		fprintf(stderr, "DEBUG-PIPELINE: Left process exiting with status %d\n", exit_code);
		exit(exit_code);
	}
	
	// Parent: close write end after fork
	close(pipe_fd[1]);
	
	// Execute right command
	right_pid = fork();
	if (right_pid == -1) {
		perror("fork (right)");
		close(pipe_fd[0]);
		kill(left_pid, SIGTERM);
		waitpid(left_pid, NULL, 0);
		return 1;
	}
	
	if (right_pid == 0) {
		// Child process for right command
		int exit_code = exec_pipe_right(pipe_node->right, pipe_fd, vars);
		fprintf(stderr, "DEBUG-PIPELINE: Right process exiting with status %d\n", exit_code);
		exit(exit_code);
	}
	
	// Parent: close read end after fork
	close(pipe_fd[0]);
	
	// Wait for processes
	fprintf(stderr, "DEBUG-PIPELINE: Waiting for left process (pid=%d)\n", left_pid);
	waitpid(left_pid, &left_status, 0);
	fprintf(stderr, "DEBUG-PIPELINE: Left process exited with status %d\n", 
			WIFEXITED(left_status) ? WEXITSTATUS(left_status) : 128 + WTERMSIG(left_status));
	
	fprintf(stderr, "DEBUG-PIPELINE: Waiting for right process (pid=%d)\n", right_pid);
	waitpid(right_pid, &right_status, 0);
	fprintf(stderr, "DEBUG-PIPELINE: Right process exited with status %d\n", 
			WIFEXITED(right_status) ? WEXITSTATUS(right_status) : 128 + WTERMSIG(right_status));
	
	// CRITICAL FIX: ALWAYS use the right side's status, regardless of left side failures
	status = right_status;
	
	// Extract and set the actual exit code
	if (WIFEXITED(status)) {
		vars->error_code = WEXITSTATUS(status);
		fprintf(stderr, "DEBUG-PIPELINE: Setting pipeline exit code to %d (from right side)\n", vars->error_code);
	} else if (WIFSIGNALED(status)) {
		vars->error_code = 128 + WTERMSIG(status);
		fprintf(stderr, "DEBUG-PIPELINE: Setting pipeline exit code to %d (signal+128)\n", vars->error_code);
	}
	else
	{
		vars->error_code = status;
	}
	
	return vars->error_code;
}