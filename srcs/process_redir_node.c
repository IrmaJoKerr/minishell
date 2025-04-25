/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   process_redir_node.c                               :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: bleow <bleow@student.42kl.edu.my>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/04/23 13:25:08 by bleow             #+#    #+#             */
/*   Updated: 2025/04/25 23:33:30 by bleow            ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../includes/minishell.h"

/*
Finds and links previous redirections targeting the same command.
- Scans backward through token list
- Links earlier redirections to this one if they target the same command
Works with process_redir_node().
*/
void	link_prev_redirs(t_node *redir_node, t_node *cmd, t_vars *vars)
{
	t_node	*prev_redir;

	prev_redir = redir_node->prev;
	while (prev_redir)
	{
		if (prev_redir == redir_node)
		{
			prev_redir = prev_redir->prev;
			continue ;
		}
		if (is_redirection(prev_redir->type)
			&& get_redir_target(prev_redir, vars->pipes->last_cmd) == cmd
			&& prev_redir->redir == NULL)
		{
			prev_redir->redir = redir_node;
			break ;
		}
		prev_redir = prev_redir->prev;
	}
}

/*
Updates redirection type trackers and chains with existing redirections.
- Updates last_in_redir or last_out_redir based on type
- Chains with existing redirections targeting same command
Works with process_redir_node().
*/
void	track_redirs(t_node *redir_node, t_node *cmd, t_vars *vars)
{
	if (redir_node->type == TYPE_IN_REDIRECT
		|| redir_node->type == TYPE_HEREDOC)
	{
		if (vars->pipes->last_in_redir
			&& vars->pipes->last_in_redir != redir_node
			&& get_redir_target(vars->pipes->last_in_redir
				, vars->pipes->last_cmd) == cmd)
		{
			vars->pipes->last_in_redir->redir = redir_node;
		}
		vars->pipes->last_in_redir = redir_node;
	}
	else if (redir_node->type == TYPE_OUT_REDIRECT
		|| redir_node->type == TYPE_APPEND_REDIRECT)
	{
		if (vars->pipes->last_out_redir
			&& vars->pipes->last_out_redir != redir_node
			&& get_redir_target(vars->pipes->last_out_redir
				, vars->pipes->last_cmd) == cmd)
		{
			vars->pipes->last_out_redir->redir = redir_node;
		}
		vars->pipes->last_out_redir = redir_node;
	}
}

/*
Links input and output redirection nodes targeting the same command.
- If input and output redirections target the same command, links them
- Creates a redirection chain from input to output
Works with process_redir_node().
*/
void	link_in_out_redirs(t_vars *vars)
{
	t_node	*in_target;
	t_node	*out_target;
	t_node	*last_in;
	t_node	*last_out;
	t_node	*last_cmd;

	last_in = vars->pipes->last_in_redir;
	last_out = vars->pipes->last_out_redir;
	last_cmd = vars->pipes->last_cmd;
	if (last_in && last_out)
	{
		in_target = get_redir_target(last_in, last_cmd);
		out_target = get_redir_target(last_out, last_cmd);
		if (in_target == out_target && !last_in->redir)
		{
			last_in->redir = last_out;
		}
	}
}

/*
Retrieves the target command for a redirection node.
- Checks if the previous node is a command.
- If not, uses the last command seen in the pipe structure.
- Ensures the target command is valid.
Returns:
- Pointer to the target command node.
- NULL if no valid target found.
*/
t_node	*get_redir_target(t_node *current, t_node *last_cmd)
{
	t_node	*target;

	target = NULL;
	if (current->prev && current->prev->type == TYPE_CMD)
		target = current->prev;
	else
		target = last_cmd;
	return (target);
}

/*
Updates pipe structure when commands are redirected.
- Traverses pipe chain looking for references to the command.
- Replaces command references with redirection node references.
- Preserves pipe structure while incorporating redirections.
- Handles both left and right side command replacements.
*/
void	upd_pipe_redir(t_node *pipe_root, t_node *cmd, t_node *redir)
{
	t_node	*pipe_node;

	if (!pipe_root || !cmd || !redir)
		return ;
	pipe_node = pipe_root;
	while (pipe_node)
	{
		if (pipe_node->left == cmd)
		{
			pipe_node->left = redir;
		}
		else if (pipe_node->right == cmd)
		{
			pipe_node->right = redir;
		}
		if (pipe_node->right && pipe_node->right->type == TYPE_PIPE)
			pipe_node = pipe_node->right;
		else
			break ;
	}
}
