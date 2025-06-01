/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   initnode.c                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: bleow <bleow@student.42kl.edu.my>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/03/13 15:53:06 by bleow             #+#    #+#             */
/*   Updated: 2025/06/01 20:25:19 by bleow            ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../includes/minishell.h"

/*
Sets up basic node structure framework.
- Handles node type, token, and argument array initialization.
- Has special case for head nodes.
- Sets a default token if token is NULL.
Returns:
- 1 on success.
- 0 on failure.
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
Applies quote type to all characters in the first argument of a node.
Sets each character's quote type in the arg_quote_type array.
*/
void	set_quote_type(t_node *node, int quote_type)
{
	int	len;
	int	i;

	if (!node->arg_quote_type || !node->args || !node->args[0])
		return ;
	len = ft_strlen(node->args[0]);
	i = 0;
	while (i < len)
	{
		node->arg_quote_type[0][i] = quote_type;
		i++;
	}
}

/*
Initializes a token node with the given type and content
Sets up character-level quote tracking when appropriate
*/
// t_node	*initnode(t_tokentype type, char *token)
// {
// 	t_node	*node;
// 	int		quote_type;

// 	quote_type = 0;
// 	node = malloc(sizeof(t_node));
// 	if (!node)
// 		return (NULL);
// 	if (type == TYPE_SINGLE_QUOTE)
// 		quote_type = TYPE_SINGLE_QUOTE;
// 	else if (type == TYPE_DOUBLE_QUOTE)
// 		quote_type = TYPE_DOUBLE_QUOTE;
// 	if (!make_nodeframe(node, type, token))
// 	{
// 		free(node);
// 		return (NULL);
// 	}
// 	if (type == TYPE_SINGLE_QUOTE || type == TYPE_DOUBLE_QUOTE)
// 		set_quote_type(node, quote_type);
// 	return (node);
// }
t_node	*initnode(t_tokentype type, char *token)
{
    t_node	*node;
    int		quote_type;

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
    if (type == TYPE_SINGLE_QUOTE || type == TYPE_DOUBLE_QUOTE)
        set_quote_type(node, quote_type);
    // DEBUG PRINT
    if (node->args && node->args[0])
        fprintf(stderr, "[DEBUG] initnode: type=%d, args[0]='%s'\n", type, node->args[0]);
    return (node);
}
