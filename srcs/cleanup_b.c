/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   cleanup_b.c                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: bleow <bleow@student.42kl.edu.my>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/03/16 01:03:50 by bleow             #+#    #+#             */
/*   Updated: 2025/04/10 22:42:06 by bleow            ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../includes/minishell.h"

/*
Free a single token node and its arguments.
- Takes a node pointer and frees its arguments array.
- Properly frees character-level quote type arrays.
- Then frees the node itself.
Works with cleanup_token_list().
*/
void	free_token_node(t_node *node)
{
	int	i;
	
	if (!node)
		return ;
	if (node->args)
		ft_free_2d(node->args, ft_arrlen(node->args));
	
	if (node->arg_quote_type)
	{
		i = 0;
		while (node->arg_quote_type[i])
		{
			free(node->arg_quote_type[i]);
			i++;
		}
		free(node->arg_quote_type);
	}
	// Free the node itself
	free(node);
}

/*
Clean up the token list by freeing all nodes.
- Traverses the linked list of tokens.
- Frees each node and its arguments.
- Resets head and current pointers in vars.
- Called when processing a new command line.
Works with cleanup_exit().
*/
void	cleanup_token_list(t_vars *vars)
{
	t_node	*current;
	t_node	*next;
	int		count;

	count = 0;
	if (!vars || !vars->head)
		return ;
	current = vars->head;
	while (current)
	{
		next = current->next;
		free_token_node(current);
		current = next;
		count++;
	}
	vars->head = NULL;
	vars->current = NULL;
}
