/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   execute.c                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: bleow <bleow@student.42kl.edu.my>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/03/01 22:26:13 by bleow             #+#    #+#             */
/*   Updated: 2025/04/10 22:46:37 by bleow            ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../includes/minishell.h"

/*
Handles command execution status and updates error code.
- Processes exit status from waitpid() for child processes.
- For normal exits, stores the exit code (0-255) directly.
- For signals, adds 128 to the signal number (POSIX standard).
Returns:
The final error code stored in vars->error_code.
Works with exec_child_cmd() and execute_pipes().
*/
int handle_cmd_status(int status, t_vars *vars)
{
	int exit_code;
	
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
	return exit_code;
}

/*
Handles redirection setup for output files.
- Opens file for writing in truncate or append mode.
- Redirects stdout to the opened file.
- Properly handles and reports errors.
Returns:
1 on success, 0 on failure.
Works with setup_redirection().
*/
int	setup_out_redir(t_node *node, t_vars *vars)
{
	char	*file;
	int		flags;
	
	if (!node->right || !node->right->args || !node->right->args[0])
		return (0);
	flags = O_WRONLY | O_CREAT;
	file = node->right->args[0];
	if (vars->pipes->out_mode == OUT_MODE_APPEND)
		flags |= O_APPEND;
	else
		flags |= O_TRUNC;
	vars->pipes->redirection_fd = open(file, flags, 0644);
	if (vars->pipes->redirection_fd == -1)
	{
		shell_error(file, ERR_PERMISSIONS, vars);
		return (0);
	}
	if (dup2(vars->pipes->redirection_fd, STDOUT_FILENO) == -1)
	{
		close(vars->pipes->redirection_fd);
		return (0);
	}
	return (1);
}

/*
Handles redirection setup for input files.
- Opens file for reading.
- Redirects stdin to read from the file.
- Properly handles and reports errors.
Returns:
1 on success, 0 on failure.
Works with setup_redirection().
*/
int	setup_in_redir(t_node *node, t_vars *vars)
{
	char	*file;
	int		i;
	
	if (!node->right || !node->right->args || !node->right->args[0])
	{
		return (0);
	}
	file = node->right->args[0];
	vars->pipes->redirection_fd = open(file, O_RDONLY);
	if (vars->pipes->redirection_fd == -1)
	{
		not_found_error(file);
		if (vars->pipes->pids)
		{
			i = 0;
			while (i < vars->pipes->pipe_count)
			{
				if (vars->pipes->pids[i] > 0)
					kill(vars->pipes->pids[i], SIGTERM);
				i++;
			}
		}
		vars->error_code = 1;
		return (0);
	}
	if (dup2(vars->pipes->redirection_fd, STDIN_FILENO) == -1)
	{
		close(vars->pipes->redirection_fd);
		return (0);
	}
	return (1);
}

/*
Sets up appropriate redirection based on node type.
- Handles all redirection types (input, output, append, heredoc).
- Creates or opens files with appropriate permissions.
- Redirects stdin/stdout as needed.
Returns:
1 on success, 0 on failure.
Works with exec_redirect_cmd().
*/
int setup_redirection(t_node *node, t_vars *vars)
{
	reset_redirect_fds(vars);
	vars->pipes->current_redirect = node;
	if (node->right && node->right->args)
		process_arg_quotes(&node->right->args[0]);
	fprintf(stderr, "DEBUG: Setting up redirection type=%d operator='%s' file='%s'\n",
		node->type, 
		node->args ? node->args[0] : "(null)",
		(node->right && node->right->args) ? node->right->args[0] : "(null)");
	if (node->type == TYPE_IN_REDIRECT)
	{
		if (!setup_in_redir(node, vars))
		{
			vars->error_code = 1;
			return (0);
		}
	}
	else if (node->type == TYPE_OUT_REDIRECT)
	{
		vars->pipes->out_mode = OUT_MODE_TRUNCATE;
		if (!setup_out_redir(node, vars))
		{
			vars->error_code = 1;
			return (0);
		}
	}
	else if (node->type == TYPE_APPEND_REDIRECT)
	{
		vars->pipes->out_mode = OUT_MODE_APPEND;
		if (!setup_out_redir(node, vars))
		{
			vars->error_code = 1;
			return (0);
		}
	}
	else if (node->type == TYPE_HEREDOC)
	{
		if (!node->right || !node->right->args || !node->right->args[0])
		{
			vars->error_code = 1;
			return (0);
		}
		if (handle_heredoc(node, vars) == -1)
		{
			vars->error_code = 1;
			return (0);
		}
	}
	return (1);
}

/*
Executes a command with redirection.
- Saves original file descriptors.
- Sets up redirection according to node type.
- Executes the command with redirection in place.
- Restores original file descriptors afterward.
Returns:
Result of command execution.
Works with execute_cmd().
*/
int	exec_redirect_cmd(t_node *node, char **envp, t_vars *vars)
{
	int	saved_stdout;
	int	saved_stdin;
	int	fd;
	int	result;

	if (!node->left || !node->right)
		return (1);
	saved_stdout = dup(STDOUT_FILENO);
	saved_stdin = dup(STDIN_FILENO);
	fd = -1;
	if (!setup_redirection(node, vars))
		return (1);
	result = execute_cmd(node->left, envp, vars);
	dup2(saved_stdout, STDOUT_FILENO);
	dup2(saved_stdin, STDIN_FILENO);
	cleanup_fds(saved_stdout, saved_stdin);
	if (fd > 2)
		close(fd);
	return (result);
}

