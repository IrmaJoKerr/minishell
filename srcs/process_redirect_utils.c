/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   process_redirect_utils.c                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: bleow <bleow@student.42kl.edu.my>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/04/26 00:21:59 by bleow             #+#    #+#             */
/*   Updated: 2025/05/29 09:25:30 by bleow            ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../includes/minishell.h"

/*
Resets redirection tracking state in the pipes structure.
- Clears command and redirection node pointers.
- Prepares pipes structure for new redirection processing.
Works with proc_ast_redir to clean state before processing.
*/
void	reset_redir_tracking(t_pipe *pipes)
{
	if (!pipes)
		return ;
	pipes->last_cmd = NULL;
	pipes->redir_root = NULL;
	pipes->last_in_redir = NULL;
	pipes->last_out_redir = NULL;
	pipes->cmd_redir = NULL;
}

/*
Determines if a redirection node has valid adjacent commands.
- Checks if next node exists and is a command.
Returns:
- 1 if redirection has valid syntax.
- 0 otherwise.
*/
int	is_valid_redir_node(t_node *current)
{
	if (!current)
		return (0);
	if (!is_redirection(current->type))
		return (0);
	if (current->args && current->args[0])
		return (1);
	if (!current->next || (current->next->type != TYPE_CMD
			&& current->next->type != TYPE_ARGS))
		return (0);
	return (1);
}

// /*
// Configures a redirection node with source and target commands.
// - Sets left child to source command node.
// - Sets right child to target command/filename node.
// - Establishes the redirection relationship in the AST.
// Works with proc_ast_redir().
// */
// void	set_redir_node(t_node *redir, t_node *cmd, t_node *target)
// {
// 	if (!redir || !cmd || !target)
// 		return ;
// 	redir->left = cmd;
// 	redir->right = target;
// }
// void	set_redir_node(t_node *redir, t_node *cmd, t_node *target)
// {
// 	if (!redir || !cmd || !target)
// 	{
// 		return ;
// 	}
// 	redir->left = cmd;
// 	redir->right = target;
// }

/*
Finds redirection nodes associated with a specific command node.
- Searches through the redirection list for nodes targeting the command.
- Uses get_redir_target() to determine if a redirection points to the command.
- Handles NULL inputs safely.
Returns:
- The first redirection node that targets the command.
- NULL if no matching redirection found.
Works with link_redir_to_cmd_node() to connect commands and redirections in the AST.
*/
t_node *find_cmd_redir(t_node *redir_root, t_node *cmd_node, t_vars *vars)
{
	t_node *current;

	// Explicitly mark vars as used to avoid compiler warning
	(void)vars;
	if (!redir_root || !cmd_node)
	{
		return (NULL);
	}
	current = redir_root;
	while (current)
	{
		if (is_redirection(current->type) && current->redir == cmd_node)
		{
			return (current);
		}
		current = current->next;
	}
	return (NULL);
}
