/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   cleanup_b.c                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: bleow <bleow@student.42kl.edu.my>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/03/16 01:03:50 by bleow             #+#    #+#             */
/*   Updated: 2025/05/22 17:06:42 by bleow            ###   ########.fr       */
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
	size_t	arg_count;

	if (!node)
		return ;
	arg_count = 0;
	if (node->args)
		arg_count = ft_arrlen(node->args);
	if (node->args)
		ft_free_2d(node->args, arg_count);
	if (node->arg_quote_type)
	{
		ft_free_int_2d(node->arg_quote_type, arg_count);
		node->arg_quote_type = NULL;
	}
	free(node);
}
