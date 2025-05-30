/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   lexer_utils.c                                      :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: bleow <bleow@student.42kl.edu.my>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/05/30 12:08:52 by bleow             #+#    #+#             */
/*   Updated: 2025/05/30 12:09:03 by bleow            ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../includes/minishell.h"

/*
Appends a TYPE_NULL sentinel token to the end of the token linked list.
The function updates the list's 'current' pointer to this new null token.
*/
void	add_null_token_stop(t_vars *vars)
{
	t_node	*null_node;

	if (!vars || !vars->head)
		return ;
	null_node = initnode(TYPE_NULL, "");
	if (!null_node)
		return ;
	if (vars->current)
	{
		vars->current->next = null_node;
		null_node->prev = vars->current;
		vars->current = null_node;
	}
}
