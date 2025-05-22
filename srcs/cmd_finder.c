/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   cmd_finder.c                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: bleow <bleow@student.42kl.edu.my>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/04/05 10:40:16 by bleow             #+#    #+#             */
/*   Updated: 2025/05/22 17:06:35 by bleow            ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../includes/minishell.h"

/*
Initializes the command node search based on search mode.
- For FIND_ALL mode: Resets command count and returns the head of
  the token list.
- For other modes: Returns the provided start node.
Returns:
- The appropriate starting node for the search.
*/
t_node	*init_find_cmd(t_node *start, t_vars *vars)
{
	t_node	*result;

	if (vars->find_mode == FIND_ALL && vars)
	{
		vars->cmd_count = 0;
		result = vars->head;
	}
	else
		result = start;
	return (result);
}

/*
Handles command node processing based on the current mode.
Returns:
- A node if we found what we're looking for
- NULL to continue searching.
*/
t_node	*mode_action(t_node *current, t_node **last_cmd, t_vars *vars)
{
	if (current->type == TYPE_CMD)
	{
		if (vars->find_mode == FIND_ALL && vars->cmd_count < 100)
		{
			vars->cmd_nodes[vars->cmd_count] = current;
			vars->cmd_count++;
		}
		*last_cmd = current;
		if (vars->find_mode == FIND_NEXT && vars->find_start != current)
		{
			return (current);
		}
	}
	if (vars->find_mode == FIND_PREV && current == vars->find_tgt)
	{
		return (*last_cmd);
	}
	if (vars->find_mode == FIND_PREV && current->type == TYPE_PIPE)
	{
		*last_cmd = NULL;
	}
	return (NULL);
}

/*
Sets up find_cmd temporary variables in the vars struct.
Used at the start and end of find_cmd().
*/
void	reset_find_cmd(t_vars *vars, t_node *start, t_node *target, int mode)
{
	if (!vars)
		return ;
	vars->find_start = start;
	vars->find_tgt = target;
	vars->find_mode = mode;
}

/*
Core command finding function with multiple modes.
Modes:
- FIND_LAST: Find last command in the list (from head to end).
- FIND_PREV: Find last command before a specific node.
- FIND_NEXT: Find next command after a specific node.
- FIND_ALL: Returns first command node and populates vars->cmd_nodes
  with all commands.
Returns:
- The found command node.
- NULL if not found
*/
t_node	*find_cmd(t_node *start, t_node *target, int mode, t_vars *vars)
{
	t_node	*current;
	t_node	*last_cmd;
	t_node	*result;

	if ((!vars) || (!start && mode != FIND_ALL))
		return (NULL);
	last_cmd = NULL;
	reset_find_cmd(vars, start, target, mode);
	current = init_find_cmd(start, vars);
	while (current)
	{
		result = mode_action(current, &last_cmd, vars);
		if (result)
		{
			reset_find_cmd(vars, NULL, NULL, 0);
			return (result);
		}
		current = current->next;
	}
	reset_find_cmd(vars, NULL, NULL, 0);
	return (last_cmd);
}
