/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   process_redirect.c                                 :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: bleow <bleow@student.42kl.edu.my>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/04/06 22:40:07 by bleow             #+#    #+#             */
/*   Updated: 2025/05/05 04:54:35 by bleow            ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../includes/minishell.h"

/*
Analyses a redirection node during pipe integration.
- Checks if the node is the head of its redirection chain
- Finds the target command for the redirection
- Updates pipe structure to use redirection nodes
Returns:
1 if current node should be processed further
0 if we should skip to the next node
*/
int	chk_redir_nodes(t_vars *vars, t_node *current)
{
	t_node	*chain_head;
	t_node	*target_cmd;

	chain_head = find_redir_chain_head(current, vars->pipes->last_cmd);
	if (chain_head != current)
	{
		return (0);
	}
	target_cmd = get_redir_target(current, vars->pipes->last_cmd);
	if (target_cmd && vars->pipes->pipe_root)
	{
		upd_pipe_redir(vars->pipes->pipe_root, target_cmd, current);
	}
	return (1);
}

/*
Finds the head (first) redirection node in a chain of redirections.
- Scans backward from current node to find first redirection
- All redirections in the chain must target the same command
Returns:
- The first (earliest) redirection node in the chain
- The original node if no earlier nodes found
Works with link_redirs_pipes().
*/
t_node	*find_redir_chain_head(t_node *current, t_node *last_cmd)
{
	t_node	*chain_head;
	t_node	*prev_redir;
	t_node	*current_target;
	t_node	*prev_target;
	int		same_target;

	chain_head = current;
	prev_redir = current->prev;
	current_target = get_redir_target(current, last_cmd);
	while (prev_redir)
	{
		prev_target = get_redir_target(prev_redir, last_cmd);
		same_target = (prev_target == current_target);
		if (!is_redirection(prev_redir->type) || !same_target)
			break ;
		chain_head = prev_redir;
		prev_redir = prev_redir->prev;
	}
	return (chain_head);
}

/*
Swaps a command node with its associated redirection node in the AST.
This function integrates redirections with pipe structures.
- Checks if the current node is a command node.
- Finds any redirection nodes targeting this command.
- Replaces the command node reference with the redirection node.
- Uses double pointer to modify the original node reference.
*/
void	swap_cmd_redir(t_node **node_ptr, t_vars *vars)
{
	t_node	*redir_node;

	if (*node_ptr && (*node_ptr)->type == TYPE_CMD)
	{
		redir_node = find_cmd_redir(vars->pipes->redir_root,
				*node_ptr, vars);
		if (redir_node)
			*node_ptr = redir_node;
	}
}

/*
Processes a chain of pipe nodes to link with redirection nodes.
- Recursively traverses the pipe chain, processing one pipe node at a time.
- Replaces commands on left side with their associated redirections.
- Handles right side either as command or as next pipe in chain.
Works with link_redirs_pipes() to process all levels of the pipe structure.
*/
void	proc_pipe_chain(t_node *start_pipe, t_vars *vars)
{
	t_node	*current_pipe;

	current_pipe = start_pipe;
	while (current_pipe)
	{
		swap_cmd_redir(&(current_pipe->left), vars);
		if (current_pipe->right && current_pipe->right->type == TYPE_CMD)
			swap_cmd_redir(&(current_pipe->right), vars);
		else if (current_pipe->right && current_pipe->right->type == TYPE_PIPE)
		{
			current_pipe = current_pipe->right;
			continue ;
		}
		break ;
	}
}

/*
Integrates redirection nodes with the pipe structure.
- Updates pipe node references to point to redirection nodes.
- Ensures pipes use redirection nodes instead of direct commands.
- Makes pipe commands output to redirections correctly.
Works with proc_redir() when pipe nodes exist.
*/
void	link_redirs_pipes(t_vars *vars)
{
	if (!vars || !vars->pipes || !vars->pipes->pipe_root
		|| !vars->head || !vars->pipes->redir_root)
		return ;
	swap_cmd_redir(&(vars->pipes->pipe_root->left), vars);
	if (vars->pipes->pipe_root->right)
	{
		if (vars->pipes->pipe_root->right->type == TYPE_CMD)
			swap_cmd_redir(&(vars->pipes->pipe_root->right), vars);
		else if (vars->pipes->pipe_root->right->type == TYPE_PIPE)
			proc_pipe_chain(vars->pipes->pipe_root->right, vars);
	}
}
