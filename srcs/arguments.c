/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   arguments.c                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: bleow <bleow@student.42kl.edu.my>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/03/01 21:36:41 by bleow             #+#    #+#             */
/*   Updated: 2025/04/06 20:57:20 by bleow            ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../includes/minishell.h"

/*
Creates an argument array for a node with a single argument.
Allocates memory for the array and initializes it with the given token.
The quote type is set to default 0 (no quotes).
Returns NULL on failure.
*/
void	create_args_array(t_node *node, char *token)
{
	char	**args;
	int		*quote_types;

	args = malloc(sizeof(char *) * 2);
	if (!args)
		return ;
	args[0] = ft_strdup(token);
	if (!args[0])
	{
		free(args);
		return ;
	}
	args[1] = NULL;
	node->args = args;
	quote_types = malloc(sizeof(int) * 2);
	if (!quote_types)
		return ;
	quote_types[0] = 0;
	quote_types[1] = 0;
	node->arg_quote_type = quote_types;
}
