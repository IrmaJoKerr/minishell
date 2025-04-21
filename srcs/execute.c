/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   execute.c                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: bleow <bleow@student.42kl.edu.my>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/03/01 22:26:13 by bleow             #+#    #+#             */
/*   Updated: 2025/04/21 17:35:50 by bleow            ###   ########.fr       */
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
Works with exec_child_cmd() and execute_pipes().
*/
int	handle_cmd_status(int status, t_vars *vars)
{
	int	exit_code;
	
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
	return (exit_code);
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
	
	if (!node->right || !node->right->args || !node->right->args[0])
		return (0);
	file = node->right->args[0];
	if (!chk_permissions(file, O_RDONLY, vars))
	{
		end_pipe_processes(vars);
		return (0);
	}
	vars->pipes->redirection_fd = open(file, O_RDONLY);
	if (vars->pipes->redirection_fd == -1)
	{
		not_found_error(file, vars);
		end_pipe_processes(vars);
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
Handles redirection setup for output files.
- Opens file for writing in truncate or append mode.
- Redirects stdout to the opened file.
- Properly handles and reports errors.
Returns:
- 1 on success.
- 0 on failure.
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
	if (!chk_permissions(file, flags, vars))
		return (0);
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
Terminates all active child processes in a pipeline.
- Safely iterates through the PID array.
- Sends SIGTERM to each valid child process.
- Prevents orphaned processes during error conditions.
Works with setup_in_redir() and other error handling paths.
*/
void	end_pipe_processes(t_vars *vars)
{
	int	i;

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
}

/*
Sets up a specific type of redirection based on node type.
- Handles input, output, append, and heredoc redirections.
- Updates mode flags and calls appropriate setup functions.
- Centralizes error handling for all redirection types.
Returns:
1 on success, 0 on failure (with error_code set).
Works with setup_redirection().
*/
int redir_mode_setup(t_node *node, t_vars *vars)
{
	int	result;
	
	if (node->type == TYPE_IN_REDIRECT)
		result = setup_in_redir(node, vars);
	else if (node->type == TYPE_OUT_REDIRECT)
	{
		vars->pipes->out_mode = OUT_MODE_TRUNCATE;
		result = setup_out_redir(node, vars);
	}
	else if (node->type == TYPE_APPEND_REDIRECT)
	{
		vars->pipes->out_mode = OUT_MODE_APPEND;
		result = setup_out_redir(node, vars);
	}
	else if (node->type == TYPE_HEREDOC)
	{
		fprintf(stderr, "[DBG_HEREDOC] redir_mode_setup(). Calling handle heredoc()\n");
		result = handle_heredoc(node, vars);
	}
	else
		result = 0;
	if (!result)
		vars->error_code = 1;
	return (result);
}

/*
Sets up appropriate redirection based on node type.
- Handles all redirection types (input, output, append, heredoc).
- Creates or opens files with appropriate permissions.
- Redirects stdin/stdout as needed.
Returns:
- 1 on success.
- 0 on failure.
Works with exec_redirect_cmd().
*/
// int	setup_redirection(t_node *node, t_vars *vars)
// {
// 	int	result;
	
// 	vars->pipes->current_redirect = node;
// 	if (node->right && node->right->args)
// 		process_arg_quotes(&node->right->args[0]);
// 	result = redir_mode_setup(node, vars);
// 	return (result);
// }
// int	setup_redirection(t_node *node, t_vars *vars)
// {
// 	int	result;
	
// 	vars->pipes->current_redirect = node;
// 	if (node->right && node->right->args)
// 		(void)strip_outer_quotes(&node->right->args[0]);
// 	result = redir_mode_setup(node, vars);
// 	return (result);
// }
// int setup_redirection(t_node *node, t_vars *vars)
// {
//     int	result;
    
//     vars->pipes->current_redirect = node;
//     if (node->right && node->right->args)
//     {
//         if (node->type == TYPE_HEREDOC)
//         {
//             // For heredoc, check if the delimiter is quoted and set expansion flag
//             int was_quoted = strip_outer_quotes(&node->right->args[0]);
//             if (was_quoted)
//             {
//                 // If quotes were removed, we should NOT expand variables in the heredoc
//                 vars->pipes->hd_expand = 0;
//                 vars->pipes->last_heredoc = node;
//                 fprintf(stderr, "[DBG_HEREDOC] setup_redirection: Found quoted heredoc delimiter, "
//                         "setting hd_expand=0\n");
//             }
//         }
//         else
//         {
//             // For other redirections, just strip quotes and ignore the return value
//             (void)strip_outer_quotes(&node->right->args[0]);
//         }
//     }
//     result = redir_mode_setup(node, vars);
//     return (result);
// }
// int setup_redirection(t_node *node, t_vars *vars)
// {
//     int result;
    
//     vars->pipes->current_redirect = node;
//     if (node->right && node->right->args)
//     {
//         if (node->type == TYPE_HEREDOC)
//         {
//             // Make a temporary copy of the delimiter for checking quotes
//             char *temp_copy = ft_strdup(node->right->args[0]);
//             if (!temp_copy)
//                 return (0);
                
//             // Check if the delimiter has quotes
//             int was_quoted = strip_outer_quotes(&temp_copy);
//             free(temp_copy); // Free temporary copy
            
//             if (was_quoted)
//             {
//                 // If quotes were present, disable expansion but DO NOT modify original delimiter
//                 vars->pipes->hd_expand = 0;
//                 vars->pipes->last_heredoc = node;
//                 fprintf(stderr, "[DBG_HEREDOC] setup_redirection: Found quoted heredoc delimiter, "
//                         "setting hd_expand=0\n");
//             }
//         }
//         else
//         {
//             // For other redirections, actually strip the quotes
//             (void)strip_outer_quotes(&node->right->args[0]);
//         }
//     }
    
//     result = redir_mode_setup(node, vars);
//     return (result);
// }
// int setup_redirection(t_node *node, t_vars *vars)
// {
//     int result;
    
//     vars->pipes->current_redirect = node;
//     if (node->right && node->right->args)
//     {
//         if (node->type == TYPE_HEREDOC)
//         {
//             // Check if delimiter appears quoted
//             char *delim = node->right->args[0];
//             size_t len = ft_strlen(delim);
//             int appears_quoted = (len >= 2 && ft_isquote(delim[0]) && 
//                                  delim[0] == delim[len-1]);
            
//             if (appears_quoted)
//             {
//                 // If quotes were present, disable expansion
//                 vars->pipes->hd_expand = 0;
//                 vars->pipes->last_heredoc = node;
//                 fprintf(stderr, "[DBG_HEREDOC] setup_redirection: Found quoted "
//                         "heredoc delimiter, setting hd_expand=0\n");
//             }
//         }
//         else
//         {
//             // For other redirections, actually strip the quotes
//             (void)strip_outer_quotes(&node->right->args[0]);
//         }
//     }
    
//     result = redir_mode_setup(node, vars);
//     return (result);
// }
int setup_redirection(t_node *node, t_vars *vars)
{
    int result;
    t_node *cmd_node;

    vars->pipes->current_redirect = node;

    // Find the command node associated with this redirection
    // Use find_cmd with FIND_PREV mode, searching from the head up to the current redirection node.
    cmd_node = find_cmd(vars->head, node, FIND_PREV, vars); // Corrected function call
    if (!cmd_node)
    {
        // It's possible for a redirection to be the first thing (e.g., < file cat)
        // In this case, find_cmd(FIND_PREV) might return NULL.
        // We need to decide how to handle this. Maybe find the *next* command?
        // Or perhaps the logic assumes a command always precedes or follows.
        // For now, let's assume a command should be found or it's an error/edge case.
        fprintf(stderr, "bleshell: Error: Command node not found relative to redirection.\n");
        vars->error_code = ERR_DEFAULT; // Set an error code
        return (0); // Indicate failure
    }
    vars->pipes->cmd_redir = cmd_node; // Store the target command node

    // Store the last redirection node encountered
    if (node->type == TYPE_IN_REDIRECT || node->type == TYPE_HEREDOC)
        vars->pipes->last_in_redir = node;
    else if (node->type == TYPE_OUT_REDIRECT || node->type == TYPE_APPEND_REDIRECT)
        vars->pipes->last_out_redir = node;


    if (node->right && node->right->args)
    {
        if (node->type == TYPE_HEREDOC)
        {
            // No action needed here regarding expansion flag or delimiter processing.
            fprintf(stderr, "[DBG_HEREDOC] setup_redirection: Processing heredoc node (expansion flag already set to %d)\n", vars->pipes->hd_expand);
        }
        else
        {
            // For other redirections (<, >, >>), strip quotes from the filename/target
            (void)strip_outer_quotes(&node->right->args[0]);
            fprintf(stderr, "[DEBUG] setup_redirection: Stripped quotes from target: '%s'\n", node->right->args[0]);
        }
    }
    // Check for missing filename/delimiter only for non-heredoc redirections
    else if (node->type != TYPE_HEREDOC)
    {
         fprintf(stderr, "bleshell: syntax error near unexpected token `newline'\n");
         vars->error_code = ERR_SYNTAX;
         return (0);
    }


    // Call the function that actually performs the dup2 based on type
    result = redir_mode_setup(node, vars);
    return (result);
}

/*
Processes a chain of redirections for a command node.
- Sets up each redirection in the chain.
- Navigates through connected redirections.
- Handles errors and cleanup if a redirection fails.
Returns:
- 1 on success (all redirections processed)
- 0 on failure (at least one redirection failed)
Works with exec_redirect_cmd().
*/
int	proc_redir_chain(t_node *start_node, t_node *cmd_node, t_vars *vars)
{
	t_node	*current_node;
	
	current_node = start_node;
	while (current_node && is_redirection(current_node->type))
	{
		vars->pipes->current_redirect = current_node;
		if (!setup_redirection(current_node, vars))
		{
			reset_redirect_fds(vars);
			return (0);
		}
		if (current_node->redir)
			current_node = current_node->redir;
		else
		{
			t_node *next_redir = get_next_redir(current_node, cmd_node);
			if (next_redir)
				current_node = next_redir;
			else
				break ;
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
	
	if (node->args && node->args[0])
	{
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
// int	exec_external_cmd(t_node *node, char **envp, t_vars *vars)
// {
// 	pid_t	pid;
// 	int		status;
// 	char	*cmd_path;

// 	if (!node || !node->args || !node->args[0])
// 	{
// 		vars->error_code = 1;
// 		return vars->error_code;
// 	}
// 	cmd_path = get_cmd_path(node->args[0], envp);
// 	if (!cmd_path)
// 	{
// 		ft_putstr_fd("bleshell: ", 2);
// 		ft_putstr_fd(node->args[0], 2);
// 		ft_putendl_fd(": command not found", 2);
// 		return (vars->error_code = 0);
// 	}
// 	fprintf(stderr, "[DEBUG] exec_external_cmd: Before fork, pid=%d\n", getpid());
// 	pid = fork();
// 	if (pid < 0)
// 	{
// 		ft_putstr_fd("bleshell: fork failed\n", 2);
// 		free(cmd_path);
// 		return (vars->error_code = 1);
// 	}
// 	if (pid == 0)
// 	{
// 		fprintf(stderr, "[DEBUG] exec_external_cmd: Child process pid=%d\n", getpid());
// 		execve(cmd_path, node->args, envp);
// 		perror("bleshell");
// 		free(cmd_path);
// 		exit(127);
// 	}
// 	fprintf(stderr, "[DEBUG] exec_external_cmd: Parent process pid=%d, waiting for child=%d\n", 
// 			getpid(), pid);
// 	free(cmd_path);
// 	waitpid(pid, &status, 0);
// 	fprintf(stderr, "[DEBUG] exec_external_cmd: Child %d exited with status=%d, WIFEXITED=%d\n",
// 			pid, status, WIFEXITED(status));
// 	return (handle_cmd_status(status, vars));
// }
// int	exec_external_cmd(t_node *node, char **envp, t_vars *vars)
// {
//     pid_t	pid;
//     int		status;
//     char	*cmd_path;
//     int		error_code;

//     if (!node || !node->args || !node->args[0])
//     {
//         vars->error_code = 1;
//         return vars->error_code;
//     }
//     cmd_path = get_cmd_path(node->args[0], envp);
//     if (!cmd_path)
//     {
//         shell_error(node->args[0], ERR_CMD_NOT_FOUND, vars);
//         return vars->error_code;
//     }
//     pid = fork();
//     if (pid < 0)
//     {
//         free(cmd_path);
//         return (vars->error_code = 1);
//     }
//     if (pid == 0)
//     {
//         execve(cmd_path, node->args, envp);
//         if (errno == EACCES)
//             error_code = ERR_PERMISSIONS;
//         else if (errno == ENOENT)
//             error_code = ERR_CMD_NOT_FOUND;
//         else if (errno == EISDIR)
//             error_code = ERR_ISDIRECTORY;
//         else
//             error_code = ERR_CMD_NOT_FOUND;
//         shell_error(cmd_path, error_code, NULL);
//         free(cmd_path);
//         exit(127);
//     }
//     free(cmd_path);
//     waitpid(pid, &status, 0);
//     return (handle_cmd_status(status, vars));
// }
/*
Handles command execution in the child process.
- Attempts to execute the command with execve
- On failure, determines appropriate error type from errno
- Reports errors using shell_error
- Cleans up resources and exits with code 127
Note: This function never returns (it either executes or exits)
*/
static void	exec_child(char *cmd_path, char **args, char **envp)
{
    int	error_code;
    
    execve(cmd_path, args, envp);
    if (errno == EACCES)
        error_code = ERR_PERMISSIONS;
    else if (errno == ENOENT)
        error_code = ERR_CMD_NOT_FOUND;
    else if (errno == EISDIR)
        error_code = ERR_ISDIRECTORY;
    else
        error_code = ERR_CMD_NOT_FOUND;
    shell_error(cmd_path, error_code, NULL);
    free(cmd_path);
    exit(127);
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
int exec_external_cmd(t_node *node, char **envp, t_vars *vars)
{
    pid_t pid;
    int   status;
    char  *cmd_path;

    cmd_path = get_cmd_path(node, envp, vars);
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
    return (handle_cmd_status(status, vars));
}
