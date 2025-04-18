/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   cleanup_b.c                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: bleow <bleow@student.42kl.edu.my>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/03/16 01:03:50 by bleow             #+#    #+#             */
/*   Updated: 2025/04/18 21:45:45 by bleow            ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../includes/minishell.h"

/*
Free a single token node and its arguments.
- Takes a node pointer and frees its arguments array.
- Properly frees character-level quote type arrays.
- Then frees the node itself.
Works with cleanup_token_list(), build_token_linklist(), maketoken().
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
	free(node);
}
