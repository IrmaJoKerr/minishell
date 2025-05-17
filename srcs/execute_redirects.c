/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   execute_redirects.c                                :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: bleow <bleow@student.42kl.edu.my>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/04/25 22:39:34 by bleow             #+#    #+#             */
/*   Updated: 2025/05/16 04:06:51 by bleow            ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../includes/minishell.h"

/*
Handles redirection setup for input files.
- Opens file for reading.
- Redirects stdin to read from the file.
- Properly handles and reports errors.
Returns:
- 1 on success.
- 0 on failure.
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
Sets up heredoc redirection handling.
- Checks if heredoc content is ready.
- Initiates interactive mode if end delimiter not found.
- Handles the actual heredoc setup via handle_heredoc().
Returns:
- 1 on success
- 0 on failure
Works with redir_mode_setup().
*/
int	setup_heredoc_redir(t_node *node, t_vars *vars)
{
	int	result;

	result = 1;
	if (!vars->hd_text_ready)
	{
		if (!interactive_hd_mode(vars))
			return (0);
	}
	result = handle_heredoc(node, vars);
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
// int	proc_redir_chain(t_node *start_node, t_node *cmd_node, t_vars *vars) PRE ADDED DEBUG PRINTS
// {
// 	t_node	*current_node;
// 	t_node	*next_redir;

// 	current_node = start_node;
// 	while (current_node && is_redirection(current_node->type))
// 	{
// 		vars->pipes->current_redirect = current_node;
// 		if (!setup_redirection(current_node, vars))
// 		{
// 			reset_redirect_fds(vars);
// 			return (0);
// 		}
// 		if (current_node->redir)
// 			current_node = current_node->redir;
// 		else
// 		{
// 			next_redir = get_next_redir(current_node, cmd_node);
// 			if (next_redir)
// 				current_node = next_redir;
// 			else
// 				break ;
// 		}
// 	}
// 	return (1);
// }
int	proc_redir_chain(t_node *start_node, t_node *cmd_node, t_vars *vars)
{
	t_node	*current_node;
	t_node	*next_redir;

	fprintf(stderr, "DEBUG-REDIR-CHAIN: Processing redirection chain starting with node type=%d\n", 
		start_node->type);
	current_node = start_node;
	while (current_node && is_redirection(current_node->type))
	{
		vars->pipes->current_redirect = current_node;
		fprintf(stderr, "DEBUG-REDIR-CHAIN: Processing redirection node type=%d, target: %s\n", 
			current_node->type, 
			current_node->right && current_node->right->args ? 
			current_node->right->args[0] : "NULL");
		if (!setup_redirection(current_node, vars))
		{
			fprintf(stderr, "DEBUG-REDIR-CHAIN: Setup redirection failed\n");
			reset_redirect_fds(vars);
			return (0);
		}
		if (current_node->redir)
		{
			fprintf(stderr, "DEBUG-REDIR-CHAIN: Moving to linked redirection node\n");
			current_node = current_node->redir;
		}
		else
		{
			next_redir = get_next_redir(current_node, cmd_node);
			if (next_redir)
			{
				fprintf(stderr, "DEBUG-REDIR-CHAIN: Moving to next redirection node in list\n");
				current_node = next_redir;
			}
			else
			{
				fprintf(stderr, "DEBUG-REDIR-CHAIN: No more redirections in chain\n");
				break ;
			}
		}
	}
	fprintf(stderr, "DEBUG-REDIR-CHAIN: Redirection chain processing complete\n");
	return (1);
}
