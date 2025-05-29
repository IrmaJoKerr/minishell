/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   process_redirect_utils.c                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: bleow <bleow@student.42kl.edu.my>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/04/26 00:21:59 by bleow             #+#    #+#             */
/*   Updated: 2025/05/29 17:51:05 by bleow            ###   ########.fr       */
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

/*
Finds redirection nodes associated with a specific command node.
- Searches through the redirection list for nodes targeting the command.
Returns:
- The first redirection node that targets the command.
- NULL if no matching redirection found.
*/
t_node	*find_cmd_redir(t_node *redir_root, t_node *cmd_node)
{
	t_node	*current;

	if (!redir_root || !cmd_node)
		return (NULL);
	current = redir_root;
	while (current)
	{
		if (is_redirection(current->type) && current->redir == cmd_node)
			return (current);
		current = current->next;
	}
	return (NULL);
}
