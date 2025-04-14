/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   arguments.c                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: bleow <bleow@student.42kl.edu.my>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/03/01 21:36:41 by bleow             #+#    #+#             */
/*   Updated: 2025/04/12 23:45:14 by bleow            ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../includes/minishell.h"

/* 
Creates an argument array for a node with a single argument.
Allocates memory for the array and initializes it with the given token.
Also allocates character-level quote type arrays for each argument.
*/
void create_args_array(t_node *node, char *token)
{
	char	**args;
	int		**quote_types;
	int		i;
	int		len;
	
	// Allocate arguments array
	args = malloc(sizeof(char *) * 2);
	if (!args)
		return;
	// Duplicate token as first argument
	args[0] = ft_strdup(token);
	if (!args[0])
	{
		free(args);
		return;
	}
	// Set NULL terminator
	args[1] = NULL;
	node->args = args;
	// Allocate quote types array
	quote_types = malloc(sizeof(int *) * 2);
	if (!quote_types)
		return;
	// Get token length for quote types
	len = ft_strlen(token);
	// Allocate character-level quote types
	quote_types[0] = malloc(sizeof(int) * (len + 1));
	if (!quote_types[0])
	{
		// Add debug print here
		fprintf(stderr, "DEBUG[create_args_array]: Failed to allocate quote_types[0]. "
				"node=%p, type=%d, token='%s', len=%d\n",
				(void*)node, node->type, token ? token : "NULL", len);
		// Critical fix: Free quote_types if inner allocation fails
		free(quote_types);
		return ;
	}
	// Initialize quote types to 0 (unquoted)
	i = 0;
	while (i < len)
	{
		quote_types[0][i] = 0;
		i++;
	}
	// Set NULL terminator
	quote_types[0][len] = -1; // Sentinel value
	quote_types[1] = NULL;
	node->arg_quote_type = quote_types;
}
