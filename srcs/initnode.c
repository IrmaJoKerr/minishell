/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   initnode.c                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: lechan <lechan@student.42kl.edu.my>        +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/03/13 15:53:06 by bleow             #+#    #+#             */
/*   Updated: 2025/03/22 19:21:02 by lechan           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../includes/minishell.h"

/*
Sets up basic node structure framework.
Handles node type, token, and argument array initialization.
Has special case for head nodes.
Sets a default token if token is NULL.
Returns 1 on success, 0 on failure.
*/
int	make_nodeframe(t_node *node, t_tokentype type, char *token)
{
	node->type = type;
	node->next = NULL;
	node->prev = NULL;
	node->left = NULL;
	node->right = NULL;
	node->arg_quote_type = NULL;
	if (!token)
		token = (char *)get_token_str(type);
	create_args_array(node, token);
	if (!node->args)
		return (0);
	return (1);
}

t_node	*initnode(t_tokentype type, char *token)
{
	t_node	*node;
	int		quote_type;

	quote_type = 0;
	node = malloc(sizeof(t_node));
	if (!node)
		return (NULL);
	if (type == TYPE_SINGLE_QUOTE || type == 5)
		quote_type = 1;
	else if (type == TYPE_DOUBLE_QUOTE || type == 4)
		quote_type = 2;
	if (!make_nodeframe(node, type, token))
	{
		ft_safefree((void **)&node);
		return (NULL);
	}
	if (node->arg_quote_type && (type == TYPE_SINGLE_QUOTE
			|| type == TYPE_DOUBLE_QUOTE || type == 5 || type == 4))
		node->arg_quote_type[0] = quote_type;
	return (node);
}
