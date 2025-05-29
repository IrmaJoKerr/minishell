/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   process_pipes.c                                    :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: bleow <bleow@student.42kl.edu.my>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/04/06 22:40:50 by bleow             #+#    #+#             */
/*   Updated: 2025/05/29 09:03:57 by bleow            ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../includes/minishell.h"

/*
Sets up pipe node connections by linking to command nodes.
- Connects pipe_node's left side to the previous command.
- Connects pipe_node's right side to the next command.
- Updates the pipes structure for tracking.
Works with process_first_pipe and process_addon_pipes.
*/
void	setup_pipe_node(t_node *pipe_node, t_node *left_cmd, t_node *right_cmd)
{
	if (!pipe_node || !left_cmd || !right_cmd)
		return ;
	pipe_node->left = left_cmd;
	pipe_node->right = right_cmd;
}

/*
Finds and processes the first pipe node in token stream.
- Searches for first pipe token.
- Identifies commands before and after the pipe.
- Sets up pipe connections.
Returns:
The pipe node if found and valid.
NULL if no valid pipe configuration exists.
Works with proc_ast_pipes.
*/
t_node	*process_first_pipe(t_vars *vars)
{
	t_node	*current;
	t_node	*next_cmd;

	if (!vars || !vars->pipes)
		return (NULL);
	current = vars->head;
	vars->pipes->last_cmd = NULL;
	while (current)
	{
		if (current->type == TYPE_CMD)
			vars->pipes->last_cmd = current;
		else if (current->type == TYPE_PIPE)
		{
			next_cmd = find_cmd(current->next, NULL, FIND_NEXT, vars);
			if (vars->pipes->last_cmd && next_cmd)
			{
				setup_pipe_node(current, vars->pipes->last_cmd, next_cmd);
				vars->pipes->last_pipe = current;
				vars->pipes->in_pipe = 1;
				return (current);
			}
		}
		current = current->next;
	}
	return (NULL);
}

/*
Processes remaining pipe nodes after the first pipe.
- Continues scanning from after the first pipe.
- Sets up additional pipe nodes.
- Links them into the pipe chain.
Returns:
Nothing (void function).
Works with proc_ast_pipes.
*/
void	process_addon_pipes(t_vars *vars)
{
	t_node	*current;
	t_node	*next_cmd;

	if (!vars || !vars->pipes || !vars->pipes->last_pipe)
		return ;
	current = vars->pipes->last_pipe->next;
	if (!current)
		return ;
	vars->pipes->last_cmd = NULL;
	while (current)
	{
		if (current->type == TYPE_CMD)
			vars->pipes->last_cmd = current;
		else if (current->type == TYPE_PIPE)
		{
			next_cmd = find_cmd(current->next, NULL, FIND_NEXT, vars);
			if (vars->pipes->last_cmd && next_cmd)
			{
				setup_pipe_node(current, vars->pipes->last_cmd, next_cmd);
				vars->pipes->last_pipe->right = current;
				vars->pipes->last_pipe = current;
			}
		}
		current = current->next;
	}
}
