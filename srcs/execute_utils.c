/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   execute_utils.c                                    :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: bleow <bleow@student.42kl.edu.my>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/04/25 22:30:17 by bleow             #+#    #+#             */
/*   Updated: 2025/05/29 16:15:14 by bleow            ###   ########.fr       */
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
int setup_redirection(t_node *node, t_vars *vars)
{
	t_node	*cmd_node;
	int		result;

	vars->pipes->current_redirect = node;
	cmd_node = find_cmd(vars->head, node, FIND_PREV, vars);
	if (!cmd_node && (node->type == TYPE_OUT_REDIRECT
		|| node->type == TYPE_APPEND_REDIRECT))
	{
		vars->pipes->out_mode = (node->type == TYPE_APPEND_REDIRECT) ? 
					   OUT_MODE_APPEND : OUT_MODE_TRUNCATE;
		// Just create the file
		if (node->args && node->args[0])
		{
			char *filename = node->args[0];
			int flags = (vars->pipes->out_mode == OUT_MODE_APPEND) ? 
				   (O_WRONLY | O_CREAT | O_APPEND) : 
				   (O_WRONLY | O_CREAT | O_TRUNC);
			// Open the file, just to create it
			int fd = open(filename, flags, 0644);
			if (fd >= 0)
			{
				close(fd);
				return 1; // Success
			}
			// If open fails, still return 1 to continue processing
			return 1;
		}
		return 1; // Continue processing
	}
	// Standard redirection handling for redirections with commands
	if (!cmd_node)
	{
		vars->error_code = ERR_DEFAULT;
		return (0);
	}
	vars->pipes->cmd_redir = cmd_node;
	if (node->type == TYPE_IN_REDIRECT || node->type == TYPE_HEREDOC)
	{
		vars->pipes->last_in_redir = node;
	}
	else if (node->type == TYPE_OUT_REDIRECT
		|| node->type == TYPE_APPEND_REDIRECT)
	{
		vars->pipes->last_out_redir = node;
	}
	if (!proc_redir_target(node, vars))
		return (0);
	result = redir_mode_setup(node, vars);
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
int	proc_redir_target(t_node *node, t_vars *vars)
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