/*
Executes a child process for external commands.
- Forks a child process.
- In child: executes the external command.
- In parent: waits for child and processes exit status.
Returns:
Exit code from the command execution.
Works with execute_cmd().
*/
int	exec_child_cmd(t_node *node, char **envp, t_vars *vars, char *cmd_path)
{
	pid_t	pid;
	int		status;

	pid = fork();
	if (pid == 0)
	{
		if (execve(cmd_path, node->args, envp) == -1)
		{
			perror("bleshell: exec");
			exit(1);
		}
	}
	else if (pid < 0)
	{
		perror("bleshell: fork");
		free(cmd_path);
		return (1);
	}
	else
	{
		waitpid(pid, &status, 0);
		free(cmd_path);
		return (handle_cmd_status(status, vars));
	}
	return (0);
}

int	exec_std_cmd(t_node *node, char **envp, t_vars *vars)
{
	char	*cmd_path;
	int		i;

	if (!node->args || !node->args[0])
		return (1);
	i = 0;
	while (node->args[i])
		i++;
	if (is_builtin(node->args[0]))
		return (execute_builtin(node->args[0], node->args, vars));
	cmd_path = get_cmd_path(node->args[0], envp);
	if (!cmd_path)
	{
		ft_putstr_fd("bleshell: command not found: ", 2);
		ft_putendl_fd(node->args[0], 2);
		vars->error_code = 0;
		return (vars->error_code);
	}
	return (exec_child_cmd(node, envp, vars, cmd_path));
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
	
	result = 0;
	if (!node)
	{
		DBG_PRINTF(DEBUG_EXEC, "execute_cmd: NULL node\n");
		return (vars->error_code = 1);
	}
	DBG_PRINTF(DEBUG_EXEC, "execute_cmd: Node type=%d, content='%s'\n", 
	   node->type, node->args ? node->args[0] : "NULL");
	//Handle different node types
	if (node->type == TYPE_CMD)
	{
		// Handle regular command
		result = exec_std_cmd(node, envp, vars);
	}
	else if (is_redirection(node->type))
	{
		DBG_PRINTF(DEBUG_EXEC, "Executing redirection - left child: %p, right child: %p\n",
		   (void*)node->left, (void*)node->right);
		if (node->left)
		   DBG_PRINTF(DEBUG_EXEC, "Left child type=%d content=%s\n", 
			   node->left->type, node->left->args ? node->left->args[0] : "NULL");
		if (node->right)
		   DBG_PRINTF(DEBUG_EXEC, "Right child type=%d content=%s\n", 
			   node->right->type, node->right->args ? node->right->args[0] : "NULL");
		// Add heredoc-specific debugging
		if (node->type == TYPE_HEREDOC) {
		   DBG_PRINTF(DEBUG_EXEC, "Executing heredoc with delimiter: '%s', heredoc_fd=%d\n",
			   node->right && node->right->args ? node->right->args[0] : "NULL",
			   vars->pipes->heredoc_fd);
		}
		result = exec_redirect_cmd(node, envp, vars);
	}
	else if (node->type == TYPE_PIPE)
	{
		// For pipe nodes, validate they have both left and right children
		if (!node->left || !node->right) {
			//DBG_PRINTF(DEBUG_EXEC, "execute_cmd: Invalid pipe node (missing child)\n");
			return (vars->error_code = 1);
		}
		result = execute_pipes(node, vars);
	} 
	else
	{
		// Unhandled node type
		//DBG_PRINTF(DEBUG_EXEC, "execute_cmd: Unhandled node type %d\n", node->type);
		result = 1;
	}
	// Ensure error code is set in vars
	vars->error_code = result;
	return result;
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
	char	*cmd_path;

	if (!node || !node->args || !node->args[0])
	{
		// Set error code consistently for invalid arguments case
		vars->error_code = 1;
		return vars->error_code;
	}
	// Get command path
	cmd_path = get_cmd_path(node->args[0], envp);
	if (!cmd_path)
	{
		ft_putstr_fd("bleshell: ", 2);
		ft_putstr_fd(node->args[0], 2);
		ft_putendl_fd(": command not found", 2);
		return (vars->error_code = 0);
	}

	// Fork and execute
	pid = fork();
	if (pid < 0)
	{
		ft_putstr_fd("bleshell: fork failed\n", 2);
		free(cmd_path);
		return (vars->error_code = 1);
	}
	
	// Child process
	if (pid == 0)
	{
		execve(cmd_path, node->args, envp);
		// If execve returns, an error occurred
		perror("bleshell");
		free(cmd_path);
		exit(127);
	}
	
	// Parent process
	free(cmd_path);
	waitpid(pid, &status, 0);
	
	// Handle exit status and update vars->error_code
	return (handle_cmd_status(status, vars));
}
