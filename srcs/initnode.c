/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   initnode.c                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: bleow <bleow@student.42kl.edu.my>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/03/13 15:53:06 by bleow             #+#    #+#             */
/*   Updated: 2025/04/22 04:22:30 by bleow            ###   ########.fr       */
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
	node->redir = NULL;
	node->arg_quote_type = NULL;
	if (!token)
		token = (char *)get_token_str(type);
	make_node_arrays(node, token);
	if (!node->args)
		return (0);
	return (1);
}

/*
Initializes a token node with the given type and content
Sets up character-level quote tracking when appropriate
*/
t_node *initnode(t_tokentype type, char *token)
{
	t_node  *node;
	int     quote_type;
	
	quote_type = 0;
	node = malloc(sizeof(t_node));
	if (!node)
		return (NULL);
	if (type == TYPE_SINGLE_QUOTE)
		quote_type = TYPE_SINGLE_QUOTE;
	else if (type == TYPE_DOUBLE_QUOTE)
		quote_type = TYPE_DOUBLE_QUOTE;
	if (!make_nodeframe(node, type, token))
	{
		free(node);
		return (NULL);
	}
	if (node->arg_quote_type && node->args && node->args[0] &&
		(type == TYPE_SINGLE_QUOTE || type == TYPE_DOUBLE_QUOTE))
	{
		int len = ft_strlen(node->args[0]);
		int i = 0;
		while (i < len)
		{
			node->arg_quote_type[0][i] = quote_type;
			i++;
		}
	}
	return (node);
}
