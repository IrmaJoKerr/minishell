/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   initnode.c                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: bleow <bleow@student.42kl.edu.my>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/03/13 15:53:06 by bleow             #+#    #+#             */
/*   Updated: 2025/04/10 03:04:14 by bleow            ###   ########.fr       */
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

// t_node	*initnode(t_tokentype type, char *token)
// {
// 	t_node	*node;
// 	int		quote_type;

// 	quote_type = 0;
// 	node = malloc(sizeof(t_node));
// 	if (!node)
// 		return (NULL);
// 	if (type == TYPE_SINGLE_QUOTE || type == 5)
// 		quote_type = 1;
// 	else if (type == TYPE_DOUBLE_QUOTE || type == 4)
// 		quote_type = 2;
// 	if (!make_nodeframe(node, type, token))
// 	{
// 		free(node);
// 		return (NULL);
// 	}
// 	if (node->arg_quote_type && (type == TYPE_SINGLE_QUOTE
// 			|| type == TYPE_DOUBLE_QUOTE || type == 5 || type == 4))
// 		node->arg_quote_type[0] = quote_type;
// 	return (node);
// }
/*
Initializes a token node with the given type and content
Sets up character-level quote tracking when appropriate
*/
t_node *initnode(t_tokentype type, char *token)
{
    t_node  *node;
    int     quote_type;
    
    // Set default quote type
    quote_type = 0;
    
    // Allocate node
    node = malloc(sizeof(t_node));
    if (!node)
        return (NULL);
    
    // Set quote type based on node type
    if (type == TYPE_SINGLE_QUOTE)
        quote_type = TYPE_SINGLE_QUOTE;
    else if (type == TYPE_DOUBLE_QUOTE)
        quote_type = TYPE_DOUBLE_QUOTE;
    
    // Initialize node framework
    if (!make_nodeframe(node, type, token))
    {
        free(node);
        return (NULL);
    }
    
    // Set character-level quote types for quoted content
    if (node->arg_quote_type && node->args && node->args[0] &&
        (type == TYPE_SINGLE_QUOTE || type == TYPE_DOUBLE_QUOTE))
    {
        int len = ft_strlen(node->args[0]);
        int i = 0;
        
        // Set each character to the proper quote type
        while (i < len)
        {
            node->arg_quote_type[0][i] = quote_type;
            i++;
        }
    }
    
    return (node);
}
