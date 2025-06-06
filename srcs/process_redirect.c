/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   process_redirect.c                                 :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: bleow <bleow@student.42kl.edu.my>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/04/06 22:40:07 by bleow             #+#    #+#             */
/*   Updated: 2025/05/30 12:10:39 by bleow            ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../includes/minishell.h"

/*
Swaps a command node with its associated redirection node in the AST.
This function integrates redirections with pipe structures.
- Checks if the current node is a command node.
- Finds any redirection nodes targeting this command.
- Replaces the command node reference with the redirection node.
- Uses double pointer to modify the original node reference.
*/
void	link_redir_to_cmd_node(t_node **node_ptr, t_vars *vars)
{
	t_node	*redir_node;

	if (*node_ptr && (*node_ptr)->type == TYPE_CMD)
	{
		redir_node = find_cmd_redir(vars->pipes->redir_root, *node_ptr);
		if (redir_node)
		{
			(*node_ptr)->redir = redir_node;
		}
	}
}

/*
Processes a chain of pipe nodes to link with redirection nodes.
- Recursively traverses the pipe chain, processing one pipe node at a time.
- Replaces commands on left side with their associated redirections.
- Handles right side either as command or as next pipe in chain.
Works with proc_ast_redir() to process all levels of the pipe structure.
*/
void	proc_pipe_chain(t_node *start_pipe, t_vars *vars)
{
	t_node	*current_pipe;

	current_pipe = start_pipe;
	while (current_pipe)
	{
		link_redir_to_cmd_node(&(current_pipe->left), vars);
		if (current_pipe->right && current_pipe->right->type == TYPE_CMD)
			link_redir_to_cmd_node(&(current_pipe->right), vars);
		else if (current_pipe->right && current_pipe->right->type == TYPE_PIPE)
		{
			current_pipe = current_pipe->right;
			continue ;
		}
		break ;
	}
}

/*
Validates that all redirection operators in the token list have valid targets.
- Ensures no redirection is at the end of input
- Ensures no redirection is followed by another operator
- Reports appropriate syntax errors
Returns:
- 1 if all redirections have valid targets
- 0 otherwise (with error_code set)
*/
int	validate_redir_targets(t_vars *vars)
{
	t_node	*current;

	current = vars->head;
	while (current)
	{
		if (is_redirection(current->type))
		{
			if (!current->args || !current->args[0])
			{
				tok_syntax_error_msg("newline", vars);
				return (0);
			}
		}
		current = current->next;
	}
	return (1);
}
