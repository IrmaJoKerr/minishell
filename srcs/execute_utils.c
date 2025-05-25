/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   execute_utils.c                                    :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: bleow <bleow@student.42kl.edu.my>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/04/25 22:30:17 by bleow             #+#    #+#             */
/*   Updated: 2025/05/25 21:07:12 by bleow            ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../includes/minishell.h"

/*
Handles command execution in the child process.
- Attempts to execute the command with execve
- On failure, determines appropriate error type from errno
- Reports errors using shell_error
- Cleans up resources and exits with code 127
Note: This function never returns (it either executes or exits)
*/
void	exec_child(char *cmd_path, char **args, char **envp)
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
// 	int		result;
// 	t_node	*cmd_node;

// 	vars->pipes->current_redirect = node;
// 	cmd_node = find_cmd(vars->head, node, FIND_PREV, vars);
// 	if (!cmd_node)
// 	{
// 		vars->error_code = ERR_DEFAULT;
// 		return (0);
// 	}
// 	vars->pipes->cmd_redir = cmd_node;
// 	if (node->type == TYPE_IN_REDIRECT || node->type == TYPE_HEREDOC)
// 		vars->pipes->last_in_redir = node;
// 	else if (node->type == TYPE_OUT_REDIRECT
// 		|| node->type == TYPE_APPEND_REDIRECT)
// 	{
// 		vars->pipes->last_out_redir = node;
// 	}
// 	if (!proc_redir_target(node, vars))
// 		return (0);
// 	result = redir_mode_setup(node, vars);
// 	return (result);
// }
int setup_redirection(t_node *node, t_vars *vars)
{
    fprintf(stderr, "DEBUG-SETUP-REDIR: ===== SETUP REDIRECTION START =====\n");
    fprintf(stderr, "DEBUG-SETUP-REDIR: Redirection type=%s, file='%s'\n",
            get_token_str(node->type), node->args ? node->args[0] : "NULL");
    
    // Debug initial state
    if (vars->pipes) {
        fprintf(stderr, "DEBUG-SETUP-REDIR: Initial redirection_fd=%d, out_mode=%d\n", 
                vars->pipes->redirection_fd, vars->pipes->out_mode);
    }
    
    // Debug FD state before redirection
    fprintf(stderr, "DEBUG-SETUP-REDIR: FDs before: stdin=%d, stdout=%d, stderr=%d\n",
            fileno(stdin), fileno(stdout), fileno(stderr));
    
    // Track the redirection node
    vars->pipes->current_redirect = node;
    
    // Find the command node
    t_node *cmd_node = find_cmd(vars->head, node, FIND_PREV, vars);
    if (!cmd_node)
    {
        fprintf(stderr, "DEBUG-SETUP-REDIR: No target command found\n");
        vars->error_code = ERR_DEFAULT;
        fprintf(stderr, "DEBUG-SETUP-REDIR: ===== SETUP REDIRECTION END (FAILED) =====\n");
        return (0);
    }
    
    fprintf(stderr, "DEBUG-SETUP-REDIR: Target command is '%s'\n", 
            cmd_node->args ? cmd_node->args[0] : "NULL");
    
    vars->pipes->cmd_redir = cmd_node;
    
    // Update last redirection pointers
    if (node->type == TYPE_IN_REDIRECT || node->type == TYPE_HEREDOC) {
        fprintf(stderr, "DEBUG-SETUP-REDIR: Updating last_in_redir\n");
        vars->pipes->last_in_redir = node;
    }
    else if (node->type == TYPE_OUT_REDIRECT || node->type == TYPE_APPEND_REDIRECT) {
        fprintf(stderr, "DEBUG-SETUP-REDIR: Updating last_out_redir\n");
        vars->pipes->last_out_redir = node;
    }
    
    // Process the target
    if (!proc_redir_target(node, vars)) {
        fprintf(stderr, "DEBUG-SETUP-REDIR: proc_redir_target failed\n");
        fprintf(stderr, "DEBUG-SETUP-REDIR: ===== SETUP REDIRECTION END (FAILED) =====\n");
        return (0);
    }
    
    // Setup redirection mode
    int result = redir_mode_setup(node, vars);
    
    // Debug final state
    if (vars->pipes) {
        fprintf(stderr, "DEBUG-SETUP-REDIR: Final redirection_fd=%d, out_mode=%d\n", 
                vars->pipes->redirection_fd, vars->pipes->out_mode);
    }
    
    // Debug FD state after redirection
    fprintf(stderr, "DEBUG-SETUP-REDIR: FDs after: stdin=%d, stdout=%d, stderr=%d\n",
            fileno(stdin), fileno(stdout), fileno(stderr));
    
    if (result)
        fprintf(stderr, "DEBUG-SETUP-REDIR: ===== SETUP REDIRECTION END (SUCCESS) =====\n");
    else
        fprintf(stderr, "DEBUG-SETUP-REDIR: ===== SETUP REDIRECTION END (FAILED) =====\n");
    
    return (result);
}

/*
Validates redirection target and processes quotes.
- Checks if redirection target exists
- Removes quotes from target if needed
- Reports syntax error if target is missing
Returns:
- 1 if target is valid
- 0 if target is invalid (with error_code set)
*/
// int proc_redir_target(t_node *node, t_vars *vars)
// {
//     fprintf(stderr, "DEBUG-REDIR-TARGET: Processing redirection target for node type %s\n", 
//             get_token_str(node->type));
    
//     if (!node) {
//         fprintf(stderr, "DEBUG-REDIR-TARGET: NULL node\n");
//         return (0);
//     }
    
//     fprintf(stderr, "DEBUG-REDIR-TARGET: Node args pointer: %p\n", (void*)node->args);
    
//     // Using node->args[0] directly instead of node->right->args[0]
//     if (node && node->args && node->args[0])
//     {
//         fprintf(stderr, "DEBUG-REDIR-TARGET: Found valid filename: '%s'\n", node->args[0]);
        
//         if (node->type != TYPE_HEREDOC)
//         {
//             fprintf(stderr, "DEBUG-REDIR-TARGET: Before stripping quotes: '%s'\n", node->args[0]);
//             strip_outer_quotes(&node->args[0], vars);
//             fprintf(stderr, "DEBUG-REDIR-TARGET: After stripping quotes: '%s'\n", node->args[0]);
//         }
        
//         return (1);
//     }
//     else if (node && node->type != TYPE_HEREDOC)
//     {
//         fprintf(stderr, "DEBUG-REDIR-TARGET: Missing filename for redirection\n");
//         tok_syntax_error_msg("newline", vars);
//         return (0);
//     }
    
//     return (1);
// }
int proc_redir_target(t_node *node, t_vars *vars)
{
    if (node && node->args && node->args[0])
    {
        if (node->type != TYPE_HEREDOC)
            strip_outer_quotes(&node->args[0], vars);
        
        return (1);
    }
    else if (node && node->type != TYPE_HEREDOC)
    {
        tok_syntax_error_msg("newline", vars);
        return (0);
    }
    
    return (1);
}

/*
Sets up a specific type of redirection based on node type.
- Handles input, output, append, and heredoc redirections.
- Updates mode flags and calls appropriate setup functions.
- Centralizes error handling for all redirection types.
- Triggers interactive heredoc gathering if content is not ready.
Returns:
- 1 on success.
- 0 on failure (with error_code set).
Works with setup_redirection().
*/
int	redir_mode_setup(t_node *node, t_vars *vars)
{
	int	result;

	result = 0;
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
		result = setup_heredoc_redir(node, vars);
	if (!result)
		vars->error_code = ERR_DEFAULT;
	return (result);
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
