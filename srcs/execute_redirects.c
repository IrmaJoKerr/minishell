/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   execute_redirects.c                                :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: bleow <bleow@student.42kl.edu.my>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/04/25 22:39:34 by bleow             #+#    #+#             */
/*   Updated: 2025/05/30 12:37:20 by bleow            ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../includes/minishell.h"

/*
Handles case when input file is missing or inaccessible.
For pipe contexts, redirects stdin from /dev/null.
*/
int	handle_bad_infile(t_vars *vars)
{
	int	null_fd;

	if (vars->pipes && vars->pipes->pipe_root != NULL)
	{
		null_fd = open("/dev/null", O_RDONLY);
		if (null_fd == -1)
		{
			vars->error_code = ERR_DEFAULT;
			return (0);
		}
		if (dup2(null_fd, STDIN_FILENO) == -1)
		{
			close(null_fd);
			vars->error_code = ERR_DEFAULT;
			return (0);
		}
		close(null_fd);
		return (1);
	}
	else
		return (0);
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
int	proc_redir_chain(t_node *start_node, t_vars *vars)
{
	t_node	*current;
	t_node	*prev_node;
	int		prev_result;
	int		result;

	prev_node = NULL;
	current = start_node;
	while (current)
	{
		if (current == prev_node)
			return (0);
		result = setup_redirection(current, vars);
		if (!result)
			return (0);
		prev_result = result;
		prev_node = current;
		current = current->next_redir;
	}
	return (prev_result);
}

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
