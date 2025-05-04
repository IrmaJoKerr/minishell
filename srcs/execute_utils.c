/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   execute_utils.c                                    :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: bleow <bleow@student.42kl.edu.my>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/04/25 22:30:17 by bleow             #+#    #+#             */
/*   Updated: 2025/05/03 16:20:18 by bleow            ###   ########.fr       */
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

// 	// if (DEBUG_REDIR) //DEBUG PRINT
// 	// { //DEBUG PRINT
// 	// 	fprintf(stderr, "In setup_redirection().Setting up redirection type: %s\n", //DEBUG PRINT
// 	// 		get_token_str(node->type)); // DEBUG PRINT
// 	// 	if (node->right && node->right->args) //DEBUG PRINT
// 	// 		fprintf(stderr, "  Target: %s\n", node->right->args[0]); //DEBUG PRINT
// 	// } //debug print
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
// 	// if (DEBUG_REDIR) //DEBUG PRINT
// 	// 	fprintf(stderr, "In setup_redirection().Redirection setup result: %d\n", result); //DEBUG PRINT
// 	return (result);
// }
int	setup_redirection(t_node *node, t_vars *vars)
{
    int		result;
    t_node	*cmd_node;

    fprintf(stderr, "[DEBUG-REDIR] Setting up redirection type: %s\n", 
        get_token_str(node->type));
    
    if (node->right && node->right->args)
        fprintf(stderr, "[DEBUG-REDIR] Target: %s\n", node->right->args[0]);

    fprintf(stderr, "[DEBUG-REDIR] Current command args before redirection:\n");
    if (vars->cmd_nodes[0] && vars->cmd_nodes[0]->args) {
        int i = 0;
        while (vars->cmd_nodes[0]->args[i]) {
            fprintf(stderr, "  Arg[%d]: %s\n", i, vars->cmd_nodes[0]->args[i]);
            i++;
        }
    }
    
    vars->pipes->current_redirect = node;
    cmd_node = find_cmd(vars->head, node, FIND_PREV, vars);
    
    if (!cmd_node)
    {
        fprintf(stderr, "[DEBUG-REDIR] No command found for redirection\n");
        vars->error_code = ERR_DEFAULT;
        return (0);
    }
    
    vars->pipes->cmd_redir = cmd_node;
    if (node->type == TYPE_IN_REDIRECT || node->type == TYPE_HEREDOC)
        vars->pipes->last_in_redir = node;
    else if (node->type == TYPE_OUT_REDIRECT
        || node->type == TYPE_APPEND_REDIRECT)
    {
        vars->pipes->last_out_redir = node;
    }
    
    if (!proc_redir_target(node, vars)) {
        fprintf(stderr, "[DEBUG-REDIR] Failed to process redirection target\n");
        return (0);
    }
    
    result = redir_mode_setup(node, vars);
    fprintf(stderr, "[DEBUG-REDIR] Redirection setup result: %d\n", result);
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
// int	proc_redir_target(t_node *node, t_vars *vars)
// {
// 	if (node->right && node->right->args && node->right->args[0])
// 	{
// 		if (node->type != TYPE_HEREDOC)
// 			strip_outer_quotes(&node->right->args[0], vars);
// 		return (1);
// 	}
// 	else if (node->type != TYPE_HEREDOC)
// 	{
// 		tok_syntax_error_msg("newline", vars);
// 		return (0);
// 	}
// 	return (1);
// }
int proc_redir_target(t_node *node, t_vars *vars)
{
    fprintf(stderr, "[DEBUG-TARGET] Processing redirection target\n");
    
    if (node) {
        fprintf(stderr, "[DEBUG-TARGET] Redirection node: ");
        print_node_content(stderr, node);
        fprintf(stderr, "\n");
    }
    
    if (node->right && node->right->args && node->right->args[0])
    {
        fprintf(stderr, "[DEBUG-TARGET] Target before processing: '%s'\n", node->right->args[0]);
        
        if (node->type != TYPE_HEREDOC) {
            // Enhanced quote handling for redirection targets
            char *path = node->right->args[0];
            size_t len = ft_strlen(path);
            
            // Check if path is wrapped in quotes
            if (len >= 2) {
                if ((path[0] == '"' && path[len-1] == '"') ||
                    (path[0] == '\'' && path[len-1] == '\'')) {
                    
                    fprintf(stderr, "[DEBUG-TARGET] Stripping outer quotes from path\n");
                    char *new_path = ft_substr(path, 1, len-2);
                    if (new_path) {
                        free(path);
                        node->right->args[0] = new_path;
                        fprintf(stderr, "[DEBUG-TARGET] Path after quote removal: '%s'\n", new_path);
                    }
                } else {
                    // Use existing strip_outer_quotes for non-obvious quote cases
                    strip_outer_quotes(&node->right->args[0], vars);
                    fprintf(stderr, "[DEBUG-TARGET] Path after processing: '%s'\n", node->right->args[0]);
                }
            } else {
                // No quotes to strip for very short strings
                fprintf(stderr, "[DEBUG-TARGET] Path too short for quote stripping\n");
            }
        }
        return (1);
    }
    else if (node->type != TYPE_HEREDOC)
    {
        fprintf(stderr, "[DEBUG-TARGET] Missing target for redirection\n");
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
