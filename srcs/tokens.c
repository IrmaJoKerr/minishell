/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   tokens.c                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: bleow <bleow@student.42kl.edu.my>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/03/02 04:36:48 by bleow             #+#    #+#             */
/*   Updated: 2025/03/02 17:35:52 by bleow            ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "includes/minishell.h"

/*
Makes a new node for a command. Works with handle_cmd_token.
*/
t_node	*make_cmdnode(char *token)
{
	t_node	*node;

	node = initnode(TYPE_CMD, token);
	if (!node)
		return (NULL);
	node->args = malloc(sizeof(char *) * 2);
	if (!node->args)
	{
		free(node);
		return (NULL);
	}
	node->args[0] = ft_strdup(token);
	node->args[1] = NULL;
	return (node);
}

/*
Makes a new node for a command token.
*/
t_node	*handle_cmd_token(char *token)
{
	t_node	*node;

	node = make_cmdnode(token);
	if (!node)
	{
		free(token);
		return (NULL);
	}
	return (node);
}

/*
Makes a new node for other types of tokens.
*/
t_node	*handle_other_token(char *token, t_tokentype type)
{
	t_node	*node;

	node = initnode(type, token);
	if (!node)
	{
		free(token);
		return (NULL);
	}
	return (node);
}
