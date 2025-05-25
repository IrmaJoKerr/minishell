/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   execute.c                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: bleow <bleow@student.42kl.edu.my>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/03/01 22:26:13 by bleow             #+#    #+#             */
/*   Updated: 2025/05/25 17:50:43 by bleow            ###   ########.fr       */
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
Works with exec_external_cmd() and execute_pipes().
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
Executes a command with redirection.
- Saves original file descriptors.
- Sets up redirection according to node type.
- Executes the command with redirection in place.
- Restores original file descriptors afterward.
Returns:
- Result of command execution.
Works with execute_cmd().
*/
// int exec_redirect_cmd(t_node *node, char **envp, t_vars *vars)
// {
//     int     result;
//     t_node  *cmd_node;

//     if (!node->left || !node->right)
//         return (1);
    
//     cmd_node = node->left;
//     while (cmd_node && is_redirection(cmd_node->type))
//         cmd_node = cmd_node->left;
    
//     // Save original file descriptors
//     vars->pipes->saved_stdin = dup(STDIN_FILENO);
//     vars->pipes->saved_stdout = dup(STDOUT_FILENO);
    
//     // Process redirection chain - check the return value
//     if (!proc_redir_chain(node, cmd_node, vars))
//     {
//         reset_redirect_fds(vars);
//         reset_terminal_after_heredoc();
//         return (vars->error_code); // Return the error code, don't execute command
//     }
    
//     // Only execute command if proc_redir_chain succeeded
//     result = execute_cmd(cmd_node, envp, vars);
    
//     // Clean up
//     reset_redirect_fds(vars);
//     reset_terminal_after_heredoc();
    
//     return (result);
// }
// int exec_redirect_cmd(t_node *node, char **envp, t_vars *vars)
// {
//     int     result;
//     t_node  *cmd_node;

//     fprintf(stderr, "DEBUG: exec_redirect_cmd for node type: %s\n", 
//             get_token_str(node->type));
    
//     if (!node->left || !node->right)
//         return (1);
    
//     cmd_node = node->left;
//     while (cmd_node && is_redirection(cmd_node->type))
//         cmd_node = cmd_node->left;
    
//     // Save original file descriptors
//     vars->pipes->saved_stdin = dup(STDIN_FILENO);
//     vars->pipes->saved_stdout = dup(STDOUT_FILENO);
    
//     // Process redirection chain - check the return value
//     fprintf(stderr, "DEBUG: Processing redirection chain\n");
//     if (!proc_redir_chain(node, cmd_node, vars))
//     {
//         fprintf(stderr, "DEBUG: Redirection failed, skipping command execution\n");
//         reset_redirect_fds(vars);
//         reset_terminal_after_heredoc();
//         return (vars->error_code); // Return the error code, don't execute command
//     }
    
//     // Only execute command if proc_redir_chain succeeded
//     fprintf(stderr, "DEBUG: Executing command after successful redirection\n");
//     result = execute_cmd(cmd_node, envp, vars);
    
//     // Clean up
//     reset_redirect_fds(vars);
//     reset_terminal_after_heredoc();
    
//     return (result);
// }
int exec_redirect_cmd(t_node *node, char **envp, t_vars *vars)
{
    int     result;
    t_node  *cmd_node;

    fprintf(stderr, "DEBUG-REDIR-EXEC: Executing redirection command for node type=%s, filename='%s'\n", 
            get_token_str(node->type), node->args ? node->args[0] : "NULL");
    
    if (!node->left)
    {
        fprintf(stderr, "DEBUG-REDIR-EXEC: No command (left) node found\n");
        return (1);
    }
    
    cmd_node = node->left;
    fprintf(stderr, "DEBUG-REDIR-EXEC: Command node is '%s'\n",
            cmd_node->args ? cmd_node->args[0] : "NULL");
    
    // Save original file descriptors
    vars->pipes->saved_stdin = dup(STDIN_FILENO);
    vars->pipes->saved_stdout = dup(STDOUT_FILENO);
    
    // Process redirection chain - check the return value
    fprintf(stderr, "DEBUG-REDIR-EXEC: Processing redirection chain\n");
    if (!proc_redir_chain(node, cmd_node, vars))
    {
        fprintf(stderr, "DEBUG-REDIR-EXEC: Redirection chain processing failed, error_code=%d\n",
                vars->error_code);
        reset_redirect_fds(vars);
        reset_terminal_after_heredoc();
        return (vars->error_code);
    }
    
    // Only execute command if proc_redir_chain succeeded
    fprintf(stderr, "DEBUG-REDIR-EXEC: Executing command after successful redirection\n");
    result = execute_cmd(cmd_node, envp, vars);
    
    // Clean up
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
int execute_cmd(t_node *node, char **envp, t_vars *vars)
{
    int result;
    // Save current error code to detect redirection errors
    int prev_error = vars->error_code;

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
    
    // Preserve redirection errors that occurred before command execution
    if (prev_error == ERR_REDIRECTION)
        vars->error_code = prev_error;
    else
        vars->error_code = result;
    
    return (vars->error_code);
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
	int		return_code;
	char	*cmd_path;

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
	return_code = handle_cmd_status(status, vars);
	return (return_code);
}
