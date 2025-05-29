/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   process_redir_node.c                               :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: bleow <bleow@student.42kl.edu.my>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/04/23 13:25:08 by bleow             #+#    #+#             */
/*   Updated: 2025/05/30 04:15:13 by bleow            ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../includes/minishell.h"

void	store_single_redir_node(t_node *redir_node, t_node *cmd_node,
			t_vars *vars)
{
	redir_node->redir = cmd_node;
	redir_node->left = cmd_node;
	if (redir_node->type == TYPE_IN_REDIR
		|| redir_node->type == TYPE_HEREDOC)
		vars->pipes->last_in_redir = redir_node;
	else if (redir_node->type == TYPE_OUT_REDIR
		|| redir_node->type == TYPE_APPD_REDIR)
		vars->pipes->last_out_redir = redir_node;
}

/*
Finds and links previous redirections targeting the same command.
- Scans backward through token list
- Links earlier redirections to this one if they target the same command
Works with pre_ast_redir_proc().
*/
void	make_cmd_redir_chain(t_node *cmd_node, t_vars *vars,
		t_node **first_redir, t_node **prev_redir)
{
	t_node	*current;

	*first_redir = NULL;
	*prev_redir = NULL;
	current = cmd_node;
	while (current && current->next)
	{
		current = current->next;
		if (current->type == TYPE_PIPE || current->type == TYPE_CMD)
			break ;
		if (is_redirection(current->type))
		{
			store_single_redir_node(current, cmd_node, vars);
			if (!(*first_redir))
			{
				*first_redir = current;
				cmd_node->redir = *first_redir;
			}
			if (*prev_redir)
				(*prev_redir)->next_redir = current;
			*prev_redir = current;
			current->next_redir = NULL;
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
	else if (current->next && current->next->type == TYPE_CMD)
		target = current->next;
	else if (last_cmd)
		target = last_cmd;
	return (target);
}
