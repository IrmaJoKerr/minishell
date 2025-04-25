/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   make_node_arrays.c                                 :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: bleow <bleow@student.42kl.edu.my>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/04/18 21:07:33 by bleow             #+#    #+#             */
/*   Updated: 2025/04/25 07:02:48 by bleow            ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../includes/minishell.h"

/* 
Sets up the arguments array for a node.
Returns:
- The allocated args array.
- NULL if error.
*/
char	**setup_args(char *token)
{
	char	**args;

	args = malloc(sizeof(char *) * 2);
	if (!args)
		return (NULL);
	args[0] = ft_strdup(token);
	if (!args[0])
	{
		free(args);
		return (NULL);
	}
	args[1] = NULL;
	return (args);
}

/*
Sets up the quote tracking arrays for an argument.
Returns:
- The allocated quote types array.
- NULL if error.
*/
int	**setup_quotes(int len)
{
	int	**quote_types;
	int	i;

	quote_types = malloc(sizeof(int *) * 2);
	if (!quote_types)
		return (NULL);
	quote_types[0] = malloc(sizeof(int) * (len + 1));
	if (!quote_types[0])
	{
		free(quote_types);
		return (NULL);
	}
	i = 0;
	while (i < len)
	{
		quote_types[0][i] = 0;
		i++;
	}
	quote_types[0][len] = -1;
	quote_types[1] = NULL;
	return (quote_types);
}

/* 
Creates an argument array for a node with a single argument.
Allocates memory for the array and initializes it with the given token.
Also allocates character-level quote type arrays for each argument.
*/
void	make_node_arrays(t_node *node, char *token)
{
	int	len;

	node->args = setup_args(token);
	if (!node->args)
		return ;
	len = ft_strlen(token);
	node->arg_quote_type = setup_quotes(len);
}
